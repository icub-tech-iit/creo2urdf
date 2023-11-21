/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Utils.h>

class ElementTreeManager {

public:
    ElementTreeManager();
    ElementTreeManager(pfcFeature_ptr feat);

    ~ElementTreeManager();

    bool buildElementTree(pfcFeature_ptr feat);
    int getConstraintType();
    std::string getParentName();

private:
    wfcElementTree_ptr tree = nullptr;
    std::string parent_name;
    std::string child_name;

    std::string retrieveChildName();

};
