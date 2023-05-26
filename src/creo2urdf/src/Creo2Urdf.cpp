/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */
#define _USE_MATH_DEFINES

#include <pfcGlobal.h>
#include <pfcModel.h>
#include <pfcSolid.h>
#include <pfcShrinkwrap.h>
#include <pfcAssembly.h>
#include <pfcComponentFeat.h>

#include <wfcGeometry.h>
#include <wfcModelItem.h>

#include <cmath>
#include <string>
#include <array>
#include <map>


#include <iDynTree/Model/Model.h>
#include <iDynTree/ModelIO/ModelExporter.h>
#include <iDynTree/ModelIO/ModelLoader.h>
#include <iDynTree/Model/RevoluteJoint.h>
#include <iDynTree/KinDynComputations.h>
#include <iDynTree/Model/Traversal.h>


constexpr double mm_to_m   = 1e-3;
constexpr double mm2_to_m2 = 1e-6;
constexpr double epsilon   = 1e-12;
constexpr double rad2deg   = 57.295779513;


std::array<std::string, 5> relevant_csys_names = {
"SCSYS_NECK_1",
"SCSYS_NECK_2",
"SCSYS_NECK_3", 
"SCSYS_HEAD", 
"SCSYS_REALSENSE"
};

std::map<std::string, std::string> child_axis_map = {
    {"SIM_ECUB_HEAD_NECK_2", "NECK_PITCH_AXIS"},
    {"SIM_ECUB_HEAD_NECK_3", "NECK_ROLL_AXIS"},
    {"SIM_ECUB_HEAD", "NECK_YAW_AXIS"},
    {"SIM_ECUB_REALSENSE", "REALSENSE_PITCH_AXIS"}
};

enum class c2uLogLevel
{
    NONE = 0,
    INFO,
    WARN
};

const std::map<c2uLogLevel, std::string> log_level_key = {
    {c2uLogLevel::NONE, "c2uNONE"},
    {c2uLogLevel::INFO, "c2uINFO"},
    {c2uLogLevel::WARN, "c2uWARN"}
};

void printToMessageWindow(std::string message, c2uLogLevel log_level = c2uLogLevel::INFO)
{
    pfcSession_ptr session_ptr = pfcGetProESession();
    xstringsequence_ptr msg_sequence = xstringsequence::create();
    msg_sequence->append(xstring(message.c_str()));
    session_ptr->UIClearMessage();
    session_ptr->UIDisplayMessage("creo2urdf.txt", log_level_key.at(log_level).c_str(), msg_sequence);
}

void printRotationMatrix(pfcTransform3D_ptr m)
{
    printToMessageWindow(to_string(m->GetMatrix()->get(0, 0)) + " " + to_string(m->GetMatrix()->get(0, 1)) + " " + to_string(m->GetMatrix()->get(0, 2)));
    printToMessageWindow(to_string(m->GetMatrix()->get(1, 0)) + " " + to_string(m->GetMatrix()->get(1, 1)) + " " + to_string(m->GetMatrix()->get(1, 2)));
    printToMessageWindow(to_string(m->GetMatrix()->get(2, 0)) + " " + to_string(m->GetMatrix()->get(2, 1)) + " " + to_string(m->GetMatrix()->get(2, 2)));
}

void printRotationMatrix(pfcMatrix3D_ptr m)
{
    printToMessageWindow(to_string(m->get(0, 0)) + " " + to_string(m->get(0, 1)) + " " + to_string(m->get(0, 2)));
    printToMessageWindow(to_string(m->get(1, 0)) + " " + to_string(m->get(1, 1)) + " " + to_string(m->get(1, 2)));
    printToMessageWindow(to_string(m->get(2, 0)) + " " + to_string(m->get(2, 1)) + " " + to_string(m->get(2, 2)));
}


