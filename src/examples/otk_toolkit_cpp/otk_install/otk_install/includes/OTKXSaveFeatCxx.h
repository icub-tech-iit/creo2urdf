/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXSAVEFEATCXX_H
#define OTKXSAVEFEATCXX_H



#include <wfcElementTree.h>
#include <wfcCollection.h>


/*************************************************************************************/
/* This class creates elements of different types */
/*************************************************************************************/
class CreateElem
{
public:
	/* CreateIntegerElem - creates an integer element */
	void  CreateIntegerElem (int id, int value, int level, wfcElements_ptr Elems);

	/* CreateDoubleElem - creates a double element */
	void  CreateDoubleElem (int id, double value, int level, wfcElements_ptr Elems);

	/* CreateBooleanElem - creates a boolean element */
	void  CreateBooleanElem (int id, bool value, int level, wfcElements_ptr Elems);

	/* CreateASCIIStringElem - creates a ASCII string element */
	void  CreateASCIIStringElem (int id, xstring value, int level, wfcElements_ptr Elems);

	/* CreateStringElem - creates a string element */
	void  CreateStringElem (int id, xstring value, int level,wfcElements_ptr Elems);

	/* CreateComponentModelElem - creates a PRO_E_COMPONENT_MODEL element 
	This is a special value element */
	void CreateComponentModelElem (int id, int level,wfcElements_ptr Elems, xstring name, int Type);

	/* Methods for creating surface/curve collection elements */
	wfcElement_ptr   CreateCollectionElem (int id, int level);
	void  CreateCurveCollectionElem (int id, int level,wfcElements_ptr Elems);
	void  CreateCurveCollectionElem (int id, int level, 
									wfcCurveCollectionInstructions_ptr WCrvCollInstrs, 
									wfcElements_ptr Elems);	
	void CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType,
									double value, pfcModel_ptr Model, int RefType, int RefId, 
									wfcCurveCollectionInstrAttribute Attr, wfcElements_ptr Elems);

	void CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType, double value, 
									pfcModel_ptr Model, int RefType, int RefId, 
									wfcElements_ptr Elems);

	void CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType, double value, 
									wfcCurveCollectionInstrAttribute Attr, wfcElements_ptr Elems);

	void CreateCurveCollectionElem (int id, int level, wfcCurveCollectionInstrType InstrType, double value, 
									wfcElements_ptr Elems);



	void  CreateSurfaceCollectionElem (int id, int level,  wfcElements_ptr Elems);
	void  CreateSurfaceCollectionElem (int id, int level, 
										wfcSurfaceCollectionInstructions_ptr WSurfaceCollInstrs, 
										wfcElements_ptr Elems);

	void CreateSurfaceCollectionElem (int id, int level, 
									  wfcSurfaceCollectionRefType CollRefType, 
									  pfcModel_ptr Model, int RefType, int RefId, 
									  wfcSurfaceCollectionInstrType InstrType, 
									  bool Include, wfcElements_ptr Elems);

	/* CreateSelectionElem - creates a selection element */
	void  CreateSelectionElem (int id, int level, pfcModel_ptr Model, int RefType,
							   int RefId, wfcElements_ptr Elems);

	/* CreateCompoundElem - creates a compound element */
	void  CreateCompoundElem (int id, int level, wfcElements_ptr Elems);

	/* CreateTransformElem - creates a transform element */
	void  CreateTransformElem (int id, int level, wfcElements_ptr Elems, 
							   double Row1[], double Row2[], double Row3[], double Row4[]);	

};

/*************************************************************************************/
/* This class has helper methods for creating surface/curve collection elements */
/*************************************************************************************/
class CollectionHelpers
{
public:
	wfcCrvCollectionInstrAttributes_ptr CreateCurveCollInstrAttrs (int Attr[], int size);
	wfcCrvCollectionInstrAttributes_ptr CreateCurveCollInstrAttrs_One (int Attr);
	wfcCurveCollectionInstruction_ptr CreateCurveInstruction (int Type, double value,
															  pfcSelections_ptr References, 
															  wfcCrvCollectionInstrAttributes_ptr Attrs);

	void CreateCurveCollInstrs (wfcCurveCollectionInstruction_ptr CrvCollInstr, 
								wfcCurveCollectionInstructions_ptr WCrvCollInstrs);
	pfcSelection_ptr CreateSelection (pfcModel_ptr Model, int Type, int Id);
	void CreateSelections (pfcModel_ptr Model, int Type, int Id, pfcSelections_ptr Sels);
	pfcSelections_ptr CreateSelections_One (pfcModel_ptr Model, int Type, int Id);

	wfcSurfaceCollectionInstruction_ptr CreateSurfCollInstr (
															 wfcSurfaceCollectionInstrType Type, 
															 bool Include, 
															 wfcSurfaceCollectionReferences_ptr Refs);
	wfcSurfaceCollectionReference_ptr CreateSurfCollReference (
															   wfcSurfaceCollectionRefType RefType, 
															   pfcModel_ptr Model, int Type, int Id);

};


#endif /* OTKXSAVEFEATCXX_H */
