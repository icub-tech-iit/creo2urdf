/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Creo2Urdf.h>
#include <creo2urdf/Utils.h>

void Creo2Urdf::OnCommand() {

    pfcSession_ptr session_ptr = pfcGetProESession();
    pfcModel_ptr model_ptr = session_ptr->GetCurrentModel();

    // TODO Principal units probably to be changed from MM to M before getting the model properties
    // pfcSolid_ptr solid_ptr = pfcSolid::cast(session_ptr->GetCurrentModel());
    // auto length_unit = solid_ptr->GetPrincipalUnits()->GetUnit(pfcUnitType::pfcUNIT_LENGTH);
    // length_unit->Modify(pfcUnitConversionFactor::Create(0.001), length_unit->GetReferenceUnit()); // IT DOES NOT WORK

    idyn_model = iDynTree::Model::Model(); // no trivial way to clear the model

    if (!loadYamlConfig("ERGOCUB_all_options.yaml"))
    {
        printToMessageWindow("Failed to run Creo2Urdf!", c2uLogLevel::WARN);
        return;
    }

    iDynRedirectErrors idyn_redirect;
    idyn_redirect.redirectBuffer(std::cerr.rdbuf(), "iDynTreeErrors.txt");

    bool ret;

    auto asm_component_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
    if (asm_component_list->getarraysize() == 0) {
        printToMessageWindow("There are no FEATURES in the asm", c2uLogLevel::WARN);
        return;
    }

    // Let's clear the map in case of multiple click
    if (joint_info_map.size() > 0) {
        joint_info_map.clear();
    }

    // Let's traverse the model tree and get all links and axis properties
    for (int i = 0; i < asm_component_list->getarraysize(); i++)
    {      
        auto feat = pfcFeature::cast(asm_component_list->get(i));
        // auto feat_id = feat->GetId();


        if (feat->GetFeatType() != pfcFeatureType::pfcFEATTYPE_COMPONENT)
        {
            continue;
        }

        xintsequence_ptr seq = xintsequence::create();
        seq->append(feat->GetId());

        pfcComponentPath_ptr comp_path = pfcCreateComponentPath(pfcAssembly::cast(model_ptr), seq);

        auto component_handle = session_ptr->RetrieveModel(pfcComponentFeat::cast(feat)->GetModelDescr());

        iDynTree::Transform H_child = iDynTree::Transform::Identity();
        std::tie(ret, H_child) = getTransformFromRootToChild(comp_path, component_handle);

        if (!ret)
        {
            return;
        }

        auto link_name = string(component_handle->GetFullName());
        //printToMessageWindow(link_name);

        auto mass_prop = pfcSolid::cast(component_handle)->GetMassProperty();
        
        iDynTree::Link link;
        link.setInertia(fromCreo(mass_prop, H_child));

        if (!link.getInertia().isPhysicallyConsistent())
        {
            printToMessageWindow(link_name + " is NOT physically consistent!", c2uLogLevel::WARN);
        }

        LinkInfo l_info{ link_name, component_handle, H_child };
        link_info_map.insert(std::make_pair(link_name, l_info));
        populateJointInfoMap(component_handle);

        idyn_model.addLink(config["rename"][link_name].Scalar(), link);
        addMeshAndExport(component_handle);

        // TODO when we have an additional frame to add
        // idyn_model.addAdditionalFrameToLink(string(name), string(name) + "_" + string(csys_list->get(0)->GetName()), fromCreo(transform)); 
    }

    // Now we have to add joints to the iDynTree model

    //printToMessageWindow("Axis info map has size " + to_string(joint_info_map.size()));
    for (auto joint_info : joint_info_map) {
        auto parent_link_name = joint_info.second.parent_link_name;
        auto child_link_name = joint_info.second.child_link_name;
        auto axis_name = joint_info.second.name;
        //printToMessageWindow("AXIS " + axis_name + " has parent link: " + parent_link_name + " has child link : " + child_link_name);
        // This handles the case of a "cut" assembly, where we have an axis but we miss the child link.
        if (child_link_name.empty()) {
            continue;
        }

        auto joint_name = renameJoint(parent_link_name + "--" + child_link_name);
        auto root_H_parent_link = link_info_map.at(parent_link_name).root_H_link;
        auto root_H_child_link = link_info_map.at(child_link_name).root_H_link;
        auto child_model = link_info_map.at(child_link_name).modelhdl;

        //printToMessageWindow("Parent link H " + root_H_parent_link.toString());
        //printToMessageWindow("Child  link H " + root_H_child_link.toString());
        iDynTree::Transform parent_H_child = iDynTree::Transform::Identity();
        parent_H_child = root_H_parent_link.inverse() * root_H_child_link;

        //printToMessageWindow("H_parent: " + H_parent.toString());
        //printToMessageWindow("H_child: " + H_child.toString());
        //printToMessageWindow("prev_link_H_link: " + H_parent_to_child.toString());

        if (joint_info.second.type == JointType::Revolute) {
            iDynTree::Direction axis;
            std::tie(ret, axis) = getRotationAxisFromPart(child_model, axis_name, child_link_name, root_H_child_link);

            if (!ret)
            {
                return;
            }

            if (config["reverseRotationAxis"].Scalar().find(joint_name) != std::string::npos)
            {
               // printToMessageWindow("Reversing axis of " + joint_name);
                axis = axis.reverse();
            }

            iDynTree::RevoluteJoint joint(parent_H_child, { axis, parent_H_child.getPosition() });
            // Should be 0 the origin of the axis, the displacement is already considered in transform
            //{ o->get(0) * mm_to_m, o->get(1) * mm_to_m, o->get(2) * mm_to_m } });

    // TODO let's put the limits hardcoded, to be retrieved from Creo
            double min = 0.0;
            double max = M_PI;
            joint.enablePosLimits(true);
            joint.setPosLimits(0, min, max);
            // TODO we have to retrieve the rest transform from creo
            //joint.setRestTransform();

            if (idyn_model.addJoint(config["rename"][parent_link_name].Scalar(),
                config["rename"][child_link_name].Scalar(), joint_name, &joint) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT " + joint_name, c2uLogLevel::WARN);

                return;
            }
        }
        else if (joint_info.second.type == JointType::Fixed) {
            iDynTree::FixedJoint joint(parent_H_child);
            if (idyn_model.addJoint(config["rename"][parent_link_name].Scalar(), 
                config["rename"][child_link_name].Scalar(), joint_name, &joint) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT " + joint_name, c2uLogLevel::WARN);
                return;
            }
        }
        //printToMessageWindow("Joint " + joint_name);
    }

    std::ofstream idyn_model_out("iDynTreeModel.txt");
    idyn_model_out << idyn_model.toString();
    idyn_model_out.close();

    iDynTree::ModelExporterOptions export_options;
    export_options.robotExportedName = config["robotName"].Scalar();
    export_options.baseLink = config["rename"]["SIM_ECUB_1-1_ROOT_LINK"].Scalar();

    exportModelToUrdf(idyn_model, export_options);

    return;
}

