/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Utilites for Combined state.
  
*/
#include <fstream>
#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <pfcLayer.h>

#include <wfcAssembly.h>
#include <wfcSolid.h>
#include <wfcLayerState.h>
#include <wfcCombState.h>

#include <OTKXUtils.h> 
#include <OTKXEnum.h>



pfcModelItem_ptr otkUtilMdlItemSelect(pfcModelItems_ptr pModelItems, xstring itemtype, xbool IsRegTest, ofstream& fp_out_combstate);



// Utility to return combined state from input solid.
// Pass CombStateNum = -1 to select layer state through UI
//*********************************************
wfcCombState_ptr otkUtilSelectCombinedState (wfcWSolid_ptr WSolid, 
                                      int CombStateNum, ofstream&  fp_out_combstate)
{
  try
  {	
	pfcSession_ptr Session = pfcGetCurrentSession ();

	wfcCombStates_ptr WCombStates =   WSolid->ListCombStates();
	if(WCombStates == NULL)
		return 0;

	int CombStatesArraySize = WCombStates->getarraysize ();
	if(CombStatesArraySize > 0)
	{
	  if(CombStateNum != -1 &&(CombStateNum > 0&& CombStateNum <= CombStatesArraySize))
	  {
		  wfcCombState_ptr WCombState = WCombStates->get(CombStateNum -1);
		  return (WCombState);
	  }
	  // Select from UI
	  else
	  {
		char buff[200];
		xstring line;
		xstringsequence_ptr msgs = xstringsequence::create();

		line += "Layer States: ";
		for (int i = 0; i < CombStatesArraySize; i++)
		{
		  wfcCombState_ptr WCombState = WCombStates->get(i);
		  if(WCombState != NULL)
		  {
			if(i+1 < CombStatesArraySize)
				sprintf(buff, "%d: %s, ",i+1, (const char *)  WCombState->GetName());
			else
				sprintf(buff, "%d: %s ",i+1, (const char *)  WCombState->GetName());

			line += buff;
		  }
		}
		msgs->append(line);
		Session->UIDisplayMessage("layer_state.txt", "USER Enter number to select %0s", msgs);
		int SelectedCombStateNo = Session->UIReadIntMessage(1, CombStatesArraySize);
		if(SelectedCombStateNo > 0 && SelectedCombStateNo <= CombStatesArraySize)
		{
			wfcCombState_ptr WCombState = WCombStates->get(SelectedCombStateNo -1);
			return (WCombState);
		}
	  }
	}
	return 0;
  }
  OTK_EXCEPTION_HANDLER(fp_out_combstate);
  return (0);
}


