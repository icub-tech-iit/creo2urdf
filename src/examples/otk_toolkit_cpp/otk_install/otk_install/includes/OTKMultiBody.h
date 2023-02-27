/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXMULTIBODY_H
#define OTKXMULTIBODY_H




#include <pfcSelect.h>
#include <wfcSession.h>
#include <pfcUI.h>
#include <wfcSolid.h>
#include <pfcGlobal.h>
#include <pfcExceptions.h> 
#include <fstream>
#include <OTKXUtils.h>
#include <wfcElemIds.h>
#include <wfcAssembly.h>
#include <ProDataShareFeat.h>
#include <ProFeatType.h>
#include <ProElemId.h>
#include <uifcPushButton.h>
#include <uifcCore.h>
#include <uifcDialog.h>
#include <uifcDefaultListeners.h>
#include <cipxx.h>
#include <pfcComponentFeat.h>

class OTKMultiBody;

class OTKMultiBodyCopyOptions : virtual public uifcDefaultPushButtonListener
{
public:
	OTKMultiBodyCopyOptions() : mAppearCopy(xtrue), mParamCopy(xtrue), mNamesCopy(xtrue), mLayersCopy(xtrue),
		mMatsCopy(xtrue), mConstrBodyCopy(xtrue) 
	{}

	void  OnActivate(uifcPushButton_ptr handle);

private:	
	xbool mAppearCopy;
	xbool mParamCopy;
	xbool mNamesCopy;
	xbool mLayersCopy;
	xbool mMatsCopy;
	xbool mConstrBodyCopy;	
	friend class OTKMultiBody;
};

class OtkAppearUIAccessListener : public pfcUICommandAccessListener
{
private:
	xbool access;
public:
	OtkAppearUIAccessListener(xbool available) : access(available)
	{}

	pfcCommandAccess OnCommandAccess(xbool AllowToggle) {
		if (access == xtrue)
			return pfcACCESS_AVAILABLE;
		else
			return pfcACCESS_INVISIBLE;
	}
	void SetRestoreAction(xbool restoreFlag) {
		access = restoreFlag;
	}
};

class OTKMultiBody : virtual public pfcUICommandActionListener
{
  
 public:
	 OTKMultiBody(xstring inExample="Assembly To MultiBody", OtkAppearUIAccessListener* inAccess = NULL, OtkAppearUIAccessListener* inAppearAccess = NULL) : mExample(inExample), mAsmCounter(1), mPrtCounter(1), mFeatCounter(1),
	  mCopyOPtions (new OTKMultiBodyCopyOptions()), mAccessSet(inAccess), mAccessSetAppear(inAppearAccess)
	 {}
	
    void OnCommand();

	wfcWFeature_ptr CreateCopyGeomBodyFeature(wfcWSelection_ptr owner, pfcSelections_ptr bodies, pfcSelection_ptr external);	

	pfcSelections_ptr CollectBodiesFromPart(pfcPart_ptr part, pfcComponentPath_ptr path = 0);

	void AssemblePart(pfcAssembly_ptr asmbly, pfcPart_ptr part);

	wfcWAssembly_ptr CreateAssemblyFromBodies(pfcPart_ptr part,pfcSelections_ptr bodies);

	void SetBodyCopyOptions(wfcElements_ptr elems);

	void DisplayCopyOptsDialog();	

	void ApplyApperanceToBodies(pfcSolid_ptr sld);

	void RestoreApperance(pfcSolid_ptr sld);

  private:    
	xstring mExample;
	static wfcWSession_ptr mSession;
	int mAsmCounter;
	int mPrtCounter;
	int mFeatCounter;
	static wfcAppearances_ptr wAppearances;
	OTKMultiBodyCopyOptions* mCopyOPtions;	
	OtkAppearUIAccessListener* mAccessSet;
	OtkAppearUIAccessListener* mAccessSetAppear;
};



#endif /* OTKXMULTIBODY_H */
