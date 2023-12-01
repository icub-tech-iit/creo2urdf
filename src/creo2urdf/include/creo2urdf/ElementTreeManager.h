/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Utils.h>

static const std::map<ProAsmcompSetType, JointType> proAsmCompSetType_to_JointType
{
    {PRO_ASM_SET_TYPE_PIN, JointType::Revolute},
    {PRO_ASM_SET_TYPE_SLIDER, JointType::Linear},
    {PRO_ASM_SET_TYPE_BALL, JointType::Spherical},
    {PRO_ASM_SET_TYPE_FIXED, JointType::Fixed},
    {PRO_ASM_SET_TYPE_WELD, JointType::Fixed},
    {PRO_ASM_SET_USER_DEFINED_TYPE, JointType::Fixed} /* WE ASSUME THAT A USER DEFINED IS FIXED */
};


class ElementTreeManager {

public:
    ElementTreeManager();
    ElementTreeManager(pfcFeature_ptr feat, std::map<std::string, JointInfo>& joint_info_map);

    ~ElementTreeManager();

    bool populateJointInfoFromElementTree(pfcFeature_ptr feat, std::map<std::string, JointInfo>& joint_info_map);
    int getConstraintType();
    std::string getParentName();
    std::string getChildName();

private:
    wfcElementTree_ptr tree = nullptr;
    wfcWFeature_ptr wfeat = nullptr;
    pfcSolid_ptr parent_solid;
    pfcSolid_ptr child_solid;
   /* std::string retrieveCommonDatumName(pfcModelItemType type);*/
    string getConstraintDatum(pfcFeature_ptr feat, pfcComponentConstraintType constraint_type, pfcModelItemType datum_type);
    bool retrieveSolidReferences();
    std::string retrievePartName();
    std::pair<double, double> retrieveLimits(pfcFeature_ptr feat);
};
