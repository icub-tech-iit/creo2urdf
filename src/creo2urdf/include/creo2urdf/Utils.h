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
#include <unordered_map>

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

/**
 * @brief Small positive value used for numerical precision comparisons.
 */
constexpr double epsilon = 1e-12;

/**
 * @brief Conversion factor from radians to degrees.
 */
constexpr double rad2deg = 180.0 / M_PI;

/**
 * @brief Conversion factor from degrees to radians.
 */
constexpr double deg2rad = 1 / rad2deg;

/**
 * @brief Standard gravitational acceleration in the z-direction.
 * 
 * The gravity_z constant represents the standard gravitational acceleration in the z-direction.
 * Its value is set to -9.81 m/s^2.
 */
constexpr double gravity_z = -9.81;

const std::unordered_map<std::string, std::string> mesh_types_supported_extension_map{{"stl_binary", ".stl"},
                                                                                      {"stl_ascii",  ".stl"},
                                                                                      {"step",       ".stp"}
};

 * @brief Enum representing the log levels of creo2urdf.
 * 
 * This enumeration defines different log levels that can be used to categorize log messages.
 * The levels range from the least severe (NONE) to the most severe (WARN).
 * The log levels need to match the ones defined in text/usascii/creo2urdf.txt.
 */
enum class c2uLogLevel
{
    NONE = 0,    ///< Messages that need no specific connotation.
    INFO,        ///< Informational messages that provide general information about creo2urdf behavior.
    WARN,        ///< Warning messages indicating potential issues or unexpected conditions.
    PROMPT       ///< Prompt messages that request user input
};

/**
 * @brief Map associating c2uLogLevel with corresponding string representations.
 * 
 * This static map is used to associate each c2uLogLevel enumeration value with its
 * corresponding string representation. The string representation matches the keys defined in
 * text/usascii/creo2urdf.txt. The prefixes %CI, %CW %CP define the type of message displayed, using a specific icon.
 */
static const std::map<c2uLogLevel, std::string> log_level_key = {
    {c2uLogLevel::NONE, "c2uNONE"},
    {c2uLogLevel::INFO, "c2uINFO"},
    {c2uLogLevel::WARN, "c2uWARN"},
    {c2uLogLevel::PROMPT, "c2uPROMPT"}
};

/**
 * @brief Enum representing types of sensors.
 * 
 * The SensorType enumeration defines different types of sensors that may be used in an application.
 * The None value is provided as a default or invalid option.
 */
enum class SensorType {
    None = -1,      ///< Default or invalid sensor type.
    Accelerometer,  ///< Accelerometer sensor type.
    Gyroscope,      ///< Gyroscope sensor type.
    Camera,         ///< Camera sensor type, usually RGB.
    Depth,          ///< Depth sensor type, usually associated with a RGBD camera.
    Ray             ///< Ray sensor type, such as LIDAR.
};

/**
 * @brief Enum representing simple shapes that can be associated to links.
 * 
 * The ShapeType enumeration defines different geometric shapes that may be used
 * as links to simplify collision evaluation.
 * The None value is provided as a default or invalid option.
 */
enum class ShapeType {
    Box,        ///< Box shape type.
    Cylinder,   ///< Cylinder shape type.
    Sphere,     ///< Sphere shape type.
    None        ///< Default or invalid shape type.
};

/**
 * @brief Mapping of SensorType to string representations for use within the URDF specification.
 */
static const std::map<SensorType, std::string> sensor_type_map = {
    {SensorType::Accelerometer, "accelerometer"},
    {SensorType::Gyroscope, "gyroscope"},
    {SensorType::Camera, "camera"},
    {SensorType::Depth, "depth"},
    {SensorType::Ray, "ray"}
};

/**
 * @brief Mapping of ShapeType to string representations for use within the URDF specification.
 */
