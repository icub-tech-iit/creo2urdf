/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Example for layer state.
  
*/


#include <pfcSession.h>
#include <pfcModel.h>
#include <pfcGlobal.h>
#include <pfcFeature.h>
#include <pfcSolid.h>
#include <pfcUI.h>
#include <pfcLayer.h>

#include <ciplib.h>

#include <wfcSolid.h>
#include <wfcPart.h>
#include <wfcAssembly.h>
#include <wfcCombState.h>
#include <pfcExceptions.h>
#include <wfcGlobal.h> 
#include <OTKXUtils.h> 

#include <fstream>

ofstream fp_out_layerstate;


// Creates new layer state
extern "C" wfcStatus otkCreateLayerState ()
{
	try
	{
		fp_out_layerstate.open("LayerState.txt", ios::out);
		fp_out_layerstate << "- Layer State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		if(WSolid == NULL)
		{
			fp_out_layerstate.close();
			return wfcTK_NO_ERROR;
		}

		Session->UIDisplayMessage("layer_state.txt", "USER New layer state Name", 0);
		xstring wLayerStateName = Session->UIReadStringMessage();
		OTKUtilCreateLayerState (WSolid, wLayerStateName, fp_out_layerstate);
		fp_out_layerstate.close();
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_layerstate);
	fp_out_layerstate.close();
	return wfcTK_NO_ERROR;
}

// Update existing layer state
extern "C" wfcStatus otkUpdateLayerState ()
{
	try
	{
		fp_out_layerstate.open("LayerState.txt", ios::out);
		fp_out_layerstate << "- Layer State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcLayerState_ptr WLayerState = otkUtilSelectLayerState(WSolid, (int) -1, fp_out_layerstate);
		if(WLayerState != NULL)
			otkUtilUpdateLayerState(WSolid, WLayerState, -1, xstringnil, -1, -1, fp_out_layerstate);

		fp_out_layerstate.close();
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_layerstate);
	fp_out_layerstate.close();
	return wfcTK_NO_ERROR;
}

// Shows layer state info into Pro/E browser window
extern "C" wfcStatus otkLayerStateInfo ()
{
	try
	{
		xstring line;
		xstring InfoFile_name;
        
		fp_out_layerstate.open("LayerState.txt", ios::out);
		fp_out_layerstate << "- Layer State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcLayerState_ptr WLayerState = otkUtilSelectLayerState(WSolid, (int) -1, fp_out_layerstate);
		if(WLayerState != NULL)
		{
			InfoFile_name = "LayerState_info.txt";
			ofstream fp_InfoFile;

			fp_InfoFile.open((const char *)InfoFile_name, ios::out);
			otkUtilLayerStateInfo(WSolid, WLayerState, fp_InfoFile, fp_out_layerstate);
			fp_InfoFile.close();

			// Code to display info file into Pro/E browser window
			pfcWindow_ptr current_win = Session->GetCurrentWindow ();
			line = "file://";
			line += Session->GetCurrentDirectory();
			line += InfoFile_name;
			current_win->SetURL(line);
		}
		fp_out_layerstate.close();
	           
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_layerstate);
	fp_out_layerstate.close();
	return wfcTK_NO_ERROR;
}

// Deletes layer state
extern "C" wfcStatus otkDeleteLayerState ()
{
	try
	{
		fp_out_layerstate.open("LayerState.txt", ios::out);
		fp_out_layerstate << "- Layer State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcLayerState_ptr WLayerState = otkUtilSelectLayerState(WSolid, (int) -1, fp_out_layerstate);
		if(WLayerState != NULL)
			otkUtilDeleteLayerState(WLayerState, fp_out_layerstate);
		fp_out_layerstate.close();
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_layerstate);
	fp_out_layerstate.close();
	return wfcTK_NO_ERROR;
}




