/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef SENSORIZER_H
#define SENSORIZER_H

#include <creo2urdf/Utils.h>

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml/xmlwriter.h>

#include <yaml-cpp/yaml.h>

/**
 * @brief Represents a Sensorizer with functions to read and assign sensor information.
 */
struct Sensorizer {

    /**
     * @brief Reads force/torque sensors configuration from a YAML node.
     * @param config The YAML node containing sensor configuration.
     */
    void readFTSensorsFromConfig(const YAML::Node& config);

    /**
     * @brief Reads general sensors configuration from a YAML node.
     * @param config The YAML node containing sensor configuration.
     */
    void readSensorsFromConfig(const YAML::Node& config);

    /**
     * @brief Assigns a 3D transform to a force/torque sensor based on provided information.
     * @param link_info_map A map of link information.
     * @param joint_info_map A map of joint information.
     * @param scale The scale for the position part of the 3D transform.
     */
    void assignTransformToFTSensor(const std::map<std::string, LinkInfo>& link_info_map,
                                   const std::map<std::string, JointInfo>& joint_info_map,
                                   const std::array<double, 3> scale);

    /**
     * @brief Assigns a 3D transform to all sensors based on provided information.
     * @param exported_frame_info_map A map of exported frame information.
     */
    void assignTransformToSensors(const std::map<std::string, ExportedFrameInfo>& exported_frame_info_map);

    /**
     * @brief Builds a vector of XML trees as strings for force/torque sensors, 
	 * starting from the information retrieved in the YAML. The XML trees are returned in this
	 * way so that they can be added as blobs using iDynTree.
	 * 
     * @return A vector of strings representing the XML blobs.
     */
    std::vector<std::string> buildFTXMLBlobs();

    /**
     * @brief Builds a vector of XML trees as strings for general sensors, 
	 * starting from the information retrieved in the YAML. The XML trees are returned in this
	 * way so that they can be added as blobs using iDynTree.
	 * 
     * @return A vector of strings representing the XML blobs.
     */
    std::vector<std::string> buildSensorsXMLBlobs();

    /**
     * @brief Map containing information about force/torque sensors.
     */
    std::map<std::string, FTSensorInfo> ft_sensors;

    /**
     * @brief Vector containing information about general sensors.
     */
    std::vector<SensorInfo> sensors;
};


#endif // !SENSORIZER_H
