/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */
#define _USE_MATH_DEFINES

#include <creo2urdf/Creo2Urdf.h>

void Creo2UrdfActionListerner::OnCommand() {

    pfcSession_ptr session_ptr = pfcGetProESession();
    pfcModel_ptr model_ptr = session_ptr->GetCurrentModel();
    pfcSolid_ptr solid_ptr = pfcSolid::cast(session_ptr->GetCurrentModel());

    // TODO Principal units probably to be changed from MM to M before getting the model properties
    //auto length_unit = solid_ptr->GetPrincipalUnits()->GetUnit(pfcUnitType::pfcUNIT_LENGTH);
    // length_unit->Modify(pfcUnitConversionFactor::Create(0.001), length_unit->GetReferenceUnit()); // IT DOES NOT WORK

    iDynTree::Model idyn_model;
    iDynTree::ModelExporterOptions export_options;
    export_options.robotExportedName = "ECUB_HEAD";
    export_options.baseLink = "SIM_ECUB_HEAD_NECK_1";

    auto asm_component_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
    if (asm_component_list->getarraysize() == 0) {
        printToMessageWindow("There are no FEATURES in the asm", c2uLogLevel::WARN);
        return;
    }

    std::ofstream idyn_model_out("iDynTreeModel.txt");
    std::ofstream idyn_error_out("iDynTreeErrors.txt");
    std::streambuf* cerr_old_buf = std::cerr.rdbuf(); // store the original cerr buffer
    std::cerr.rdbuf(idyn_error_out.rdbuf());          // Pass file buffer to set it as new buffer

    std::string link_parent_name = "";

    iDynTree::Transform H_parent = iDynTree::Transform::Identity();

    int component_counter = 0;
    bool ret;

    for (int i = 0; i < asm_component_list->getarraysize(); i++)
    {
        iDynTree::Link link_child;
        iDynTree::Transform H_child = iDynTree::Transform::Identity();
        iDynTree::Transform H_parent_to_child = iDynTree::Transform::Identity();
        auto comp = asm_component_list->get(i);
        auto feat = pfcFeature::cast(comp);
        // auto feat_id = feat->GetId();


        if (feat->GetFeatType() != pfcFeatureType::pfcFEATTYPE_COMPONENT)
        {
            continue;
        }

        auto modelhdl = session_ptr->RetrieveModel(pfcComponentFeat::cast(feat)->GetModelDescr());
        auto link_child_name = string(modelhdl->GetFullName());

        printToMessageWindow(link_child_name);

        xintsequence_ptr seq = xintsequence::create();
        seq->append(feat->GetId());

        pfcComponentPath_ptr comp_path = pfcCreateComponentPath(pfcAssembly::cast(model_ptr), seq);

        auto asm_csys_H_csys = fromCreo(comp_path->GetTransform(xtrue));

        iDynTree::Transform csys_H_child;
        std::tie(ret, csys_H_child) = getTransformFromPart(modelhdl, link_child_name);

        H_child = asm_csys_H_csys * csys_H_child;

        if (!ret)
        {
            std::cerr.rdbuf(cerr_old_buf);
            return;
        }

        auto mass_prop = pfcSolid::cast(modelhdl)->GetMassProperty();
        auto com = mass_prop->GetGravityCenter();                     // TODO transform the center of mass in relative coords
        auto comInertia = mass_prop->GetCenterGravityInertiaTensor(); // TODO GetCoordSysInertia ?

        link_child.setInertia(fromCreo(mass_prop, H_child));

        if (!link_child.getInertia().isPhysicallyConsistent())
        {
            printToMessageWindow(link_child_name + " is NOT physically consistent!", c2uLogLevel::WARN);
        }

        /*
        printToMessageWindow("Model name is " + std::string(name) + " and weighs " + to_string(mass_prop->GetMass()));
        printToMessageWindow("Center of mass: x: " + to_string(com->get(0)) + " y: " + to_string(com->get(1)) + " z: " + to_string(com->get(2)));
        printToMessageWindow("Inertia tensor:");
        printToMessageWindow(to_string(comInertia->get(0, 0)) + " " + to_string(comInertia->get(0, 1)) + " " + to_string(comInertia->get(0, 2)));
        printToMessageWindow(to_string(comInertia->get(1, 0)) + " " + to_string(comInertia->get(1, 1)) + " " + to_string(comInertia->get(1, 2)));
        printToMessageWindow(to_string(comInertia->get(2, 0)) + " " + to_string(comInertia->get(2, 1)) + " " + to_string(comInertia->get(2, 2)));
        */

        if (component_counter > 0)
        {
            iDynTree::Direction axis;
            std::tie(ret, axis) = getRotationAxisFromPart(modelhdl, link_child_name, H_child);

            if (!ret)
            {
                std::cerr.rdbuf(cerr_old_buf);
                return; 
            }

            H_parent_to_child = H_parent.inverse() * H_child;

            //printToMessageWindow("H_parent: " + H_parent.toString());
            //printToMessageWindow("H_child: " + H_child.toString());
            //printToMessageWindow("prev_link_H_link: " + H_parent_to_child.toString());

            iDynTree::RevoluteJoint joint(H_parent_to_child, {axis, H_parent_to_child.getPosition()});
                                                                // Should be 0 the origin of the axis, the displacement is already considered in transform
                                                                //{ o->get(0) * mm_to_m, o->get(1) * mm_to_m, o->get(2) * mm_to_m } });

            // TODO let's put the limits hardcoded, to be retrieved from Creo
            double min = 0.0;
            double max = M_PI;
            joint.enablePosLimits(true);
            joint.setPosLimits(0, min, max);
            // TODO we have to retrieve the rest transform from creo
            //joint.setRestTransform();

            if (idyn_model.addJointAndLink(link_parent_name, link_parent_name + "--" +link_child_name, &joint, link_child_name, link_child) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT!", c2uLogLevel::WARN);

                std::cerr.rdbuf(cerr_old_buf);
                return;
            }
            printToMessageWindow(to_string(component_counter) + ": " + link_parent_name + "--" + link_child_name);
        }
        else
        {
            printToMessageWindow("First link, skipping joint addition");
            idyn_model.addLink(string(link_child_name), link_child);
        }

        addMeshAndExport(modelhdl, link_child_name, component_counter, idyn_model);

        link_parent_name = link_child_name;
        H_parent = H_child;
        component_counter++;

        // TODO when we have an additional frame to add
        // idyn_model.addAdditionalFrameToLink(string(name), string(name) + "_" + string(csys_list->get(0)->GetName()), fromCreo(transform)); 
    }

    idyn_model_out << idyn_model.toString();
    idyn_model_out.close();
    iDynTree::ModelExporter mdl_exporter;
    mdl_exporter.init(idyn_model);
    mdl_exporter.setExportingOptions(export_options);

    if (!mdl_exporter.isValid())
    {
        printToMessageWindow("Model is not valid!", c2uLogLevel::WARN);
    }

    /*
    if (idyn_model.getLinkIndex("SIM_ECUB_HEAD_NECK_1") == iDynTree::LINK_INVALID_INDEX)
    {
        printToMessageWindow("[ERROR] URDFStringFromModel: specified baseLink is not part of the model");
    }

    iDynTree::Traversal exportTraversal;
    if (!idyn_model.computeFullTreeTraversal(exportTraversal, idyn_model.getLinkIndex("SIM_ECUB_HEAD_NECK_1")))
    {
        printToMessageWindow("[ERROR] URDFStringFromModel: error in computeFullTreeTraversal", c2uLogLevel::WARN);
    }
    else
    {
        printToMessageWindow("Traversal completed successfully");
    }
    */

    if (!mdl_exporter.exportModelToFile("model.urdf"))
    {
        printToMessageWindow("Error exporting the urdf. See iDynTreeErrors.txt for details", c2uLogLevel::WARN);
    }
    else
    {
        printToMessageWindow("Urdf created successfully!");
    }

    std::cerr.rdbuf(cerr_old_buf); // Restore original cerr buffer after using iDynTree
    return;
}



pfcCommandAccess Creo2UrdfAccessListener::OnCommandAccess(xbool AllowErrorMessages) {
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
