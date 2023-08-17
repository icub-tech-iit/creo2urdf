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
        link_info_map.clear();
        exported_frame_info_map.clear();
        ft_sensors.clear();
    }

    if(config["scale"].IsDefined()) {
        scale = config["scale"].as<std::array<double,3>>();
    }

    if (config["originXYZ"].IsDefined()) {
        originXYZ = config["originXYZ"].as<std::array<double, 3>>();
    }

    if (config["originRPY"].IsDefined()) {
        originRPY = config["originRPY"].as<std::array<double, 3>>();
    }

    readExportedFramesFromConfig();
    //readSensorsFromConfig();
    readFTSensorsFromConfig();
    readAssignedInertiasFromConfig();

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
        
        auto link_name = string(component_handle->GetFullName());
        //printToMessageWindow(link_name);

        iDynTree::Transform root_H_link = iDynTree::Transform::Identity();
        
        std::string urdf_link_name = getRenameElementFromConfig(link_name);

        std::string link_frame_name = "";

        for (const auto& lf : config["linkFrames"]) {
            if (lf["linkName"].Scalar() != urdf_link_name)
            {
                continue;
            }
            link_frame_name = lf["frameName"].Scalar();
        }
        std::tie(ret, root_H_link) = getTransformFromRootToChild(comp_path, component_handle, link_frame_name, scale);

        if (!ret)
        {
            return;
        }

        auto mass_prop = pfcSolid::cast(component_handle)->GetMassProperty();
        
        iDynTree::Link link;
        link.setInertia(computeSpatialInertiafromCreo(mass_prop, root_H_link, urdf_link_name));

        if (!link.getInertia().isPhysicallyConsistent())
        {
            printToMessageWindow(link_name + " is NOT physically consistent!", c2uLogLevel::WARN);
        }

        LinkInfo l_info{urdf_link_name, component_handle, root_H_link, link_frame_name };
        link_info_map.insert(std::make_pair(link_name, l_info));
        populateJointInfoMap(component_handle);
        populateExportedFrameInfoMap(component_handle);
        populateFTMap(component_handle);

        idyn_model.addLink(urdf_link_name, link);
        addMeshAndExport(component_handle, link_frame_name);
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

        auto joint_name = getRenameElementFromConfig(parent_link_name + "--" + child_link_name);
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

            // TODO let's put the limits hardcoded, to be retrieved from Creo
            double min = 0.0;
            double max = M_PI;
            joint.enablePosLimits(true);
            joint.setPosLimits(0, min, max);
            // TODO we have to retrieve the rest transform from creo
            //joint.setRestTransform();

            if (idyn_model.addJoint(getRenameElementFromConfig(parent_link_name),
                getRenameElementFromConfig(child_link_name), joint_name, &joint) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT " + joint_name, c2uLogLevel::WARN);

                return;
            }
        }
        else if (joint_info.second.type == JointType::Fixed) {
            iDynTree::FixedJoint joint(parent_H_child);
            if (idyn_model.addJoint(getRenameElementFromConfig(parent_link_name),
                getRenameElementFromConfig(child_link_name), joint_name, &joint) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT " + joint_name, c2uLogLevel::WARN);
                return;
            }
        }
    }

    // Let's add all the exported frames
    for (auto exported_frame_info : exported_frame_info_map) {
        std::string reference_link = exported_frame_info.second.frameReferenceLink;
        if (idyn_model.getLinkIndex(reference_link) == iDynTree::LINK_INVALID_INDEX) {
            // TODO FATAL?!
            printToMessageWindow("Failed to add additional frame, link " + reference_link + " is not in the model", c2uLogLevel::WARN);
            continue;
        }
        if (!idyn_model.addAdditionalFrameToLink(reference_link, exported_frame_info.second.exportedFrameName,
            exported_frame_info.second.linkFrame_H_additionalFrame * exported_frame_info.second.additionalTransformation)) {
            printToMessageWindow("Failed to add additional frame  " + exported_frame_info.second.exportedFrameName, c2uLogLevel::WARN);
            continue;
        }
    }

    std::ofstream idyn_model_out("iDynTreeModel.txt");
    idyn_model_out << idyn_model.toString();
    idyn_model_out.close();

    buildFTXMLBlobs();

    iDynTree::ModelExporterOptions export_options;
    export_options.robotExportedName = config["robotName"].Scalar();
    export_options.baseLink = config["rename"]["SIM_ECUB_1-1_ROOT_LINK"].Scalar();
    if (config["XMLBlobs"].IsDefined()) {
        export_options.xmlBlobs = config["XMLBlobs"].as<std::vector<std::string>>();
        // Adding gazebo pose as xml blob at the end of the urdf.
        std::string gazebo_pose_xml_str{""};
        gazebo_pose_xml_str = to_string(originXYZ[0]) + " " + to_string(originXYZ[1]) + " " + to_string(originXYZ[2]) + " " + to_string(originRPY[0]) + " " + to_string(originRPY[1]) + " " + to_string(originRPY[2]);
        gazebo_pose_xml_str = "<gazebo><pose>" + gazebo_pose_xml_str + "</pose></gazebo>";
        export_options.xmlBlobs.push_back(gazebo_pose_xml_str);
    }

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