static const std::map<ShapeType, std::string> shape_type_map = {
    { ShapeType::Box, "box" },
    { ShapeType::Cylinder, "cylinder"},
    { ShapeType::Sphere, "sphere"},
    { ShapeType::None, "empty"},
};

/**
 * @brief Mapping of SensorType to string representations for Gazebo URDF format.
 * 
 * The gazebo_sensor_type_map provides a mapping between SensorType enumeration values
 * and their corresponding string representations specifically tailored for Gazebo URDF format.
 */
static const std::map<SensorType, std::string> gazebo_sensor_type_map = {
    {SensorType::Accelerometer, "imu"},
    {SensorType::Gyroscope, "gyroscope"},
    {SensorType::Camera, "camera"},
    {SensorType::Depth, "depth"},
    {SensorType::Ray, "ray"}
};

/**
 * @brief Struct representing information about a sensor.
 * 
 * The SensorInfo struct encapsulates all the sensor information needed to work
 * when loading it from a URDF file.
 */
struct SensorInfo {
    std::string sensorName{ "" };               ///< Name of the sensor.
    std::string frameName{ "" };                ///< Name of the associated frame.
    std::string linkName{ "" };                 ///< Name of the link it is attached to.
    std::string exportedFrameName{ "" };        ///< Name of the exported frame.
    iDynTree::Transform transform{ iDynTree::Transform::Identity() };  ///< 3D transform associated with the sensor.
    bool exportFrameInURDF{ false };            ///< Flag indicating whether to export the frame in URDF.
    SensorType type{ SensorType::None };        ///< Type of the sensor.
    double updateRate{ 100 };                   ///< Update rate of the sensor.
    std::vector<std::string> xmlBlobs;          ///< Additional XML blobs that can be appended to the XML tree.
};

/**
 * @brief Information about a Force Torque sensor. Forces are measured in N, torques in N*m.
 */
struct FTSensorInfo {
    bool directionChildToParent{true}; ///< Flag indicating the direction from child to parent.
    std::string frame{"sensor"}; ///< Frame associated with the FT sensor.
    std::string sensorName{""}; ///< Name of the FT sensor.
    std::string frameName{""}; ///< Name of the frame.
    std::string linkName{" "}; ///< Name of the associated link.
    std::string exportedFrameName{""}; ///< Name of the exported frame.
    iDynTree::Transform parent_link_H_sensor{iDynTree::Transform::Identity()}; ///< 3D transform from parent link to sensor.
    iDynTree::Transform child_link_H_sensor{iDynTree::Transform::Identity()}; ///< 3D transform from child link to sensor.
    bool exportFrameInURDF{false}; ///< Flag indicating whether to export the frame in URDF.
    std::vector<std::string> xmlBlobs; ///< Vector of XML blobs that can be appended to the XML tree.
};

/**
 * @brief Information about an exported frame.
 */
struct ExportedFrameInfo {
    std::string frameReferenceLink{""}; ///< Link that the frame belongs to.
    std::string exportedFrameName{""}; ///< Name of the exported frame.
    iDynTree::Transform linkFrame_H_additionalFrame{iDynTree::Transform::Identity()}; ///< 3D transform from link frame to additional frame.
    iDynTree::Transform additionalTransformation{iDynTree::Transform::Identity()}; ///< Additional 3D transform.
};

/**
 * @brief Information about collision geometry. Useful to simplify the evaluation of collisions between meshes.
 */
struct CollisionGeometryInfo {
    ShapeType shape{ShapeType::None}; ///< Type of the collision shape.
    std::array<double, 3> size{1.0, 1.0, 1.0}; ///< Size of the collision geometry.
    double radius{1.0}; ///< Radius of the collision geometry (if applicable).
    double length{1.0}; ///< Length of the collision geometry (if applicable).
    iDynTree::Transform link_H_geometry{iDynTree::Transform::Identity()}; ///< 3D transform from link reference frame to simplified geometry.
};

/**
 * @brief Enumeration representing types of joints and their allowed motion.
 */