int OtkUtilCombinedStateInfo (wfcWSolid_ptr WSolid, wfcCombState_ptr WCombState, ofstream& InfoFile, 
                           ofstream& fp_out_combstate)
{
  try
  {	
	char file_buff[250]; 

	if(WCombState == 0)
		return -1;

	wfcCombStateData_ptr WCombStateData = WCombState->GetCombStateData();

	InfoFile << "Combined State Info:" << endl;

	InfoFile<<"\n - CombState data:"<<endl;
	xstring  CombStateName = WCombStateData->GetCombStateName();
	sprintf (file_buff, "\t - CombState name: %s ", (const char *)CombStateName);
	InfoFile<<file_buff<<endl;
    
	xbool isexploded = WCombStateData->GetIsExploded();
	if(isexploded == xtrue)
		InfoFile<<"\t - IsExploded: TRUE"<<endl;
	else
		InfoFile<<"\t - IsExploded: FALSE"<<endl;
    
	xint clipOption = WCombStateData->GetClipOption();
	if(clipOption == wfcVIS_OPT_NONE)
		InfoFile<<"\t - ClipOption: wfcVIS_OPT_NONE"<<endl;
	else if(clipOption == wfcVIS_OPT_FRONT)
		InfoFile<<"\t - ClipOption: wfcVIS_OPT_FRONT"<<endl;
	else if(clipOption == wfcVIS_OPT_BACK)
		InfoFile<<"\t - ClipOption: wfcVIS_OPT_BACK"<<endl;
	
	wfcCombState_ptr WActiveCombState = WSolid->GetActiveCombState();
	if(WActiveCombState != 0)
	{
		wfcCombStateData_ptr ActiveCombStateData = WActiveCombState->GetCombStateData();
		if(ActiveCombStateData != 0)
		{
			xstring  ActiveCombStateName = ActiveCombStateData->GetCombStateName();
			if(!(ActiveCombStateName.IsEmpty()))
				sprintf (file_buff, "\t - Active CombState name: %s ", const_cast<char*>((cStringT)ActiveCombStateName));
			InfoFile<<file_buff<<endl;
		}
	}

	pfcModelItems_ptr ReferenceStates = WCombStateData->GetReferences ();
	int ReferencesSize = ReferenceStates->getarraysize();
	sprintf(file_buff,"\t - Number of CombState References: %d \n", ReferencesSize);
	InfoFile<<file_buff<<endl;

	for(int i = 0; i < ReferencesSize; i++)
	{
		pfcModelItem_ptr RefItem = ReferenceStates->get(i);
		switch(RefItem->GetType())
		{
			case pfcITEM_SIMPREP:
				OtkUtilSimpRepInfo(wfcWSimpRep::cast(RefItem), InfoFile, fp_out_combstate);
			break;

			case pfcITEM_EXPLODED_STATE:
				OtkUtilExplodedStateInfo(wfcWExplodedState::cast(RefItem), InfoFile, fp_out_combstate);
			break;

			case pfcITEM_LAYER_STATE:
				otkUtilLayerStateInfo(WSolid, wfcLayerState::cast(RefItem), InfoFile, fp_out_combstate);
			break;

			case pfcITEM_STYLE_STATE:
				OtkUtilStyleStateInfo(WSolid, wfcStyleState::cast(RefItem), InfoFile, fp_out_combstate);
			break;
			case pfcITEM_VIEW:
			{
				wfcViewModelitem_ptr viewModelItem = wfcViewModelitem::cast(RefItem);
				pfcView_ptr wView = viewModelItem->GetViewFromModelItem();

				OtkUtilViewInfo(wView, InfoFile, fp_out_combstate);


				break;
			}
		}
	}
	return 0;
  }
  OTK_EXCEPTION_HANDLER(fp_out_combstate);
  return (0);
}

