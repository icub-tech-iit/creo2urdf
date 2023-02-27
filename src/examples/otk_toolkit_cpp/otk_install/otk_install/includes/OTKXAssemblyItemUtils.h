/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXASSEMBLYITEMUTILS_H
#define OTKXASSEMBLYITEMUTILS_H




#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <wfcSolid.h>
#include <pfcSession.h>
#include <pfcModel.h>
#include <wfcPart.h>
#include <wfcAssembly.h>
#include <wfcComponentFeat.h>
#include <OTKXUtils.h> 

#include <fstream>

//***********************************************************
wfcAssemblyItems_ptr OTKXUtilPopulateAssemblyItemsDimensionArray(
			wfcAssemblyItems_ptr AsmItemArray,		
			wfcWAssembly_ptr WAsm,
			pfcModel_ptr ItemOwner, 
			xintsequence_ptr DimIds, 
			wfcWComponentPath_ptr ItemCompPath,
			ofstream &fp_out_asmitem);

//***********************************************************
wfcAssemblyItems_ptr OTKXUtilPopulateAssemblyItemsFeatureArray(
			wfcAssemblyItems_ptr AsmItemArray,		
			wfcWAssembly_ptr WAsm,
			pfcModel_ptr ItemOwner, 
			xintsequence_ptr FeatIds, 
			wfcWComponentPath_ptr ItemCompPath,
			ofstream &fp_out_asmitem);

//***********************************************************
wfcWComponentFeat_ptr OTKXUtilSetFlexibleComponent(pfcSolid_ptr OwnerSolid, 
	xint FeatId, wfcAssemblyItems_ptr AsmItemArray, ofstream &fp_out_asmitem);

//***********************************************************
pfcModel_ptr OTKXUtilCreateFlexibleModel (pfcSolid_ptr OwnerSolid, 
	xint FeatId, wfcWComponentPath_ptr WAsmCompPath, 
	wfcAssemblyItems_ptr AsmItemArray, ofstream &fp_out_asmitem);


//***********************************************************
void OTKXUtilCreatePredefinedFlexibilityComponent(pfcSolid_ptr OwnerSolid, 
	xint FeatId, wfcAssemblyItems_ptr AsmItemArray, pfcModel_ptr ItemOwner, 
	ofstream &fp_out_asmitem);


#endif /* OTKXCROSSSECTION_H */