std::array<double, 3> computeUnitVectorFromAxis(pfcCurveDescriptor_ptr axis_data)
{
    auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

    // There are just two points in the array
    pfcPoint3D_ptr pstart = axis_line->GetEnd1();
    pfcPoint3D_ptr pend = axis_line->GetEnd2();

    std::array<double, 3> unit_vector = {0, 0, 0};

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

iDynTree::SpatialInertia fromCreo(pfcMassProperty_ptr mass_prop, iDynTree::Transform H)
{
    auto com = mass_prop->GetGravityCenter();
    auto inertia_tensor = mass_prop->GetCenterGravityInertiaTensor();
    iDynTree::RotationalInertiaRaw idyn_inertia_tensor = iDynTree::RotationalInertiaRaw::Zero();

    for (int i_row = 0; i_row < idyn_inertia_tensor.rows(); i_row++) {
        for (int j_col = 0; j_col < idyn_inertia_tensor.cols(); j_col++) {
            idyn_inertia_tensor.setVal(i_row, j_col, inertia_tensor->get(i_row, j_col) * mm2_to_m2);
        }
    }

    iDynTree::Position com_child({ com->get(0) * mm_to_m , com->get(1) * mm_to_m, com->get(2) * mm_to_m });
    com_child = H.inverse() * com_child;  // TODO verify

    iDynTree::SpatialInertia sp_inertia(mass_prop->GetMass(), com_child, idyn_inertia_tensor);
    sp_inertia.fromRotationalInertiaWrtCenterOfMass(mass_prop->GetMass(), com_child, idyn_inertia_tensor);

    return sp_inertia;
}

iDynTree::Transform fromCreo(pfcTransform3D_ptr creo_trf)
{
    iDynTree::Transform idyn_trf;
    auto o = creo_trf->GetOrigin();
    auto m = creo_trf->GetMatrix();
    idyn_trf.setPosition({ o->get(0) * mm_to_m, o->get(1) * mm_to_m, o->get(2) * mm_to_m });
    idyn_trf.setRotation({ m->get(0,0), m->get(1,0), m->get(2,0),
                           m->get(0,1), m->get(1,1), m->get(2,1),
                           m->get(0,2), m->get(1,2), m->get(2,2) });

    return idyn_trf;
}

bool validateTransform(pfcTransform3D_ptr creo_matrix)
{
    iDynTree::ModelLoader mdl_loader;
    if (!mdl_loader.loadModelFromFile("model.urdf"))
    {
        printToMessageWindow("Could not load urdf for validation!", c2uLogLevel::WARN);
        return false;
    }

    auto model_urdf = mdl_loader.model();

    size_t n_frames = model_urdf.getNrOfFrames();

    printToMessageWindow("Running model validation");

    iDynTree::KinDynComputations computer;
    computer.loadRobotModel(model_urdf);
    auto urdf_trf = computer.getRelativeTransform("BAR", "BARLONGER");
    auto urdf_rpy = urdf_trf.getRotation().asRPY();
    /*
    printToMessageWindow("URDF trf between BAR and BARLONGER as RPY:\nR: "
        + to_string(urdf_rpy(0) * rad2deg) +
        "\nP: " + to_string(urdf_rpy(1) * rad2deg) +
        "\nY: " + to_string(urdf_rpy(2) * rad2deg));
    */

    auto creo_trf = fromCreo(creo_matrix);
    auto creo_rpy = creo_trf.getRotation().asRPY();

    /*
    printToMessageWindow("Creo trf between BAR and BARLONGER as RPY:\nR: "
        + to_string(creo_rpy(0) * rad2deg) +
        "\nP: " + to_string(creo_rpy(1) * rad2deg) +
        "\nY: " + to_string(creo_rpy(2) * rad2deg));
        */

    printToMessageWindow("Transform error as RPY:\nR: "
        + to_string(urdf_rpy(0) - creo_rpy(0)) +
        "\nP: " + to_string(urdf_rpy(1) - creo_rpy(1)) +
        "\nY: " + to_string(urdf_rpy(2) - creo_rpy(2)));

    return true;
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

std::pair<bool, iDynTree::Transform> getTransformFromPart(pfcModel_ptr modelhdl, const std::string& link_child_name) {

    iDynTree::Transform H_child;

    auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);
    if (csys_list->getarraysize() == 0) {
        printToMessageWindow("There are no CSYS in the part " + string(link_child_name), c2uLogLevel::WARN);
        
        H_child = iDynTree::Transform::Identity();
        
        return { false, H_child };
    }

    for (size_t i = 0; i < csys_list->getarraysize(); i++)
    {
        auto csys_elem = csys_list->get(i);

        auto csys = pfcCoordSystem::cast(csys_elem);

        if (std::find(relevant_csys_names.begin(), relevant_csys_names.end(), string(csys->GetName())) == relevant_csys_names.end())
        {
            continue;
        }

        auto trf = csys->GetCoordSys();

        auto m = trf->GetMatrix();
        auto o = trf->GetOrigin();

        H_child = fromCreo(trf);
        
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

std::pair<bool, iDynTree::Direction> getRotationAxisFromPart(pfcModel_ptr modelhdl, const std::string& link_child_name, iDynTree::Transform H_child) {

    iDynTree::Direction axis_unit_vector;

    auto axes_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_AXIS);
    // printToMessageWindow("There are " + to_string(axes_list->getarraysize()) + " axes");
    if (axes_list->getarraysize() == 0) {
        printToMessageWindow("There is no AXIS in the part " + string(link_child_name), c2uLogLevel::WARN);

        axis_unit_vector.zero();
        return { false, axis_unit_vector};
    }

    pfcAxis* axis = nullptr;

    for (size_t i = 0; i < axes_list->getarraysize(); i++)
    {
        auto axis_elem = pfcAxis::cast(axes_list->get(i));

        if (string(axis_elem->GetName()) == child_axis_map.at(string(link_child_name)))
        {
            axis = axis_elem;
            // printToMessageWindow("The axis is called " + string(axis_elem->GetName()));
        }
    }

    auto axis_data = wfcWAxis::cast(axis)->GetAxisData();

    auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

    auto unit = computeUnitVectorFromAxis(axis_line);

    axis_unit_vector.setVal(0, unit[0]);
    axis_unit_vector.setVal(1, unit[1]);
    axis_unit_vector.setVal(2, unit[2]);

    axis_unit_vector = H_child.inverse() * axis_unit_vector;  // We might benefit from performing this operation directly in Creo
    axis_unit_vector.Normalize();

    /*
    printToMessageWindow(string(axis->GetName()) + ": (" + std::to_string(axis_unit_vector[0]) + ", "
                                                        + std::to_string(axis_unit_vector[1]) + ", "
                                                        + std::to_string(axis_unit_vector[2]) + ")");
    */
    return { true, axis_unit_vector };
}

class Creo2UrdfActionListerner : public pfcUICommandActionListener {
public:
    void OnCommand() override {

        iDynTree::ModelExporter mdl_exporter;
        pfcSession_ptr session_ptr = pfcGetProESession();
        pfcModel_ptr model_ptr = session_ptr->GetCurrentModel();
        pfcSolid_ptr solid_ptr = pfcSolid::cast(session_ptr->GetCurrentModel());

        // TODO Principal units probably to be changed from MM to M before getting the model properties
        //auto length_unit = solid_ptr->GetPrincipalUnits()->GetUnit(pfcUnitType::pfcUNIT_LENGTH);
        // length_unit->Modify(pfcUnitConversionFactor::Create(0.001), length_unit->GetReferenceUnit()); // IT DOES NOT WORK

        iDynTree::Model idyn_model;
        iDynTree::ModelExporterOptions export_options;
        export_options.robotExportedName = "ECUB_HEAD";
        export_options.baseLink = "SIM_ECUB_HEAD_NECK_1";

        auto asm_component_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
        if (asm_component_list->getarraysize() == 0) {
            printToMessageWindow("There are no FEATURES in the asm", c2uLogLevel::WARN);
            return;
        }

        std::ofstream idyn_model_out("iDynTreeModel.txt");
        std::ofstream idyn_error_out("iDynTreeErrors.txt");
        std::streambuf* cerr_old_buf = std::cerr.rdbuf(); // store the original cerr buffer
        std::cerr.rdbuf(idyn_error_out.rdbuf());          // Pass file buffer to set it as new buffer

        std::string link_parent_name = "";

        iDynTree::Transform H_parent = iDynTree::Transform::Identity();

        int component_counter = 0;
        bool ret;

        for (int i = 0; i < asm_component_list->getarraysize(); i++)
        {
            iDynTree::Link link_child;
            iDynTree::Transform H_child = iDynTree::Transform::Identity();
            iDynTree::Transform H_parent_to_child = iDynTree::Transform::Identity();
            auto comp = asm_component_list->get(i);
            auto feat = pfcFeature::cast(comp);
            // auto feat_id = feat->GetId();


            if (feat->GetFeatType() != pfcFeatureType::pfcFEATTYPE_COMPONENT)
            {
                continue;
            }

            auto modelhdl = session_ptr->RetrieveModel(pfcComponentFeat::cast(feat)->GetModelDescr());
            auto link_child_name = modelhdl->GetFullName();

            printToMessageWindow(std::string(link_child_name));

            std::tie(ret, H_child) = getTransformFromPart(modelhdl, string(link_child_name));
            if (!ret)
            {
                std::cerr.rdbuf(cerr_old_buf);
                return;
            }

            auto mass_prop = pfcSolid::cast(modelhdl)->GetMassProperty();
            auto com = mass_prop->GetGravityCenter();                     // TODO transform the center of mass in relative coords
            auto comInertia = mass_prop->GetCenterGravityInertiaTensor(); // TODO GetCoordSysInertia ?

            link_child.setInertia(fromCreo(mass_prop, H_child));

            if (!link_child.getInertia().isPhysicallyConsistent())
            {
                printToMessageWindow("Link " + string(link_child_name) + " is NOT physically consistent!", c2uLogLevel::WARN);
            }

            /*
            printToMessageWindow("Model name is " + std::string(name) + " and weighs " + to_string(mass_prop->GetMass()));
            printToMessageWindow("Center of mass: x: " + to_string(com->get(0)) + " y: " + to_string(com->get(1)) + " z: " + to_string(com->get(2)));
            printToMessageWindow("Inertia tensor:");
            printToMessageWindow(to_string(comInertia->get(0, 0)) + " " + to_string(comInertia->get(0, 1)) + " " + to_string(comInertia->get(0, 2)));
            printToMessageWindow(to_string(comInertia->get(1, 0)) + " " + to_string(comInertia->get(1, 1)) + " " + to_string(comInertia->get(1, 2)));
            printToMessageWindow(to_string(comInertia->get(2, 0)) + " " + to_string(comInertia->get(2, 1)) + " " + to_string(comInertia->get(2, 2)));
            */

            if (component_counter > 0)
            {
                iDynTree::Direction axis;
                std::tie(ret, axis) = getRotationAxisFromPart(modelhdl, string(link_child_name), H_child);

                if (!ret)
                {
                    std::cerr.rdbuf(cerr_old_buf);
                    return; 
                }

                H_parent_to_child = H_parent.inverse() * H_child;

                //printToMessageWindow("H_parent: " + H_parent.toString());
                //printToMessageWindow("H_child: " + H_child.toString());
                //printToMessageWindow("prev_link_H_link: " + H_parent_to_child.toString());

                iDynTree::RevoluteJoint joint(H_parent_to_child, {{axis[0], axis[1], axis[2]},
                                                                   H_parent_to_child.getPosition()});
                                                                   // Should be 0 the origin of the axis, the displacement is already considered in transform
                                                                   //{ o->get(0) * mm_to_m, o->get(1) * mm_to_m, o->get(2) * mm_to_m } });

                // TODO let's put the limits hardcoded, to be retrieved from Creo
                double min = 0.0;
                double max = M_PI;
                joint.enablePosLimits(true);
                joint.setPosLimits(0, min, max);
                // TODO we have to retrieve the rest transform from creo
                //joint.setRestTransform()

                if (idyn_model.addJointAndLink(link_parent_name, link_parent_name + "--" + string(link_child_name), &joint, string(link_child_name), link_child) == iDynTree::JOINT_INVALID_INDEX) {
                    printToMessageWindow("FAILED TO ADD JOINT!", c2uLogLevel::WARN);

                    std::cerr.rdbuf(cerr_old_buf);
                    return;
                }
                printToMessageWindow(to_string(component_counter) + ": " + link_parent_name + "--" + string(link_child_name));
            }
            else
            {
                printToMessageWindow("First link, skipping joint addition");
                idyn_model.addLink(string(link_child_name), link_child);
            }


            link_parent_name = string(link_child_name);
            H_parent = H_child;

            // Getting just the first csys is a valid assumption for the MVP-1, for more complex asm we will need to change it

            //printToMessageWindow("Using " + relevant_csys_names[component_counter] + " to make stl");
            modelhdl->Export(link_child_name + ".stl", pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create(relevant_csys_names[component_counter].c_str())));

            component_counter++;

            // Replace the first 5 bytes of the binary file with a string different than "solid"
            // to avoid issues with stl parsers.
            // For details see: https://github.com/icub-tech-iit/creo2urdf/issues/16
            sanitizeSTL(string(link_child_name) + ".stl");

            // Lets add the mesh to the link
            iDynTree::ExternalMesh visualMesh;
            // Meshes are in millimeters, while iDynTree models are in meters
            iDynTree::Vector3 scale; scale(0) = scale(1) = scale(2) = mm_to_m;
            visualMesh.setScale(scale);
            // Let's assign a gray as default color
            iDynTree::Vector4 color;
            iDynTree::Material material;
            color(0) = color(1) = color(2) = 0.5;
            color(3) = 1.0;
            material.setColor(color);
            visualMesh.setMaterial(material);
            // Assign transform
            // TODO Right now maybe it is not needed it ie exported respct the link csys
            // visualMesh.setLink_H_geometry(H_parent_to_child);

            // Assign name
            visualMesh.setFilename(string(link_child_name) + ".stl");
            // TODO Right now let's consider visual and collision with the same mesh
            idyn_model.visualSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(string(link_child_name))].push_back(visualMesh.clone());
            idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(string(link_child_name))].push_back(visualMesh.clone());

            //idyn_model.addAdditionalFrameToLink(string(name), string(name) + "_" + string(csys_list->get(0)->GetName()), fromCreo(transform)); TODO when we have an additional frame to add
        }

        idyn_model_out << idyn_model.toString();
        idyn_model_out.close();

        mdl_exporter.init(idyn_model);
        mdl_exporter.setExportingOptions(export_options);

        if (!mdl_exporter.isValid())
        {
            printToMessageWindow("Model is not valid!", c2uLogLevel::WARN);
        }

        /*
        if (idyn_model.getLinkIndex("SIM_ECUB_HEAD_NECK_1") == iDynTree::LINK_INVALID_INDEX)
        {
            printToMessageWindow("[ERROR] URDFStringFromModel: specified baseLink is not part of the model");
        }

       iDynTree::Traversal exportTraversal;
        if (!idyn_model.computeFullTreeTraversal(exportTraversal, idyn_model.getLinkIndex("SIM_ECUB_HEAD_NECK_1")))
        {
            printToMessageWindow("[ERROR] URDFStringFromModel: error in computeFullTreeTraversal", c2uLogLevel::WARN);
        }
        else
        {
            printToMessageWindow("Traversal completed successfully");
        }
        */

        if (!mdl_exporter.exportModelToFile("model.urdf"))
        {
            printToMessageWindow("Error exporting the urdf. See iDynTreeErrors.txt for details", c2uLogLevel::WARN);
        }
        else
        {
            printToMessageWindow("Urdf created successfully!");
        }

        std::cerr.rdbuf(cerr_old_buf); // Restore original cerr buffer after using iDynTree


        // Do not validate yet, it crashes creo
       //validateTransform(validation_trf);

        return;
    }
};


