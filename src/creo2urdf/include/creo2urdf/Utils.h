/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <string>
#include <array>
#include <map>

#include <pfcGlobal.h>
#include <pfcModel.h>
#include <pfcSolid.h>
#include <pfcShrinkwrap.h>
#include <pfcAssembly.h>
#include <pfcComponentFeat.h>

#include <wfcGeometry.h>
#include <wfcFeature.h>
#include <wfcModelItem.h>
#include <wfcModel.h>
#include <wfcGlobal.h>
#include <wfcElemIds.h>

#include <ProAsmcomp.h>

#include <iDynTree/Model/Model.h>
#include <iDynTree/ModelIO/ModelExporter.h>
#include <iDynTree/ModelIO/ModelLoader.h>
#include <iDynTree/Model/RevoluteJoint.h>
#include <iDynTree/Model/FixedJoint.h>
#include <iDynTree/KinDynComputations.h>
#include <iDynTree/Model/Traversal.h>

#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <libxml/xmlwriter.h>

#include <yaml-cpp/yaml.h>

constexpr double epsilon = 1e-12;
constexpr double rad2deg = 180.0 / M_PI;
constexpr double deg2rad = 1 / rad2deg;
constexpr double gravity_z = -9.81;

enum class c2uLogLevel
{
    NONE = 0,
    INFO,
    WARN,
    PROMPT
};

static const std::map<c2uLogLevel, std::string> log_level_key = {
    {c2uLogLevel::NONE, "c2uNONE"},
    {c2uLogLevel::INFO, "c2uINFO"},
    {c2uLogLevel::WARN, "c2uWARN"},
    {c2uLogLevel::PROMPT, "c2uPROMPT"}
};

enum class SensorType {
    None = -1,
    Accelerometer,
    Gyroscope,
    Camera,
    Depth,
    Ray,
};

enum class ShapeType {
    Box,
    Cylinder,
    Sphere,
    None
};

static const std::map<SensorType, std::string> sensor_type_map = {
    {SensorType::Accelerometer, "accelerometer"},
    {SensorType::Gyroscope, "gyroscope"},
    {SensorType::Camera, "camera"},
    {SensorType::Depth, "depth"},
    {SensorType::Ray, "ray"}
};

static const std::map<ShapeType, std::string> shape_type_map = {
    { ShapeType::Box, "box" },
    { ShapeType::Cylinder, "cylinder"},
    { ShapeType::Sphere, "sphere"},
    { ShapeType::None, "empty"},
};

static const std::map<SensorType, std::string> gazebo_sensor_type_map = {
    {SensorType::Accelerometer, "imu"},
    {SensorType::Gyroscope, "gyroscope"},
    {SensorType::Camera, "camera"},
    {SensorType::Depth, "depth"},
    {SensorType::Ray, "ray"}
};

struct SensorInfo {
    std::string sensorName{ "" };
    std::string frameName{ "" };
    std::string linkName{ "" };
    std::string exportedFrameName{ "" };
    iDynTree::Transform transform{ iDynTree::Transform::Identity() };
    bool exportFrameInURDF{ false };
    SensorType type{ SensorType::None };
    double updateRate{ 100 };
    std::vector<std::string> xmlBlobs;
};

struct FTSensorInfo {
    bool directionChildToParent{ true };
    std::string frame{ "sensor" };
    std::string sensorName{ "" };
    std::string frameName{ "" };
    std::string linkName { " " };
    std::string exportedFrameName{ "" };
    iDynTree::Transform parent_link_H_sensor{ iDynTree::Transform::Identity() };
    iDynTree::Transform child_link_H_sensor{ iDynTree::Transform::Identity() };
    bool exportFrameInURDF{ false };
    std::vector<std::string> xmlBlobs;
};

struct ExportedFrameInfo {
    std::string frameReferenceLink{ "" };
    std::string exportedFrameName{ "" };
    iDynTree::Transform linkFrame_H_additionalFrame{ iDynTree::Transform::Identity() };
    iDynTree::Transform additionalTransformation{ iDynTree::Transform::Identity() }; // additionalFrameOld_H_additionalFrame 
};

struct CollisionGeometryInfo {
    ShapeType shape{ ShapeType::None };
    std::array<double, 3> size {1.0, 1.0, 1.0};
    double radius{ 1.0 };
    double length{ 1.0 };
    iDynTree::Transform link_H_geometry{iDynTree::Transform::Identity()};
};


enum class JointType {
    Revolute,
    Fixed,
    Linear,
    Spherical,
    None
};

struct JointInfo {
    std::string datum_name{""}; // datum name can be the axis name for the revolute , and the csys for the fixed
    std::string parent_link_name{""};
    std::string child_link_name{""};
    JointType type{ JointType::Revolute };
    struct Limits {
        double min = 0.0;
        double max = 360.0;
    };
    Limits limits;
    struct DynamicParams
    {
        double damping = 1.0;
        double friction = 0.0;
    };
    DynamicParams dynamics;
};

struct LinkInfo {
    std::string name{ "" };
    pfcModel_ptr modelhdl{ nullptr };
    iDynTree::Transform root_H_link{ iDynTree::Transform::Identity() };
    std::string link_frame_name{ "" };
};


class iDynRedirectErrors {
public:

    iDynRedirectErrors() {
        old_buf = nullptr;

    }

    ~iDynRedirectErrors() {
        restoreBuffer();
    }

    void redirectBuffer(std::streambuf* old_buffer, const std::string& filename)
    {
        old_buf = old_buffer;
        idyn_out = std::ofstream(filename);
        std::cerr.rdbuf(idyn_out.rdbuf());
    }

    void restoreBuffer() {
        if (old_buf != nullptr) {
            std::cerr.rdbuf(old_buf);
        }

        if (idyn_out.is_open()) {
            idyn_out.close();
        }
    }

private:
    std::streambuf* old_buf;
    std::ofstream idyn_out;
};


template <class T>
T stringToEnum(const std::map<T, std::string> & map, const std::string & s)
{
    for (auto& t : map)
        if (t.second == s) return t.first;

    return static_cast<T>(-1);
}

std::array<double, 3> computeUnitVectorFromAxis(pfcCurveDescriptor_ptr axis_data);

iDynTree::Transform fromCreo(pfcTransform3D_ptr creo_trf, const array<double, 3>& scale = { 1.0,1.0,1.0 });

void printToMessageWindow(std::string message, c2uLogLevel log_level = c2uLogLevel::INFO);

void printTransformMatrix(pfcTransform3D_ptr m);

void printRotationMatrix(pfcMatrix3D_ptr m);

void sanitizeSTL(std::string stl);

std::pair<bool, iDynTree::Transform> getTransformFromRootToChild(pfcComponentPath_ptr comp_path, pfcModel_ptr modelhdl, const std::string& link_frame_name, const array<double, 3>& scale);

std::pair<bool, iDynTree::Transform> getTransformFromPart(pfcModel_ptr modelhdl, const std::string& link_frame_name, const array<double, 3>& scale);

std::pair<bool, iDynTree::Direction> getRotationAxisFromPart(pfcModel_ptr modelhdl, const std::string& axis_name, const std::string& link_frame_name, const array<double, 3>& scale);

#endif // !UTILS_H
