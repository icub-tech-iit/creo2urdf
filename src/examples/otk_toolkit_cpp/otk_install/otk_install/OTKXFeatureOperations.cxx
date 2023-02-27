/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 This example will work on the current model as input and do the following: 
	- Ask the user to select feature operation type ( 1-Suppress | 2-Resume | 3-Delete | 4-Reorder )
	- For Suppress/Delete - Ask the user to select upto 2 features to suppress/delete & do the operation
	- For Resume 
		- Ask the user to to enter the number of features to resume (max 2)
	    - Ask the user to enter the feat ids of the features to be resumed
		- Resume the features
    - Reorder Feature
	    - Ask the user to select upto 2 features to reorder
		- Ask the user to enter the Post-Reorder Feature Number 
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
#include <wfcSolidInstructions.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h>

#include <fstream>

ofstream fp_out_featops;

void otkSuppressFeatures ( wfcWSolid_ptr myWSolid );
void otkDeleteFeatures ( wfcWSolid_ptr myWSolid );
void otkResumeFeatures ( wfcWSolid_ptr myWSolid );
void otkReorderFeatures ( wfcWSolid_ptr myWSolid );

//*********************************************
extern "C" wfcStatus otkFeatureOperations ()
{
	try
	{		
		fp_out_featops.open("ListFeatureOperations.txt", ios::out);
	    fp_out_featops << "- Features Operations Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
	
		wfcWSolid_ptr myWSolid = wfcWSolid::cast(Session->GetCurrentModel());

        if (!myWSolid)
		  return wfcTK_NO_ERROR;

		Session->UIDisplayMessage("feat_ops.txt", 
			"USER Enter type of Feature Operation", 0);

		xint choice = Session->UIReadIntMessage(1, 4);
			
		if (1 == choice)
		{
			/*Suppress Features*/
			otkSuppressFeatures (myWSolid);		
		}

		if (2 == choice)
		{
			/*Resume Features*/
			otkResumeFeatures (myWSolid);
		}

		if (3 == choice)
		{
			/*Delete Features*/
			otkDeleteFeatures (myWSolid);		
		}

		if (4 == choice)
		{
			/*Reorder Features*/
			otkReorderFeatures (myWSolid);
		}

		fp_out_featops.close();
		
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_featops);

	return wfcTK_GENERAL_ERROR;
}

//*********************************************
void otkSuppressFeatures ( wfcWSolid_ptr myWSolid )
{

	pfcSelectionOptions_ptr SelOptions;

	try
	{
		pfcSession_ptr Session = pfcGetCurrentSession ();

		Session->UIDisplayMessage("feat_ops.txt", "USER Select Features to Suppress (Max 2)", 0);

		SelOptions = pfcSelectionOptions::Create("feature");

		SelOptions->SetMaxNumSels(2);

		pfcSelections_ptr FeatSelections = Session->Select(SelOptions, 0);

		xintsequence_ptr feat_ids = xintsequence::create();

		for (int i=0 ; i<FeatSelections->getarraysize() ; i++)
		{
			pfcModelItem_ptr SelMItem = FeatSelections->get(i)->GetSelItem();

			fp_out_featops << "Feat ID: " << SelMItem->GetId() << endl;
			cout << "Feat ID: " << SelMItem->GetId() << endl;

			feat_ids->append( SelMItem->GetId() );
		}

		wfcFeatSuppressOrDeleteOptions_ptr SuppOpts = wfcFeatSuppressOrDeleteOptions::create();

		SuppOpts->append(wfcFEAT_SUPP_OR_DEL_NO_OPTS);

		myWSolid->SuppressFeatures(feat_ids, SuppOpts, 0);
	}
	OTK_EXCEPTION_HANDLER(fp_out_featops);
}

//*********************************************
void otkResumeFeatures ( wfcWSolid_ptr myWSolid )
{
	pfcSelectionOptions_ptr SelOptions;

	try
	{
		pfcSession_ptr Session = pfcGetCurrentSession ();

		Session->UIDisplayMessage("feat_ops.txt", "USER How many Features to Resume? (Max 2)", 0);
		
		xint num_feats = Session->UIReadIntMessage(1, 2);

		xintsequence_ptr feat_ids = xintsequence::create();

		for (int i=0 ; i<num_feats ; i++)
		{
			Session->UIDisplayMessage("feat_ops.txt", "USER Enter Feature-ID to Resume", 0);

			xint this_id = Session->UIReadIntMessage(0, 32767);

			feat_ids->append(this_id);
		}

		wfcFeatResumeOptions_ptr ResOpts = wfcFeatResumeOptions::create();

		ResOpts->append(wfcFEAT_RESUME_INCLUDE_PARENTS);

		myWSolid->ResumeFeatures(feat_ids, ResOpts, 0);

		Session->FlushCurrentWindow();
	}
	OTK_EXCEPTION_HANDLER(fp_out_featops);
}

//*********************************************
void otkDeleteFeatures ( wfcWSolid_ptr myWSolid )
{
	pfcSelectionOptions_ptr SelOptions;

	try
	{
		pfcSession_ptr Session = pfcGetCurrentSession ();

		Session->UIDisplayMessage("feat_ops.txt", "USER Select Features to Delete (Max 2)", 0);

		SelOptions = pfcSelectionOptions::Create("feature");

		SelOptions->SetMaxNumSels(2);

		pfcSelections_ptr FeatSelections = Session->Select(SelOptions, 0);

		xintsequence_ptr feat_ids = xintsequence::create();

		for (int i=0 ; i<FeatSelections->getarraysize() ; i++)
		{
			pfcModelItem_ptr SelMItem = FeatSelections->get(i)->GetSelItem();

			fp_out_featops << "Feat ID: " << SelMItem->GetId() << endl;
			cout << "Feat ID: " << SelMItem->GetId() << endl;

			feat_ids->append( SelMItem->GetId() );
		}

		wfcFeatSuppressOrDeleteOptions_ptr DelOpts = wfcFeatSuppressOrDeleteOptions::create();

		DelOpts->append(wfcFEAT_SUPP_OR_DEL_NO_OPTS);

		myWSolid->DeleteFeatures(feat_ids, DelOpts, 0);
	}
	OTK_EXCEPTION_HANDLER(fp_out_featops);
}

//*********************************************
void otkReorderFeatures ( wfcWSolid_ptr myWSolid )
{
	pfcSelectionOptions_ptr SelOptions;

	try
	{
		pfcSession_ptr Session = pfcGetCurrentSession ();

		Session->UIDisplayMessage("feat_ops.txt", "USER Select Features to Reorder (Max 2)", 0);
		
		SelOptions = pfcSelectionOptions::Create("feature");

		SelOptions->SetMaxNumSels(2);

		pfcSelections_ptr FeatSelections = Session->Select(SelOptions, 0);

		xintsequence_ptr feat_ids = xintsequence::create();

		for (int i=0 ; i<FeatSelections->getarraysize() ; i++)
		{
			pfcModelItem_ptr SelMItem = FeatSelections->get(i)->GetSelItem();

			fp_out_featops << "Feat ID: " << SelMItem->GetId() << endl;
			cout << "Feat ID: " << SelMItem->GetId() << endl;

			feat_ids->append( SelMItem->GetId() );
		}

		Session->UIDisplayMessage("feat_ops.txt", "USER Enter New Feature Number Post-Reorder (of 1st feature in Reorder set):", 0);

		xint new_feat_num = Session->UIReadIntMessage(0, 32767);

		myWSolid->ReorderFeatures(feat_ids, new_feat_num, 0);

	}
	OTK_EXCEPTION_HANDLER(fp_out_featops);
}
