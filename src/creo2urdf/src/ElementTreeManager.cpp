/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/ElementTreeManager.h>

ElementTreeManager::ElementTreeManager()
{}

ElementTreeManager::ElementTreeManager(pfcFeature_ptr feat, std::map<std::string, JointInfo>& joint_info_map)
{
    if (!populateJointInfoFromElementTree(feat, joint_info_map))
    {
        printToMessageWindow("Feature does not support element trees!", c2uLogLevel::WARN);
    }
}

ElementTreeManager::~ElementTreeManager() {}

bool ElementTreeManager::populateJointInfoFromElementTree(pfcFeature_ptr feat, std::map<std::string, JointInfo>& joint_info_map)
{
    wfeat = wfcWFeature::cast(feat);

    try
    {
        tree = wfeat->GetElementTree(nullptr, wfcFEAT_EXTRACT_NO_OPTS);
    }
    xcatchbegin
    xcatchcip(pfcXToolkitInvalidType)
    {
        return false;
    }
    xcatchend
    
    JointInfo joint;

    if (!retrieveSolidReferences())
        return false;

    joint.child_link_name = getChildName();
    joint.parent_link_name = getParentName();
    joint.type = proAsmCompSetType_to_JointType.at(static_cast<ProAsmcompSetType>(getConstraintType()));

    if (joint.type == JointType::Revolute)
    {
        joint.datum_name = retrieveCommonDatumName(pfcModelItemType::pfcITEM_AXIS);
    }
    else if (joint.type == JointType::Fixed)
    {
        joint.datum_name = retrieveCommonDatumName(pfcModelItemType::pfcITEM_COORD_SYS);
    }

    joint_info_map.insert({ joint.datum_name, joint });

    return true;
}

int ElementTreeManager::getConstraintType()
{
    if (tree == nullptr)
    {
        return -1;
    }

    wfcElemPathItems_ptr elemItems = wfcElemPathItems::create();
    wfcElemPathItem_ptr Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SETS);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SET);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SET_TYPE);
    elemItems->append(Item);

    try {
        return tree->GetElement(wfcElementPath::Create(elemItems))->GetValue()->GetIntValue();
    }
    xcatchbegin
    xcatchcip(pfcXBadGetArgValue)
    {
        printToMessageWindow("Invalid constraint data type in element tree!", c2uLogLevel::WARN);
        return -1;
    }
    xcatchend
}

std::string ElementTreeManager::retrieveCommonDatumName(pfcModelItemType type)
{
    auto child_datums = getSolidDatumNames(child_solid, type);
    auto parent_datums = getSolidDatumNames(parent_solid, type);

    // sort to reduce search complexity
    std::sort(child_datums.begin(), child_datums.end());
    std::sort(parent_datums.begin(), parent_datums.end());

    std::vector<std::string> matched_datums;

    // Find the common elements using set_intersection
    std::set_intersection(child_datums.begin(), child_datums.end(),
        parent_datums.begin(), parent_datums.end(),
        std::back_inserter(matched_datums));

    matched_datums.erase(remove_if(matched_datums.begin(), matched_datums.end(),
        [](string element) {return (element == "CSYS" || element == "ASM_CSYS" || element.empty()); }), matched_datums.end()
    );

    if (matched_datums.size() == 0)
    {
        printToMessageWindow(getParentName() + " and " + getChildName() +
            " have no axes in common!", c2uLogLevel::WARN);
        return "";
    }

    if (matched_datums.size() > 1)
    {
        printToMessageWindow(getParentName() + " and " + getChildName() +
            " have " + to_string(matched_datums.size()) + " datums in common!", c2uLogLevel::WARN);
    }

    return matched_datums[0];
}

std::string ElementTreeManager::getParentName()
{
    if (tree == nullptr)
    {
        return "";
    }
    try{
        return  std::string(parent_solid->GetFullName());
    }
    xcatchbegin
    xcatchcip(defaultEx)
    {
        return "";
    }
    xcatchend
}

std::string ElementTreeManager::getChildName()
{
    if (tree == nullptr)
    {
        return "";
    }
    try {

    return std::string(child_solid->GetFullName());
    }
    xcatchbegin
    xcatchcip(defaultEx)
    {
        return "";
    }
    xcatchend
}

bool ElementTreeManager::retrieveSolidReferences()
{
    auto parents = wfeat->GetExternalParents(wfcExternalReferenceType::wfcALL_REF_TYPES);

    if (parents == NULL) {

        return false;
    }

    for (int l = 0; l < parents->getarraysize(); l++)
    {
        auto extrefs = parents->get(l)->GetExtRefs();

        if (extrefs == NULL) {
            return false;
        }

        // each element in this array is given by a constraint and
        // the number of parts that compose it
        // e.g. 2 parts x 3 constraints = size(extrefs) = 6
        // We assume there are only two parts and they are named differently

        if (extrefs->getarraysize() == 0)
            return false;

        for (int m = 0; m < extrefs->getarraysize(); m++) {
            auto extref = extrefs->get(m)->GetAsmcomponents()->GetPathToRef()->GetLeaf();

            if (std::string(extref->GetFullName()) != retrievePartName())
            {
                parent_solid = extref;
            }
            if (std::string(extref->GetFullName()) == retrievePartName())
            {
                child_solid = extref;
            }
        }
    }

    return true;
}

std::string ElementTreeManager::retrievePartName()
{
    if (tree == nullptr)
    {
        return "";
    }

    wfcElemPathItems_ptr elemItems = wfcElemPathItems::create();
    elemItems->append(wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_MODEL));

    // The element tree shows the child name, aka the current part as 
    // <PRO_E_COMPONENT_MODEL type = "pointer" application = "model"> <name_of_part>.prt < / PRO_E_COMPONENT_MODEL>
    // and it cannot be retrieved with GetValue() because it throws an exception
    try {
        wfcElement_ptr element = tree->GetElement(wfcElementPath::Create(elemItems));
        return std::string(element->GetSpecialValueElem()->GetComponentModel()->GetFullName());
    }
    xcatchbegin
    xcatchcip(defaultEx)
    {
        return "";
    }
    xcatchend
}

std::pair<double, double> ElementTreeManager::retrieveLimits(pfcFeature_ptr feat)
{
    wfcElemPathItems_ptr elemItems = wfcElemPathItems::create();
    wfcElemPathItem_ptr Item;
    wfcElement_ptr element;

    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SETS);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SET);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_SETS);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_SET);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_MAX_LIMIT);
    elemItems->append(Item);
    Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_MAX_LIMIT_VAL);
    elemItems->append(Item);

    wfcElementPath_ptr limitpath = wfcElementPath::Create(elemItems);
    element = tree->GetElement(limitpath);

    double max = element->GetValue()->GetDoubleValue();

     elemItems->clear();

     Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SETS);
     elemItems->append(Item);
     Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SET);
     elemItems->append(Item);
     Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_SETS);
     elemItems->append(Item);
     Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_SET);
     elemItems->append(Item);
     Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_MIN_LIMIT);
     elemItems->append(Item);
     Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_MIN_LIMIT_VAL);
     elemItems->append(Item);

     limitpath = wfcElementPath::Create(elemItems);
     element = tree->GetElement(limitpath);

     double min = element->GetValue()->GetDoubleValue();

    return std::pair<double, double>(min, max);
}
