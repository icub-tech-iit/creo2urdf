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

struct Sensorizer {

    void readFTSensorsFromConfig(const YAML::Node& config);
	void readSensorsFromConfig(const YAML::Node& config);
	void assignTransformToFTSensor(const std::map<std::string, LinkInfo>& link_info_map, const std::array<double, 3> scale);
	void assignTransformToSensors(const std::map<std::string, ExportedFrameInfo>& exported_frame_info_map);
	std::vector<std::string> buildFTXMLBlobs();
	std::vector<std::string> buildSensorsXMLBlobs();

	std::map<std::string, FTSensorInfo> ft_sensors;
	std::vector<SensorInfo> sensors;
};

#endif // !SENSORIZER_H
