/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef CREO2URDF_H
#define CREO2URDF_H

#include <pfcGlobal.h>
#include <creo2urdf/Utils.h>
#include <yaml-cpp/yaml.h>

enum class JointType {
    Revolute,
    Fixed,
    Linear,
    Spherical,
    None
};

enum class SensorType {
    Accelerometer,
    Gyroscope,
    Camera,
    Depth,
    Ray,
    None
};

static const std::map<std::string, SensorType> sensor_type_map = {
    {"accelerometer", SensorType::Accelerometer},
    {"gyroscope", SensorType::Gyroscope},
    {"camera", SensorType::Camera},
    {"depth", SensorType::Depth},
    {"ray", SensorType::Ray}
};                       

struct JointInfo {
    std::string name{""};
    std::string parent_link_name{""};
    std::string child_link_name{""};
    JointType type {JointType::Revolute};
};

struct LinkInfo {
    std::string name{""};
    pfcModel_ptr modelhdl{ nullptr };
    iDynTree::Transform root_H_link { iDynTree::Transform::Identity() };
};

struct SensorInfo {
    std::string sensorName{""};
    std::string frameName{""};
    std::string linkName{""};
    bool exportFrameInURDF{false};
    SensorType type{SensorType::None};
    double updateRate{100};
    std::vector<std::string> xmlBlobs;
};

struct FTSensorInfo {
    std::string jointName{""};
    bool directionChildToParent{ true };
    std::string frame{"sensor"};
    std::string frameName{""};
    std::vector<std::string> xmlBlobs;
};

class Creo2Urdf : public pfcUICommandActionListener {
public:
    void OnCommand() override;

    bool exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options);
    void populateJointInfoMap(pfcModel_ptr modelhdl);
    void readSensorsFromConfig();
    void readFTSensorsFromConfig();
    bool addMeshAndExport(pfcModel_ptr component_handle, const std::string& stl_transform);
    bool loadYamlConfig(const std::string& filename);
    std::string renameElementFromConfig(const std::string& elem_name);

private:
    iDynTree::Model idyn_model;
    std::map<std::string, JointInfo> joint_info_map;
    std::map<std::string, LinkInfo> link_info_map;
    YAML::Node config;
    std::vector<SensorInfo> sensors;
    std::vector<FTSensorInfo> ft_sensors;
};

class Creo2UrdfAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // !CREO2URDF_H
