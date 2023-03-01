/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
   
Utilites for Assembly Item / Flexible Component operations

*/

#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <wfcSolid.h>
#include <pfcSession.h>
#include <pfcModel.h>
#include <wfcPart.h>
#include <wfcAssembly.h>
#include <wfcComponentFeat.h>
#include <OTKXUtils.h> 
#include <OTKXAssemblyItemUtils.h>

#include <fstream>


//***********************************************************
/* Use dimension Ids to append Assembly Item array 
 */
wfcAssemblyItems_ptr OTKXUtilPopulateAssemblyItemsDimensionArray(
			 wfcAssemblyItems_ptr AsmItemArray,		
			 wfcWAssembly_ptr WAsm,
			 pfcModel_ptr ItemOwner, 
			 xintsequence_ptr DimIds, 
			 wfcWComponentPath_ptr ItemCompPath,
			 ofstream &fp_out_asmitem)
{	
  try 
    {
      for (xint i=0 ; i<DimIds->getarraysize() ; i++)
	{
	  wfcAssemblyItemInstructions_ptr ThisAsmInstr = 
	    wfcAssemblyItemInstructions::Create(ItemOwner, pfcITEM_DIMENSION, DimIds->get(i) );
	  
	  if (ItemCompPath != 0)
	    ThisAsmInstr->SetItemCompPath(ItemCompPath);
	  
	  wfcAssemblyItem_ptr ThisAsmItem = WAsm->CreateAssemblyItem(ThisAsmInstr);
	  
	  AsmItemArray->append(ThisAsmItem);
	}
    }

  OTK_EXCEPTION_HANDLER(fp_out_asmitem);
  
  return (AsmItemArray);
  
}


//***********************************************************
/* Use feature Ids to append Assembly Item array 
 */
wfcAssemblyItems_ptr OTKXUtilPopulateAssemblyItemsFeatureArray(
			wfcAssemblyItems_ptr AsmItemArray,		
			wfcWAssembly_ptr WAsm,
			pfcModel_ptr ItemOwner, 
			xintsequence_ptr FeatIds, 
			wfcWComponentPath_ptr ItemCompPath,
			ofstream &fp_out_asmitem)
{	
  try
    {
      for (xint i=0 ; i<FeatIds->getarraysize() ; i++)
	{
	  wfcAssemblyItemInstructions_ptr ThisAsmInstr = 
	    wfcAssemblyItemInstructions::Create(ItemOwner, pfcITEM_FEATURE, FeatIds->get(i) );
	  
	  if (ItemCompPath != 0)
	    ThisAsmInstr->SetItemCompPath(ItemCompPath);
	  
	  wfcAssemblyItem_ptr ThisAsmItem = WAsm->CreateAssemblyItem(ThisAsmInstr);
	  
	  AsmItemArray->append(ThisAsmItem);
	}
    }
  
  OTK_EXCEPTION_HANDLER(fp_out_asmitem);
  
  return (AsmItemArray);
  
}

//***********************************************************
/*  Check component flexibility & set as flexible using 
    provided Assembly Item array 
*/
wfcWComponentFeat_ptr OTKXUtilSetFlexibleComponent(pfcSolid_ptr OwnerSolid, 
	xint FeatId, wfcAssemblyItems_ptr AsmItemArray, ofstream &fp_out_asmitem)
{
  pfcComponentFeat_ptr CFeat = 0;
  wfcWComponentFeat_ptr WCFeat = 0 ;
  
  try 
    {
      pfcFeatures_ptr Feats = OwnerSolid->ListFeaturesByType(xtrue, pfcFEATTYPE_COMPONENT);
      
      for (xint i=0 ; i<Feats->getarraysize() ; i++)
	{
	  if (Feats->get(i)->GetId() == FeatId) 
	    {
	      fp_out_asmitem << "For Feature ID: " << Feats->get(i)->GetId() << endl ;
	      fp_out_asmitem << "With Feature Number: " << Feats->get(i)->GetNumber() << endl ;
	      
	      CFeat = pfcComponentFeat::cast(Feats->get(i));				
	      WCFeat = wfcWComponentFeat::cast(CFeat);
	    }
	}
      
      xbool is_flex = WCFeat->IsFlexible();
      
      fp_out_asmitem << " Is Component Already Flexible ? : " << (is_flex ? "YES" : "NO") << endl;
      
      if (!is_flex)
	{
	  fp_out_asmitem << "Making Component Flexible..." << endl; 
	  
	  WCFeat->SetAsFlexible(AsmItemArray);
	  
	  is_flex = WCFeat->IsFlexible();
	  
	  fp_out_asmitem << " Is Component Flexible ? : " << (is_flex ? "YES" : "NO") << endl;
	}
    }
  
  OTK_EXCEPTION_HANDLER(fp_out_asmitem);
  
  return (WCFeat);
  
}

//***********************************************************
/*  Create flexible model using provided Assembly Item array 
 */
pfcModel_ptr OTKXUtilCreateFlexibleModel (pfcSolid_ptr OwnerSolid, 
	xint FeatId, wfcWComponentPath_ptr WAsmCompPath, 
	wfcAssemblyItems_ptr AsmItemArray, ofstream &fp_out_asmitem)
{
  pfcModel_ptr FlexModel = 0;
  
  try 
    {
      wfcWComponentFeat_ptr WCFeat = OTKXUtilSetFlexibleComponent(OwnerSolid,
								  FeatId, AsmItemArray, fp_out_asmitem);
      
      fp_out_asmitem << "Creating Flexible Model..." << endl; 
      
      FlexModel = WCFeat->CreateFlexibleModel(WAsmCompPath);
    }
  
  OTK_EXCEPTION_HANDLER(fp_out_asmitem);
  
  return (FlexModel);
}


//***********************************************************
/*  Check component flexibility and create predefined 
    flexibility component using provided Assembly Item array 
*/
void OTKXUtilCreatePredefinedFlexibilityComponent(
	pfcSolid_ptr OwnerSolid, 
	xint FeatId, wfcAssemblyItems_ptr AsmItemArray, 
	pfcModel_ptr ItemOwner, 
	ofstream &fp_out_asmitem )
{
  try {
    
    pfcFeatures_ptr Feats = OwnerSolid->ListFeaturesByType(xtrue, pfcFEATTYPE_COMPONENT);
    
    pfcComponentFeat_ptr CFeat;
    wfcWComponentFeat_ptr WCFeat ;
    
    for (xint i=0 ; i<Feats->getarraysize() ; i++)
      {
	if (Feats->get(i)->GetId() == FeatId) 
	  {
	    fp_out_asmitem << "For Feature ID: " << Feats->get(i)->GetId() << endl ;
	    fp_out_asmitem << "With Feature Number: " << Feats->get(i)->GetNumber() << endl ;
	    
	    CFeat = pfcComponentFeat::cast(Feats->get(i));				
	    WCFeat = wfcWComponentFeat::cast(CFeat);
	  }
      }
    
    xbool is_flex = WCFeat->IsFlexible();
    
    fp_out_asmitem << " Is Component Already Flexible ? : " << (is_flex ? "YES" : "NO") << endl;
    
    if (!is_flex)
      {
	fp_out_asmitem << "Creating Predefined Flexibility Component..." << endl; 
	
	WCFeat->CreatePredefinedFlexibilityComponent(ItemOwner, AsmItemArray);
	
	is_flex = WCFeat->IsFlexible();
	
	fp_out_asmitem << " Is Component Flexible ? : " << (is_flex ? "YES" : "NO") << endl;
      }	    
  }

  OTK_EXCEPTION_HANDLER(fp_out_asmitem);
}

//***********************************************************