int OtkUtilSimpRepInfo(wfcWSimpRep_ptr WSimpRep, ofstream& InfoFile, ofstream& fp_out_combstate)
{
  char file_buff[250] = "\0"; 
  otkxEnums EnumInst;

  if(WSimpRep == 0)
	return -1;
  try
  {	
	pfcSimpRepInstructions_ptr SimpRepInstructions = WSimpRep->GetInstructions();

	// Simprep instructions are not found
	if(SimpRepInstructions == NULL)
	{
		InfoFile<<"\t -Simprep instructions are not found"<<endl;
		return (0);
	}

	InfoFile << "- SimpRep Info -" << endl;

	xbool istemp = SimpRepInstructions->GetIsTemporary();
	if(istemp == xtrue)
	{
		InfoFile<<"\t -Temp v alue: TRUE"<<endl;

		pfcRetrieveExistingSimpRepInstructions_ptr CreateInstr =	pfcRetrieveExistingSimpRepInstructions::cast (SimpRepInstructions);
			
		xstring  SimpRepName = CreateInstr->GetExistSimpName();
		sprintf (file_buff, "\t - SimpRep name: %s ", (const char *)SimpRepName);
		InfoFile<<file_buff<<endl;

	}
	else
	{
		InfoFile<<"\t - Temp value: FALSE"<<endl;

		pfcCreateNewSimpRepInstructions_ptr CreateInstr =	pfcCreateNewSimpRepInstructions::cast (SimpRepInstructions);

		xstring  SimpRepName = CreateInstr->GetNewSimpName();
		sprintf (file_buff, "\t - SimpRep name: %s ", (const char *)SimpRepName);
		InfoFile<<file_buff<<endl;
	}
	pfcSimpRepActionType WActionType = SimpRepInstructions->GetDefaultAction();
	sprintf (file_buff, "\t - SimpRep Default Action: %s ", EnumInst.pfcSimpRepActionTypeGet(WActionType));
	InfoFile<<file_buff<<endl;

	xbool isdefault = WSimpRep->IsSimpRepInstructionDefault();
	if(isdefault == xtrue)
	  InfoFile<<"\t - Is Default Action Type: TRUE"<<endl;
    else if(isdefault == xfalse)
	 InfoFile<<"\t - Is Default Action Type: FALSE"<<endl;


	xbool tmpval = WSimpRep->GetSimprepdataTempvalue();
	if(tmpval == xtrue)
		InfoFile<<"\t - SimprepdataTempvalue: TRUE"<<endl; 
	else
		InfoFile<<"\t - SimprepdataTempvalue: FALSE"<<endl; 

	pfcSimpRepItems_ptr  pSimpRepItems = SimpRepInstructions->GetItems();

	int SimpRepItemsSize = pSimpRepItems->getarraysize();
	sprintf (file_buff, "\t - Number of Actions in SimpRep: %d ", (int)SimpRepItemsSize);
	InfoFile<<file_buff<<endl;

    for (int ii = 0; ii < SimpRepItemsSize; ii++)
    {
    	pfcSimpRepItem_ptr  psimpItem = pSimpRepItems->get(ii);
    	pfcSimpRepAction_ptr   psimpRepAction = psimpItem->GetAction();
    	if(psimpRepAction != NULL)
    	{
    		pfcSimpRepActionType actType =  psimpRepAction ->GetType();
			sprintf (file_buff, "\t - SimpRep Action Type: %s ", EnumInst.pfcSimpRepActionTypeGet(actType));
			InfoFile<<file_buff<<endl;

    		if(actType == pfcSIMPREP_SUBSTITUTE)
    		{
	    		pfcSimpRepItemPath_ptr  psimpItemPath = psimpItem->GetItemPath();

	    		xstring SubstName = WSimpRep->GetSimprepSubstitutionName(psimpItemPath);
				sprintf (file_buff, "\t - SimpRep SubstitutionName: %s ", (const char *)SubstName);
				InfoFile<<file_buff<<endl;
    		}
   		}
    }
    return 0;
  }
  OTK_EXCEPTION_HANDLER(fp_out_combstate);
  return 0;
}