class Creo2UrdfAccessListener : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override {
        auto model = pfcGetProESession()->GetCurrentModel();
        if (!model) {
            return pfcCommandAccess::pfcACCESS_AVAILABLE;
        }
        auto type = model->GetType();
        if (type != pfcMDL_PART && type != pfcMDL_ASSEMBLY) {
            return pfcCommandAccess::pfcACCESS_UNAVAILABLE;
        }
        return pfcCommandAccess::pfcACCESS_AVAILABLE;
    }
};


class ValidatorListener : public pfcUICommandActionListener {
public:
    void OnCommand() override {
        printToMessageWindow("TODO: This button is used to validate the model");
        return;
    }
};

class ValidatorAccessListener : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override {
        auto model = pfcGetProESession()->GetCurrentModel();
        if (!model) {
            return pfcCommandAccess::pfcACCESS_AVAILABLE;
        }
        auto type = model->GetType();
        if (type != pfcMDL_PART && type != pfcMDL_ASSEMBLY) {
            return pfcCommandAccess::pfcACCESS_UNAVAILABLE;
        }
        return pfcCommandAccess::pfcACCESS_AVAILABLE;
    }
};

/*====================================================================*\
FUNCTION : user_initialize()
PURPOSE  :
\*====================================================================*/
extern "C" int user_initialize(
    int argc,
    char* argv[],
    char* version,
    char* build,
    wchar_t errbuf[80])
{
    auto session = pfcGetProESession();

    auto cmd = session->UICreateCommand("Creo2Urdf", new Creo2UrdfActionListerner());
    cmd->AddActionListener(new Creo2UrdfAccessListener()); // To be checked it is odd
    cmd->Designate("ui.txt", "Run Creo2Urdf", "Run Creo2Urdf", "Run Creo2Urdf");

    auto cmd_validate = session->UICreateCommand("Validator", new ValidatorListener());
    cmd_validate->AddActionListener(new ValidatorAccessListener()); // To be checked it is odd
    cmd_validate->Designate("ui.txt", "Run Validation", "Run Validation", "Run Validation");

    session->RibbonDefinitionfileLoad("creo2urdf.rbn");

    return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{
}