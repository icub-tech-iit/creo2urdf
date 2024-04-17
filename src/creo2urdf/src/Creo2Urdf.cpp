/**
 * @file Creo2Urdf.cpp
 * @brief Contains definitions for the Creo2Urdf class.
 * @copyright (C) 2006-2024 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Creo2Urdf.h>
#include <creo2urdf/Utils.h>
#include <pfcExceptions.h>

#include <iDynTree/PrismaticJoint.h>
#include <iDynTree/EigenHelpers.h>

#include <Eigen/Core>

void Creo2Urdf::OnCommand() {

    pfcSession_ptr session_ptr = pfcGetProESession();
    if (!session_ptr) {
        printToMessageWindow("Failed to get the session", c2uLogLevel::WARN);
        return;
    }
    if (!m_asm_model_ptr) {
        m_asm_model_ptr = session_ptr->GetCurrentModel();
        if (!m_asm_model_ptr) {
            printToMessageWindow("Failed to get the current model", c2uLogLevel::WARN);
            return;
        }
    }


    // TODO Principal units probably to be changed from MM to M before getting the model properties
    // pfcSolid_ptr solid_ptr = pfcSolid::cast(session_ptr->GetCurrentModel());
    // auto length_unit = solid_ptr->GetPrincipalUnits()->GetUnit(pfcUnitType::pfcUNIT_LENGTH);
    // length_unit->Modify(pfcUnitConversionFactor::Create(0.001), length_unit->GetReferenceUnit()); // IT DOES NOT WORK

    idyn_model = iDynTree::Model::Model(); // no trivial way to clear the model
    auto yaml_file_open_option = pfcFileOpenOptions::Create("*.yml,*.yaml");
    yaml_file_open_option->SetDialogLabel("Select the yaml");

    // YAML file path
    if (m_yaml_path.empty()) {
        m_yaml_path = string(session_ptr->UIOpenFile(yaml_file_open_option));
    }
    if (!loadYamlConfig(m_yaml_path))
    {
        printToMessageWindow("Failed to run Creo2Urdf!", c2uLogLevel::WARN);
        return;
    }
    // CSV file path
    if (m_csv_path.empty()) {
        auto csv_file_open_option = pfcFileOpenOptions::Create("*.csv");
        csv_file_open_option->SetDialogLabel("Select the csv");
        m_csv_path = string(session_ptr->UIOpenFile(csv_file_open_option));
    }
    rapidcsv::Document joints_csv_table(m_csv_path, rapidcsv::LabelParams(0, 0));
    // Output folder path
    if (m_output_path.empty()) {
        auto output_folder_open_option = pfcDirectorySelectionOptions::Create();
        output_folder_open_option->SetDialogLabel("Select the output dir");
        m_output_path = string(session_ptr->UISelectDirectory(output_folder_open_option));
    }
    printToMessageWindow("Output path is: " + m_output_path);
    

    iDynRedirectErrors idyn_redirect;
    idyn_redirect.redirectBuffer(std::cerr.rdbuf(), "iDynTreeErrors.txt");

    bool ret;

    auto asm_component_list = m_asm_model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
    if (asm_component_list->getarraysize() == 0) {
        printToMessageWindow("There are no FEATURES in the asm", c2uLogLevel::WARN);
        return;
    }

    // Let's clear the map in case of multiple click
    if (joint_info_map.size() > 0) {
        joint_info_map.clear();
        link_info_map.clear();
        exported_frame_info_map.clear();
        assigned_inertias_map.clear();
        assigned_collision_geometry_map.clear();
    }

    if (config["warningsAreFatal"].IsDefined()) {
        warningsAreFatal = config["warningsAreFatal"].as<bool>();
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

    if (config["exportAllUseradded"].IsDefined()) {
        exportAllUseradded = config["exportAllUseradded"].as<bool>();
    }

    readExportedFramesFromConfig();
    readAssignedInertiasFromConfig();
    readAssignedCollisionGeometryFromConfig();

    Sensorizer sensorizer;

    sensorizer.readFTSensorsFromConfig(config);
    sensorizer.readSensorsFromConfig(config);

    ElementTreeManager elem_tree;

    // Let's traverse the model tree and get all links and axis properties
    for (int i = 0; i < asm_component_list->getarraysize(); i++)
    {      
        auto feat = pfcFeature::cast(asm_component_list->get(i));

        if (feat->GetFeatType() != pfcFeatureType::pfcFEATTYPE_COMPONENT)
        {
            continue;
        }

        xintsequence_ptr seq = xintsequence::create();
        seq->append(feat->GetId());

        elem_tree.populateJointInfoFromElementTree(feat, joint_info_map);

        pfcComponentPath_ptr comp_path = pfcCreateComponentPath(pfcAssembly::cast(m_asm_model_ptr), seq);

        auto component_handle = session_ptr->RetrieveModel(pfcComponentFeat::cast(feat)->GetModelDescr());
        
        auto link_name = string(component_handle->GetFullName());

        iDynTree::Transform root_H_link = iDynTree::Transform::Identity();
        iDynTree::Transform csysPart_H_link = iDynTree::Transform::Identity();
        
        std::string urdf_link_name = getRenameElementFromConfig(link_name);

        std::string link_frame_name = "";

        for (const auto& lf : config["linkFrames"]) {
            if (lf["linkName"].Scalar() != urdf_link_name)
            {
                continue;
            }
            link_frame_name = lf["frameName"].Scalar();
        }

        if (link_frame_name.empty()) {
            printToMessageWindow(link_name + " misses the frame in the linkFrames section, CSYS will be used instead", c2uLogLevel::WARN);
            link_frame_name = "CSYS";
        }
        std::tie(ret, root_H_link) = getTransformFromRootToChild(comp_path, component_handle, link_frame_name, scale);

        if (!ret && warningsAreFatal)
        {
            return;
        }

        auto mass_prop = pfcSolid::cast(component_handle)->GetMassProperty();

        std::tie(ret, csysPart_H_link) = getTransformFromPart(component_handle, link_frame_name, scale);
        if (!ret && warningsAreFatal)
        {
            return;
        }
        
        iDynTree::Link link;
        link.setInertia(computeSpatialInertiafromCreo(mass_prop, csysPart_H_link, urdf_link_name));

        if (!link.getInertia().isPhysicallyConsistent())
        {
            printToMessageWindow(link_name + " is NOT physically consistent!", c2uLogLevel::WARN);
            if (warningsAreFatal) {
                return;
            }
        }

        LinkInfo l_info{urdf_link_name, component_handle, root_H_link, link_frame_name };
        link_info_map.insert(std::make_pair(link_name, l_info));
        populateExportedFrameInfoMap(component_handle);
        
        idyn_model.addLink(urdf_link_name, link);
        if (!addMeshAndExport(component_handle, link_frame_name)) {
            printToMessageWindow("Failed to export mesh for " + link_name, c2uLogLevel::WARN);
            if (warningsAreFatal) {
                return;
            }
        }
    }

    // Now we have to add joints to the iDynTree model

    for (auto & joint_info : joint_info_map) {
        auto parent_link_name = joint_info.second.parent_link_name;
        auto child_link_name = joint_info.second.child_link_name;
        auto axis_name = joint_info.second.datum_name;
        // This handles the case of a "cut" assembly, where we have an axis but we miss the child link.
        if (child_link_name.empty()) {
            continue;
        }

        auto joint_name = getRenameElementFromConfig(parent_link_name + "--" + child_link_name);
        auto root_H_parent_link = link_info_map.at(parent_link_name).root_H_link;
        auto root_H_child_link = link_info_map.at(child_link_name).root_H_link;
        auto child_model = link_info_map.at(child_link_name).modelhdl;
        auto parent_model = link_info_map.at(parent_link_name).modelhdl;
        auto parent_link_frame = link_info_map.at(parent_link_name).link_frame_name;

        //printToMessageWindow("Parent link H " + root_H_parent_link.toString());
        //printToMessageWindow("Child  link H " + root_H_child_link.toString());
        iDynTree::Transform parent_H_child = iDynTree::Transform::Identity();
        parent_H_child = root_H_parent_link.inverse() * root_H_child_link;

        if (joint_info.second.type == JointType::Revolute || joint_info.second.type == JointType::Linear) {

            iDynTree::Direction axis;
            std::tie(ret, axis) = getAxisFromPart(parent_model, axis_name, parent_link_frame, scale);

            if (!ret && warningsAreFatal)
            {
                return;
            }

            if (config["reverseRotationAxis"].IsDefined() && 
                config["reverseRotationAxis"].Scalar().find(joint_name) != std::string::npos)
            {
                axis = axis.reverse();
            }

            std::shared_ptr<iDynTree::IJoint> joint_sh_ptr;
            if (joint_info.second.type == JointType::Revolute) {
                joint_sh_ptr = std::make_shared<iDynTree::RevoluteJoint>();
                dynamic_cast<iDynTree::RevoluteJoint*>(joint_sh_ptr.get())->setAxis(iDynTree::Axis(axis, parent_H_child.getPosition()));
            }
            else if (joint_info.second.type == JointType::Linear) {
                joint_sh_ptr = std::make_shared<iDynTree::PrismaticJoint>();
                dynamic_cast<iDynTree::PrismaticJoint*>(joint_sh_ptr.get())->setAxis(iDynTree::Axis(axis, parent_H_child.getPosition()));
            }

            joint_sh_ptr->setRestTransform(parent_H_child);
            double conversion_factor = 1.0;
            if (joint_info.second.type == JointType::Revolute) {
                conversion_factor = deg2rad;
            }

            // Read limits from CSV data, until it is possible to do so from Creo directly
            if (joints_csv_table.GetRowIdx(joint_name) >= 0) {
                double min = joints_csv_table.GetCell<double>("lower_limit", joint_name) * conversion_factor;
                double max = joints_csv_table.GetCell<double>("upper_limit", joint_name) * conversion_factor;

                joint_sh_ptr->enablePosLimits(true);
                joint_sh_ptr->setPosLimits(0, min, max);
                // TODO we have to retrieve the rest transform from creo
                //joint.setRestTransform();

                min = joints_csv_table.GetCell<double>("damping", joint_name);
                max = joints_csv_table.GetCell<double>("friction", joint_name);
                joint_sh_ptr->setJointDynamicsType(iDynTree::URDFJointDynamics);
                joint_sh_ptr->setDamping(0, min);
                joint_sh_ptr->setStaticFriction(0, max);
            }

            if (idyn_model.addJoint(getRenameElementFromConfig(parent_link_name),
                getRenameElementFromConfig(child_link_name), joint_name, joint_sh_ptr.get()) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT " + joint_name, c2uLogLevel::WARN);
                if (warningsAreFatal) {
                    return;
                }
            }
        }
        else if (joint_info.second.type == JointType::Fixed) {
            iDynTree::FixedJoint joint(parent_H_child);
            if (idyn_model.addJoint(getRenameElementFromConfig(parent_link_name),
                getRenameElementFromConfig(child_link_name), joint_name, &joint) == iDynTree::JOINT_INVALID_INDEX) {
                printToMessageWindow("FAILED TO ADD JOINT " + joint_name, c2uLogLevel::WARN);
                if (warningsAreFatal) {
                    return;
                }
            }
        }
    }

    // Assign the transforms for the sensors
    sensorizer.assignTransformToSensors(exported_frame_info_map, link_info_map, scale);
    // Assign the transforms for the ft sensors
    sensorizer.assignTransformToFTSensor(exported_frame_info_map, link_info_map, joint_info_map, scale);

    // Let's add sensors and ft sensors frames

    for (auto& sensor : sensorizer.sensors) {
        if (sensor.exportFrameInURDF) {
            if (!idyn_model.addAdditionalFrameToLink(sensor.linkName, sensor.exportedFrameName,
                sensor.transform)) {
                printToMessageWindow("Failed to add additional frame  " + sensor.exportedFrameName, c2uLogLevel::WARN);
                continue;
            }
        }
    }

    for (auto& ftsensor : sensorizer.ft_sensors) {
        if (ftsensor.second.exportFrameInURDF) {
            auto joint_idx = idyn_model.getJointIndex(ftsensor.first);
            if (joint_idx == iDynTree::LINK_INVALID_INDEX) {
                // TODO FATAL?!
                printToMessageWindow("Failed to add additional frame, ftsensor: " + ftsensor.second.sensorName + " is not in the model", c2uLogLevel::WARN);
                continue;
            }

            auto joint = idyn_model.getJoint(joint_idx);
            auto link_name = idyn_model.getLinkName(joint->getFirstAttachedLink());

            if (!idyn_model.addAdditionalFrameToLink(link_name, ftsensor.second.exportedFrameName,
                ftsensor.second.parent_link_H_sensor)) {
                printToMessageWindow("Failed to add additional frame  " + ftsensor.second.exportedFrameName, c2uLogLevel::WARN);
                continue;
            }
        }
    }

    // Let's add all the exported frames
    for (auto & exported_frame_info : exported_frame_info_map) {
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

    iDynTree::ModelExporterOptions export_options;
    export_options.robotExportedName = config["robotName"].Scalar();

    if (config["root"].IsDefined())
        export_options.baseLink = config["root"].Scalar();
    else
        export_options.baseLink = "root_link";

    
    if (config["XMLBlobs"].IsDefined()) {
        export_options.xmlBlobs = config["XMLBlobs"].as<std::vector<std::string>>();
        // Adding gazebo pose as xml blob at the end of the urdf.
        std::string gazebo_pose_xml_str{""};
        gazebo_pose_xml_str = to_string(originXYZ[0]) + " " + to_string(originXYZ[1]) + " " + to_string(originXYZ[2]) + " " + to_string(originRPY[0]) + " " + to_string(originRPY[1]) + " " + to_string(originRPY[2]);
        gazebo_pose_xml_str = "<gazebo><pose>" + gazebo_pose_xml_str + "</pose></gazebo>";
        export_options.xmlBlobs.push_back(gazebo_pose_xml_str);
    }

    // Add FTs and other sensors as XML blobs for now

    std::vector<std::string> ft_xml_blobs = sensorizer.buildFTXMLBlobs();
    std::vector<std::string> sens_xml_blobs = sensorizer.buildSensorsXMLBlobs();

    export_options.xmlBlobs.insert(export_options.xmlBlobs.end(), ft_xml_blobs.begin(), ft_xml_blobs.end());
    export_options.xmlBlobs.insert(export_options.xmlBlobs.end(), sens_xml_blobs.begin(), sens_xml_blobs.end());

    exportModelToUrdf(idyn_model, export_options);

    // Let's clear the map in case of multiple click TODO UNIFY
    m_yaml_path.clear();
    m_csv_path.clear();
    m_output_path.clear();
    config = YAML::Node();
    m_asm_model_ptr = nullptr;

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

    if (!mdl_exporter.exportModelToFile(m_output_path+ "\\" + "model.urdf"))
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

    iDynTree::RotationalInertiaRaw idyn_inertia_tensor_csysPart_orientation = iDynTree::RotationalInertiaRaw::Zero();
    iDynTree::RotationalInertiaRaw idyn_inertia_tensor_link_orientation = iDynTree::RotationalInertiaRaw::Zero();

    bool assigned_inertia_flag = assigned_inertias_map.find(link_name) != assigned_inertias_map.end();
    for (int i_row = 0; i_row < idyn_inertia_tensor_csysPart_orientation.rows(); i_row++) {
        for (int j_col = 0; j_col < idyn_inertia_tensor_csysPart_orientation.cols(); j_col++) {
            if ((assigned_inertia_flag) && (i_row == j_col)) {
                // The assigned inertia is already expressed in the link frame
                idyn_inertia_tensor_link_orientation.setVal(i_row, j_col, assigned_inertias_map.at(link_name)[i_row]);
            }
            else {
                idyn_inertia_tensor_csysPart_orientation.setVal(i_row, j_col, inertia_tensor->get(i_row, j_col) * scale[i_row] * scale[j_col]);
            }
        }
    }

    iDynTree::Position com_child({ com->get(0) * scale[0] , com->get(1) * scale[1], com->get(2) * scale[2] });

    // Account for csysPart_H_link transformation
    // See https://github.com/icub-tech-iit/ergocub-software/issues/224#issuecomment-1985692598 for full contents

    // The COM returned by Creo's GetGravityCenter seems to be expressed in the root frame, so we need 
    // to transform it back to the link frame before passing it to iDynTree's fromRotationalInertiaWrtCenterOfMass
    com_child = H.inverse() * com_child;

    // The inertia returned by Creo's GetCenterGravityInertiaTensor seems to be expressed with the COM as the
    // point in which it is expressed, and with the orientation of the CSYS of the part, so we rotate it back with
    // the orientation of the link frame, unless an assignedInertia is used
    if (!assigned_inertia_flag) {
        // Note, this auto-defined methods are Eigen::Map, so they are reference to data that remains
        // stored in the original iDynTree object, see https://eigen.tuxfamily.org/dox/group__TutorialMapClass.html
        auto inertia_tensor_root = iDynTree::toEigen(idyn_inertia_tensor_csysPart_orientation);
        auto inertia_tensor_link = iDynTree::toEigen(idyn_inertia_tensor_link_orientation);
        auto csysPart_R_link = iDynTree::toEigen(H.getRotation());

        // See Equation 15 of https://ocw.mit.edu/courses/16-07-dynamics-fall-2009/dd277ec654440f4c2b5b07d6c286c3fd_MIT16_07F09_Lec26.pdf
        inertia_tensor_link = csysPart_R_link.transpose()*inertia_tensor_root*csysPart_R_link;
    }

    double mass{ 0.0 };
    if (config["assignedMasses"][link_name].IsDefined()) {
        mass = config["assignedMasses"][link_name].as<double>();
    }
    else {
        mass = mass_prop->GetMass();
    }
    iDynTree::SpatialInertia sp_inertia(mass, com_child, idyn_inertia_tensor_link_orientation);
    sp_inertia.fromRotationalInertiaWrtCenterOfMass(mass, com_child, idyn_inertia_tensor_link_orientation);

    return sp_inertia;
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
        // If true the exported_frame_info_map is not populated w/ the data from yaml
        if (exportAllUseradded) {
            if (csys_name.find("SCSYS") == std::string::npos ||
               (exported_frame_info_map.find(csys_name) != exported_frame_info_map.end())) {
                continue;
            }
            ExportedFrameInfo ef_info;
            ef_info.frameReferenceLink = getRenameElementFromConfig(link_name);
            ef_info.exportedFrameName = csys_name;
            exported_frame_info_map.insert(std::make_pair(csys_name, ef_info));
        }
        
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

void Creo2Urdf::readAssignedInertiasFromConfig() {
    if (!config["assignedInertias"].IsDefined()) {
        return;
    }
    for (const auto& ai : config["assignedInertias"]) {
        std::array<double, 3> assignedInertia { ai["xx"].as<double>(), ai["yy"].as<double>(), ai["zz"].as<double>()};
        assigned_inertias_map.insert(std::make_pair(ai["linkName"].Scalar(), assignedInertia));
    }
}

void Creo2Urdf::readAssignedCollisionGeometryFromConfig() {
    if (!config["assignedCollisionGeometry"].IsDefined()) {
        return;
    }
    for (const auto& cg : config["assignedCollisionGeometry"]) {
        CollisionGeometryInfo cgi;
        cgi.shape = stringToEnum<ShapeType>(shape_type_map, cg["geometricShape"]["shape"].Scalar());
        switch (cgi.shape)
        {
        case ShapeType::Box:
            cgi.size = cg["geometricShape"]["size"].as < array<double, 3>>();
            break;
        case ShapeType::Cylinder:
            cgi.radius = cg["geometricShape"]["radius"].as<double>();
            cgi.length = cg["geometricShape"]["lenght"].as<double>();
            break;
        case ShapeType::Sphere:
            cgi.radius = cg["geometricShape"]["radius"].as<double>();
            break;
        case ShapeType::None:
            break;
        default:
            break;
        }
        auto origin = cg["geometricShape"]["origin"].as < std::array<double, 6>>();
        cgi.link_H_geometry.setPosition({ origin[0], origin[1], origin[2] });
        cgi.link_H_geometry.setRotation(iDynTree::Rotation::RPY(origin[3], origin[4], origin[5]));
        assigned_collision_geometry_map.insert(std::make_pair(cg["linkName"].Scalar(), cgi));
    }
}

void Creo2Urdf::readExportedFramesFromConfig() {

    if (!config["exportedFrames"].IsDefined() || exportAllUseradded)
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

bool Creo2Urdf::addMeshAndExport(pfcModel_ptr component_handle, const std::string& mesh_transform)
{
    std::string file_extension = ".stl";
    std::string link_name = component_handle->GetFullName();
    std::string renamed_link_name = link_name;
 
    if (config["rename"][link_name].IsDefined())
    {
        renamed_link_name = config["rename"][link_name].Scalar();
    }


    if (config["stringToRemoveFromMeshFileName"].IsDefined())
    {
        link_name.erase(link_name.find(config["stringToRemoveFromMeshFileName"].Scalar()), 
            config["stringToRemoveFromMeshFileName"].Scalar().length());
    }
    std::string meshFormat = "stl_binary";
    if (config["meshFormat"].IsDefined()) {

        meshFormat = config["meshFormat"].Scalar();
        if (mesh_types_supported_extension_map.find(meshFormat) != mesh_types_supported_extension_map.end()) {
            file_extension = mesh_types_supported_extension_map.at(meshFormat);
        }
        else {
            printToMessageWindow("Mesh format " + meshFormat + " is not supported", c2uLogLevel::WARN);
            if (warningsAreFatal) {
                return false;
            }
        }
    }

    // Make all alphabetic characters lowercase
    if (config["forcelowercase"].IsDefined() && config["forcelowercase"].as<bool>())
    {
        std::transform(link_name.begin(), link_name.end(), link_name.begin(),
            [](unsigned char c) { return std::tolower(c); });
    }

    std::string mesh_file_name = m_output_path + "\\" + link_name;

    if (meshFormat != "step") {
        // We use ExportIntf3D for step format, applies the extension to the file name.
        mesh_file_name += file_extension;
    }
    try {
        if (meshFormat == "stl_binary") {
            component_handle->Export(mesh_file_name.c_str(), pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create(mesh_transform.c_str())));
        }
        else if(meshFormat =="stl_ascii") {
            component_handle->Export(mesh_file_name.c_str(), pfcExportInstructions::cast(pfcSTLASCIIExportInstructions().Create(mesh_transform.c_str())));
        }
        else if (meshFormat == "step") {
            component_handle->ExportIntf3D(mesh_file_name.c_str(), pfcExportType::pfcEXPORT_STEP);
        }
        else {
            return false;
        }

    }
    xcatchbegin
    xcatchcip(defaultEx)
    {
        printToMessageWindow(": exception caught: " + string(pfcXPFC::cast(defaultEx)->GetMessage()));
        return false;
    }
    xcatchend

    // Replace the first 5 bytes of the binary file with a string different than "solid"
    // to avoid issues with stl parsers.
    // For details see: https://github.com/icub-tech-iit/creo2urdf/issues/16
    if (meshFormat == "stl_binary") {
        sanitizeSTL(mesh_file_name);
    }

    // Lets add the mesh to the link
    iDynTree::ExternalMesh visualMesh;
    // Meshes are in millimeters, while iDynTree models are in meters
    visualMesh.setScale({scale});

    iDynTree::Vector4 color;
    iDynTree::Material material;

    if(config["assignedColors"][renamed_link_name].IsDefined())
    {
        for (size_t i = 0; i < config["assignedColors"][renamed_link_name].size(); i++)
            color(i) = config["assignedColors"][renamed_link_name][i].as<double>();
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
    string file_format = "%s";
    if (config["filenameformat"].IsDefined()) {
        file_format = config["filenameformat"].Scalar();
    }

    // We assume there is only one of occurrence to replace
    file_format.replace(file_format.find("%s"), file_format.length(), link_name);
    file_format += file_extension;

    visualMesh.setFilename(file_format);

    if (assigned_collision_geometry_map.find(renamed_link_name) != assigned_collision_geometry_map.end()) {
        auto geometry_info = assigned_collision_geometry_map.at(renamed_link_name);
        switch (geometry_info.shape)
        {
        case ShapeType::Box: {
            iDynTree::Box idyn_box;
            idyn_box.setX(geometry_info.size[0]); idyn_box.setY(geometry_info.size[1]); idyn_box.setZ(geometry_info.size[2]);
            idyn_box.setLink_H_geometry(geometry_info.link_H_geometry);
            idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_name)].push_back(idyn_box.clone());
        }
            break;
        case ShapeType::Cylinder: {
            iDynTree::Cylinder idyn_cylinder;
            idyn_cylinder.setLength(geometry_info.length);
            idyn_cylinder.setRadius(geometry_info.radius);
            idyn_cylinder.setLink_H_geometry(geometry_info.link_H_geometry);
            idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_name)].push_back(idyn_cylinder.clone());
        }
            break;
        case ShapeType::Sphere: {
            iDynTree::Sphere idyn_sphere;
            idyn_sphere.setRadius(geometry_info.radius);
            idyn_sphere.setLink_H_geometry(geometry_info.link_H_geometry);
            idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_name)].push_back(idyn_sphere.clone());
        }
            break;
        case ShapeType::None:
            break;
        default:
            break;
        }

    }
    else {
        idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_name)].push_back(visualMesh.clone());
    }
    idyn_model.visualSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(renamed_link_name)].push_back(visualMesh.clone());


    return true;
}

bool Creo2Urdf::loadYamlConfig(const std::string& filename)
{
    try 
    {
        auto config_temp = YAML::LoadFile(filename);
        auto folder_path = extractFolderPath(filename);
        if (config_temp["includes"].IsDefined() && config_temp["includes"].IsSequence()) {
            for (const auto& include : config_temp["includes"]) {
                auto include_filename = folder_path + include.as<std::string>();
                auto include_config = YAML::LoadFile(include_filename);
                mergeYAMLNodes(config, include_config);
            }
        }
        else
        {
            config = config_temp;
        }
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
