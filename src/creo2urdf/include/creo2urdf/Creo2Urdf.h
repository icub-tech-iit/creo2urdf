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

class Creo2Urdf : public pfcUICommandActionListener {
public:
    void OnCommand() override;

    bool exportModelToUrdf(iDynTree::Model mdl, iDynTree::ModelExporterOptions options);
    bool addMeshAndExport(const std::string& link_child_name, const std::string& csys_name);

private:
    pfcModel_ptr component_handle;
    iDynTree::Model idyn_model;
};

class Creo2UrdfAccess : public pfcUICommandAccessListener {
public:
    pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override;
};

#endif // !CREO2URDF_H