iDynTree::SpatialInertia Creo2Urdf::computeSpatialInertiafromCreo(pfcMassProperty_ptr mass_prop, iDynTree::Transform H, const std::string& link_name) {
    auto com = mass_prop->GetGravityCenter();
    auto inertia_tensor = mass_prop->GetCenterGravityInertiaTensor();
    iDynTree::RotationalInertiaRaw idyn_inertia_tensor = iDynTree::RotationalInertiaRaw::Zero();
    bool assigned_inertia_flag = assigned_inertias_map.find(link_name) != assigned_inertias_map.end();
    for (int i_row = 0; i_row < idyn_inertia_tensor.rows(); i_row++) {
        for (int j_col = 0; j_col < idyn_inertia_tensor.cols(); j_col++) {
            if ((assigned_inertia_flag) && (i_row == j_col))
            {
                idyn_inertia_tensor.setVal(i_row, j_col, assigned_inertias_map.at(link_name)[i_row]);
            }
            else {
                idyn_inertia_tensor.setVal(i_row, j_col, inertia_tensor->get(i_row, j_col) * scale[i_row] * scale[j_col]);
            }
        }
    }

    iDynTree::Position com_child({ com->get(0) * scale[0] , com->get(1) * scale[1], com->get(2) * scale[2] });
    com_child = H.inverse() * com_child;  // TODO verify
    double mass{ 0.0 };
    if (config["assignedMasses"][link_name].IsDefined()) {
        mass = config["assignedMasses"][link_name].as<double>();
    }
    else {
        mass = mass_prop->GetMass();
    }
    iDynTree::SpatialInertia sp_inertia(mass, com_child, idyn_inertia_tensor);
    sp_inertia.fromRotationalInertiaWrtCenterOfMass(mass, com_child, idyn_inertia_tensor);

    return sp_inertia;
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

    for (xint i = 0; i < csys_list->getarraysize(); i++)
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

void Creo2Urdf::populateExportedFrameInfoMap(pfcModel_ptr modelhdl) {

    // The revolute joints are defined by aligning along the
    // rotational axis
    auto link_name = string(modelhdl->GetFullName());
    auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);

    if (csys_list->getarraysize() == 0) {
        printToMessageWindow("There is no CSYS in the part " + link_name, c2uLogLevel::WARN);
    }
    // Now let's handle csys, they can form fixed links (FT sensors), or define exported frames
    for (xint i = 0; i < csys_list->getarraysize(); i++)
    {
        auto csys_name = string(csys_list->get(i)->GetName());
        if (exported_frame_info_map.find(csys_name) != exported_frame_info_map.end()) {
            auto& exported_frame_info = exported_frame_info_map.at(csys_name);
            auto& link_info = link_info_map.at(link_name);
            bool ret{ false };
            iDynTree::Transform csys_H_additionalFrame {iDynTree::Transform::Identity()};
            iDynTree::Transform csys_H_linkFrame {iDynTree::Transform::Identity()};
            iDynTree::Transform linkFrame_H_additionalFrame {iDynTree::Transform::Identity()};

            std::tie(ret, csys_H_additionalFrame) = getTransformFromPart(modelhdl, csys_name, scale);
            std::tie(ret, csys_H_linkFrame) = getTransformFromPart(modelhdl, link_info.link_frame_name, scale);

            linkFrame_H_additionalFrame = csys_H_linkFrame.inverse() * csys_H_additionalFrame;
            exported_frame_info.linkFrame_H_additionalFrame = linkFrame_H_additionalFrame;

        }
    }
}

