/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/



#include <OTKMultiBody.h>

#define OTK_BODY_COPY_DIALOG "otkx_body_copy"
#define OTK_BODY_COPY_OK "CommitOK"
#define OTK_BODY_COPY_CANCEL "CommitCancel"

wfcWSession_ptr OTKMultiBody::mSession = NULL;
wfcAppearances_ptr OTKMultiBody::wAppearances = new wfcAppearances();

static ofstream mBodyExceptionLog;
/*
	Button callback action for 
	(1) MultiBody To Assembly (2)Assembly To MultiBody (3)Identify bodies & Apply Appearance (4)Restore bodies
*/
void OTKMultiBody::OnCommand()
{  
	mSession = wfcWSession::cast(pfcGetCurrentSessionWithCompatibility(pfcC4Compatible));	

	mBodyExceptionLog.open("body_example_exception.inf");
  try
    {         
		pfcModel_ptr mdl = mSession->GetCurrentModel();
		
		if (mdl->GetType() == pfcMDL_PART && mExample.Match("MultiBody To Assembly"))
		{
			pfcSelections_ptr bodySels = CollectBodiesFromPart(pfcPart::cast(mdl));
			if (bodySels->getarraysize() < 1)
				return;

			DisplayCopyOptsDialog();
			wfcWAssembly_ptr asmbly = CreateAssemblyFromBodies(pfcPart::cast(mdl),bodySels);
			if (asmbly != NULL)
			{
				asmbly->DisplayInNewWindow();
				mSession->GetModelWindow(pfcModel::cast(asmbly))->Activate();
			}
		}
		else if (mdl->GetType() == pfcMDL_ASSEMBLY && mExample.Match("Assembly To MultiBody"))
		{
			wfcWAssembly_ptr asmbly = wfcWAssembly::cast(mdl);
			wfcWComponentPaths_ptr paths = asmbly->ListDisplayedComponents();

			DisplayCopyOptsDialog();
			
			char prtName[32];
			sprintf(prtName, "asm_to_multibody%d", mPrtCounter);
			pfcPart_ptr part = mSession->CreatePart(prtName);
			mPrtCounter++;

			wfcWSelection_ptr ownerPrt = wfcWSelection::cast(pfcCreateModelSelection(pfcModel::cast(part)));
			pfcSelection_ptr asmblySel = pfcCreateModelSelection(pfcModel::cast(asmbly));
			for (int ii = 0; ii < paths->getarraysize(); ii++)
			{
				pfcSolid_ptr  leafSolid = paths->get(ii)->GetLeaf();
				if (pfcModel::cast(leafSolid)->GetType() == pfcMDL_ASSEMBLY)
					continue;

				pfcSelections_ptr bodies = CollectBodiesFromPart(pfcPart::cast(leafSolid), pfcComponentPath::cast(paths->get(ii)));
				CreateCopyGeomBodyFeature(ownerPrt, bodies, asmblySel);							
			}
			part->DisplayInNewWindow();
			mSession->GetModelWindow(pfcModel::cast(part))->Activate();						
		}

		else if (mdl->GetType() == pfcMDL_PART && mExample.Match("Identify Bodies"))
		{
			ApplyApperanceToBodies(pfcSolid::cast(mdl));
			mSession->GetCurrentWindow()->Repaint();
			mAccessSet->SetRestoreAction(false);
			mAccessSetAppear->SetRestoreAction(true);
			
		}
		else if (mdl->GetType() == pfcMDL_PART && mExample.Match("Restore Bodies appearance"))
		{
			RestoreApperance(pfcSolid::cast(mdl));
			mSession->GetCurrentWindow()->Repaint();			
			mAccessSet->SetRestoreAction(true);
			mAccessSetAppear->SetRestoreAction(false);
		}	
    }
  OTK_EXCEPTION_HANDLER(mBodyExceptionLog);
  mBodyExceptionLog.close();
    
  return;
}
/*
	Display copy options for body copy.
*/
void OTKMultiBody::DisplayCopyOptsDialog()
{
	uifcCreateDialog(OTK_BODY_COPY_DIALOG, OTK_BODY_COPY_DIALOG);
	uifcPushButtonFind(OTK_BODY_COPY_DIALOG, OTK_BODY_COPY_OK)->AddActionListener(uifcPushButtonListener::cast(mCopyOPtions));
	uifcPushButtonFind(OTK_BODY_COPY_DIALOG, OTK_BODY_COPY_CANCEL)->AddActionListener(uifcPushButtonListener::cast(mCopyOPtions));
	uifcActivateDialog(OTK_BODY_COPY_DIALOG);
	uifcDestroyDialog(OTK_BODY_COPY_DIALOG);
}

