/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
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

        try
        {
            sensors.push_back({ s["sensorName"].Scalar(),
                                s["frameName"].Scalar(),
                                s["linkName"].Scalar(),
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
            ft_sensors.insert(
                {
                    s["jointName"].Scalar(),
                    {
                        s["directionChildToParent"].as<bool>(),
                        s["frame"].Scalar(), 
                        s["frameName"].Scalar(),
                        iDynTree::Transform::Identity(),
                        s["sensorBlobs"].as<std::vector<std::string>>()
                    }
                });
        }
    }

}

void Sensorizer::assignTransformToFTSensor(pfcModel_ptr modelhdl, const iDynTree::Transform & link_transform, const std::array<double, 3> scale)
{
    // Iterate over all sensors
    for (auto& f : ft_sensors)
    {
        auto trf = getTransformFromPart(modelhdl, f.second.frameName, scale);

        // if the part contains the FT frame use it to assign the transform
        if (trf.first)
        {
            f.second.transform = link_transform.inverse() * trf.second;
            break;
        }
    }
}

std::vector<std::string> Sensorizer::buildFTXMLBlobs()
{
    std::vector<std::string> ft_xml_blobs;

    for (const auto& ft : ft_sensors)
    {
        xmlDocPtr doc = NULL;
        xmlNodePtr root_node = NULL, node = NULL;
        doc = xmlNewDoc(NULL);
        root_node = xmlNewNode(NULL, BAD_CAST "gazebo");

        xmlDocSetRootElement(doc, root_node);

        xmlNewProp(root_node, BAD_CAST "reference", BAD_CAST ft.first.c_str());

        node = xmlNewChild(root_node, NULL, BAD_CAST "sensor", NULL);
        xmlNewProp(node, BAD_CAST "name", BAD_CAST ft.first.c_str());
        xmlNewProp(node, BAD_CAST "type", BAD_CAST "force_torque");

        auto node1 = xmlNewChild(node, NULL, BAD_CAST "always_on", BAD_CAST "1");
        node1 = xmlNewChild(node, NULL, BAD_CAST "update_rate", BAD_CAST "100");
        node1 = xmlNewChild(node, NULL, BAD_CAST "force_torque", NULL);

        auto node2 = xmlNewChild(node1, NULL, BAD_CAST "frame", BAD_CAST ft.second.frame.c_str());

        auto& trf = ft.second.transform;

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

        for (auto blob : ft.second.xmlBlobs)
        {
            blob.erase(std::remove_if(blob.begin(), blob.end(),
                [](unsigned char c) {
                    return !std::isprint(c);
                }),
                blob.end());

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

        xmlNewProp(root_node, BAD_CAST "name", BAD_CAST ft.first.c_str());
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

        for (auto blob : s.xmlBlobs)
        {
            blob.erase(std::remove_if(blob.begin(), blob.end(),
                [](unsigned char c) {
                    return !std::isprint(c);
                }),
                blob.end());

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