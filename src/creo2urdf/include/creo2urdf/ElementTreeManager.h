

#include <creo2urdf/Utils.h>

class ElementTreeManager {

public:

    bool buildElementTree(pfcFeature_ptr feat)
    {
        wfcWFeature_ptr wfeat = wfcWFeature::cast(feat);
        tree = wfeat->GetElementTree(nullptr, wfcFEAT_EXTRACT_NO_OPTS);

        auto parents = wfeat->GetExternalParents(wfcExternalReferenceType::wfcALL_REF_TYPES);

        if (parents != NULL) {

            for (int l = 0; l < parents->getarraysize(); l++) {

                auto extrefs = parents->get(l)->GetExtRefs();

                if (extrefs != NULL) {

                    for (int m = 0; m < extrefs->getarraysize(); m++) {
                        parent_name = std::string( extrefs->get(m)->GetAsmcomponents()->GetPathToRef()->GetLeaf()->GetFileName() );
                        printToMessageWindow(parent_name);
                    }

                }
            }
        }

        return true;
    }
    
    
    int getConstraintType()
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

        wfcElementPath_ptr constraintPath = wfcElementPath::Create(elemItems);

        wfcElement_ptr element = tree->GetElement(constraintPath);

        return element->GetValue()->GetIntValue();
    }


    std::string getParentPart() 
    {
        if (tree == nullptr)
        {
            return "";
        }

        return parent_name;



        std::string xml_parent = "";

        try {
            //wfcElemPathItems_ptr elemItems = wfcElemPathItems::create();
            //wfcElemPathItem_ptr Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SETS);
            //elemItems->append(Item);
            //Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_SET);
            //elemItems->append(Item);
            //Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_SETS);
            //elemItems->append(Item);
            //Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_SET);
            //elemItems->append(Item);
            //Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_REFS);
            //elemItems->append(Item);
            //Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_GREEN_REF);
            //elemItems->append(Item);
            ////Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_XML_);
            ////elemItems->append(Item);
            ////Item = wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID, wfcPRO_E_COMPONENT_JAS_REFS);
            ////elemItems->append(Item);

            //wfcElementPath_ptr path = wfcElementPath::Create(elemItems);

            //wfcElement_ptr element = tree->GetElement(path);
            //xml_parent = "";

            auto elements = tree->ListTreeElements();
            for (xint i = 0; i < elements->getarraysize(); i++)
            {
                auto element = elements->get(i);
                printToMessageWindow("ID:" + to_string(element->GetId()));
            }

        }
        xcatchbegin
        xcatchcip(defaultEx)
        {
            return "";
        }
        xcatchend

        return xml_parent;
    }

private:
    wfcElementTree_ptr tree = nullptr;
    std::string parent_name;

};