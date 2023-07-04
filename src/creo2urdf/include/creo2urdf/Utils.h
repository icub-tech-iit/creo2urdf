/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef UTILS_H
#define UTILS_H

#include <pfcGlobal.h>

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
#include <wfcModelItem.h>


#include <iDynTree/Model/Model.h>
#include <iDynTree/ModelIO/ModelExporter.h>
#include <iDynTree/ModelIO/ModelLoader.h>
#include <iDynTree/Model/RevoluteJoint.h>
#include <iDynTree/Model/FixedJoint.h>
#include <iDynTree/KinDynComputations.h>
#include <iDynTree/Model/Traversal.h>


constexpr double mm_to_m = 1e-3;
constexpr double mm2_to_m2 = 1e-6;
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


static const std::map<std::string, std::string> link_csys_map = {
    {"SIM_ECUB_ROOT_LINK","SCSYS_ROOT"},
    {"SIM_ECUB_TORSO_1","SCSYS_TORSO_1"},
    {"SIM_ECUB_TORSO_2","SCSYS_TORSO_2"},
    {"SIM_ECUB_CHEST","SCSYS_CHEST"},
    {"SIM_ECUB_L_SHOULDER_1","SCSYS_L_SHOULDER_1"},
    {"SIM_ECUB_L_SHOULDER_2","SCSYS_L_SHOULDER_2"},
    {"SIM_ECUB_L_SHOULDER_3","SCSYS_L_SHOULDER_3"},
    {"SIM_ECUB_L_UPPERARM","SCSYS_L_UPPERARM"},
    {"SIM_ECUB_R_SHOULDER_1","SCSYS_R_SHOULDER_1"},
    {"SIM_ECUB_R_SHOULDER_2","SCSYS_R_SHOULDER_2"},
    {"SIM_ECUB_R_SHOULDER_3","SCSYS_R_SHOULDER_3"},
    {"SIM_ECUB_R_UPPERARM","SCSYS_R_UPPERARM"},
    {"SIM_ECUB_HEAD_NECK_1", "SCSYS_NECK_1"},
    {"SIM_ECUB_HEAD_NECK_2", "SCSYS_NECK_2"},
    {"SIM_ECUB_HEAD_NECK_3", "SCSYS_NECK_3"},
    {"SIM_ECUB_HEAD", "SCSYS_HEAD"},
    {"SIM_ECUB_REALSENSE", "SCSYS_REALSENSE"}
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

std::array<double, 3> computeUnitVectorFromAxis(pfcCurveDescriptor_ptr axis_data);

iDynTree::SpatialInertia fromCreo(pfcMassProperty_ptr mass_prop, iDynTree::Transform H);

iDynTree::Transform fromCreo(pfcTransform3D_ptr creo_trf);

void printToMessageWindow(std::string message, c2uLogLevel log_level = c2uLogLevel::INFO);

void printTransformMatrix(pfcTransform3D_ptr m);

void printRotationMatrix(pfcMatrix3D_ptr m);

void sanitizeSTL(std::string stl);

std::pair<bool, iDynTree::Transform> getTransformFromRootToChild(pfcComponentPath_ptr comp_path, pfcModel_ptr modelhdl);

std::pair<bool, iDynTree::Transform> getTransformFromPart(pfcModel_ptr modelhdl, const std::string& link_child_name);

std::pair<bool, iDynTree::Direction> getRotationAxisFromPart(pfcModel_ptr modelhdl, const std::string& axis_name, const std::string& link_child_name, iDynTree::Transform H_child);

bool addMeshAndExport(pfcModel_ptr modelhdl, const std::string& link_child_name, const std::string& csys_name, iDynTree::Model& idyn_model);

#endif // !UTILS_H