int OtkUtilExplodedStateInfo(wfcWExplodedState_ptr wExpldState, ofstream& InfoFile, ofstream& fp_out_combstate)
{
  char file_buff[250]; 
  otkxEnums EnumInst;

  if(wExpldState == 0)
	return -1;
  
  try
  {
	xstring ExpldStateName = wExpldState->GetExplodedStateName();

	InfoFile << "- Exploded State Info -" << endl;

	sprintf (file_buff, "\t - ExplodedState name: %s ", (const char *)ExpldStateName);
	InfoFile<<file_buff<<endl;

	wfcExplodedAnimationMoveInstructions_ptr wExpldStateMvInstructions = wExpldState->GetExplodedStateMoves(); 

	int arraySize = wExpldStateMvInstructions->getarraysize();
	sprintf (file_buff, "\t - Exploded Animation MoveInstructions array size: %d", arraySize);
	InfoFile<<file_buff<<endl;

   	for (int i = 0; i < arraySize; i++)
	{
		sprintf (file_buff, "\t - Exploded Animation MoveInstruction: %d info", (i+1));
		InfoFile<<file_buff<<endl;

 		wfcExplodedAnimationMoveInstruction_ptr MoveInstruction = wExpldStateMvInstructions->get(i);
 		wfcExplodedAnimationMove_ptr WExpldAnimMove = MoveInstruction->GetMove ();
		 		
		wfcWComponentPaths_ptr WCompPaths = MoveInstruction->GetCompSet ();

		int CompSetarraySize1 = WCompPaths->getarraysize();
		sprintf (file_buff, "\t - CompSet array size: %d", CompSetarraySize1);
		InfoFile<<file_buff<<endl;

		wfcExplodedAnimationMoveType wMoveType = WExpldAnimMove->GetMoveType();
		sprintf (file_buff, "\t - ExplodedState Move Type: %s ", EnumInst.wfcExplodedAnimationMoveTypeGet(wMoveType));
		InfoFile<<file_buff<<endl;

		pfcPoint3D_ptr WStartPt = WExpldAnimMove->GetStartPoint();
		sprintf (file_buff, "\t - StartPoint: [%f, %f, %f] ", WStartPt->get(0), WStartPt->get(1), WStartPt->get(2));
		InfoFile<<file_buff<<endl;

		pfcVector3D_ptr WDirVector = WExpldAnimMove->GetDirVector();
		sprintf (file_buff, "\t - DirVector: [%f, %f, %f] ", WDirVector->get(0), WDirVector->get(1), WDirVector->get(2));
		InfoFile<<file_buff<<endl;

		double WValue = WExpldAnimMove->GetValue();
		sprintf (file_buff, "\t - ExplodedState Move: %f ", WValue);
		InfoFile<<file_buff<<endl;
	}
	wfcWComponentPaths_ptr wCompPaths = wExpldState->GetExplodedcomponents();
	int arraySize1 = wCompPaths->getarraysize();
	sprintf (file_buff, "\t - Exploded components array size: %d", arraySize1);
	InfoFile<<file_buff<<endl;

    return 0;
  }
  OTK_EXCEPTION_HANDLER(fp_out_combstate);
  return 0;
}

//Asks user to enter number and retuns that element from pfcModelItems array
pfcModelItem_ptr otkUtilMdlItemSelect(pfcModelItems_ptr pModelItems, xstring itemtype, xbool IsRegTest, ofstream& fp_out_combstate)
{
	char buff[200];
	xstring line;
	xstringsequence_ptr msgs = xstringsequence::create();
	pfcSession_ptr Session = pfcGetCurrentSession ();

	line += itemtype;
	line += ": ";
	try
	{
		xint index;
		int arraySize = pModelItems->getarraysize ();
		if(arraySize <=0)
			return 0;
		
		// for reg test
		if(IsRegTest == xtrue)
			index = 1;

		// For OTK example: Asks user to give I/P from UI
		else
		{
			for (int i = 0; i < arraySize; i++)
			{
				pfcModelItem_ptr pModelItem = pModelItems->get(i);
				if(pModelItem != NULL)
				{
					if(i+1 < arraySize)
						sprintf(buff, "%d: %s, ",i+1, (const char *)  pModelItem->GetName());
					else
						sprintf(buff, "%d: %s ",i+1, (const char *)  pModelItem->GetName());
					line += buff;
				}
			}
			msgs->append(line);
			Session->UIDisplayMessage("combstate_msg.txt", "USER Enter number to select %0s", msgs);
			index = Session->UIReadIntMessage(1, arraySize);
		}
		if(index <= 0 || index > arraySize)
			return NULL;

		return(pModelItems->get(index -1));
	}
	OTK_EXCEPTION_HANDLER(fp_out_combstate);

	return 0;
}