void Creo2Urdf::populateFTMap(pfcModel_ptr modelhdl)
{
    // The revolute joints are defined by aligning along the
    // rotational axis
    auto link_name = string(modelhdl->GetFullName());
    auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);

    for (auto& f : ft_sensors)
    {
        // Now let's handle csys, they can form fixed links (FT sensors), or define exported frames
        for (xint i = 0; i < csys_list->getarraysize(); i++)
        {
            auto csys_name = string(csys_list->get(i)->GetName());

            if (csys_name == f.frameName)
            {
                printToMessageWindow(link_name);
                auto& link_info = link_info_map.at(link_name);
                auto root_H_ft = getTransformFromPart(modelhdl, csys_name, scale).second;
                forcetorque_transform_map.insert(std::make_pair(f.jointName, link_info.root_H_link.inverse() * root_H_ft));
            }
        }

    }
}

void Creo2Urdf::readAssignedInertiasFromConfig() {
    for (const auto& ai : config["assignedInertias"]) {
        std::array<double, 3> assignedInertia { ai["xx"].as<double>(), ai["yy"].as<double>(), ai["zz"].as<double>()};
        assigned_inertias_map.insert(std::make_pair(ai["linkName"].Scalar(), assignedInertia));
    }
}

void Creo2Urdf::readExportedFramesFromConfig() {

    if (!config["exportedFrames"].IsDefined())
        return;

    for (const auto& ef : config["exportedFrames"]) {
        ExportedFrameInfo ef_info;
        ef_info.frameReferenceLink = ef["frameReferenceLink"].Scalar();
        ef_info.exportedFrameName  = ef["exportedFrameName"].Scalar();
        if (ef["additionalTransformation"].IsDefined()) {
            auto xyzrpy = ef["additionalTransformation"].as<std::vector<double>>();
            iDynTree::Transform additionalFrameOld_H_additionalFrame {iDynTree::Transform::Identity()};
            additionalFrameOld_H_additionalFrame.setPosition({ xyzrpy[0], xyzrpy[1], xyzrpy[2] });
            additionalFrameOld_H_additionalFrame.setRotation({ iDynTree::Rotation::RPY( xyzrpy[3], xyzrpy[4], xyzrpy[5]) });
            ef_info.additionalTransformation = additionalFrameOld_H_additionalFrame;
        }
        exported_frame_info_map.insert(std::make_pair(ef["frameName"].Scalar(), ef_info));
    }
}

void Creo2Urdf::readSensorsFromConfig()
{
    if (!config["sensors"].IsDefined())
        return;

    for (const auto& s : config["sensors"]) {

        bool export_frame = false;

        // This is the only key that is not uniformly defined
        // in the config of sensors
        if (s["exportFrameInURDF"].IsDefined())
        {
            export_frame = s["exportFrameInURDF"].as<bool>();
        }

        double update_rate = 100;
        if (s["updateRate"].IsDefined())
        {
            update_rate = s["updateRate"].as<double>();
        }

        try
        {
            printToMessageWindow("Found sensor " + s["sensorName"].Scalar());

            sensors.push_back({ s["sensorName"].Scalar(),
                                s["frameName"].Scalar(),
                                s["linkName"].Scalar(),
                                export_frame,
                                sensor_type_map.at(s["sensorType"].Scalar()),
                                update_rate,
                                s["sensorBlobs"].as<std::vector<std::string>>()});
        }
        catch (YAML::Exception & e)
        {
            printToMessageWindow(e.msg, c2uLogLevel::WARN);
        }

    }
}

void Creo2Urdf::readFTSensorsFromConfig()
{
    for (const auto& s : config["forceTorqueSensors"]) {

        ft_sensors.push_back({ s["jointName"].Scalar(),
                               s["directionChildToParent"].as<bool>(),
                               s["frame"].Scalar(),
                               s["frameName"].Scalar(),
                               s["sensorBlobs"].as<std::vector<std::string>>() });
    }
}

/*
  <gazebo reference="l_foot_front_ft_sensor">
    <sensor name="l_foot_front_ft_sensor" type="force_torque">
      <always_on>1</always_on>
      <update_rate>100</update_rate>
      <force_torque>
        <frame>sensor</frame>
        <measure_direction>parent_to_child</measure_direction>
      </force_torque>
      <pose>0.0 0.0 0.022299999999999986 0.0 -0.0 -2.0943952105869315</pose>
      <plugin name="left_foot_front_ft_plugin" filename="libgazebo_yarp_forcetorque.so">
        <yarpConfigurationFile>model://ergoCub/conf/FT/gazebo_ergocub_left_foot_front_ft.ini</yarpConfigurationFile>
    </plugin>
    </sensor>
  </gazebo>

  <sensor name="l_foot_front_ft_sensor" type="force_torque">
    <parent joint="l_foot_front_ft_sensor"/>
    <force_torque>
      <frame>sensor</frame>
      <measure_direction>parent_to_child</measure_direction>
    </force_torque>
    <origin rpy="0.0 -0.0 -2.0943952105869315" xyz="0.0 0.0 0.022299999999999986"/>
  </sensor>

*/

