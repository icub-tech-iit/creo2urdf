/**
 * @file Utils.cpp
 * @brief Contains definitions for utility functions used in the plugin.
 * 
* @copyright (C) 2006-2024 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Utils.h>

std::array<double, 3> computeUnitVectorFromAxis(pfcCurveDescriptor_ptr axis_data)
{
    auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

    // There are just two points in the array
    pfcPoint3D_ptr pstart = axis_line->GetEnd1();
    pfcPoint3D_ptr pend = axis_line->GetEnd2();

    std::array<double, 3> unit_vector = { 0, 0, 0 };

    double module = sqrt(pow(pend->get(0) - pstart->get(0), 2) +
        pow(pend->get(1) - pstart->get(1), 2) +
        pow(pend->get(2) - pstart->get(2), 2));

    if (module < epsilon)
    {
        return unit_vector;
    }

    unit_vector[0] = (pend->get(0) - pstart->get(0)) / module;
    unit_vector[1] = (pend->get(1) - pstart->get(1)) / module;
    unit_vector[2] = (pend->get(2) - pstart->get(2)) / module;

    return unit_vector;
}

iDynTree::Transform fromCreo(pfcTransform3D_ptr creo_trf, const array<double, 3>& scale)
{
    iDynTree::Transform idyn_trf;
    auto o = creo_trf->GetOrigin();
    auto m = creo_trf->GetMatrix();
    idyn_trf.setPosition({ o->get(0) * scale[0], o->get(1) * scale[1], o->get(2) * scale[2]});
    idyn_trf.setRotation({ m->get(0,0), m->get(1,0), m->get(2,0),
                           m->get(0,1), m->get(1,1), m->get(2,1),
                           m->get(0,2), m->get(1,2), m->get(2,2) });

    return idyn_trf;
}

std::vector<string> getSolidDatumNames(pfcSolid_ptr solid, pfcModelItemType type)
{
    std::vector<string> result;
    auto items = solid->ListItems(type);
    if (items->getarraysize() == 0) {
        printToMessageWindow("There is no Axis in " + string(solid->GetFullName()), c2uLogLevel::WARN);
        return result;
    }

    for (int i = 0; i < items->getarraysize(); i++)
    {
        result.push_back(std::string(items->get(i)->GetName()));
    }

    return result;
}

void printToMessageWindow(std::string message, c2uLogLevel log_level)
{
    pfcSession_ptr session_ptr = pfcGetProESession();
    xstringsequence_ptr msg_sequence = xstringsequence::create();
    msg_sequence->append(xstring(message.c_str()));
    session_ptr->UIClearMessage();
    session_ptr->UIDisplayMessage("creo2urdf.txt", log_level_key.at(log_level).c_str(), msg_sequence);
}

void printTransformMatrix(pfcTransform3D_ptr m)
{
    printToMessageWindow(fromCreo(m).toString());
}

void printRotationMatrix(pfcMatrix3D_ptr m)
{
    printToMessageWindow(to_string(m->get(0, 0)) + " " + to_string(m->get(0, 1)) + " " + to_string(m->get(0, 2)));
    printToMessageWindow(to_string(m->get(1, 0)) + " " + to_string(m->get(1, 1)) + " " + to_string(m->get(1, 2)));
    printToMessageWindow(to_string(m->get(2, 0)) + " " + to_string(m->get(2, 1)) + " " + to_string(m->get(2, 2)));
}

void sanitizeSTL(std::string stl)
{
    size_t n_bytes = 5;
    char placeholder[6] = "robot";
    std::ofstream output(stl, std::ios::binary | std::ios::out | std::ios::in);

    for (size_t i = 0; i < n_bytes; i++)
    {
        output.seekp(i);
        output.write(&placeholder[i], 1);
    }
    output.close();
}

std::pair<bool, iDynTree::Transform> getTransformFromOwnerToLinkFrame(pfcComponentPath_ptr comp_path, pfcModel_ptr modelhdl, const std::string& link_frame_name, const array<double, 3>& scale) {
    
    iDynTree::Transform csysAsm_H_link = iDynTree::Transform::Identity();

    auto csysAsm_H_csysPart = fromCreo(comp_path->GetTransform(xtrue), scale);
    iDynTree::Transform csysPart_H_link;

    bool ret = false;
    std::tie(ret, csysPart_H_link) = getTransformFromPart(modelhdl, link_frame_name, scale);

    if (!ret)
    {
        printToMessageWindow("Unable to get the transform "  + link_frame_name + " in " + string(modelhdl->GetFullName()), c2uLogLevel::WARN);
        return make_pair(false, csysAsm_H_link);
    }

    csysAsm_H_link = csysAsm_H_csysPart * csysPart_H_link;

    return make_pair(true, csysAsm_H_link);

}

std::pair<bool, iDynTree::Transform> getTransformFromPart(pfcModel_ptr modelhdl, const std::string& link_frame_name, const array<double, 3>& scale) {

    iDynTree::Transform H_child;
    auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);

    auto link_child_name = string(modelhdl->GetFullName());
    
    if (csys_list->getarraysize() == 0) {
        printToMessageWindow("There are no Coordinate Systems in the part " + link_child_name, c2uLogLevel::WARN);

        H_child = iDynTree::Transform::Identity();

        return { false, H_child };
    }

    for (size_t i = 0; i < csys_list->getarraysize(); i++)
    {
        auto csys_elem = csys_list->get(xint(i));

        auto csys = pfcCoordSystem::cast(csys_elem);

        if (string(csys->GetName()) != link_frame_name)
        {
            continue;
        }

        auto trf = csys->GetCoordSys();

        auto m = trf->GetMatrix();
        auto o = trf->GetOrigin();

        H_child = fromCreo(trf, scale);

        /*
        printToMessageWindow("csys name " + string(csys->GetName()));
        printToMessageWindow("origin x: " + to_string(o->get(0)) + " y: " + to_string(o->get(1)) + " z: " + to_string(o->get(2)));
        printToMessageWindow("transform:");
        printToMessageWindow(to_string(m->get(0, 0)) + " " + to_string(m->get(0, 1)) + " " + to_string(m->get(0, 2)));
        printToMessageWindow(to_string(m->get(1, 0)) + " " + to_string(m->get(1, 1)) + " " + to_string(m->get(1, 2)));
        printToMessageWindow(to_string(m->get(2, 0)) + " " + to_string(m->get(2, 1)) + " " + to_string(m->get(2, 2)));
        */
        //printToMessageWindow(string(csys_feat->GetFeatTypeName()));

        return { true, H_child };
    }

    return { false, H_child };
}

