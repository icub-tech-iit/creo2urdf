/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 This example will work on the current Solid (Model/Assembly) as input and do the following: 
  a) Creates new combined state by taking name as I/P from user.
  b) CombinedSate Info: Asks user to enter number refering to "Existing combined Stated" from the list and 
     displays that combined state info into Pro/E browser.
  c) Delete combined state: Asks user to enter number refering to "Existing combined Stated" from the list and 
     deleted that combined state.
  
*/


#include <pfcSession.h>
#include <pfcModel.h>
#include <ciplib.h>
#include <pfcGlobal.h>
#include <pfcFeature.h>
#include <pfcSolid.h>
#include <pfcUI.h>
#include <wfcSolid.h>
#include <wfcPart.h>
#include <wfcAssembly.h>
#include <wfcCombState.h>
#include <pfcExceptions.h>
#include <wfcGlobal.h> 
#include <OTKXUtils.h> 

#include <fstream>


// Creates new combined state
extern "C" wfcStatus otkCreateNewCombState ()
{
	ofstream fp_out_combstate;
	try
	{		
		fp_out_combstate.open("CombinedState.txt", ios::out);
		fp_out_combstate << "- Combined State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
	
		wfcWSolid_ptr myWSolid = wfcWSolid::cast(Session->GetCurrentModel());

		Session->UIDisplayMessage("combstate_msg.txt", "USER New Combined state Name", 0);
		xstring NewCombStateName = Session->UIReadStringMessage();

		wfcCombState_ptr wCombState = otkUtilCreateCombinedState (myWSolid, NewCombStateName, false, fp_out_combstate);

		fp_out_combstate.close();
		return wfcTK_NO_ERROR;

	}
	OTK_EXCEPTION_HANDLER(fp_out_combstate);
	return wfcTK_NO_ERROR;
}


// Shows combined state info into Pro/E browser window
extern "C" wfcStatus otkCombinedStateInfo ()
{
	ofstream fp_out_combstate;
	try
	{
		fp_out_combstate.open("CombState.txt", ios::out);
		fp_out_combstate << "- Combined State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcCombState_ptr WCombState = otkUtilSelectCombinedState (WSolid, -1, fp_out_combstate);

		if(WCombState != NULL)
		{
			xstring InfoFile_name = "CombState_info.txt";
			ofstream fp_InfoFile;

			fp_InfoFile.open((const char *)InfoFile_name, ios::out);
			// Utility to write combined state info
			OtkUtilCombinedStateInfo (WSolid, WCombState, fp_InfoFile, fp_out_combstate);
			fp_InfoFile.close();
			
			// Code to display info file into Pro/E browser window
			pfcWindow_ptr current_win = Session->GetCurrentWindow ();
			xstring line;
			line = "file://";
			line += Session->GetCurrentDirectory();
			line += InfoFile_name;
			current_win->SetURL(line);
		}
		fp_out_combstate.close();
		return wfcTK_NO_ERROR;

	}
	OTK_EXCEPTION_HANDLER(fp_out_combstate);
	fp_out_combstate.close();
	return wfcTK_NO_ERROR;
}

// Deletes Combined state
extern "C" wfcStatus otkDeleteCombinedState ()
{
	ofstream fp_out_combstate;
	try
	{
		fp_out_combstate.open("CombinedState.txt", ios::out);
		fp_out_combstate << "- Combined State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcCombState_ptr WCombState = otkUtilSelectCombinedState (WSolid,(int)  -1, fp_out_combstate);
		if(WCombState != NULL)
			WSolid->DeleteCombState(WCombState);

		fp_out_combstate.close();
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_combstate);
	fp_out_combstate.close();
	return wfcTK_NO_ERROR;
}
