/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <OTKXSaveFeatCxx.h>
#include <wfcElemIds.h>
#include <pfcGlobal.h>
#include <OTKXEnum.h>
#include <OTKXUtils.h> 


/* CreateIntegerElem - creates an integer element */
void CreateElem::CreateIntegerElem (int id, int value, int level, wfcElements_ptr Elems)
{
	
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
 	newElemArgVal->SetIntValue(value); 
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	Elems->append(Elem);	
}

/* CreateDoubleElem - creates a double element */
void CreateElem::CreateDoubleElem (int id, double value, int level,wfcElements_ptr Elems)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
 	newElemArgVal->SetDoubleValue(value); 
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	Elems->append(Elem);
}

/* CreateBooleanElem - creates a boolean element */
void CreateElem::CreateBooleanElem (int id, bool value, int level,wfcElements_ptr Elems)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
 	newElemArgVal->SetBoolValue(value); 
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	Elems->append(Elem);
}

/* CreateASCIIStringElem - creates a ASCII string element */
void CreateElem::CreateASCIIStringElem (int id, xstring value, int level, wfcElements_ptr Elems)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
 	newElemArgVal->SetASCIIStringValue(value); 
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	Elems->append(Elem);
}

void CreateElem::CreateStringElem (int id, xstring value, int level, wfcElements_ptr Elems)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
 	newElemArgVal->SetStringValue(value); 
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	Elems->append(Elem);	
}

wfcElement_ptr CreateElem::CreateCollectionElem (int id, int level)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
	newElemArgVal->SetSelectionValue(0);  	
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	return Elem;
}

void  CreateElem::CreateSelectionElem (int id, int level, pfcModel_ptr Model, int RefType, int RefId, wfcElements_ptr Elems)
{	
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

	try
     {
        pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
	      pfcModelItem_ptr ModelItem = Model->GetItemById (pfcModelItemType(RefType), RefId);
	      pfcSelection_ptr Selection = pfcCreateModelItemSelection(ModelItem);
	      newElemArgVal->SetSelectionValue(Selection);  	
	      wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	      Elems->append(Elem);	
      }
	  OTK_EXCEPTION_HANDLER(exception_info);
    
}

void CreateElem::CreateCompoundElem (int id, int level, wfcElements_ptr Elems)
{
	wfcElement_ptr Elem = wfcElement::Create (id, 0, level); 
	Elems->append(Elem);
}

void CreateElem::CreateCurveCollectionElem (int id, int level, wfcElements_ptr Elems)
{
	wfcElement_ptr Elem = wfcElement::Create (id, 0, level); 
	wfcCollection_ptr WCollection = wfcCollection::Create ();
	Elem->SetElemCollection(WCollection );
	Elems->append(Elem);
}

void CreateElem::CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstructions_ptr WCrvCollInstrs, wfcElements_ptr Elems)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
	newElemArgVal->SetSelectionValue(0);  	
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 	
	
	wfcCurveCollection_ptr WCurveColln = wfcCurveCollection::Create();
	WCurveColln->SetInstructions(WCrvCollInstrs);	

	wfcCollection_ptr WCollection = wfcCollection::Create ();
	WCollection->SetCrvCollection (WCurveColln);	

	Elem->SetElemCollection(WCollection );
	
	Elems->append( Elem);	
}

void CreateElem::CreateSurfaceCollectionElem (int id, int level, wfcElements_ptr Elems)
{
	wfcElement_ptr Elem = wfcElement::Create (id, 0, level); 
	wfcCollection_ptr WCollection = wfcCollection::Create ();
	Elem->SetElemCollection(WCollection );
	Elems->append(Elem);
}

void CreateElem::CreateSurfaceCollectionElem (int id, int level, wfcSurfaceCollectionInstructions_ptr WSurfaceCollInstrs, wfcElements_ptr Elems)
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
	newElemArgVal->SetSelectionValue(0);  	
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 	
	
	wfcSurfaceCollection_ptr WSurfaceCollection = wfcSurfaceCollection::Create();
	WSurfaceCollection->SetInstructions(WSurfaceCollInstrs);	

	wfcCollection_ptr WCollection = wfcCollection::Create ();
	WCollection->SetSurfCollection (WSurfaceCollection);	

	Elem->SetElemCollection(WCollection );
	
	Elems->append(Elem);	
}


void CreateElem::CreateComponentModelElem (int id, int level, wfcElements_ptr Elems, xstring name, int Type)
{
	wfcElement_ptr Elem = wfcElement::Create (id, 0, level); 
	wfcSpecialValue_ptr WSpecialValue = wfcSpecialValue::Create();
	pfcSession_ptr		Session = pfcGetCurrentSession ();
	
	pfcModel_ptr WModel = Session->GetModel(name, (pfcModelType)Type);
	pfcSolid_ptr WComponentModel = pfcSolid::cast(WModel);
	WSpecialValue->SetComponentModel(WComponentModel);
	Elem->SetSpecialValueElem(WSpecialValue);
	Elems->append(Elem);

}

