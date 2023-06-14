/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#pragma once

#include <pfcGlobal.h>
#include <creo2urdf/Utils.h>

class Creo2UrdfActionListerner : public pfcUICommandActionListener {
public:
    void OnCommand() override;
};

class Creo2UrdfAccessListener : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};
