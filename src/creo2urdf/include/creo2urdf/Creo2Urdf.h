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


class Creo2Urdf : public pfcUICommandActionListener {
public:
    void OnCommand() override;

private:

    bool exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options);
    iDynTree::SpatialInertia computeSpatialInertiafromCreo(pfcMassProperty_ptr mass_prop, iDynTree::Transform H, const std::string& link_name);
    void populateJointInfoMap(pfcModel_ptr modelhdl);
    void populateExportedFrameInfoMap(pfcModel_ptr modelhdl);
    void readAssignedInertiasFromConfig();
    void readAssignedCollisionGeometryFromConfig();
    void readExportedFramesFromConfig();
    bool addMeshAndExport(pfcModel_ptr component_handle, const std::string& mesh_transform);
    bool loadYamlConfig(const std::string& filename);
    std::string getRenameElementFromConfig(const std::string& elem_name);

    iDynTree::Model idyn_model;
    std::map<std::string, JointInfo> joint_info_map;
    std::map<std::string, LinkInfo> link_info_map;
    std::map<std::string, ExportedFrameInfo> exported_frame_info_map;
    std::map<std::string, std::array<double,3>> assigned_inertias_map; // 0 -> xx, 1 -> yy, 2 -> zz
    std::map<std::string, CollisionGeometryInfo> assigned_collision_geometry_map;
    YAML::Node config;
    bool exportAllUseradded{ false };
    
    array<double, 3> scale{ 1.0, 1.0, 1.0 };
    array<double, 3> originXYZ {0.0, 0.0, 0.0};
    array<double, 3> originRPY {0.0, 0.0, 0.0};
    bool warningsAreFatal{ true };
    std::string m_output_path{ "" };
};

class Creo2UrdfAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // !CREO2URDF_H