int OtkUtilStyleStateInfo(wfcWSolid_ptr WSolid, wfcStyleState_ptr WstyleState, ofstream& InfoFile, ofstream& fp_out_combstate)
{
  char file_buff[250]; 

  if(WstyleState == 0)
	return -1;
  try
  {
	xstring StyleStateName =  WstyleState->GetName();

	InfoFile << "- StyleState State Info -" << endl;

	sprintf(file_buff,"\t - StyleState Name: %s", (const char*)StyleStateName);
	InfoFile<<file_buff<<endl;

	xint StyleStateId = WstyleState->GetId();
	if(StyleStateId > 0)
	{
		sprintf(file_buff,"\t - StyleState Id: %d", (int) StyleStateId);
		InfoFile<<file_buff<<endl;
	}
	xbool isdefault = WstyleState->IsStyleStateDefault();

	if(isdefault == xtrue)
		InfoFile<<"\t - StyleState is default \n"<<endl;
	else
		InfoFile<<"\t - StyleState is not default \n"<<endl;

	wfcStyleState_ptr ActiveWstyleState = WSolid->GetActiveStyleState();

	if(ActiveWstyleState != 0)
	{
		sprintf(file_buff,"\t - Active StyleState Name: %s", const_cast<char*>((cStringT) ActiveWstyleState->GetName() ));
		InfoFile<<file_buff<<endl;
	}
	return 0;
  }
  OTK_EXCEPTION_HANDLER(fp_out_combstate);
  return 0;
}