void Creo2Urdf::buildFTXMLBlobs()
{
    iDynTree::Traversal traversal;

    idyn_model.computeFullTreeTraversal(traversal);

    iDynTree::KinDynComputations computer;


    for (const auto& ft : ft_sensors)
    {
        std::string filename = ft.jointName + ".xml";

        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, node = NULL;
        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "gazebo");

        xmlDocSetRootElement(doc, root_node);

        node = xmlNewChild(root_node, NULL, BAD_CAST "sensor", NULL);
        auto node_attributes = xmlNewProp(node, BAD_CAST "name", BAD_CAST ft.jointName.c_str());
        node_attributes = xmlNewProp(node, BAD_CAST "type", BAD_CAST "force_torque");
        
        auto node1 = xmlNewChild(node, NULL, BAD_CAST "always_on", BAD_CAST "1");
        node1 = xmlNewChild(node, NULL, BAD_CAST "update_rate", BAD_CAST "100");
        node1 = xmlNewChild(node, NULL, BAD_CAST "force_torque", NULL);

        auto node2 = xmlNewChild(node1, NULL, BAD_CAST "frame", BAD_CAST ft.frame.c_str());

        if(ft.directionChildToParent)
            node2 = xmlNewChild(node1, NULL, BAD_CAST "measure_direction", BAD_CAST "child_to_parent");
        else
            node2 = xmlNewChild(node1, NULL, BAD_CAST "measure_direction", BAD_CAST "parent_to_child");

        auto & trf = forcetorque_transform_map.at(ft.jointName);

        std::string pose_xyz_rpy = trf.getPosition().toString() + " " + trf.getRotation().asRPY().toString();

        node1 = xmlNewChild(node, NULL, BAD_CAST "pose", BAD_CAST pose_xyz_rpy.c_str());

        xmlSaveFormatFileEnc(filename.c_str(), doc, "UTF-8", 1);

        xmlFreeDoc(doc);
        xmlCleanupParser();
    }
}

bool Creo2Urdf::addMeshAndExport(pfcModel_ptr component_handle, const std::string& stl_transform)
{
    //printToMessageWindow("Using " + relevant_csys_names[component_counter] + " to make stl");

    std::string link_child_name = component_handle->GetFullName();
    std::string stl_file_name = link_child_name + ".stl";
    std::string renamed_link_child_name = config["rename"][link_child_name].Scalar();

    // Make all alphabetic characters lowercase
    std::transform(stl_file_name.begin(), stl_file_name.end(), stl_file_name.begin(),
        [](unsigned char c) { return std::tolower(c); });

    component_handle->Export(stl_file_name.c_str(), pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create(stl_transform.c_str())));

    // Replace the first 5 bytes of the binary file with a string different than "solid"
    // to avoid issues with stl parsers.
    // For details see: https://github.com/icub-tech-iit/creo2urdf/issues/16
    sanitizeSTL(stl_file_name);

    // Lets add the mesh to the link
    iDynTree::ExternalMesh visualMesh;
    // Meshes are in millimeters, while iDynTree models are in meters
    visualMesh.setScale({scale});
    // Let's assign a gray as default color
    iDynTree::Vector4 color;
    iDynTree::Material material;

    if(config["assignedColors"][renamed_link_child_name].IsDefined())
    {
        for (size_t i = 0; i < config["assignedColors"][renamed_link_child_name].size(); i++)
            color(i) = config["assignedColors"][renamed_link_child_name][i].as<double>();
    } 
    else
    {
        color(0) = color(1) = color(2) = 0.5;
        color(3) = 1;
    }

    material.setColor(color);
    visualMesh.setMaterial(material);
    // Assign transform
    // TODO Right now maybe it is not needed it ie exported respct the link csys
    // visualMesh.setLink_H_geometry(H_parent_to_child);

    // Assign name
    visualMesh.setFilename(stl_file_name);
    // TODO Right now let's consider visual and collision with the same mesh
    idyn_model.visualSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_child_name)].push_back(visualMesh.clone());
    idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_child_name)].push_back(visualMesh.clone());

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

std::string Creo2Urdf::getRenameElementFromConfig(const std::string& elem_name)
{
    if (config["rename"][elem_name].IsDefined())
    {
        std::string new_name = config["rename"][elem_name].Scalar();
        return new_name;
    }
    else
    {
        printToMessageWindow("Element " + elem_name + " is not present in the configuration file!", c2uLogLevel::WARN);
        return elem_name;
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
