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

#include <rapidcsv.h>

enum class JointType {
    Revolute,
    Fixed,
    Linear,
    Spherical,
    None
};

enum class SensorType {
    None = -1,
    Accelerometer,
    Gyroscope,
    Camera,
    Depth,
    Ray,
};

static const std::map<SensorType, std::string> sensor_type_map = {
    {SensorType::Accelerometer, "accelerometer"},
    {SensorType::Gyroscope, "gyroscope"},
    {SensorType::Camera, "camera"},
    {SensorType::Depth, "depth"},
    {SensorType::Ray, "ray"}
};                       

static const std::map<SensorType, std::string> gazebo_sensor_type_map = {
    {SensorType::Accelerometer, "imu"},
    {SensorType::Gyroscope, "gyroscope"},
    {SensorType::Camera, "camera"},
    {SensorType::Depth, "depth"},
    {SensorType::Ray, "ray"}
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
    std::string link_frame_name {""};
};

struct ExportedFrameInfo {
    std::string frameReferenceLink {""};
    std::string exportedFrameName {""};
    iDynTree::Transform linkFrame_H_additionalFrame { iDynTree::Transform::Identity() };
    iDynTree::Transform additionalTransformation { iDynTree::Transform::Identity() }; // additionalFrameOld_H_additionalFrame 
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

private:

    bool exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options);
    iDynTree::SpatialInertia computeSpatialInertiafromCreo(pfcMassProperty_ptr mass_prop, iDynTree::Transform H, const std::string& link_name);
    void populateJointInfoMap(pfcModel_ptr modelhdl);
    void populateExportedFrameInfoMap(pfcModel_ptr modelhdl);
    void populateFTMap(pfcModel_ptr modelhdl);
    void readAssignedInertiasFromConfig();
    void readExportedFramesFromConfig();
    void readSensorsFromConfig();
    void readFTSensorsFromConfig();
    std::vector<std::string> buildFTXMLBlobs();
    std::vector<std::string> buildSensorsXMLBlobs();
    bool addMeshAndExport(pfcModel_ptr component_handle, const std::string& stl_transform);
    bool loadYamlConfig(const std::string& filename);
    std::string getRenameElementFromConfig(const std::string& elem_name);
    std::pair<double, double> getLimitsFromElemTree(pfcFeature_ptr feat);

    iDynTree::Model idyn_model;
    std::map<std::string, JointInfo> joint_info_map;
    std::map<std::string, LinkInfo> link_info_map;
    std::map<std::string, ExportedFrameInfo> exported_frame_info_map;
    std::map<std::string, std::array<double,3>> assigned_inertias_map; // 0 -> xx, 1 -> yy, 2 -> zz
    std::map<std::string, iDynTree::Transform> forcetorque_transform_map;
    YAML::Node config;
    std::vector<SensorInfo> sensors;
    std::vector<FTSensorInfo> ft_sensors;
    array<double, 3> scale{ 1.0, 1.0, 1.0 };
    array<double, 3> originXYZ {0.0, 0.0, 0.0};
    array<double, 3> originRPY {0.0, 0.0, 0.0};
};

class Creo2UrdfAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // !CREO2URDF_H