/*
	Copy values from Dialog
*/
void OTKMultiBodyCopyOptions::OnActivate(uifcPushButton_ptr handle)
{
	try
	{
		if (handle->GetComponent().Match(OTK_BODY_COPY_OK))
		{
			mAppearCopy = uifcCheckButtonFind(handle->GetDialog(), "cp_appear")->GetCheckedState();
			mParamCopy = uifcCheckButtonFind(handle->GetDialog(), "cp_params")->GetCheckedState();
			mNamesCopy = uifcCheckButtonFind(handle->GetDialog(), "cp_names")->GetCheckedState();
			mLayersCopy = uifcCheckButtonFind(handle->GetDialog(), "cp_layers")->GetCheckedState();
			mMatsCopy = uifcCheckButtonFind(handle->GetDialog(), "cp_mats")->GetCheckedState();
			mConstrBodyCopy = uifcCheckButtonFind(handle->GetDialog(), "cp_constr")->GetCheckedState();
		}

		uifcExitDialog(handle->GetDialog(), 1);

	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);
}
/*
	Collect bodies from part or component part
*/
pfcSelections_ptr OTKMultiBody::CollectBodiesFromPart(pfcPart_ptr part,pfcComponentPath_ptr path)
{
	pfcSelections_ptr bodySels = pfcSelections::create();
	try
	{
		pfcModelItems_ptr items = part->ListItems(pfcITEM_BODY);	

		for (int ii = 0; ii < items->getarraysize(); ii++)
			bodySels->append(pfcCreateModelItemSelection(items->get(ii), path));

	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);

	return bodySels;
}
/*
	Build element tree for body copy options
*/
void OTKMultiBody::SetBodyCopyOptions(wfcElements_ptr elems)
{
	try
	{
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_PROPS, NULL, 1));
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_APPEARANCE, pfcCreateIntArgValue((mCopyOPtions->mAppearCopy) ? 1 : 0), 2));
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_PARAMETERS, pfcCreateIntArgValue((mCopyOPtions->mParamCopy) ? 1 : 0), 2));
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_NAMES, pfcCreateIntArgValue((mCopyOPtions->mNamesCopy) ? 1 : 0), 2));
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_LAYERS, pfcCreateIntArgValue((mCopyOPtions->mLayersCopy) ? 1 : 0), 2));
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_MATERIALS, pfcCreateIntArgValue((mCopyOPtions->mMatsCopy) ? 1 : 0), 2));
		elems->append(wfcElement::Create(PRO_E_DSF_CPY_BODY_TYPE, pfcCreateIntArgValue((mCopyOPtions->mConstrBodyCopy) ? 1 : 0), 2));


	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);
}
/*
	Build element tree and create external copy geometry features copying bodies.
*/
wfcWFeature_ptr OTKMultiBody::CreateCopyGeomBodyFeature(wfcWSelection_ptr owner, pfcSelections_ptr bodies, pfcSelection_ptr external)
{
	try
	{
		char featName[32];
		sprintf(featName, "otk_cg_body%d", mFeatCounter);
		mFeatCounter++;	

		wfcElements_ptr elems = wfcElements::create();
		elems->append(wfcElement::Create(wfcPRO_E_FEATURE_TREE, NULL, 0));
		elems->append(wfcElement::Create(wfcPRO_E_FEATURE_TYPE, pfcCreateIntArgValue(PRO_FEAT_GEOM_COPY), 1));
		elems->append(wfcElement::Create(wfcPRO_E_CG_FEAT_SUB_TYPE, pfcCreateIntArgValue(PRO_CG_COPY_GEOM), 1));
		elems->append(wfcElement::Create(wfcPRO_E_CG_REFS_TYPE, pfcCreateIntArgValue(PRO_CG_COLL_REFS), 1));
		elems->append(wfcElement::Create(wfcPRO_E_STD_FEATURE_NAME, pfcCreateStringArgValue(featName), 1));

		elems->append(wfcElement::Create(wfcPRO_E_CG_LOCATION, NULL, 1));
		elems->append(wfcElement::Create(wfcPRO_E_DSF_EXT_LOCAL_TYPE, pfcCreateIntArgValue(PRO_DSF_PLACE_EXTERNAL), 2));
		elems->append(wfcElement::Create(wfcPRO_E_DSF_SEL_REF_MDL, pfcCreateSelectionArgValue(external), 2));

		elems->append(wfcElement::Create(wfcPRO_E_CG_PLACEMENT, NULL, 2));
		elems->append(wfcElement::Create(wfcPRO_E_CG_PLACE_TYPE, pfcCreateIntArgValue(PRO_CG_PLC_DEFAULT), 3));

		elems->append(wfcElement::Create(wfcPRO_E_CG_PG_OR_REFS, NULL, 1));
		elems->append(wfcElement::Create(wfcPRO_E_CG_REFS_COLL, NULL, 2));

		wfcElement_ptr bodyCollElem = wfcElement::Create(PRO_E_CG_BODY_COLL, NULL, 3);
		bodyCollElem->SetSelections(bodies);
		elems->append(bodyCollElem);

		SetBodyCopyOptions(elems);

		wfcElementTree_ptr tree = mSession->CreateElementTree(elems);
		wfcFeatCreateOptions_ptr opts = wfcFeatCreateOptions::create();
		opts->append(wfcFEAT_CR_NO_OPTS);

		return owner->CreateFeature(tree, opts,NULL);		
	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);

	return NULL;
}
/*
	Build element tree and create component placement feature in an assembly
*/
void OTKMultiBody::AssemblePart(pfcAssembly_ptr asmbly, pfcPart_ptr part)
{
	try
	{
		wfcElements_ptr elems = wfcElements::create();
		elems->append(wfcElement::Create(wfcPRO_E_FEATURE_TREE, NULL, 0));
		elems->append(wfcElement::Create(wfcPRO_E_FEATURE_TYPE, pfcCreateIntArgValue(PRO_FEAT_COMPONENT), 1));
		 

		wfcElement_ptr modelElem = wfcElement::Create(wfcPRO_E_COMPONENT_MODEL, NULL, 1);
		wfcSpecialValue_ptr specialValue = wfcSpecialValue::Create();
		specialValue->SetComponentModel(pfcSolid::cast(part));
		modelElem->SetSpecialValueElem(specialValue);
		elems->append(modelElem);

		elems->append(wfcElement::Create(wfcPRO_E_COMPONENT_CONSTRAINTS, NULL, 1));
		elems->append(wfcElement::Create(wfcPRO_E_COMPONENT_CONSTRAINT, NULL, 2));
		elems->append(wfcElement::Create(wfcPRO_E_COMPONENT_CONSTR_TYPE, pfcCreateIntArgValue(PRO_ASM_DEF_PLACEMENT), 3));

		wfcElementTree_ptr tree = mSession->CreateElementTree(elems);
		wfcFeatCreateOptions_ptr opts = wfcFeatCreateOptions::create();
		opts->append(wfcFEAT_CR_INCOMPLETE_FEAT);

		wfcWAssembly::cast(asmbly)->WCreateFeature(tree, opts);
	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);
}
/*
	Create assembly from multibody part using copy geometry feature
*/
wfcWAssembly_ptr OTKMultiBody::CreateAssemblyFromBodies(pfcPart_ptr mbPart,pfcSelections_ptr bodies)
{
	try
	{
		char asmname[32];		
		sprintf(asmname, "multibody_to_asm%d", mAsmCounter);
		wfcWAssembly_ptr asmbly = wfcWAssembly::cast(mSession->CreateAssembly(asmname));
		mAsmCounter++;
		
		pfcSelection_ptr mdlSel = pfcCreateModelSelection(pfcModel::cast(mbPart));
		for (int ii = 0; ii < bodies->getarraysize(); ii++)
		{
			pfcSelections_ptr bodySels = pfcSelections::create();
			bodySels->append(bodies->get(ii));

			char prtName[32];
			sprintf(prtName, "body%d", mPrtCounter);
			pfcPart_ptr part = mSession->CreatePart(prtName);
			mPrtCounter++;

			CreateCopyGeomBodyFeature(wfcWSelection::cast(pfcCreateModelSelection(pfcModel::cast(part))),
				bodySels, pfcCreateModelSelection(pfcModel::cast(mbPart)));

			if(part != NULL)
				AssemblePart(pfcAssembly::cast(asmbly),part);
		}

		return asmbly;

	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);

	return NULL;
}
/*
	Apply different colours  to all bodies in a part
*/
void OTKMultiBody::ApplyApperanceToBodies(pfcSolid_ptr sld)
{
	try
	{
		if (sld == NULL)
			return;

		pfcSelections_ptr bodies = CollectBodiesFromPart(pfcPart::cast(sld));
		if (bodies == NULL)
			return;

		double rColor = 0.300000, gColor = 0.500000, bColor = 1.000000;
		for (int ii = 0; ii < bodies->getarraysize(); ii++)
		{	
			wfcAppearance_ptr appearanceGet = wfcWSelection::cast(bodies->get(ii))->GetVisibleAppearance();
			if(appearanceGet == 0)
				wAppearances->append(0);
			else
				wAppearances->append(appearanceGet);
			
			wfcAppearance_ptr appearanceSet = wfcAppearance::Create(0.400000, 0.900000, 0.100000, 0.600000, 0.000000,
				pfcColorRGB::Create(rColor, gColor, bColor), pfcColorRGB::Create(1.000000, 1.000000, 1.000000),
				0.050000, "ref_color55", "ptc-fabric-leather", " ", " ");
			
			wfcWSelection::cast(bodies->get(ii))->SetVisibleAppearance(appearanceSet);			
			double  nfactor = (1.0 - 0.0) * (ii+1) / (bodies->getarraysize() + 1) + 0.0;

			if ((rColor + nfactor) < 1.000000)
				rColor = rColor + nfactor;
			else
				rColor = 0.400000;
			if ((gColor + nfactor) < 1.000000)
				gColor = gColor + nfactor;
			else
				gColor = 0.100000;
			if ((bColor - nfactor) > 0.000000)
				bColor = bColor - nfactor;
			else
				bColor = 0.700000;
		}
		
	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);
}
/*
	Restore colors to all bodies in a part
*/
void OTKMultiBody::RestoreApperance(pfcSolid_ptr sld)
{
	try
	{
		if (sld == NULL)
			return;

		pfcSelections_ptr bodies = CollectBodiesFromPart(pfcPart::cast(sld));
		if (bodies == NULL)
			return;

		for (int i = 0; i < bodies->getarraysize(); i++) {			
			if (wAppearances->get(i) == 0) 
			{
				wfcWSelection::cast(bodies->get(i))->SetVisibleAppearance(0);				
			}
			else {
				wfcWSelection::cast(bodies->get(i))->SetVisibleAppearance(wAppearances->get(i));
			}
		}
	}
	OTK_EXCEPTION_HANDLER(mBodyExceptionLog);
}