std::tuple<bool, iDynTree::Direction, iDynTree::Transform> getAxisFromPart(pfcModel_ptr modelhdl, const std::string& axis_name, const string& link_frame_name, const array<double, 3>& scale) {

    iDynTree::Direction axis_unit_vector;
    iDynTree::Transform oldChild_H_newChild = iDynTree::Transform::Identity();
    axis_unit_vector.zero();

    auto axes_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_AXIS);
    if (axes_list->getarraysize() == 0) {
        printToMessageWindow("There is no Axis in the part " + string(modelhdl->GetFullName()), c2uLogLevel::WARN);

        return { false, axis_unit_vector, oldChild_H_newChild };
    }

    if (axis_name.empty())
        return { false, axis_unit_vector, oldChild_H_newChild };

    pfcAxis* axis = nullptr;

    for (size_t i = 0; i < axes_list->getarraysize(); i++)
    {
        auto axis_elem = pfcAxis::cast(axes_list->get(xint(i)));
        if (string(axis_elem->GetName()) == axis_name)
        {
            axis = axis_elem;
        }
    }

    auto axis_data = wfcWAxis::cast(axis)->GetAxisData();

    auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

    if (link_frame_name == "CSYS") {
        // We use the medium point of the axis as offset
        pfcPoint3D_ptr pstart = axis_line->GetEnd1();
        pfcPoint3D_ptr pend = axis_line->GetEnd2();
        auto x = ((pend->get(0) + pstart->get(0)) / 2.0) * scale[0];
        auto y = ((pend->get(1) + pstart->get(1)) / 2.0) * scale[1];
        auto z = ((pend->get(2) + pstart->get(2)) / 2.0) * scale[2];
        oldChild_H_newChild.setPosition({ x, y, z });

    }

    // There are just two points in the array

    auto unit = computeUnitVectorFromAxis(axis_line);

    axis_unit_vector.setVal(0, unit[0]);
    axis_unit_vector.setVal(1, unit[1]);
    axis_unit_vector.setVal(2, unit[2]);

   // auto csysAsm_H_csysPart = fromCreo(comp_path->GetTransform(xtrue), scale);
    

    auto& csys_H_child = getTransformFromPart(modelhdl, link_frame_name, scale).second;

    axis_unit_vector = csys_H_child.inverse() * axis_unit_vector;  // We might benefit from performing this operation directly in Creo
    axis_unit_vector.Normalize();
    
    return { true, axis_unit_vector, oldChild_H_newChild };
}

std::string extractFolderPath(const std::string& filePath) {
    auto found = std::find_if(filePath.rbegin(), filePath.rend(),
        [](char c) { return c == '/' || c == '\\'; });

    if (found != filePath.rend()) {
        return std::string(filePath.begin(), found.base());
    }
    else {
        return "";
    }
}


void mergeYAMLNodes(YAML::Node& dest, const YAML::Node& src) {

    for (const auto& item : src) {
        std::string key = item.first.as<std::string>();
        if (!dest[key]) {
            // If the key doesn't exist in the destination node, simply add it
            dest[key] = item.second;
        }
        else {
            // If the key exists in the destination node, merge the values
            // If both values are maps, recursively merge them
            if (dest[key].IsMap() && item.second.IsMap()) {
                mergeYAMLNodes(dest[key], item.second);
            }
            else {
                // Otherwise, overwrite the value in the destination node
                dest[key] = item.second;
            }
        }
    }
}