bool Creo2Urdf::exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options) {
    iDynTree::ModelExporter mdl_exporter;

    mdl_exporter.init(mdl);
    mdl_exporter.setExportingOptions(options);

    if (!mdl_exporter.isValid())
    {
        printToMessageWindow("Model is not valid!", c2uLogLevel::WARN);
        return false;
    }

    if (!mdl_exporter.exportModelToFile("model.urdf"))
    {
        printToMessageWindow("Error exporting the urdf. See iDynTreeErrors.txt for details", c2uLogLevel::WARN);
        return false;
    }

    printToMessageWindow("Urdf created successfully!");
    return true;
}

void Creo2Urdf::populateJointInfoMap(pfcModel_ptr modelhdl) {

    // The revolute joints are defined by aligning along the
    // rotational axis

    auto axes_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_AXIS);
    auto link_name = string(modelhdl->GetFullName());
    // printToMessageWindow("There are " + to_string(axes_list->getarraysize()) + " axes");
    if (axes_list->getarraysize() == 0) {
        printToMessageWindow("There is no AXIS in the part " + link_name, c2uLogLevel::WARN);
    }

    pfcAxis* axis = nullptr;

    for (size_t i = 0; i < axes_list->getarraysize(); i++)
    {
        auto axis_elem = pfcAxis::cast(axes_list->get(xint(i)));
        auto axis_name_str = string(axis_elem->GetName());
        JointInfo joint_info;
        joint_info.name = axis_name_str;
        joint_info.type = JointType::Revolute;

        if (joint_info_map.find(axis_name_str) == joint_info_map.end()) {
            joint_info.parent_link_name = link_name;
            joint_info_map.insert(std::make_pair(axis_name_str, joint_info));
        }
        else {
            auto& existing_joint_info = joint_info_map.at(axis_name_str);
            existing_joint_info.child_link_name = link_name;
        }
    }

    // The fixed joint right now is defined making coincident the csys.

    auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);

    if (csys_list->getarraysize() == 0) {
        printToMessageWindow("There is no CSYS in the part " + link_name, c2uLogLevel::WARN);
    }

    for (size_t i = 0; i < csys_list->getarraysize(); i++)
    {
        auto csys_name = string(csys_list->get(i)->GetName());
        // We need to discard "general" csys, such as CSYS and ASM_CSYS
        if (csys_name.find("SCSYS") == std::string::npos) {
            continue;
        }
        JointInfo joint_info;
        joint_info.name = csys_name;
        joint_info.type = JointType::Fixed;
        if (joint_info_map.find(csys_name) == joint_info_map.end()) {
            joint_info.parent_link_name = link_name;
            joint_info_map.insert(std::make_pair(csys_name, joint_info));
        }
        else {
            auto& existing_joint_info = joint_info_map.at(csys_name);
            existing_joint_info.child_link_name = link_name;
        }
    }
}

