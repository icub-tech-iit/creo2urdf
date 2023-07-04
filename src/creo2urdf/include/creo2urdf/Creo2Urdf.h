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

enum class JointType {
    Revolute,
    Fixed,
    Linear,
    Spherical,
    None
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

class Creo2Urdf : public pfcUICommandActionListener {
public:
    void OnCommand() override;

    bool exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options);
    void populateJointInfoMap(pfcModel_ptr modelhdl);
    bool addMeshAndExport(const std::string& link_child_name, const std::string& csys_name, pfcModel_ptr component_handle);

private:
    iDynTree::Model idyn_model;
    std::map<std::string, JointInfo> joint_info_map;
    std::map<std::string, LinkInfo> link_info_map;
};

class Creo2UrdfAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // !CREO2URDF_H
