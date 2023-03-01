/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXUTILS_H
#define OTKXUTILS_H



#include <fstream>
#include <pfcExceptions.h>
#include <pfcLayer.h>

#include <wfcLayerState.h>
#include <wfcSolid.h>
#include <wfcCombState.h>
#include <wfcGeometry.h>
#include <wfcSession.h>

/* Writes exception(first parameter in macro) into file (second parameter in macro) */
#define OTK_EXCEPTION_WRITE(pfcex, fp_out) \
{ \
	xstring msg = pfcex->GetMessage(); \
    xstring method = pfcex->GetMethodName(); \
    xint err = pfcex->GetErrorCode(); \
    fp_out << "pfcXToolkitError in method " << method << " with error " << err << "\n message " << msg << "\n" << endl; \
	fp_out.close(); \
}

/* Prints exception on terminal */
#define OTK_EXCEPTION_PRINT(pfcex) \
{ \
	xstring msg = pfcex->GetMessage(); \
    xstring method = pfcex->GetMethodName(); \
    xint err = pfcex->GetErrorCode(); \
    cout << "pfcXToolkitError in method " << method << " with error " << err << "\n message " << msg << "\n" << endl; \
}

/* Writes uncaught/default exception into file (second parameter in macro) */
#define OTK_DEFAULT_EXCEPTION_WRITE(defaultex, fp_out) \
{ \
	xstring msg = defaultex->getCipTypeName(); \
    fp_out << "Uncaught " << msg << " exception \n" << endl; \
	fp_out.close(); \
}

/* Prints exception on terminal */
#define OTK_DEFAULT_EXCEPTION_PRINT(ftypeex) \
{ \
	xstring msg = ftypeex->getCipTypeName(); \
    cout << "Uncaught " << msg << " exception \n" << endl; \
}

/* Writes invalid file info into file (second parameter in macro) */
#define OTK_FILENAME_EXCEPTION_WRITE(fname, fp_out) \
{ \
	xstring msg = fname->GetMessage(); \
    fp_out << "Invalid File type: " << msg << "\n" << endl; \
	fp_out.close(); \
}

/* Prints invalid file info on terminal */
#define OTK_FILENAME_EXCEPTION_PRINT(fname) \
{ \
	xstring msg = fname->GetMessage(); \
    cout << "Invalid File type: " << msg << "\n" << endl; \
}

/* Writes invalid mdl extension info into file (second parameter in macro) */
#define OTK_MDLEXT_EXCEPTION_WRITE(mdlext, fp_out) \
{ \
	xstring msg = mdlext->GetMessage(); \
    fp_out << "Invalid File type: " << msg << "\n" << endl; \
	fp_out.close(); \
}

/* Prints invalid mdl extension info on terminal */
#define OTK_MDLEXT_EXCEPTION_PRINT(mdlext) \
{ \
	xstring msg = mdlext->GetMessage(); \
    cout << "Invalid File type: " << msg << "\n" << endl; \
}

#define OTK_EXCEPTION_HANDLER(fp_out) \
xcatchbegin \
xcatch (pfcXToolkitError, pfcex) {\
OTK_EXCEPTION_PRINT(pfcex) \
OTK_EXCEPTION_WRITE(pfcex, fp_out) }\
xcatch (pfcXInvalidFileName, pfcex) {\
OTK_FILENAME_EXCEPTION_PRINT(pfcex) \
OTK_FILENAME_EXCEPTION_WRITE(pfcex, fp_out) }\
xcatch (pfcXUnknownModelExtension, pfcex) {\
OTK_MDLEXT_EXCEPTION_PRINT(pfcex) \
OTK_MDLEXT_EXCEPTION_WRITE(pfcex, fp_out) }\
xcatchcip(pfcex) {\
OTK_DEFAULT_EXCEPTION_PRINT(pfcex) \
OTK_DEFAULT_EXCEPTION_WRITE(pfcex, fp_out) }\
xcatchend


// Utility to create LayerState
wfcLayerState_ptr OTKUtilCreateLayerState (wfcWSolid_ptr WSolid, xstring wLayerStateName, ofstream&  fp_out_layerstate);

// Utility to delete LayerState
int otkUtilDeleteLayerState (wfcLayerState_ptr WLayerState, ofstream&  fp_out_layerstate);

// Utility to update LayerState
// To take input through UI pass UpdateAct = -1, LayerName = xstringnil,  ItemNum = -1 and ItemAction = -1
int otkUtilUpdateLayerState (wfcWSolid_ptr WSolid, wfcLayerState_ptr WLayerState, int UpdateAct,  xstring LayerName, int ItemNum, int ItemAction, ofstream&  fp_out_layerstate);

// Utility to print LayerState info
int otkUtilLayerStateInfo (wfcWSolid_ptr WSolid, wfcLayerState_ptr WLayerState, ofstream& InfoFile, ofstream&  fp_out_layerstate);

