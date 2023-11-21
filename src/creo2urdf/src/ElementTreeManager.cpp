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

ElementTreeManager::ElementTreeManager(pfcFeature_ptr feat)
{
    buildElementTree(feat);
}

ElementTreeManager::~ElementTreeManager() {}

bool ElementTreeManager::buildElementTree(pfcFeature_ptr feat)
{
    wfcWFeature_ptr wfeat = wfcWFeature::cast(feat);

    tree = wfeat->GetElementTree(nullptr, wfcFEAT_EXTRACT_NO_OPTS);

    child_name = retrieveChildName();

    auto parents = wfeat->GetExternalParents(wfcExternalReferenceType::wfcALL_REF_TYPES);

    if (parents == NULL) {

        return true;
    }

    for (int l = 0; l < parents->getarraysize(); l++) {

        auto extrefs = parents->get(l)->GetExtRefs();

        if (extrefs == NULL) {
            printToMessageWindow(child_name + " has no parent links");
            return true;
        }

        // each element in this array is given by a constraint and
        // the number of parts that compose it
        // e.g. 2 parts x 3 constraints = size(extrefs) = 6
        // We assume there are only two parts and they are named differently
        for (int m = 0; m < extrefs->getarraysize(); m++) {
            auto extref_name = std::string(extrefs->get(m)->GetAsmcomponents()->GetPathToRef()->GetLeaf()->GetFullName());
            if (extref_name != child_name)
            {
                parent_name = extref_name;
            }
        }
    }

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
        printToMessageWindow("Invalid constraint type", c2uLogLevel::WARN);
        return -1;
    }
    xcatchend
}

std::string ElementTreeManager::getParentName()
{
    if (tree == nullptr)
    {
        return "";
    }

    return parent_name;
}

std::string ElementTreeManager::retrieveChildName()
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
