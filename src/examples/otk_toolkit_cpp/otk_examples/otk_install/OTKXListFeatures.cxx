/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 This example will work on the current model as input and do the following: 
  a) Write to file, failed, child of failed and child of external failed features in the model
  b) Write to file, the status flags for all features in the model

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
#include <wfcFeatureInstructions.h>
#include <wfcSession.h>
#include <wfcSolidInstructions.h>
#include <OTKXUtils.h>

#include <fstream>

ofstream fp_out_solid;

void otkListAllFailedFeatures ( wfcWSolid_ptr myWSolid );
void otkPrintFeatureIds ( pfcFeatures_ptr Feats );
void otkGetSolidFeatureStatusFlags ( wfcWSolid_ptr myWSolid );

#define LISTFEATURESINFO_TXT "ListFeaturesInfo.txt"

//*********************************************
extern "C" wfcStatus otkListFeatures ()
{
	try
	{		
	    fp_out_solid.open( LISTFEATURESINFO_TXT, ios::out);
	    fp_out_solid << "- List Features Info Example -" << endl;

		wfcWSession_ptr Session = wfcWSession::cast( pfcGetCurrentSession () );
	
		wfcWSolid_ptr myWSolid = wfcWSolid::cast(Session->GetCurrentModel());

        if (!myWSolid)
		  return wfcTK_NO_ERROR;
		/**
		   List to file failed, child of failed and 
		   child of external failed features in the model
		*/
		otkListAllFailedFeatures(myWSolid);

		/**
		   Write to file the status flags for all features in the model 
		*/
		otkGetSolidFeatureStatusFlags(myWSolid);

		fp_out_solid.close();
		
                Session->DisplayInformationWindow ( LISTFEATURESINFO_TXT, 1.0, 1.0, 33, 80) ;

		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_solid);

	return wfcTK_GENERAL_ERROR;

}

//*********************************************
void otkListAllFailedFeatures ( wfcWSolid_ptr myWSolid )
{
	pfcFeatures_ptr Failed = 0, COFailed = 0, COEFailed = 0;
	int n_failed = 0, n_co_failed = 0, n_cox_failed = 0;
	xbool NoResMode = xfalse;
		
	try
	{
		Failed = myWSolid->ListFailedFeatures();
		
		if (Failed != 0)
		{
			n_failed = Failed->getarraysize();
			fp_out_solid << "\nNumber of Failed Features = " << n_failed << endl;
			otkPrintFeatureIds(Failed);
		}
		else
			fp_out_solid << "\nNO Failed Features exist" << endl;

		/** 
		   'Child of Failed' and 'Child of External Failed' features 
		   may exist only if 'No Resolve Mode' (default mode in Creo) 
		   is active. Verifying the mode nonetheless.
		*/
		NoResMode = myWSolid->GetIsNoResolveMode();

		if (NoResMode)
		{		
			COFailed = myWSolid->ListChildOfFailedFeatures();
		
			if (COFailed != 0)
			{
				n_co_failed = COFailed->getarraysize();
				fp_out_solid << "\nNumber of 'Child of Failed' Features = " << n_co_failed << endl;
				otkPrintFeatureIds(COFailed);
			}
			else
				fp_out_solid << "\nNO 'Child of Failed' Features exist" << endl;
			
			COEFailed = myWSolid->ListChildOfExternalFailedFeatures();
		
			if (COEFailed != 0)
			{
				n_cox_failed = COEFailed->getarraysize();
				fp_out_solid << "\nNumber of 'Child of External Failed' Features = "<< n_cox_failed << endl;
				otkPrintFeatureIds(COEFailed);
			}
			else
				fp_out_solid << "\nNO 'Child of External Failed' Features exist" << endl;
		}

	}
	OTK_EXCEPTION_HANDLER(fp_out_solid);
}

//*********************************************
void otkPrintFeatureIds (pfcFeatures_ptr Feats)
{
	for (int i=0 ; i<Feats->getarraysize() ; i++)
	{
		int id = (Feats->get(i))->GetId();

		fp_out_solid <<"  ++Feature Id = "<< id << endl ;
	}	
}

//*********************************************
void otkGetSolidFeatureStatusFlags ( wfcWSolid_ptr myWSolid )
{
	wfcFeatureStatusFlags_ptr FeatStatFlags = 0;
	wfcFeatureStatusFlag_ptr ThisFlag = 0;
	
	FeatStatFlags = myWSolid->GetSolidFeatureStatusFlags();

	fp_out_solid << "\n\nWriting Feature Status Flags for All Solid Features" << endl;
	
	for (int j=0 ; j<FeatStatFlags->getarraysize() ; j++)
	{
		ThisFlag = FeatStatFlags->get(j);
		fp_out_solid << endl << "For Feature Id: " << ThisFlag->GetFeatureId() << endl;
		
		if (ThisFlag->GetIsInvalid())
			fp_out_solid << "  -Feature Status: Invalid" << endl;
			
		if (ThisFlag->GetIsInactive())
			fp_out_solid << "  -Feature Status: Inactive" << endl;
		
		if (ThisFlag->GetIsActive())
			fp_out_solid << "  -Feature Status: Active" << endl;
		
		if (ThisFlag->GetIsFamtabSuppressed())
			fp_out_solid << "  -Feature Status: Fam Table Suppressed" << endl;
		
		if (ThisFlag->GetIsSimprepSuppressed())
			fp_out_solid << "  -Feature Status: Simprep Suppressed" << endl;
		
		if (ThisFlag->GetIsProgramSuppressed())
			fp_out_solid << "  -Feature Status: Program Suppressed" << endl;
		
		if (ThisFlag->GetIsSuppressed())
			fp_out_solid << "  -Feature Status: Suppressed" << endl;
		
		if (ThisFlag->GetIsUnregenerated())
			fp_out_solid << "  -Feature Status: Unregenerated" << endl;
		
		if (ThisFlag->GetIsFailed())
			fp_out_solid << "  -Feature Status: Failed" << endl;
		
		if (ThisFlag->GetIsChildOfFailed())
			fp_out_solid << "  -Feature Status: Child of Failed" << endl;
		
		if (ThisFlag->GetIsChildOfExternalFailed())
			fp_out_solid << "  -Feature Status: Child of External Failed" << endl;						
	}

}