enum class JointType {
    Revolute,   ///< Revolute joint allows rotation around a single axis.
    Fixed,      ///< Fixed joint restricts all motion, providing no degree of freedom.
    Linear,     ///< UNAVAILABLE - Linear joint allows translational motion along a single axis.
    Spherical,  ///< UNAVAILABLE - Spherical joint allows rotation in all directions.
    None        ///< No specific joint type, used as a default or invalid option.
};

/**
 * @brief Information about a joint, including its type, limits, and dynamic parameters.
 */
struct JointInfo {
    std::string datum_name{""}; ///< Name of the joint's associated datum (axis for revolute, csys for fixed).
    std::string parent_link_name{""}; ///< Name of the parent link connected to the joint.
    std::string child_link_name{""}; ///< Name of the child link connected to the joint.
    JointType type{JointType::Revolute}; ///< Type of the joint (default is revolute).

    /**
     * @brief Limits for joint movement.
     */
    struct Limits {
        double min = 0.0; ///< Minimum allowed value for joint movement.
        double max = 360.0; ///< Maximum allowed value for joint movement.
    } limits;

    /**
     * @brief Dynamic parameters for the joint.
     */
    struct DynamicParams {
        double damping = 1.0; ///< Damping coefficient for the joint.
        double friction = 0.0; ///< Friction coefficient for the joint.
    } dynamics;
};

/**
 * @brief Information about a link, including its name, model handle, transformation, and frame name.
 */
struct LinkInfo {
    std::string name{""}; ///< Name of the link.
    pfcModel_ptr modelhdl{nullptr}; ///< Pointer to the Creo model associated with the link.
    iDynTree::Transform root_H_link{iDynTree::Transform::Identity()}; ///< 3D Transform from the root to the link's reference frame.
    std::string link_frame_name{""}; ///< Name of the link frame.
};

/**
 * @brief Utility class for redirecting to file the errors that iDynTree prints to stderr.
 * 
 * Restore of stderr to the original buffer is done on destruction.
 */
class iDynRedirectErrors {
public:
    /**
     * @brief Default constructor for iDynRedirectErrors.
     */
    iDynRedirectErrors() {
        old_buf = nullptr;
    }

    /**
     * @brief Destructor for iDynRedirectErrors.
     * 
     * Restores the standard error stream to its original buffer before the object is destroyed.
     */
    ~iDynRedirectErrors() {
        restoreBuffer();
    }

    /**
     * @brief Redirects standard error stream to a specified file.
     * 
     * @param old_buffer Pointer to the original stream buffer, which will be saved for restoration.
     * @param filename   The name of the file to which the stderr stream will be redirected.
     */
    void redirectBuffer(std::streambuf* old_buffer, const std::string& filename)
    {
        old_buf = old_buffer;
        idyn_out = std::ofstream(filename);
        std::cerr.rdbuf(idyn_out.rdbuf());
    }

    /**
     * @brief Restores the standard error stream to its original state.
     * 
     * If the standard error stream was redirected, this function restores it to its original buffer.
     * It also closes the file stream associated with the redirected stderr if it was open.
     */
    void restoreBuffer() {
        if (old_buf != nullptr) {
            std::cerr.rdbuf(old_buf);
        }

        if (idyn_out.is_open()) {
            idyn_out.close();
        }
    }

private:
    std::streambuf* old_buf;        ///< Pointer to the original stream buffer.
    std::ofstream idyn_out;         ///< File stream for redirecting stderr to a file.
};

/**
 * @brief Converts a string to an enum value using a mapping.
 * 
 * @tparam T          The type of the enum.
 * @param map        The map associating enumeration values with their string representations.
 * @param s          The string to be converted to enum value.
 * @return T         The enum value corresponding to the input string, or -1 if no match is found.
 */
template <class T>
T stringToEnum(const std::map<T, std::string> & map, const std::string & s)
{
    for (auto& t : map)
        if (t.second == s) return t.first;

    return static_cast<T>(-1);
}