bool Creo2Urdf::addMeshAndExport(pfcModel_ptr component_handle)
{
    //printToMessageWindow("Using " + relevant_csys_names[component_counter] + " to make stl");

    std::string link_child_name = component_handle->GetFullName();

    std::string csys_name = link_csys_map.at(link_child_name);

    std::string stl_file_name = link_child_name + ".stl";
    
    // Make all alphabetic characters lowercase
    std::transform(stl_file_name.begin(), stl_file_name.end(), stl_file_name.begin(),
        [](unsigned char c) { return std::tolower(c); });

    component_handle->Export(stl_file_name.c_str(), pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create(csys_name.c_str())));

    // Replace the first 5 bytes of the binary file with a string different than "solid"
    // to avoid issues with stl parsers.
    // For details see: https://github.com/icub-tech-iit/creo2urdf/issues/16
    sanitizeSTL(string(link_child_name) + ".stl");

    // Lets add the mesh to the link
    iDynTree::ExternalMesh visualMesh;
    // Meshes are in millimeters, while iDynTree models are in meters
    iDynTree::Vector3 scale; scale(0) = scale(1) = scale(2) = mm_to_m;
    visualMesh.setScale(scale);
    // Let's assign a gray as default color
    iDynTree::Vector4 color;
    iDynTree::Material material;
    color(0) = color(1) = color(2) = 0.5;
    color(3) = 1.0;
    material.setColor(color);
    visualMesh.setMaterial(material);
    // Assign transform
    // TODO Right now maybe it is not needed it ie exported respct the link csys
    // visualMesh.setLink_H_geometry(H_parent_to_child);

    // Assign name
    visualMesh.setFilename(stl_file_name);
    // TODO Right now let's consider visual and collision with the same mesh
    idyn_model.visualSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(config["rename"][link_child_name].Scalar())].push_back(visualMesh.clone());
    idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(config["rename"][link_child_name].Scalar())].push_back(visualMesh.clone());

    // idyn_model.visualSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(string(link_child_name))].push_back(visualMesh.clone());
    // idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(string(link_child_name))].push_back(visualMesh.clone());


    return true;
}

bool Creo2Urdf::loadYamlConfig(const std::string& filename)
{
    try 
    {
        config = YAML::LoadFile(filename);
    }
    catch (YAML::BadFile file_does_not_exist) 
    {
        printToMessageWindow("Configuration file " + filename + " does not exist!", c2uLogLevel::WARN);
        return false;
    }
    catch (YAML::ParserException badly_formed) 
    {
        printToMessageWindow(badly_formed.msg, c2uLogLevel::WARN);
        return false;
    }

    printToMessageWindow("Configuration file " + filename + " was loaded successfully");

    return true;
}

std::string Creo2Urdf::renameJoint(const std::string& joint_name)
{
    if (config["rename"][joint_name].IsDefined())
    {
        std::string new_name = config["rename"][joint_name].Scalar();
        // printToMessageWindow("Renaming joint " + joint_name + " to " + new_name);
        return new_name;
    }
    else
    {
        printToMessageWindow("Joint " + joint_name + " is not present in the configuration file!", c2uLogLevel::WARN);
        return joint_name;
    }
}

pfcCommandAccess Creo2UrdfAccess::OnCommandAccess(xbool AllowErrorMessages)
{
    auto model = pfcGetProESession()->GetCurrentModel();
    if (!model) {
        return pfcCommandAccess::pfcACCESS_AVAILABLE;
    }
    auto type = model->GetType();
    if (type != pfcMDL_PART && type != pfcMDL_ASSEMBLY) {
        return pfcCommandAccess::pfcACCESS_UNAVAILABLE;
    }
    return pfcCommandAccess::pfcACCESS_AVAILABLE;
}
