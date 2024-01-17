/**
 * @file Sensorizer.cpp
 * @brief Contains definitions for the Sensorizer class.
 * @copyright (C) 2006-2024 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Sensorizer.h>

void Sensorizer::readSensorsFromConfig(const YAML::Node & config)
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

        std::string sensor_name = s["linkName"].Scalar() + "_" + s["frameName"].Scalar();
        if (s["sensorName"].IsDefined()) {
            sensor_name = s["sensorName"].Scalar();
        }
        std::string exported_frame_name = sensor_name;
        if (s["exportedFrameName"].IsDefined()) {
            exported_frame_name = s["exportedFrameName"].Scalar();
        }

        try
        {
            sensors.push_back({ sensor_name,
                                s["frameName"].Scalar(),
                                s["linkName"].Scalar(),
                                exported_frame_name,
                                iDynTree::Transform::Identity(),
                                export_frame,
                                stringToEnum<SensorType>(sensor_type_map, s["sensorType"].Scalar()),
                                update_rate,
                                s["sensorBlobs"].as<std::vector<std::string>>() });
        }
        catch (YAML::Exception& e)
        {
            printToMessageWindow(e.msg, c2uLogLevel::WARN);
        }

    }
}

void Sensorizer::readFTSensorsFromConfig(const YAML::Node& config)
{
    if (config["forceTorqueSensors"].IsDefined())
    {
        for (const auto& s : config["forceTorqueSensors"])
        {
            bool export_frame = false;

            if (s["exportFrameInURDF"].IsDefined())
            {
                export_frame = s["exportFrameInURDF"].as<bool>();
            }
            std::string sensor_name = s["jointName"].Scalar();
            if (s["sensorName"].IsDefined()) {
                sensor_name = s["sensorName"].Scalar();
            }

            std::string exported_frame_name = sensor_name;
            if (s["exportedFrameName"].IsDefined()) {
                exported_frame_name = s["exportedFrameName"].Scalar();
            }
            ft_sensors.insert(
                {
                    s["jointName"].Scalar(),
                    {
                        s["directionChildToParent"].as<bool>(),
                        s["frame"].Scalar(),
                        sensor_name,
                        s["frameName"].Scalar(),
                        s["linkName"].Scalar(),
                        exported_frame_name,
                        iDynTree::Transform::Identity(),
                        iDynTree::Transform::Identity(),
                        export_frame,
                        s["sensorBlobs"].as<std::vector<std::string>>()
                    }
                });
        }
    }

}

void Sensorizer::assignTransformToFTSensor(const std::map<std::string, LinkInfo>& link_info_map, const std::map<std::string, JointInfo>& joint_info_map, const std::array<double, 3> scale)
{
    // Iterate over all sensors
    for (auto& f : ft_sensors)
    {   
        JointInfo j_info = joint_info_map.at(f.second.frameName);

        LinkInfo parent_l_info = link_info_map.at(j_info.parent_link_name);
        LinkInfo child_l_info = link_info_map.at(j_info.child_link_name);

        auto parent_csys_H_sensor      = (getTransformFromPart(parent_l_info.modelhdl, f.second.frameName, scale)).second;
        auto parent_csys_H_parent_link = (getTransformFromPart(parent_l_info.modelhdl, parent_l_info.link_frame_name, scale)).second;
        // This transform is used for exporting the ft frame
        f.second.parent_link_H_sensor = parent_csys_H_parent_link.inverse() * parent_csys_H_sensor;

        auto child_csys_H_sensor      = (getTransformFromPart(child_l_info.modelhdl, f.second.frameName, scale)).second;
        auto child_csys_H_child_link  = (getTransformFromPart(child_l_info.modelhdl, child_l_info.link_frame_name, scale)).second;
        // This transform is used for defining the pose of the ft sensor
        f.second.child_link_H_sensor = child_csys_H_child_link.inverse() * child_csys_H_sensor;
    }
}

std::vector<std::string> Sensorizer::buildFTXMLBlobs()
{
    std::vector<std::string> ft_xml_blobs;

    for (const auto& ft : ft_sensors)
    {
        xmlKeepBlanksDefault(0);
        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, node = NULL;
        doc = xmlNewDoc(NULL);
        root_node = xmlNewNode(NULL, BAD_CAST "gazebo");

        xmlDocSetRootElement(doc, root_node);

        xmlNewProp(root_node, BAD_CAST "reference", BAD_CAST ft.first.c_str());

        node = xmlNewChild(root_node, NULL, BAD_CAST "sensor", NULL);
        xmlNewProp(node, BAD_CAST "name", BAD_CAST ft.second.sensorName.c_str());
        xmlNewProp(node, BAD_CAST "type", BAD_CAST "force_torque");

        auto node1 = xmlNewChild(node, NULL, BAD_CAST "always_on", BAD_CAST "1");
        node1 = xmlNewChild(node, NULL, BAD_CAST "update_rate", BAD_CAST "100");
        node1 = xmlNewChild(node, NULL, BAD_CAST "force_torque", NULL);

        auto node2 = xmlNewChild(node1, NULL, BAD_CAST "frame", BAD_CAST ft.second.frame.c_str());

        auto& trf = ft.second.child_link_H_sensor;

        if (ft.second.directionChildToParent)
        {
            node2 = xmlNewChild(node1, NULL, BAD_CAST "measure_direction", BAD_CAST "child_to_parent");
        }
        else
        {
            node2 = xmlNewChild(node1, NULL, BAD_CAST "measure_direction", BAD_CAST "parent_to_child");
        }

        std::string pose_xyz_rpy = trf.getPosition().toString() + " " + trf.getRotation().asRPY().toString();
        node1 = xmlNewChild(node, NULL, BAD_CAST "pose", BAD_CAST pose_xyz_rpy.c_str());

        for (auto & blob : ft.second.xmlBlobs)
        {
            xmlNodePtr node_xmlblob = nullptr;

            xmlParseInNodeContext(node, blob.c_str(), blob.size(), 0, &node_xmlblob);

            if (node_xmlblob)
                xmlAddChild(node, node_xmlblob);
        }

        xmlOutputBufferPtr gazebo_doc_buffer = xmlAllocOutputBuffer(NULL);
        xmlNodeDumpOutput(gazebo_doc_buffer, doc, root_node, 0, 1, NULL);
        ft_xml_blobs.push_back(string((char*)xmlBufContent(gazebo_doc_buffer->buffer)));

        xmlOutputBufferClose(gazebo_doc_buffer);

        xmlFreeDoc(doc);

        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "sensor");

        xmlDocSetRootElement(doc, root_node);

        xmlNewProp(root_node, BAD_CAST "name", BAD_CAST ft.second.sensorName.c_str());
        xmlNewProp(root_node, BAD_CAST "type", BAD_CAST "force_torque");
        node = xmlNewChild(root_node, NULL, BAD_CAST "parent", NULL);
        xmlNewProp(node, BAD_CAST "joint", BAD_CAST ft.first.c_str());

        node = xmlNewChild(root_node, NULL, BAD_CAST "force_torque", NULL);
        xmlNewChild(node, NULL, BAD_CAST "frame", BAD_CAST ft.second.frame.c_str());
        if (ft.second.directionChildToParent)
        {
            xmlNewChild(node, NULL, BAD_CAST "measure_direction", BAD_CAST "child_to_parent");
        }
        else
        {
            xmlNewChild(node, NULL, BAD_CAST "measure_direction", BAD_CAST "parent_to_child");
        }
        node = xmlNewChild(root_node, NULL, BAD_CAST "origin", NULL);
        xmlNewProp(node, BAD_CAST "rpy", BAD_CAST trf.getRotation().asRPY().toString().c_str());
        xmlNewProp(node, BAD_CAST "xyz", BAD_CAST trf.getPosition().toString().c_str());

        xmlOutputBufferPtr sensor_doc_buffer = xmlAllocOutputBuffer(NULL);
        xmlNodeDumpOutput(sensor_doc_buffer, doc, root_node, 0, 1, NULL);
        ft_xml_blobs.push_back(string((char*)xmlBufContent(sensor_doc_buffer->buffer)));

        xmlOutputBufferClose(sensor_doc_buffer);

        xmlFreeDoc(doc);
    }

    return ft_xml_blobs;
}

void Sensorizer::assignTransformToSensors(const std::map<std::string, ExportedFrameInfo>& exported_frame_info_map)
{
    for (auto& s : sensors)
    {

        if (exported_frame_info_map.find(s.frameName) != exported_frame_info_map.end())
        {
            s.transform = exported_frame_info_map.at(s.frameName).linkFrame_H_additionalFrame;
        }
        else
        {
            printToMessageWindow(s.sensorName + ": " + s.frameName + " was not found", c2uLogLevel::WARN);
        }
    }
}

std::vector<std::string> Sensorizer::buildSensorsXMLBlobs()
{
    std::vector<std::string> xml_blobs;

    for (const auto& s : sensors)
    {
        xmlKeepBlanksDefault(0);
        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, node = NULL;
        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "gazebo");

        xmlDocSetRootElement(doc, root_node);

        xmlNewProp(root_node, BAD_CAST "reference", BAD_CAST s.linkName.c_str());

        node = xmlNewChild(root_node, NULL, BAD_CAST "sensor", NULL);
        xmlNewProp(node, BAD_CAST "name", BAD_CAST s.sensorName.c_str());

        xmlNewProp(node, BAD_CAST "type", BAD_CAST gazebo_sensor_type_map.at(s.type).c_str());

        xmlNewChild(node, NULL, BAD_CAST "always_on", BAD_CAST "1");
        xmlNewChild(node, NULL, BAD_CAST "update_rate", BAD_CAST to_string(s.updateRate).c_str());

        iDynTree::Transform trf = s.transform;

        string pose = trf.getPosition().toString() + " " + trf.getRotation().asRPY().toString();

        xmlNewChild(node, NULL, BAD_CAST "pose", BAD_CAST pose.c_str());

        for (auto & blob : s.xmlBlobs)
        {
            xmlNodePtr node_xmlblob = nullptr;

            xmlParseInNodeContext(node, blob.c_str(), blob.size(), 0, &node_xmlblob);

            if (node_xmlblob)
                xmlAddChild(node, node_xmlblob);
        }

        xmlOutputBufferPtr doc_buffer = xmlAllocOutputBuffer(NULL);
        xmlNodeDumpOutput(doc_buffer, doc, root_node, 0, 1, NULL);
        xml_blobs.push_back(string((char*)xmlBufContent(doc_buffer->buffer)));

        xmlOutputBufferClose(doc_buffer);
        xmlFreeDoc(doc);

        doc = xmlNewDoc(BAD_CAST "1.0");
        root_node = xmlNewNode(NULL, BAD_CAST "sensor");
        xmlDocSetRootElement(doc, root_node);

        xmlNewProp(root_node, BAD_CAST "name", BAD_CAST s.sensorName.c_str());
        xmlNewProp(root_node, BAD_CAST "type", BAD_CAST sensor_type_map.at(s.type).c_str());
        node = xmlNewChild(root_node, NULL, BAD_CAST "parent", NULL);
        xmlNewProp(node, BAD_CAST "link", BAD_CAST s.linkName.c_str());
        node = xmlNewChild(root_node, NULL, BAD_CAST "origin", NULL);
        xmlNewProp(node, BAD_CAST "rpy", BAD_CAST trf.getRotation().asRPY().toString().c_str());
        xmlNewProp(node, BAD_CAST "xyz", BAD_CAST trf.getPosition().toString().c_str());
        doc_buffer = xmlAllocOutputBuffer(NULL);
        xmlNodeDumpOutput(doc_buffer, doc, root_node, 0, 1, NULL);
        xml_blobs.push_back(string((char*)xmlBufContent(doc_buffer->buffer)));

        xmlOutputBufferClose(doc_buffer);
        xmlFreeDoc(doc);
    }

    return xml_blobs;
}