void  CreateElem::CreateTransformElem (int id, int level, wfcElements_ptr Elems, double Row1[], double Row2[], double Row3[], double Row4[])
{
	pfcArgValue_ptr newElemArgVal = new pfcArgValue();	
	pfcMatrix3D_ptr Matrix3D = pfcMatrix3D::create();
	for (int i=0;i<4;i++)
	{
		Matrix3D->set(0,i,Row1[i]);
	}

	for (int j=0;j<4;j++)
	{
		Matrix3D->set(1,j,Row2[j]);
	}

	for (int k=0;k<4;k++)
	{
		Matrix3D->set(2,k,Row3[k]);
	}

	for (int l=0;l<4;l++)
	{
		Matrix3D->set(3,l,Row4[l]);
	}
	pfcTransform3D_ptr value = pfcTransform3D::Create();
	value->SetMatrix(Matrix3D);
 	newElemArgVal->SetTransformValue(value); 
	wfcElement_ptr Elem = wfcElement::Create (id, newElemArgVal, level); 
	Elems->append(Elem);
}


void  CreateElem::CreateCurveCollectionElem (int id, int level,  wfcCurveCollectionInstrType InstrType,
									double value, pfcModel_ptr Model, int RefType, int RefId, wfcCurveCollectionInstrAttribute Attr, wfcElements_ptr Elems)
{	
	wfcCurveCollectionInstructions_ptr WCrvCollInstrs =  wfcCurveCollectionInstructions::create(); 

	CollectionHelpers localCollnHelper; 
	wfcCrvCollectionInstrAttributes_ptr WCrvCollInstrAttributes = localCollnHelper.CreateCurveCollInstrAttrs_One(Attr);
	pfcSelections_ptr Sels = localCollnHelper.CreateSelections_One (Model, RefType, RefId);
	wfcCurveCollectionInstruction_ptr WCrvCollInstr = localCollnHelper.CreateCurveInstruction(InstrType, value, Sels, WCrvCollInstrAttributes);
	localCollnHelper.CreateCurveCollInstrs (WCrvCollInstr, WCrvCollInstrs); 
	CreateCurveCollectionElem(wfcPRO_E_STD_CURVE_COLLECTION_APPL, level, WCrvCollInstrs, Elems); 
}


void  CreateElem::CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType,
									double value, pfcModel_ptr Model, int RefType, int RefId, wfcElements_ptr Elems)
{
	wfcCurveCollectionInstructions_ptr WCrvCollInstrs = wfcCurveCollectionInstructions::create(); 

	CollectionHelpers localCollnHelper; 
	
	pfcSelections_ptr Sels = localCollnHelper.CreateSelections_One (Model, RefType, RefId);
	wfcCurveCollectionInstruction_ptr WCrvCollInstr = localCollnHelper.CreateCurveInstruction(InstrType, value, Sels, 0);
	localCollnHelper.CreateCurveCollInstrs (WCrvCollInstr, WCrvCollInstrs); 
	CreateCurveCollectionElem(wfcPRO_E_STD_CURVE_COLLECTION_APPL, level,  WCrvCollInstrs, Elems); 
}

void  CreateElem::CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType,
									double value, wfcCurveCollectionInstrAttribute Attr, wfcElements_ptr Elems)
{

	wfcCurveCollectionInstructions_ptr WCrvCollInstrs = wfcCurveCollectionInstructions::create(); 

	CollectionHelpers localCollnHelper; 
	wfcCrvCollectionInstrAttributes_ptr WCrvCollInstrAttributes = localCollnHelper.CreateCurveCollInstrAttrs_One(Attr);
	
	wfcCurveCollectionInstruction_ptr WCrvCollInstr = localCollnHelper.CreateCurveInstruction(InstrType, value, 0, WCrvCollInstrAttributes);
	localCollnHelper.CreateCurveCollInstrs (WCrvCollInstr, WCrvCollInstrs); 
	CreateCurveCollectionElem(wfcPRO_E_STD_CURVE_COLLECTION_APPL, level, WCrvCollInstrs, Elems); 

}


void  CreateElem::CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType, double value, wfcElements_ptr Elems)
{
	wfcCurveCollectionInstructions_ptr WCrvCollInstrs = wfcCurveCollectionInstructions::create(); 

	CollectionHelpers localCollnHelper; 

	wfcCurveCollectionInstruction_ptr WCrvCollInstr = localCollnHelper.CreateCurveInstruction(InstrType, value, 0,0);
	localCollnHelper.CreateCurveCollInstrs (WCrvCollInstr, WCrvCollInstrs); 
	CreateCurveCollectionElem(wfcPRO_E_STD_CURVE_COLLECTION_APPL, level, WCrvCollInstrs, Elems); 
}

