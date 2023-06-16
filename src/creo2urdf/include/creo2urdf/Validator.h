/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <pfcGlobal.h>
#include <creo2urdf/Utils.h>

class Validator : public pfcUICommandActionListener {
public:
    void OnCommand() override;

private:

    iDynTree::Model idyn_model;
    pfcSession_ptr creo_session_ptr;
    pfcModel_ptr creo_model_ptr;
    std::map<std::string, iDynTree::Transform> link_name_to_creo_computed_trf_map;
    iDynTree::KinDynComputations computer;

    bool loadUrdfFromFile(const std::string& filename);
    bool assignCreoTransformToLink();
    bool validatePositions(iDynTree::VectorDynSize positions);
};

class ValidatorAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // VALIDATOR_H
