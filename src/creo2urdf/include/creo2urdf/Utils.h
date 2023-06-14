/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#pragma once

#define _USE_MATH_DEFINES

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
#include <iDynTree/KinDynComputations.h>
#include <iDynTree/Model/Traversal.h>


constexpr double mm_to_m = 1e-3;
constexpr double mm2_to_m2 = 1e-6;
constexpr double epsilon = 1e-12;
constexpr double rad2deg = 180.0 / M_PI;
constexpr double deg2rad = 1 / rad2deg;


enum class c2uLogLevel
{
    NONE = 0,
    INFO,
    WARN,
    PROMPT
};

const std::map<c2uLogLevel, std::string> log_level_key = {
    {c2uLogLevel::NONE, "c2uNONE"},
    {c2uLogLevel::INFO, "c2uINFO"},
    {c2uLogLevel::WARN, "c2uWARN"},
    {c2uLogLevel::PROMPT, "c2uPROMPT"}
};

class iDynRedirectErrors {
public:

    iDynRedirectErrors() {
        old_buf = nullptr;
    
    }

    ~iDynRedirectErrors() {
        if (old_buf != nullptr) {
            std::cerr.rdbuf(old_buf);
        }

        if (idyn_out.is_open()) {
            idyn_out.close();
        }
    }

    void redirectBuffer(std::streambuf* old_buffer, const std::string& filename)
    {
        old_buf = old_buffer;
        idyn_out = std::ofstream(filename);
        std::cerr.rdbuf(idyn_out.rdbuf());
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

std::pair<bool, iDynTree::Transform> getTransformFromPart(pfcModel_ptr modelhdl, const std::string& link_child_name);

std::pair<bool, iDynTree::Direction> getRotationAxisFromPart(pfcModel_ptr modelhdl, const std::string& link_child_name, iDynTree::Transform H_child);

bool addMeshAndExport(pfcModel_ptr modelhdl, const std::string& link_child_name, int component_counter, iDynTree::Model& idyn_model);