/**
 * @brief Computes the unit vector of a Creo Axis. 
 * The axis is defined by start and end point, and the magnitude is normalized. 
 * 
 * @param axis_data The axis datum as a CurveDescriptor
 * @return std::array<double, 3> The resulting normalized unit vector
 */
std::array<double, 3> computeUnitVectorFromAxis(pfcCurveDescriptor_ptr axis_data);

/**
 * @brief Converts a 3D Creo transform into iDynTree transform. The position is defined as a 3-elements vector in m, 
 * and rotation is represented as a SO(3) matrix for both representations.
 * 
 * @param creo_trf The 3D transform to convert
 * @param scale The factor used to scale the position vector (e.g. from m to mm)
 * @return iDynTree::Transform 
 */
std::vector<string> getSolidDatumNames(pfcSolid_ptr solid, pfcModelItemType type);

/**
 * @brief Get the datums defined in a Solid depending on the selected type
 * 
 * @param solid The solid in which to query the datums
 * @param type The type of datums to extract as model items
 * @return std::vector<string> A vector of datum names
 */
iDynTree::Transform fromCreo(pfcTransform3D_ptr creo_trf, const array<double, 3>& scale = { 1.0,1.0,1.0 });

/**
 * @brief Prints a string to the message window on the bottom part of the Creo Parametric UI.
 * The message can have different log levels, represented by an icon on its left side.
 * The available log levels are defined in text/usascii/creo2urdf.txt. 
 * 
 * @param message The desired message to be printed
 * @param log_level The desired log level. Can be NONE, INFO, WARN, PROMPT. 
 * The PROMPT enum requires user input to proceed. The user input is not processed yet. 
 */
void printToMessageWindow(std::string message, c2uLogLevel log_level = c2uLogLevel::INFO);

/**
 * @brief Prints to the message window a Creo 3D transform in both origin and orientation 
 * using the iDynTree representation
 * 
 * @param m The Creo 3D transform to print
 */
void printTransformMatrix(pfcTransform3D_ptr m);

/**
 * @brief Prints to the message window a Creo 3D rotation matrix in SO(3)
 * 
 * @param m The Creo 3D transform to print
 */
void printRotationMatrix(pfcMatrix3D_ptr m);
/**
 * @brief Replaces the first 5 bytes of a binary STL file with the string "robot".
 * This is necessary to avoid accidental parsing of the file as ASCII.
 * For details, see https://github.com/icub-tech-iit/creo2urdf/issues/16
 * 
 * @param stl Path of the STL file to edit
 */
void sanitizeSTL(std::string stl);

/**
 * @brief Get the Transform From Root To Child object
 * 
 * @param comp_path 
 * @param modelhdl 
 * @param link_frame_name 
 * @param scale 
 * @return std::pair<bool, iDynTree::Transform> 
 */
std::pair<bool, iDynTree::Transform> getTransformFromRootToChild(pfcComponentPath_ptr comp_path, pfcModel_ptr modelhdl, const std::string& link_frame_name, const array<double, 3>& scale);

/**
 * @brief Get the Transform From Part object
 * 
 * @param modelhdl 
 * @param link_frame_name 
 * @param scale 
 * @return std::pair<bool, iDynTree::Transform> 
 */
std::pair<bool, iDynTree::Transform> getTransformFromPart(pfcModel_ptr modelhdl, const std::string& link_frame_name, const array<double, 3>& scale);

/**
 * @brief Get the Rotation Axis From Part object
 * 
 * @param modelhdl 
 * @param axis_name 
 * @param link_frame_name 
 * @param scale 
 * @return std::pair<bool, iDynTree::Direction> 
 */
std::pair<bool, iDynTree::Direction> getAxisFromPart(pfcModel_ptr modelhdl, const std::string& axis_name, const std::string& link_frame_name, const array<double, 3>& scale);

#endif // !UTILS_H
