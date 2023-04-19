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


constexpr double mm_to_m   = 1e-3;
constexpr double mm2_to_m2 = 1e-6;
constexpr double epsilon   = 1e-12;

enum class c2uLogLevel
{
    INFO = 0,
    WARN
};

const std::map<c2uLogLevel, std::string> log_level_key = {
    {c2uLogLevel::INFO, "c2uINFO"},
    {c2uLogLevel::WARN, "c2uWARN"}
};

void printToMessageWindow(pfcSession_ptr session, std::string message, c2uLogLevel log_level = c2uLogLevel::INFO)
{
    xstringsequence_ptr msg_sequence = xstringsequence::create();
    msg_sequence->append(xstring(message.c_str()));
    session->UIClearMessage();
    session->UIDisplayMessage("creo2urdf.txt", log_level_key.at(log_level).c_str(), msg_sequence);
}

bool validateTransform()
{
    iDynTree::ModelLoader mdl_loader;
    auto model_urdf = mdl_loader.loadModelFromFile("model.urdf");

}



std::array<double, 3> computeUnitVectorFromAxis(pfcCurveDescriptor_ptr axis_data)
{
    auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

    // There are just two points in the array
    pfcPoint3D_ptr pstart = axis_line->GetEnd1();
    pfcPoint3D_ptr pend = axis_line->GetEnd2();

    std::array<double, 3> unit_vector;

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

iDynTree::SpatialInertia fromCreo(pfcMassProperty_ptr mass_prop) {
    auto com = mass_prop->GetGravityCenter();
    auto inertia_tensor = mass_prop->GetCenterGravityInertiaTensor();
    iDynTree::RotationalInertiaRaw idyn_inertia_tensor = iDynTree::RotationalInertiaRaw::Zero();
    for (int i_row = 0; i_row < idyn_inertia_tensor.rows(); i_row++) {
        for (int j_col = 0; j_col < idyn_inertia_tensor.cols(); j_col++) {
            idyn_inertia_tensor.setVal(i_row, j_col, inertia_tensor->get(i_row, j_col) * mm2_to_m2);
        }
    }

    iDynTree::SpatialInertia sp_inertia(mass_prop->GetMass(),
        { com->get(0) * mm_to_m, com->get(1) * mm_to_m, com->get(2) * mm_to_m },
        idyn_inertia_tensor);
    return sp_inertia;
}

iDynTree::Transform fromCreo(pfcTransform3D_ptr creo_trf) {
    iDynTree::Transform idyn_trf;
    auto o = creo_trf->GetOrigin();
    auto m = creo_trf->GetMatrix();
    idyn_trf.setPosition({ o->get(0) * mm_to_m, o->get(1) * mm_to_m, o->get(2) * mm_to_m });
    idyn_trf.setRotation({ m->get(0,0), m->get(0,1), m->get(0,2),
                           m->get(1,0), m->get(1,1), m->get(1,2),
                           m->get(2,0), m->get(2,1), m->get(2,2) });

    return idyn_trf;
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
        export_options.robotExportedName = "2BARS";
        export_options.baseLink = "BAR";

        auto asm_component_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
        if (asm_component_list->getarraysize() == 0) {
            printToMessageWindow(session_ptr, "There are no FEATURES in the asm", c2uLogLevel::WARN);
            return;
        }

        std::string prevLinkName = "";

        for (int i = 0; i < asm_component_list->getarraysize(); i++)
        {
            iDynTree::Link link;
            auto comp = asm_component_list->get(i);
            auto feat = pfcFeature::cast(comp);
            auto feat_id = feat->GetId();

            if (feat->GetFeatType() != pfcFeatureType::pfcFEATTYPE_COMPONENT)
            {
                continue;
            }

            auto modelhdl = session_ptr->RetrieveModel(pfcComponentFeat::cast(feat)->GetModelDescr());
            xintsequence_ptr seq = xintsequence::create();

            seq->append(feat_id);

            pfcComponentPath_ptr comp_path = pfcCreateComponentPath(pfcAssembly::cast(model_ptr), seq);

            auto transform = comp_path->GetTransform(xfalse);

            auto m = transform->GetMatrix();
            auto o = transform->GetOrigin();
            printToMessageWindow(session_ptr, "feat name: id: " + to_string(feat_id));

            printToMessageWindow(session_ptr, "origin x: " + to_string(o->get(0)) + " y: " + to_string(o->get(1)) + " z: " + to_string(o->get(2)));
            printToMessageWindow(session_ptr, "transform:");
            printToMessageWindow(session_ptr, to_string(m->get(0, 0)) + " " + to_string(m->get(0, 1)) + " " + to_string(m->get(0, 2)));
            printToMessageWindow(session_ptr, to_string(m->get(1, 0)) + " " + to_string(m->get(1, 1)) + " " + to_string(m->get(1, 2)));
            printToMessageWindow(session_ptr, to_string(m->get(2, 0)) + " " + to_string(m->get(2, 1)) + " " + to_string(m->get(2, 2)));

            auto name = modelhdl->GetFullName();
            auto mass_prop = pfcSolid::cast(modelhdl)->GetMassProperty();
            auto com = mass_prop->GetGravityCenter();
            auto comInertia = mass_prop->GetCenterGravityInertiaTensor(); // TODO GetCoordSysInertia ?

            printToMessageWindow(session_ptr, "Model name is " + std::string(name) + " and weighs " + to_string(mass_prop->GetMass()));
            printToMessageWindow(session_ptr, "Center of mass: x: " + to_string(com->get(0)) + " y: " + to_string(com->get(1)) + " z: " + to_string(com->get(2)));
            printToMessageWindow(session_ptr, "Inertia tensor:");
            printToMessageWindow(session_ptr, to_string(comInertia->get(0, 0)) + " " + to_string(comInertia->get(0, 1)) + " " + to_string(comInertia->get(0, 2)));
            printToMessageWindow(session_ptr, to_string(comInertia->get(1, 0)) + " " + to_string(comInertia->get(1, 1)) + " " + to_string(comInertia->get(1, 2)));
            printToMessageWindow(session_ptr, to_string(comInertia->get(2, 0)) + " " + to_string(comInertia->get(2, 1)) + " " + to_string(comInertia->get(2, 2)));

            auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);
            if (csys_list->getarraysize() == 0) {
                printToMessageWindow(session_ptr, "There are no CYS in the part " + string(name));
                return;
            }

            auto axes_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_AXIS);
            printToMessageWindow(session_ptr, "There are " + to_string(axes_list->getarraysize()) + " axes");
            if (axes_list->getarraysize() == 0) {
                printToMessageWindow(session_ptr, "There are no AXIS in the part " + string(name));
                return;
            }

            // TODO We assume we have 1 axis and it is the one of the joint
            auto axis = pfcAxis::cast(axes_list->get(0));
            printToMessageWindow(session_ptr, "The axis is called " + string(axis->GetName()) + " axes");

            auto axis_data = wfcWAxis::cast(axis)->GetAxisData();

            auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

            auto unit = computeUnitVectorFromAxis(axis_line);

            printToMessageWindow(session_ptr, "unit vector of axis " + string(axis->GetName()) + " is : (" + std::to_string(unit[0]) + ", " + std::to_string(unit[1]) + ", " + std::to_string(unit[2]) + ")");

            link.setInertia(fromCreo(mass_prop));
            if (i == asm_component_list->getarraysize() - 1) { // TODO This is valid only for twobars
                iDynTree::RevoluteJoint joint(fromCreo(transform), { {unit[0], unit[1], unit[2]},
                                                                      iDynTree::Position().Zero()});
                                                                     // Should be 0 the origin of the axis, the displacement is already considered in transform
                                                                    //{ o->get(0) * mm_to_m, o->get(1) * mm_to_m, o->get(2) * mm_to_m } });

                // TODO let's put the limits hardcoded, to be retrieved from Creo
                double min = 0.0;
                double max = M_PI;
                joint.enablePosLimits(true);
                joint.setPosLimits(0, min, max);
                // TODO we have to retrieve the rest transform from creo
                //joint.setRestTransform()

                if (idyn_model.addJointAndLink(prevLinkName, prevLinkName + "--" + string(name), &joint, string(name), link) == iDynTree::JOINT_INVALID_INDEX) {
                    printToMessageWindow(session_ptr, "FAILED TO ADD JOINT!");
                    return;
                }
            }
            else
            {
                prevLinkName = string(name);
                idyn_model.addLink(string(name), link);
            }


            // Getting just the first csys is a valid assumption for the MVP-1, for more complex asm we will need to change it
            modelhdl->Export(name + ".stl", pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create(csys_list->get(0)->GetName())));
            // Lets add the mess to the link
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
            // visualMesh.link_H_geometry = link_H_geometry;

            // Assign name
            visualMesh.setFilename(string(name) + ".stl");
            // TODO Right now let's consider visual and collision with the same mesh
            idyn_model.visualSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(string(name))].push_back(visualMesh.clone());
            idyn_model.collisionSolidShapes().getLinkSolidShapes()[idyn_model.getLinkIndex(string(name))].push_back(visualMesh.clone());

            //idyn_model.addAdditionalFrameToLink(string(name), string(name) + "_" + string(csys_list->get(0)->GetName()), fromCreo(transform)); TODO when we have an additional frame to add

        }

        printToMessageWindow(session_ptr, "idynModel " + idyn_model.toString());
        std::string model_str = ""; 

        mdl_exporter.init(idyn_model);
        mdl_exporter.setExportingOptions(export_options);

        if (!mdl_exporter.exportModelToFile("model.urdf")) {
            printToMessageWindow(session_ptr, "Error exporting the urdf");
        }

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

    //session->RibbonDefinitionfileLoad("tool.rbn");

    return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{
}