void CreateElem::CreateSurfaceCollectionElem (int id, int level, wfcSurfaceCollectionRefType CollRefType, pfcModel_ptr Model, int RefType, int RefId, 
									wfcSurfaceCollectionInstrType InstrType, bool Include, wfcElements_ptr Elems)
{
	CollectionHelpers localCollnHelper;
	wfcSurfaceCollectionInstructions_ptr WSurfaceCollInstrs =  wfcSurfaceCollectionInstructions::create(); 
	wfcSurfaceCollectionReferences_ptr WSrfCollRefs = wfcSurfaceCollectionReferences::create();
	wfcSurfaceCollectionReference_ptr WSrfCollRef = localCollnHelper.CreateSurfCollReference (CollRefType, Model, RefType, RefId);
	WSrfCollRefs->append(WSrfCollRef);
	wfcSurfaceCollectionInstruction_ptr WSurfaceCollInstr = localCollnHelper.CreateSurfCollInstr (InstrType, Include, WSrfCollRefs);
	WSurfaceCollInstrs->append(WSurfaceCollInstr);
	CreateSurfaceCollectionElem(wfcPRO_E_STD_SURF_COLLECTION_APPL, level, WSurfaceCollInstrs, Elems); 
}

wfcCrvCollectionInstrAttributes_ptr CollectionHelpers::CreateCurveCollInstrAttrs (int Attr[], int size)
{
	wfcCrvCollectionInstrAttributes_ptr Attrs = wfcCrvCollectionInstrAttributes::create();
	for (int i=0;i<size;i++)
	{
		wfcCrvCollectionInstrAttribute_ptr CrvAttr = wfcCrvCollectionInstrAttribute::Create ();
		CrvAttr->SetAttribute(wfcCurveCollectionInstrAttribute (Attr[i]));
		Attrs->append (CrvAttr);
	}

	return (Attrs);
}


wfcCrvCollectionInstrAttributes_ptr CollectionHelpers::CreateCurveCollInstrAttrs_One (int Attr)
{
	wfcCrvCollectionInstrAttributes_ptr Attrs = wfcCrvCollectionInstrAttributes::create();
	wfcCrvCollectionInstrAttribute_ptr CrvAttr = wfcCrvCollectionInstrAttribute::Create ();
	CrvAttr->SetAttribute(wfcCurveCollectionInstrAttribute (Attr));
	Attrs->append (CrvAttr);

	return (Attrs);
}


wfcCurveCollectionInstruction_ptr CollectionHelpers::CreateCurveInstruction (int Type, double value,
						pfcSelections_ptr References, wfcCrvCollectionInstrAttributes_ptr Attrs)
{
	wfcCurveCollectionInstruction_ptr CrvInstr = wfcCurveCollectionInstruction::Create (wfcCurveCollectionInstrType (Type));
	CrvInstr->SetValue (value);
	CrvInstr->SetReferences (References);
	CrvInstr->SetAttributes(Attrs);

	return (CrvInstr);
}

pfcSelection_ptr CollectionHelpers::CreateSelection (pfcModel_ptr Model, int Type, int Id)
{
	pfcModelItem_ptr ModelItem = Model->GetItemById (pfcModelItemType(Type), Id);

	pfcSelection_ptr Selection = pfcCreateModelItemSelection(ModelItem);

	return (Selection);

}

void CollectionHelpers::CreateSelections (pfcModel_ptr Model, int Type, int Id, pfcSelections_ptr Sels)
{
	pfcModelItem_ptr ModelItem = Model->GetItemById (pfcModelItemType(Type), Id);

	pfcSelection_ptr Selection = pfcCreateModelItemSelection(ModelItem);

	Sels->append(Selection);
}

pfcSelections_ptr CollectionHelpers::CreateSelections_One (pfcModel_ptr Model, int Type, int Id)
{
	pfcSelections_ptr Sels = pfcSelections::create();

	pfcModelItem_ptr ModelItem = Model->GetItemById (pfcModelItemType(Type), Id);

	pfcSelection_ptr Selection = pfcCreateModelItemSelection(ModelItem);

	Sels->append(Selection);

	return (Sels);
}


void CollectionHelpers::CreateCurveCollInstrs (wfcCurveCollectionInstruction_ptr CrvCollInstr, wfcCurveCollectionInstructions_ptr WCrvCollInstrs)
{
	WCrvCollInstrs->append(CrvCollInstr);
}

wfcSurfaceCollectionInstruction_ptr CollectionHelpers::CreateSurfCollInstr (wfcSurfaceCollectionInstrType Type, bool Include, wfcSurfaceCollectionReferences_ptr Refs)
{
	wfcSurfaceCollectionInstruction_ptr WSrfCollInstr = wfcSurfaceCollectionInstruction::Create(Type);
	WSrfCollInstr->SetInclude(Include);
	WSrfCollInstr->SetSrfCollectionReferences (Refs);

	return (WSrfCollInstr);
}

wfcSurfaceCollectionReference_ptr CollectionHelpers::CreateSurfCollReference (wfcSurfaceCollectionRefType RefType, pfcModel_ptr Model, int Type, int Id)
{
	wfcSurfaceCollectionReference_ptr WSrfCollRef = wfcSurfaceCollectionReference::Create (RefType);
	pfcSelection_ptr Sel = CreateSelection (Model, Type, Id);
	WSrfCollRef->SetReference (Sel);
	return (WSrfCollRef);
}