// Utility to select layer state from solid
// To select LayerState through UI pass StateNum = -1
wfcLayerState_ptr otkUtilSelectLayerState (wfcWSolid_ptr WSolid, int StateNum, ofstream&  fp_out_layerstate);

// Utility to get layer from solid using layer name
pfcLayer_ptr otkUtilGetLayerExist(wfcWSolid_ptr WSolid, xstring ExistingLayerName);

//Utility to create new combined state
wfcCombState_ptr otkUtilCreateCombinedState (wfcWSolid_ptr myWSolid, xstring NewCombStateName, xbool IsRegTest, ofstream& fp_out_combstate);

// Utility to select combined state from solid
// To select CombState through UI pass StateNum = -1
wfcCombState_ptr otkUtilSelectCombinedState (wfcWSolid_ptr WSolid, int CombStateNum, ofstream&  fp_out_layerstate);

// Utility to write combined state info
int OtkUtilCombinedStateInfo (wfcWSolid_ptr WSolid, wfcCombState_ptr WCombState, ofstream& InfoFile, ofstream& fp_out_combstate);

// Utility to write SimpRep info
int OtkUtilSimpRepInfo(wfcWSimpRep_ptr WSimpRep, ofstream& InfoFile, ofstream& fp_out_combstate);

// Utility to write exploded state info
int OtkUtilExplodedStateInfo(wfcWExplodedState_ptr wExpldState, ofstream& InfoFile, ofstream& fp_out_combstate);

// Utility to write style state info
int OtkUtilStyleStateInfo(wfcWSolid_ptr WSolid, wfcStyleState_ptr WstyleState, ofstream& InfoFile, ofstream& fp_out_combstate);

// Utility to write SimpRep info
int OtkUtilViewInfo(pfcView_ptr wView, ofstream& InfoFile, ofstream& fp_out_combstate);

// Utility to write InterfaceData info
void OtkUtilInterfaceDataInfo(wfcInterfaceData_ptr wInterfaceData, ofstream& InterfInfoFile);

// Utility to write Surface info
void OtkUtilSurfaceDescriptorInfo(wfcWSurfaceDescriptor_ptr SurfDesc, ofstream& SurfInfoFile);

// Utility to write EdgeSurface info
void OtkUtilEdgeSurfaceDataInfo(wfcEdgeSurfaceData_ptr SurfDesc, ofstream& SurfInfoFile);

// Utility to write TransSurface info
void OtkUtilTransSurfaceDescriptorInfo (pfcTransformedSurfaceDescriptor_ptr TransDesc, ofstream& SurfInfoFile);

// Utility to write Curve info
void OtkUtilCurveDescriptorInfo (pfcCurveDescriptor_ptr CurveDesc, ofstream& CurveInfoFile);

// Utility to write CompCurve info
void OtkUtilCompCurveDescriptorInfo (pfcCurveDescriptor_ptr CurveDesc, ofstream& CCurveInfoFile);

// Utility to write spline info
void OtkUtilSplineDescriptorInfo(pfcSplineDescriptor_ptr SplineDesc, ofstream& SplineInfoFile);

// Utility to write Spline Surface info
void OtkUtilSplineSurfDescriptorInfo(pfcSplineSurfaceDescriptor_ptr cSpline, ofstream& SurfInfoFile);

// Utility to write AccuracyType Enum info
char* OtkUtilAccuracyTypeEnumInfo(xint Id);

// Utility to write ContourTraversal Enum info
char* OtkUtilContourTraversalEnumInfo(xint Id);

// Utility to write SurfaceOrientation Enum info
char* OtkUtilSurfaceOrientationEnumInfo(xint Id);


// Utility for writing session info
int OtkUtilWriteSessionInfo (
       wfcWSession_ptr wSession, // input
       ofstream&       InfoFile, // input
       ofstream&       LogFile,  // input
       int             level );  // input - level details of info to be written

// Utility for writing model info
int OtkUtilWriteModelInfo (
       wfcWSession_ptr wSession, // input
       wfcWModel_ptr   wModel,   // input
       ofstream&       InfoFile, // input
       ofstream&       LogFile,  // input
       int             level );  // input - level details of info to be written

// Adds Session examples related commands
void OTKXSessionExamplesAddUICommands ( wfcWSession_ptr wSession );

// Adds Model examples related commands
void OTKXModelExamplesAddUICommands ( wfcWSession_ptr wSession );

// Utility to show/write Family Table info
int OtkUtilFamilyTableInfo (wfcWSolid_ptr pFTModel, ofstream& infoFile, ofstream&  fp_out_familytable);

// Utility to Erase Family Table
int OtkUtilFamilyTableErase (wfcWSolid_ptr wSolid, ofstream&  fp_out_familytable);

// Utility to dump wfcWFamilyTableRow info
int OtkUtilFamilyTableRowInfo(wfcWFamilyTableRow_ptr FamTableRow, ofstream& infoFile, ofstream&  fp_out_familytable);


#endif /* OTKXUTILS_H */
