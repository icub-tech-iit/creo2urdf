/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Utilites for layer state.
  
*/

#include <OTKXSaveFeatCxx.h>
#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <wfcSolid.h>
#include <pfcLayer.h>
#include <OTKXUtils.h> 
#include <wfcLayerState.h>
#include <fstream>


// Utility to select layer item from layer
wfcLayerItem_ptr otkUtilSelectLayerItemfromLayer(pfcLayer_ptr WLayer, int ItemNum, ofstream&  fp_out_layerstate)
{
  try
  {	
	pfcSession_ptr Session = pfcGetCurrentSession ();
	pfcModelItems_ptr WLayerItems = WLayer->ListItems ();
	int ItemsSize = WLayerItems->getarraysize ();

	if(ItemsSize > 0)
	{
	  if(ItemNum != -1 &&(ItemNum > 0 && ItemNum <= ItemsSize))
	  {
		  wfcLayerItem_ptr WSelectedLayerItem = wfcLayerItem::cast(WLayerItems->get(ItemNum -1));
		  return (WSelectedLayerItem);
	  }
	  // Select from UI
	  else
	  {
		char buff[200];
		xstring line;
		xstringsequence_ptr msgs = xstringsequence::create();

		line += "Layer item: ";
		for (int i = 0; i < ItemsSize; i++)
		{
		  //pfcModelItem_ptr WLayerItem = WLayerItems->get(i);
		  wfcLayerItem_ptr WLayerItem = wfcLayerItem::cast(WLayerItems->get(i));

		  if ( pfcModelItem::isObjKindOf(WLayerItem) )
		  {
			pfcModelItem_ptr modelItem = pfcModelItem::cast(WLayerItem);
			if(modelItem->GetName() != xstringnil)
			{
			  if(i+1 < ItemsSize)
				  sprintf(buff, "%d: %s, ",i+1, (const char *)  modelItem->GetName());
			  else
				  sprintf(buff, "%d: %s ",i+1, (const char *)  modelItem->GetName());

			  line += buff;
			}
		  }
		}
		msgs->append(line);
		Session->UIDisplayMessage("layer_state.txt", "USER Enter number to select %0s", msgs);
		int SelectedLayerNo = Session->UIReadIntMessage(1, ItemsSize);
		if(SelectedLayerNo > 0 && SelectedLayerNo <= ItemsSize)
		{
		  //pfcModelItem_ptr WSelectedLayerItem = WLayerItems->get(SelectedLayerNo -1);
		  wfcLayerItem_ptr WSelectedLayerItem = wfcLayerItem::cast(WLayerItems->get(SelectedLayerNo -1));
		  return (WSelectedLayerItem);
		}
	  }
	}

  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}

// Utility to return layer from layerstate
// If LayerNum == -1 then input will be taken through UI
//*********************************************
pfcLayer_ptr otkUtilSelectLayerfromLayerState (wfcLayerState_ptr WLayerState, 
                                      int LayerNum, ofstream&  fp_out_layerstate)
{
  try
  {	
	pfcSession_ptr Session = pfcGetCurrentSession ();

	// Getting LayerStateData from LayerState
	wfcLayerStateData_ptr WLayerData = WLayerState->GetLayerData();
	if(WLayerData != NULL)
	{
	  // Getting Layers from LayerStateData
	  pfcLayers_ptr WLayers = WLayerData->GetLayers ();
	  int layersSize = WLayers->getarraysize ();

	  if(layersSize > 0)
	  {
		if(LayerNum != -1 && (LayerNum > 0 && LayerNum <= layersSize))
		{
			pfcLayer_ptr WSelectedLayer = WLayers->get(LayerNum -1);
			return (WSelectedLayer);
		}
		// Select from UI
		else
		{
		  char buff[200];
		  xstring line;
		  xstringsequence_ptr msgs = xstringsequence::create();

		  line += "Layer: ";
		  for (int i = 0; i < layersSize; i++)
		  {
			pfcLayer_ptr WLayer = WLayers->get(i);
			if(WLayer != NULL)
			{
			  if(i+1 < layersSize)
				  sprintf(buff, "%d: %s, ",i+1, (const char *)  WLayer->GetName());
			  else
				  sprintf(buff, "%d: %s ",i+1, (const char *)  WLayer->GetName());

			  line += buff;
			}
		  }

		  msgs->append(line);
		  Session->UIDisplayMessage("layer_state.txt", "USER Enter number to select %0s", msgs);
		  int SelectedLayerNo = Session->UIReadIntMessage(1, layersSize);
		  if(SelectedLayerNo > 0 && SelectedLayerNo <= layersSize)
		  {
			pfcLayer_ptr WSelectedLayer = WLayers->get(SelectedLayerNo -1);
			return (WSelectedLayer);
		  }
		}
	  }
	}
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}

// Utility to select one of the associated layer for given layer item
//*********************************************
pfcLayer_ptr otkUtilSelectAssociatedLayer (wfcLayerItem_ptr WLayerItem,
                                    ofstream&  fp_out_layerstate)
{
  try
  {	
	pfcSession_ptr Session = pfcGetCurrentSession ();

	pfcLayers_ptr   WLayers = WLayerItem->GetLayers();
	int NoOfLayers = WLayers->getarraysize ();

	if(NoOfLayers == 1)
		return (WLayers->get(0));
	else if(NoOfLayers > 0)
	{
	  char buff[200];
	  xstring line;
	  xstringsequence_ptr msgs = xstringsequence::create();

	  line += "Layer: ";
	  for (int i = 0; i < NoOfLayers; i++)
	  {
		pfcLayer_ptr WLayer = WLayers->get(i);
		if(WLayer != NULL)
		{
		  if((i+1) < NoOfLayers)
			  sprintf(buff, "%d: %s, ",i+1, (const char *)  WLayer->GetName());
		  else
			  sprintf(buff, "%d: %s ",i+1, (const char *)  WLayer->GetName());

		  line += buff;
		}
	  }

	  msgs->append(line);
	  Session->UIDisplayMessage("layer_state.txt", "USER Enter number to select %0s", msgs);
	  int SelectedLayerNo = Session->UIReadIntMessage(1, NoOfLayers);
	  if(SelectedLayerNo > 0 && SelectedLayerNo <= NoOfLayers)
	  {
		  pfcLayer_ptr WSelectedLayer = WLayers->get(SelectedLayerNo -1);
		  return (WSelectedLayer);
	  }
	}
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}

// Utility to check layer (using I/P layer name) in soild 
// and if exist returns that layer else returns NULL
//*********************************************
pfcLayer_ptr otkUtilGetLayerExist(wfcWSolid_ptr WSolid, xstring LayerNamePassed)
{
	pfcLayers_ptr WmodelLayers = WSolid->ListLayers();

	int WLayersSize = WmodelLayers->getarraysize ();
	for(int i = 0; i < WLayersSize; i++)
	{
		pfcLayer_ptr WExistingLayer = WmodelLayers->get(i);
		xstring ExisintgLayerName = WExistingLayer->GetName();

		if(LayerNamePassed == ExisintgLayerName)
			return (WExistingLayer);
	}
	return (0);
}

// Utility t0 return LayerState from input solid
// Pass StateNum = -1 to select layer state through UI
//*********************************************
wfcLayerState_ptr otkUtilSelectLayerState (wfcWSolid_ptr WSolid, 
                                      int StateNum, ofstream&  fp_out_layerstate)
{
  try
  {	
	pfcSession_ptr Session = pfcGetCurrentSession ();

	wfcLayerStates_ptr WLayerStates =   WSolid->ListLayerStates();
	if(WLayerStates == NULL)
		return 0;

	int LayerStatesArraySize = WLayerStates->getarraysize ();
	if(LayerStatesArraySize > 0)
	{
	  if(StateNum != -1 &&(StateNum > 0&& StateNum <= LayerStatesArraySize))
	  {
		  wfcLayerState_ptr WLayerState = WLayerStates->get(StateNum -1);
		  return (WLayerState);
	  }
	  // Select from UI
	  else
	  {
		char buff[200];
		xstring line;
		xstringsequence_ptr msgs = xstringsequence::create();

		line += "Layer States: ";
		for (int i = 0; i < LayerStatesArraySize; i++)
		{
		  wfcLayerState_ptr WLayerState = WLayerStates->get(i);
		  if(WLayerState != NULL)
		  {
			if(i+1 < LayerStatesArraySize)
				sprintf(buff, "%d: %s, ",i+1, (const char *)  WLayerState->GetName());
			else
				sprintf(buff, "%d: %s ",i+1, (const char *)  WLayerState->GetName());

			line += buff;
		  }
		}
		msgs->append(line);
		Session->UIDisplayMessage("layer_state.txt", "USER Enter number to select %0s", msgs);
		int SelectedLayerStateNo = Session->UIReadIntMessage(1, LayerStatesArraySize);
		if(SelectedLayerStateNo > 0 && SelectedLayerStateNo <= LayerStatesArraySize)
		{
			wfcLayerState_ptr WLayerState = WLayerStates->get(SelectedLayerStateNo -1);
			return (WLayerState);
		}
	  }
	}
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}

// Creates new layerstate for current layers in UI
//*********************************************
wfcLayerState_ptr OTKUtilCreateLayerState (wfcWSolid_ptr WSolid, xstring wLayerStateName, ofstream& fp_out_layerstate)
{
  try
  {		
	pfcSession_ptr Session = pfcGetCurrentSession ();
	pfcModelType modelType = WSolid->GetType();


	if(!(wLayerStateName.IsEmpty()))
	{
	  pfcLayers_ptr WmodelLayers = WSolid->ListLayers();
	  //Adds Layer State item
	  int WLayersSize = WmodelLayers->getarraysize ();
	  if(WLayersSize > 0)
	  {
		pfcDisplayStatuses_ptr wDisplayStatuses = pfcDisplayStatuses::create();
		wfcLayerItems_ptr wLayerItems = wfcLayerItems::create();
		for (int i = 0; i < WLayersSize; i++)
		{
		  // Adds layers display status
		  pfcLayer_ptr WmodelLayer = WmodelLayers->get(i);
		  if(WmodelLayer != NULL)
		  {
			wDisplayStatuses->append(WmodelLayer->GetStatus());
		  }

		  // Adds hidden layer items
		  pfcModelItems_ptr PfcLayerItems = WmodelLayer->ListItems();
		  int WLayerItemsSize = PfcLayerItems->getarraysize ();
		  for (int j = 0; j < WLayerItemsSize; j++)
		  {
			wfcWModelItem_ptr WMdlItem = wfcWModelItem::cast(PfcLayerItems->get(j));
			xbool IsHidden = WMdlItem->IsHidden();
			if(IsHidden == xtrue)
			{
			  wLayerItems->append(wfcLayerItem::cast(WMdlItem));
			}
		  }
		}
		// Creates  new layer state
		wfcLayerStateData_ptr wLayerStateData = 
                      wfcLayerStateData::Create(WmodelLayers, wDisplayStatuses, wLayerItems, wLayerStateName);
		wfcLayerState_ptr WLayerState = WSolid->CreateLayerState(wLayerStateData);

		// Activates created layer state
		if(WLayerState != NULL)
			WLayerState->ActivateLayerState();
		return WLayerState;
	  }
	}
	return (0);
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}

// Utility to Update layerstate
// If updateAct = -1, LayerName = xstringnil,  ItemNum = -1 and ItemAction = -1 then input will be taken through UI
//*********************************************
int otkUtilUpdateLayerState (wfcWSolid_ptr WSolid, wfcLayerState_ptr WLayerState, 
                             int updateAct,  xstring LayerName, int ItemNum, 
                             int ItemAction, ofstream& fp_out_layerstate)
{
  try
  {
	pfcSession_ptr Session = pfcGetCurrentSession ();
	xstring LayerNameToUse;
	int SelectedLayerAction = -1;

	if(updateAct != -1 && (updateAct > 0 && updateAct <= 3))
		SelectedLayerAction = updateAct;
	// Take input from UI
	else
	{
		Session->UIDisplayMessage("layer_state.txt", "USER 1: Add Layer, 2: Remove Layer, 3: Layer item actions", 0);
		SelectedLayerAction = Session->UIReadIntMessage(1, 3);
	}

	pfcLayer_ptr WLayer;
	if(LayerName != xstringnil)
	{
		LayerNameToUse = LayerName;
		WLayer = otkUtilGetLayerExist(WSolid, LayerNameToUse);
		if(WLayer == NULL)
			return -1;
	}

	if(SelectedLayerAction > 0 && SelectedLayerAction <= 3)
	{
	  switch(SelectedLayerAction)
	  {
		// Adds Layer
		case 1: 
		{	
		  // Take input from UI
		  if(LayerName == xstringnil)
		  {
			Session->UIDisplayMessage("layer_state.txt", "USER Enter layer name: ", 0);
			LayerNameToUse = Session->UIReadStringMessage();

			WLayer = otkUtilGetLayerExist(WSolid, LayerNameToUse);
		  }
		  if(WLayer != NULL)
		  {
			pfcDisplayStatus DispStatus = WLayer->GetStatus();
			WLayerState->AddLayer(WLayer, DispStatus);
		  }
		  break;
		}

		//Removes Layer
		case 2:
		{
		  // Take input from UI
		  if(LayerName == xstringnil)
		  {
			Session->UIDisplayMessage("layer_state.txt", "USER Enter layer name: ", 0);
			LayerNameToUse = Session->UIReadStringMessage();

			WLayer = otkUtilGetLayerExist(WSolid, LayerNameToUse);
		  }
		  if(WLayer != NULL)
		  {
			pfcDisplayStatus DispStatus = WLayer->GetStatus();
			WLayerState->RemoveLayer(WLayer);
		  }
		  break;
		}

		//Layer item actions
		case 3:
		{
		  wfcLayerItem_ptr  WSelectedLayerItem;
		  pfcModelItem_ptr  pSelectedLayerItem;

		  if(ItemNum != -1 && WLayer != NULL)
		  {
			  WSelectedLayerItem = otkUtilSelectLayerItemfromLayer(WLayer, ItemNum, fp_out_layerstate);
			  pSelectedLayerItem = pfcModelItem::cast(WSelectedLayerItem); 
			  if(pSelectedLayerItem == NULL)
				  return 0;
		  }
		  // Select layer item through UI
		  else
		  {
			  pfcSelectionOptions_ptr WSelopts = pfcSelectionOptions::Create("feature, datum, csys");
			  WSelopts->SetMaxNumSels(1);	 

			  pfcSelections_ptr Sels = Session->Select(WSelopts, 0);
			  if (Sels == NULL)
				  return 0;

			  pSelectedLayerItem = (Sels->get(0))->GetSelItem();
			  if(pSelectedLayerItem == NULL)
				  return 0;
			  WSelectedLayerItem = wfcLayerItem::cast(pSelectedLayerItem);
		  }	

		  if(WSelectedLayerItem != NULL)
		  {
			int SelectedItemAction = -1;
			if(ItemAction != -1)
				SelectedItemAction = ItemAction;
			// Select from UI
			else
			{
			  Session->UIDisplayMessage("layer_state.txt", "USER 1: Hide, 2: Unhide, 3: RemoveNoUpdate, 4: AddNoUpdate", 0);
			  SelectedItemAction = Session->UIReadIntMessage(1, 4);
			}
			if(SelectedItemAction > 0 && SelectedItemAction <= 4)
			{
			  switch(SelectedItemAction)
			  {
				// Hide Layer item
				case 1: 
				{
				  WLayerState->HideLayerItem( pSelectedLayerItem );
				  break;
				}
				// Unhide Layer item
				case 2: 
				{
				  WLayerState->UnhideLayerItem( pSelectedLayerItem );
				  break;
				}
				// RemoveNoUpdate Layer for layer item
				case 3: 
				{
				  if(WLayer != 0)
					  WSelectedLayerItem->RemoveNoUpdate(WLayer);
				  //Select layer from UI
				  else
				  {
					WLayer = otkUtilSelectAssociatedLayer(WSelectedLayerItem, fp_out_layerstate);
					if(WLayer == NULL)
						return 0;
					WSelectedLayerItem->RemoveNoUpdate(WLayer);
				  }
				  break;
				}
				// AddNoUpdate Layer for layer item
				case 4: 
				{
				  if(WLayer != 0)
					  WSelectedLayerItem->AddNoUpdate(WLayer);
				  //Select layer from UI
				  else
				  {
					Session->UIDisplayMessage("layer_state.txt", "USER Enter layer name: ", 0);
					LayerNameToUse = Session->UIReadStringMessage();

					WLayer = otkUtilGetLayerExist(WSolid, LayerNameToUse);
					if(WLayer != NULL)
					{
						WSelectedLayerItem->AddNoUpdate(WLayer);
					}
				  }
				  break;
				}
			  }
			}
		  }
		  break;
		}
	  }
	}
	return (0);
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}

// Utility to dump layerstate info
//*********************************************
int otkUtilLayerStateInfo (wfcWSolid_ptr WSolid, wfcLayerState_ptr WLayerState, ofstream& fp_out_layerstateInfo, 
                           ofstream& fp_out_layerstate)
{
  xstring line;
  char buff[250];
  try
  {
	fp_out_layerstateInfo << "- Layer State Info -" << endl;

	// Getting GetLayerState Name from LayerState
	xstring  WStateName = WLayerState->GetLayerStateName();
	sprintf(buff, " - Layer State Name: %s, ID: %d, ", const_cast<char*>((cStringT) WStateName), WLayerState->GetId() );
	line = buff;

	// Getting LayerStateData from LayerState
	wfcLayerStateData_ptr WLayerData = WLayerState->GetLayerData();
	if(WLayerData != NULL)
	{
	  // Getting DisplayStatuses from LayerStateData
	  pfcDisplayStatuses_ptr WDisplayStatuses =  WLayerData->GetDisplayStatuses();
	  int WDisplayStatusesSize = WDisplayStatuses->getarraysize ();
	  sprintf(buff, "Number of DisplayStatuses: %d, ",WDisplayStatusesSize);
	  line += buff;

	  // Getting Layers from LayerStateData
	  pfcLayers_ptr WLayers = WLayerData->GetLayers ();
	  int layersSize = WLayers->getarraysize ();
	  sprintf(buff, "Number of layers: %d",layersSize);
	  line += buff;
	  fp_out_layerstateInfo<<line<<endl;

	  // Getting layer info
	  for (int i = 0; i < layersSize; i++)		
	  {
		pfcLayer_ptr WLayer = WLayers->get(i);
		sprintf(buff, "\t- Layer %d ID: %d, Name: %s,",(i+1), WLayer->GetId(), (const char *)  WLayer->GetName());
		line = buff;

		// Getting layers dicplay status
		pfcDisplayStatus DispStatus = WLayer->GetStatus();
		switch(DispStatus)
		{
		  case 0: 
			  line += " status: LAYER_NORMAL,";
			  break;

		  case 1:
			  line += " status: LAYER_DISPLAY,";
			  break;

		  case 2:
			  line += " status: LAYER_BLANK,";
			  break;

		  case 3:
			  line += " status: LAYER_HIDDEN,";
			  break;
		}
		pfcModelItems_ptr WLayerItems = WLayer->ListItems ();
		int LayerItemsSize = WLayerItems->getarraysize ();
		sprintf(buff, " Number of items: %d", LayerItemsSize);

		line += buff;
		fp_out_layerstateInfo<<line<<endl;

		for (int i = 0; i < LayerItemsSize; i++)
		{
		  wfcLayerItem_ptr WLayerItem  =  wfcLayerItem::cast(WLayerItems->get(i));
                              // another approach to casting: instead of calling isObjKindOf, 
                              // we do the cast without any checking - it will produce null if not applicable
                              pfcModelItem_ptr modelItem = pfcModelItem::cast(WLayerItem); 
          if (modelItem)
		  {
			xstring WLayerItemName = modelItem->GetName();
			sprintf(buff, "\t\t - Item%d ID: %d, ", (i+1), modelItem->GetId());
			line = buff;

			if(!(WLayerItemName.IsEmpty()))
			{
			  sprintf(buff, "Name: %s, ", (const char *)WLayerItemName);
			  line += buff;
			}
			// Getting LayerItems Hidden status
			xbool IsItemHidden = WLayerState->IsLayerItemHidden(modelItem);
			line += (IsItemHidden == xtrue)?"Hidden, ":"Not hidden, ";
		  }

		  // Getting LayerItems Visible status
		  xbool IsItemVisible = WLayerItem->IsLayerItemVisible();
		  line += (IsItemVisible == xtrue)?"Visible, ":"Not visible, ";

		  // Getting number of layers associated with layer item
		  pfcLayers_ptr   LayersWithItem = WLayerItem->GetLayers();
		  int NoOfLayers = LayersWithItem->getarraysize ();
		  sprintf(buff, "Associated layers: %d", NoOfLayers);
		  line += buff;
		  fp_out_layerstateInfo<<line<<endl;
	    }
	    fp_out_layerstateInfo<<endl;
	  }

	  // Getting Hidden LayerItems from LayerStateData
	  wfcLayerItems_ptr WLayerItems  =  WLayerData->GetLayerItems ();
	  int LayerItemsSize = WLayerItems->getarraysize ();
	  fp_out_layerstateInfo<<" - Hidden LayerItems in this layer state: "<<LayerItemsSize<<endl;

	  for (int i = 0; i < LayerItemsSize; i++)
	  {
		wfcLayerItem_ptr WLayerItem  =  WLayerItems->get(i);
		pfcModelItem_ptr modelItem = pfcModelItem::cast(WLayerItem);
		if (modelItem)
		{
		  sprintf(buff, "\t\t - Hidden Item%d ID: %d, ", (i+1), modelItem->GetId());
		  line = buff;

		  xstring WLayerItemName = modelItem->GetName();
		  if(!(WLayerItemName.IsEmpty()))
		  {
			sprintf(buff, "Name: %s, ", (const char *)WLayerItemName);
			line += buff;
		  }
		}

		// Getting LayerItems Visible status
		xbool IsItemVisible = WLayerItem->IsLayerItemVisible();
		line += (IsItemVisible == xtrue)?"Visible, ":"Not visible, ";

		// Getting number of layers associated with layer item
		pfcLayers_ptr   LayersWithItem = WLayerItem->GetLayers();
		int NoOfLayers = LayersWithItem->getarraysize ();
		sprintf(buff, "Number of Layers associated: %d", NoOfLayers);
		line += buff;
		fp_out_layerstateInfo<<line<<endl;

		wfcLayerState_ptr WActiveLayerState = WSolid->GetActiveLayerState();
		if(WActiveLayerState!= NULL)
		{
			fp_out_layerstateInfo<<" - Current active Layer State: "<<WActiveLayerState->GetLayerStateName()<<endl;
		}
	  }
	}
	return (0);
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}
    

// Utility to delete layerstate
//*********************************************
int otkUtilDeleteLayerState (wfcLayerState_ptr WLayerState, ofstream& fp_out_layerstate)
{
  try
  {
	WLayerState->DeleteLayerState();
	return (0);
  }
  OTK_EXCEPTION_HANDLER(fp_out_layerstate);
  return (0);
}
