/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef CREO2URDF_H
#define CREO2URDF_H

#include <creo2urdf/Utils.h>
#include <creo2urdf/Sensorizer.h>
#include <creo2urdf/ElementTreeManager.h>

#include <pfcShrinkwrap.h>
#include <pfcAssembly.h>

#include <iDynTree/ModelIO/ModelExporter.h>
#include <iDynTree/ModelIO/ModelLoader.h>
#include <iDynTree/KinDynComputations.h>
#include <iDynTree/Model/Traversal.h>

#include <rapidcsv.h>


/**
 * @class Creo2Urdf
 * @brief A class that exports a Creo mechanism assembly to URDF.
 *
 * This class inherits from pfcUICommandActionListener and provides functionality
 * to export a Creo model to URDF format, incorporating options and configurations.
 */
class Creo2Urdf : public pfcUICommandActionListener {
public:
    /**
     * @brief Callback function triggered when the button is clicked.
     * The function contains the main loop of the plugin.
     * 
     * TODO: Describe in detail the order of operations
     * 
     */
    void OnCommand() override;

private:
    /**
     * @brief Export the Creo model to URDF format.
     * @param mdl The iDynTree model to be exported.
     * @param options The exporter options for configuring the export process.
     * @return True if the export is successful, false otherwise.
     */
    bool exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options);

    /**
     * @brief Compute spatial inertia from Creo mass properties.
     * @param mass_prop The Creo mass properties.
     * @param H The transformation matrix.
     * @param link_name The name of the link.
     * @return The computed spatial inertia.
     */
    iDynTree::SpatialInertia computeSpatialInertiafromCreo(pfcMassProperty_ptr mass_prop, iDynTree::Transform H, const std::string& link_name);

    /**
     * @brief Populate the joint information map from the Creo model handle.
     * @param modelhdl The Creo model handle.
     */
    void populateJointInfoMap(pfcModel_ptr modelhdl);

    /**
     * @brief Populate the exported frame information map from the Creo model handle.
     * @param modelhdl The Creo model handle.
     */
    void populateExportedFrameInfoMap(pfcModel_ptr modelhdl);

    /**
     * @brief Read assigned inertias from the loaded YAML configuration.
     */
    void readAssignedInertiasFromConfig();

    /**
     * @brief Read assigned collision geometry from the loaded YAML configuration.
     */
    void readAssignedCollisionGeometryFromConfig();

    /**
     * @brief Read exported frames from the loaded YAML configuration.
     */
    void readExportedFramesFromConfig();

    /**
     * @brief Creates a mesh file from the Creo model in the form defined in the configuration file.
     * @param component_handle The part as a Creo model.
     * @param mesh_transform The 3D transform associated to the mesh.
     * @return True if successful, false otherwise.
     */
    bool addMeshAndExport(pfcModel_ptr component_handle, const std::string& mesh_transform);

    /**
     * @brief Load YAML configuration from a file.
     * @param filename The name of the YAML configuration file.
     * @return True if successful, false otherwise.
     */
    bool loadYamlConfig(const std::string& filename);

    /**
     * @brief Get the renamed element from the configuration.
     * @param elem_name The original element name.
     * @return The renamed element name.
     */
    std::string getRenameElementFromConfig(const std::string& elem_name);

    iDynTree::Model idyn_model; /**< The iDynTree model representing the mechanism tree. */
    std::map<std::string, JointInfo> joint_info_map; /**< Map storing information about joints. */
    std::map<std::string, LinkInfo> link_info_map; /**< Map storing information about links. */
    std::map<std::string, ExportedFrameInfo> exported_frame_info_map; /**< Map storing information about exported frames. */
    std::map<std::string, std::array<double,3>> assigned_inertias_map; /**< Map storing assigned inertias. 0 -> xx, 1 -> yy, 2 -> zz. */
    std::map<std::string, CollisionGeometryInfo> assigned_collision_geometry_map; /**< Map storing assigned collision geometries. */
    YAML::Node config; /**< YAML configuration node, storing the content of the configuration file. */
    bool exportAllUseradded{ false }; /**< Flag indicating whether to export all user-added frames. */
    
    std::array<double, 3> scale{ 1.0, 1.0, 1.0 }; /**< Scale factor for the exported model. Useful for converting between m and mm and viceversa. */
    std::array<double, 3> originXYZ {0.0, 0.0, 0.0}; /**< Origin offset in XYZ for the exported model. */
    std::array<double, 3> originRPY {0.0, 0.0, 0.0}; /**< Origin offset in Roll-Pitch-Yaw for the exported model. */
    bool warningsAreFatal{ true }; /**< Flag indicating whether warnings are treated as fatal errors. */
    std::string m_output_path{ "" }; /**< Output path for the exported URDF file. */
};

class Creo2UrdfAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // !CREO2URDF_H
