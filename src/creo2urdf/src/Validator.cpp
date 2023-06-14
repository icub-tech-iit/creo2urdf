/*
/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */
#define _USE_MATH_DEFINES

#include <creo2urdf/Validator.h>

void ValidatorListener::OnCommand() {
    std::map<std::string, iDynTree::Transform> link_name_to_creo_computed_trf_map;
    iDynTree::ModelLoader mdl_loader;
    if (!mdl_loader.loadModelFromFile("model.urdf"))
    {
        printToMessageWindow("Could not load urdf for validation!", c2uLogLevel::WARN);
        return;
    }

    auto idyn_model = mdl_loader.model();
    auto creo_session_ptr = pfcGetProESession();
    auto creo_model_ptr = creo_session_ptr->GetCurrentModel();
    bool ret{ false };
    iDynTree::VectorDynSize positions(idyn_model.getNrOfDOFs());
    for (int i = 0; i < idyn_model.getNrOfJoints(); i++) {
        printToMessageWindow("Please insert value for joint " + idyn_model.getJointName(i), c2uLogLevel::PROMPT);
        positions[i] = creo_session_ptr->UIReadRealMessage(-360.0, 360.0) * deg2rad;
        printToMessageWindow("joint" + to_string(i) + " value " + to_string(positions[i]), c2uLogLevel::INFO);
    }

    iDynTree::Transform H_child = iDynTree::Transform::Identity();

    auto asm_component_list = creo_model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
    if (asm_component_list->getarraysize() == 0) {
        printToMessageWindow("There are no FEATURES in the asm", c2uLogLevel::WARN);
        return;
    }
    // Let's first retrieve the trfs from root to all the links and store it in a map.
    for (int i = 0; i < asm_component_list->getarraysize(); i++)
    {

        auto feat = pfcFeature::cast(asm_component_list->get(i));

        if (feat->GetFeatType() != pfcFeatureType::pfcFEATTYPE_COMPONENT)
        {
            continue;
        }

        xintsequence_ptr seq = xintsequence::create();
        seq->append(feat->GetId());

        pfcComponentPath_ptr comp_path = pfcCreateComponentPath(pfcAssembly::cast(creo_model_ptr), seq);

        auto asm_csys_H_csys = fromCreo(comp_path->GetTransform(xtrue));
        auto modelhdl = creo_session_ptr->RetrieveModel(pfcComponentFeat::cast(feat)->GetModelDescr());
        auto link_child_name = string(modelhdl->GetFullName());

        iDynTree::Transform csys_H_child;
        std::tie(ret, csys_H_child) = getTransformFromPart(modelhdl, link_child_name);
        if (!ret)
        {
            printToMessageWindow("Unable to get the transform respect to the root for" + link_child_name);
            return;
        }

        H_child = iDynTree::Transform::compose(asm_csys_H_csys,csys_H_child);

        link_name_to_creo_computed_trf_map[link_child_name] = H_child;

    }
    printToMessageWindow("Running model validation");
    iDynTree::KinDynComputations computer;
    computer.loadRobotModel(idyn_model);


    // Setting the `world_T_base`
    iDynTree::Vector3 gravity; gravity.zero(); gravity(2) = -9.8;
    auto H_base = link_name_to_creo_computed_trf_map.at("SIM_ECUB_HEAD_NECK_1");


    if (!computer.setRobotState(H_base, positions,
            iDynTree::Twist(),
            iDynTree::VectorDynSize(idyn_model.getNrOfDOFs()),
            gravity))
    {
        printToMessageWindow("Could not set the robot state!", c2uLogLevel::WARN);
    }

    // Lets ask the same transform to idyntree and check the difference w/ the
    // creo computed one
    for (int i = 0; i < idyn_model.getNrOfLinks(); i++) {
        auto link_name = idyn_model.getLinkName(i);
        auto idyn_trf = computer.getWorldTransform(link_name);
        auto creo_trf = link_name_to_creo_computed_trf_map.at(link_name);

        printToMessageWindow("Position error for "+ link_name + ": "+ (idyn_trf.getPosition() - creo_trf.getPosition()).toString());
        printToMessageWindow("RPY error for " + link_name + ": " +
                            std::to_string(idyn_trf.getRotation().asRPY()[0] - creo_trf.getRotation().asRPY()[0]) + ", " +
                            std::to_string(idyn_trf.getRotation().asRPY()[1] - creo_trf.getRotation().asRPY()[1]) + ", " +
                            std::to_string(idyn_trf.getRotation().asRPY()[2] - creo_trf.getRotation().asRPY()[2]));
        //printToMessageWindow("IDYN TRANSFORM: " + idyn_trf.toString());
        //printToMessageWindow("CREO TRANSFORM: " + creo_trf.toString());
    }
    return;
}

pfcCommandAccess ValidatorAccessListener::OnCommandAccess(xbool AllowErrorMessages) {
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