//Utility to create new combined state
wfcCombState_ptr otkUtilCreateCombinedState (wfcWSolid_ptr myWSolid, xstring NewCombStateName, xbool IsRegTest, ofstream& fp_out_combstate)
{
	try
	{
		char buff[200];
		pfcModelType modelType = myWSolid->GetType();
		pfcSession_ptr Session = pfcGetCurrentSession ();

		if(!(NewCombStateName.IsEmpty()) && (modelType == pfcMDL_ASSEMBLY || modelType == pfcMDL_PART))
		{
			pfcModelItems_ptr pfcMdlItems = new pfcModelItems;

			// Adds simprep item
			pfcModelItems_ptr WSimpReps = myWSolid->ListItems(pfcITEM_SIMPREP);
			pfcModelItem_ptr pMdlItem = otkUtilMdlItemSelect(WSimpReps, "SimpRep", IsRegTest, fp_out_combstate);
			if(pMdlItem != NULL)
			{
				pfcMdlItems->append(pMdlItem);
				fp_out_combstate<<"Selected Item Type: "<< pMdlItem->GetType()<<", ID: "<<pMdlItem->GetId()<<endl;
			}

			// Code to add Style State and Exploded State items, applicable for assemblies only
			if(modelType == pfcMDL_ASSEMBLY)
			{
				// Adds Style State item
				wfcStyleStates_ptr WStyleStates =   myWSolid->ListStyleStateItems();
				int StyleStatesArraySize = WStyleStates->getarraysize ();
				if(StyleStatesArraySize > 0)
				{
					xint SelectedstyleSateNo;
					// For RegTest
					if(IsRegTest == xtrue)
					{
						SelectedstyleSateNo = 1;
					}
					// For OTK example: Asks user to give I/P from UI
					else
					{
						xstring line;
						xstringsequence_ptr msgs = xstringsequence::create();
						line += "Style State: ";
						for (int i = 0; i < StyleStatesArraySize; i++)
						{
							wfcStyleState_ptr WStyleState = WStyleStates->get(i);
							if(WStyleState != NULL)
							{
								if(i+1 < StyleStatesArraySize)
									sprintf(buff, "%d: %s, ",i+1, (const char *)  WStyleState->GetName());
								else
									sprintf(buff, "%d: %s ",i+1, (const char *)  WStyleState->GetName());

								line += buff;
							}
						}
						msgs->append(line);
						Session->UIDisplayMessage("combstate_msg.txt", "USER Enter number to select %0s", msgs);
						SelectedstyleSateNo = Session->UIReadIntMessage(1, StyleStatesArraySize);
					}
					if(SelectedstyleSateNo > 0 && SelectedstyleSateNo <= StyleStatesArraySize)
					{
						pfcModelItem_ptr pMdlItem = pfcModelItem::cast(WStyleStates->get(SelectedstyleSateNo -1));
						pfcMdlItems->append(pMdlItem);
						fp_out_combstate<<"Selected Item Type: "<< pMdlItem->GetType()<<", ID: "<<pMdlItem->GetId()<<endl;
					}
				}


				//Adds Exploded State item
				pfcModelItems_ptr WExplodedStates = myWSolid->ListItems(pfcITEM_EXPLODED_STATE);
				pfcModelItem_ptr  pMdlItem = otkUtilMdlItemSelect(WExplodedStates, "Exploded State", IsRegTest, fp_out_combstate);
				if( pMdlItem != NULL)
				{
					pfcMdlItems->append( pMdlItem);
					fp_out_combstate<<"Selected Item Type: "<< pMdlItem->GetType()<<", ID: "<<pMdlItem->GetId()<<endl;
				}
			}

			//Adds Cross Section item
			pfcXSections_ptr WXSections = myWSolid->ListCrossSections();
			int XSectionsArraySize = WXSections->getarraysize ();
			if(XSectionsArraySize > 0)
			{
				xint XSectionNo;
				// For RegTest
				if(IsRegTest == xtrue)
				{
					XSectionNo = 1;
				}
				// For OTK example: Asks user to give I/P from UI
				else
				{
					xstring line;
					xstringsequence_ptr msgs = xstringsequence::create();
					line += "Cross section: ";
					for (int i = 0; i < XSectionsArraySize; i++)
					{
						pfcXSection_ptr WXSection = WXSections->get(i);
						if(i+1 < XSectionsArraySize)
							sprintf(buff, "%d: %s, ",i+1, (const char *)  pfcModelItem::cast(WXSection)->GetName());
						else
							sprintf(buff, "%d: %s ",i+1, (const char *)  pfcModelItem::cast(WXSection)->GetName());
						line += buff;
					}
					msgs->append(line);
					Session->UIDisplayMessage("combstate_msg.txt", "USER Enter number to select %0s", msgs);
					XSectionNo = Session->UIReadIntMessage(1, XSectionsArraySize);
				}
				if(XSectionNo > 0 && XSectionNo <= XSectionsArraySize)
				{
					pfcModelItem_ptr pMdlItem = pfcModelItem::cast(WXSections->get(XSectionNo -1));
					pfcMdlItems->append(pMdlItem);
					fp_out_combstate<<"Selected Item Type: "<< pMdlItem->GetType()<<", ID: "<<pMdlItem->GetId()<<endl;
				}
			}

			//Adds Layer State item
			xstring newLayerSatetName;
			newLayerSatetName = NewCombStateName + "NewLayerState";
			wfcLayerState_ptr WLayerState = OTKUtilCreateLayerState (myWSolid, newLayerSatetName, fp_out_combstate);
			if(WLayerState != 0)
			{
				pfcMdlItems->append(pfcModelItem::cast(WLayerState));
			}

			// Creates new combined state
			wfcCombStateData_ptr WCombStateData =  wfcCombStateData::Create(pfcMdlItems,  wfcVIS_OPT_FRONT, xfalse);
			wfcCombState_ptr WCombState = myWSolid->CreateCombState(NewCombStateName, WCombStateData);

			if(WCombState != 0)
				myWSolid->ActivateCombState(WCombState);

			fp_out_combstate.close();
			return (WCombState);
		}
	}
	OTK_EXCEPTION_HANDLER(fp_out_combstate);

	return 0;
}


int OtkUtilViewInfo(pfcView_ptr View, ofstream& InfoFile, ofstream& fp_out_combstate)
{
	char file_buff[250]; 

	InfoFile << "- View Info -" << endl;
	
	if (View != 0)
	{
		xstring  ViewName = View->GetName();
		if (ViewName != xstringnil)
		{
			sprintf (file_buff, "\t - View name: %s ", (const char *)ViewName);
			InfoFile<<file_buff<<endl;
		}

	}	
	else
	{
		sprintf(file_buff,"\t - View is a default view \n");
		InfoFile<<file_buff<<endl;
	}
	
	return 0;
}
