/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
   
This example will demonstrate creating & editing a Sweep feature using element tree.
It will first collect curves & other geometry required to create sweep
It will then create an incomplete feature - without filling the wfcE_SKETCHER element.
It will then obtain the initialized section from incomplete feature & populate it with section entities.
It will then redefine feature with the filled wfcE_SKETCHER element, to complete its definition.
It will also get section from feature, edit it and redefine it.

This example also shows how to use constants defined in header files of Creo Parametric Toolkit
for setting element values in an Object Toolkit C++ application .
*/

#include <wfcElemIds.h>
#include <ProSweep.h>
#include <pfcSession.h>
#include <wfcSession.h>
#include <pfcGlobal.h>
#include <pfcExceptions.h>
#include <pfcUI.h>
#include <pfcSelect.h>
#include <wfcSolid.h>
#include <ProStdSection.h>
#include <wfcModel.h>
#include <wfcSection.h>
#include <OTKXUtils.h>

static void otkBuildElementTree(wfcCollection_ptr &coll,pfcSelections_ptr &sels , ofstream &exception_info);
static void SweepEllipseSection(wfcWFeature_ptr &feat, ofstream &exception_info);
static wfcElement_ptr GetSketcherElement(wfcElementTree_ptr &elemTree);


//*********************************************
extern "C" wfcStatus otkSweepCreate ()
{	
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);
  try
    {

      pfcSession_ptr Session = pfcGetCurrentSession ();
      pfcModel_ptr currModel = Session->GetCurrentModel();
      
      /* 
	 Set Message dialog options 
      */
      pfcMessageDialogOptions_ptr msgOpts = pfcMessageDialogOptions::Create();
      pfcMessageButtons_ptr buttons = pfcMessageButtons::create();
      buttons->append(pfcMESSAGE_BUTTON_OK);
      msgOpts->SetButtons(buttons);
      msgOpts->SetMessageDialogType(pfcMESSAGE_WARNING);
      msgOpts->SetDialogLabel("OTK Sweep Feature Create ERROR");
      
      if(!currModel)
	{
	  Session->UIShowMessageDialog("Model must be in current Session",msgOpts);
	  pfcXToolkitBadContext::Throw("pfcSession::GetCurrentModel"," otkSweepCreate"); 	
	}	  
      
      pfcModelType mdlType = currModel->GetType(); 		
      
      if((mdlType != pfcMDL_ASSEMBLY) && (mdlType != pfcMDL_PART))
	{
	  Session->UIShowMessageDialog("Model must be part or assembly",msgOpts);
	  pfcXToolkitBadContext::Throw("pfcSession::GetType"," otkSweepCreate"); 	
	}
      
      else
	{		
	  wfcWSolid_ptr WSolid = wfcWSolid::cast(currModel);
	  
	  try
	    {
	      wfcChainCollectionUIControls_ptr chainCntrls = wfcChainCollectionUIControls::create();
	      chainCntrls->append(wfcCHAINCOLLUI_ALLOW_ALL);
	      
	      /*
		Collect curves for element wfcE_STD_CURVE_COLLECTION_APPL.
		This will be a trajectory for Sweep
	      */
	      wfcCollection_ptr trajCollection = WSolid->CollectCurves(chainCntrls,NULL,xany(0),NULL);
	      
	      if(!trajCollection)
		pfcXToolkitUserAbort::Throw("pfcSession::CollectCurves"," otkSweepCreate");
	      
	      Session->UIDisplayMessage("sweep_create_msg.txt","USER Select pivot direction",NULL);
	      
	      pfcSelectionOptions_ptr dirSelopts = 	pfcSelectionOptions::Create("edge,curve,csys,surface,axis");
	      dirSelopts->SetMaxNumSels(1);	 
	      
	      /*
		Select direction for element wfcE_DIRECTION_REFERENCE
		Direction to propagate with respect to trajectory 
	      */
	      pfcSelections_ptr dirSels = Session->Select(dirSelopts);
	      
	      /*
		Start building element tree using OTK ElementTree methods
		Create incomplete sweep feature using element tree
		Add circular section to it and redfine
		Get sections of the sweep using Feature-Section methods
		Replace circular section with elliptical one
		Redefine feature using elliptical section
	      */
	      otkBuildElementTree(trajCollection,dirSels, exception_info);
	    }
	  xcatchbegin
	    
	    xcatch(pfcXToolkitUserAbort,defaultEx)
	    pfcSession_ptr Session = pfcGetCurrentSession ();
	    Session->UIShowMessageDialog("Curve Collection or Pivot Direction selection aborted. Sweep creation failed",msgOpts);
	  
	  xcatchend
	    
	    }
		   
    }
	OTK_EXCEPTION_HANDLER(exception_info);  
 
	return wfcTK_NO_ERROR;
}



static void otkBuildElementTree(wfcCollection_ptr &coll,pfcSelections_ptr &sels, ofstream &exception_info )
{	
	
  try{
    
    wfcElements_ptr elements = wfcElements::create();
    
    /*
      Create each element using wfcElement::Create() method
      NOTE 1: Value of each element should be represented by pfcArgValue_ptr
      except for compound and array elements for which value should be 0
               
      NOTE 2: Every element is placed at differnt  level in element tree present
      Level starts with integer 0.
      wfcE_FEATURE_TREE is always a Level 0 element
      
      NOTE 3: All these elements should be added in a sequence of wfcElement
      Latter same sequence will be used to build element tree
      
    */
    
    //wfcPRO_E_FEATURE_TREE
    wfcElement_ptr elem_0_0 = wfcElement::Create(wfcPRO_E_FEATURE_TREE,0,0);
    elements->append(elem_0_0);		
    
    //wfcPRO_E_FEATURE_FORM
    wfcElement_ptr elem_1_1 = wfcElement::Create(wfcPRO_E_FEATURE_FORM,pfcCreateIntArgValue(4),1);
    elements->append(elem_1_1);
    
    //wfcPRO_E_SWEEP_TYPE
    wfcElement_ptr elem_1_2 = wfcElement::Create(wfcPRO_E_SWEEP_TYPE,pfcCreateIntArgValue(PRO_SWEEP_TYPE_MULTI_TRAJ),1);
    elements->append(elem_1_2);
    
    //wfcPRO_E_SWEEP_FRAME_COMP
    wfcElement_ptr elem_1_3 = wfcElement::Create(wfcPRO_E_SWEEP_FRAME_COMP,0,1);
    elements->append(elem_1_3);
    
    //wfcPRO_E_FRM_OPT_TRAJ
    wfcElement_ptr elem_2_0 = wfcElement::Create(wfcPRO_E_FRM_OPT_TRAJ,0,2);
    elements->append(elem_2_0);
    
    //wfcPRO_E_OPT_TRAJ
    wfcElement_ptr elem_3_0 = wfcElement::Create(wfcPRO_E_OPT_TRAJ,0,3);
    elements->append(elem_3_0);

    //wfcPRO_E_STD_SEC_METHOD
    wfcElement_ptr elem_4_0 = wfcElement::Create(wfcPRO_E_STD_SEC_METHOD,pfcCreateIntArgValue(PRO_SEC_SELECT),4);
    elements->append(elem_4_0);
		
    //wfcPRO_E_STD_SEC_SELECT
    wfcElement_ptr elem_4_1 = wfcElement::Create(wfcPRO_E_STD_SEC_SELECT,0,4);
    elements->append(elem_4_1);

    //wfcPRO_E_STD_CURVE_COLLECTION_APPL
    wfcElement_ptr elem_5_0 = wfcElement::Create(wfcPRO_E_STD_CURVE_COLLECTION_APPL,NULL,5);
    elem_5_0->SetElemCollection(coll);
    elements->append(elem_5_0);
    
    //wfcPRO_E_FRAME_SETUP
    wfcElement_ptr elem_2_1 = wfcElement::Create(wfcPRO_E_FRAME_SETUP,0,2);
    elements->append(elem_2_1);
		
    //wfcPRO_E_FRM_NORMAL
    wfcElement_ptr elem_3_1 = wfcElement::Create(wfcPRO_E_FRM_NORMAL,pfcCreateIntArgValue(PRO_FRAME_PIVOT_DIR),3);
    elements->append(elem_3_1);
    
    //wfcPRO_E_FRM_PIVOT_DIR
    wfcElement_ptr elem_3_2 = wfcElement::Create(wfcPRO_E_FRM_PIVOT_DIR,0,3);
    elements->append(elem_3_2);
    
    //wfcPRO_E_DIRECTION_COMPOUND
    wfcElement_ptr elem_4_2 = wfcElement::Create(wfcPRO_E_DIRECTION_COMPOUND,0,4);
    elements->append(elem_4_2);
		 
    //wfcPRO_E_DIRECTION_REFERENCE
    wfcElement_ptr elem_5_1 = wfcElement::Create(wfcPRO_E_DIRECTION_REFERENCE,pfcCreateSelectionArgValue(sels->get(0)),5);
    elements->append(elem_5_1);
			 
    //wfcPRO_E_DIRECTION_FLIP
    wfcElement_ptr elem_5_2 = wfcElement::Create(wfcPRO_E_DIRECTION_FLIP,pfcCreateIntArgValue(1),5);
    elements->append(elem_5_2);
    
    //wfcPRO_E_FRM_ORIENT
    wfcElement_ptr elem_3_3 = wfcElement::Create(wfcPRO_E_FRM_ORIENT,pfcCreateIntArgValue(PRO_FRAME_CONSTANT),3);
    elements->append(elem_3_3);
    
    //wfcPRO_E_STD_FEATURE_NAME
    wfcElement_ptr elem_1_4 = wfcElement::Create(wfcPRO_E_STD_FEATURE_NAME,pfcCreateStringArgValue("OTK_SWEEP"),1);
    elements->append(elem_1_4);
    
    //wfcPRO_E_EXT_SURF_CUT_SOLID_TYPE
    wfcElement_ptr elem_1_5 = wfcElement::Create(wfcPRO_E_EXT_SURF_CUT_SOLID_TYPE,pfcCreateIntArgValue(PRO_SWEEP_FEAT_TYPE_SOLID),1);
    elements->append(elem_1_5);
    
    //wfcPRO_E_REMOVE_MATERIAL
    wfcElement_ptr elem_1_6 = wfcElement::Create(wfcPRO_E_REMOVE_MATERIAL,pfcCreateIntArgValue(PRO_SWEEP_MATERIAL_ADD),1);
    elements->append(elem_1_6);
    
    //wfcPRO_E_FEAT_FORM_IS_THIN
    wfcElement_ptr elem_1_7 = wfcElement::Create(wfcPRO_E_FEAT_FORM_IS_THIN,pfcCreateIntArgValue(PRO_SWEEP_FEAT_FORM_NO_THIN ),1);
    elements->append(elem_1_7);

    
   pfcSession_ptr Session = pfcGetCurrentSession ();
   wfcWSession_ptr WSession = wfcWSession::cast(Session);
   
   /*
     Pass sequence of wfcElement to CreateElementTree() to get wfcElementTree object 
   */
  
   wfcElementTree_ptr	elemTree = WSession->CreateElementTree(elements);
   
   /*
     Create an incomplete sweep feature using element tree built up till now
     
     NOTE 1: We have not defined element wfcPRO_E_SKETCHER yet
     
     NOTE 2: Feature must be created with option wfcFEAT_CR_INCOMPLETE_FEAT.
     
     NOTE 3: Mention feature regeneration options
   */  
   wfcFeatCreateOptions_ptr featOpts = wfcFeatCreateOptions::create();
   featOpts->append(wfcFEAT_CR_INCOMPLETE_FEAT);
   wfcWRegenInstructions_ptr regenInstr = wfcWRegenInstructions::Create();
   
   pfcModel_ptr currModel = Session->GetCurrentModel();
   wfcWSolid_ptr currSolid = wfcWSolid::cast(currModel);		
   
   wfcWFeature_ptr sweepFeat = currSolid->WCreateFeature(elemTree,featOpts,regenInstr);
   
   /*
     Extract element tree of incomplete sweep feature.
     Get element wfcPRO_E_SKETCHER
     Get initialized  section from element wfcPRO_E_SKETCHER
     Modify section by adding required entities.
     
     NOTE 1: We must use initialized section returned by sketchSpvalue->GetSectionValue();
     This section have defined location and orientation
     based on frame created by incomplete  sweep feature
     
     NOTE 2: This is a 3D Section because it has a defined orientation & location
     set by Creo
   */
   elemTree = sweepFeat->GetElementTree(0,wfcFEAT_EXTRACT_NO_OPTS);
 
   wfcElement_ptr sketchElem = GetSketcherElement(elemTree);
   
   wfcSpecialValue_ptr sketchSpvalue = sketchElem->GetSpecialValueElem();
   
   wfcSection_ptr sketch = sketchSpvalue->GetSectionValue();   
   
   /*    
     Adding a Circle entity to section
     NOTE: wfcSectionEntityCircle_ptr must be cast to wfcSectionEntity_ptr
           before passing it to AddEntity.
   */

   
   pfcPoint2D_ptr center = wfcCreatePoint2D(0.00,0.00);
   wfcSectionEntityCircle_ptr circle = wfcSectionEntityCircle::Create(center,30.00);
   wfcSectionEntity_ptr entity = wfcSectionEntity::cast(circle);
   
   sketch->AddEntity(entity);
   
	/* Adding a Rectangle entity to section */
	pfcOutline2D_ptr line1 = wfcCreateOutline2D(-50.00,40.00,50.00,40.00);
	wfcSectionEntityLine_ptr lineEnt1 = wfcSectionEntityLine::Create(line1);
	entity = wfcSectionEntity::cast(lineEnt1);
	sketch->AddEntity(entity);

	pfcOutline2D_ptr line2 = wfcCreateOutline2D(50.00,40.00,50.00,-40.00);
	
	wfcSectionEntityLine_ptr lineEnt2 = wfcSectionEntityLine::Create(line2);
	entity = wfcSectionEntity::cast(lineEnt2);
	sketch->AddEntity(entity);
	
	pfcOutline2D_ptr line3 = wfcCreateOutline2D(50.00,-40.00,-50.00,-40.00);
	
	wfcSectionEntityLine_ptr lineEnt3 = wfcSectionEntityLine::Create(line3);
	entity = wfcSectionEntity::cast(lineEnt3);
	sketch->AddEntity(entity);
	
	pfcOutline2D_ptr line4 = wfcCreateOutline2D(-50.00,-40.00,-50.00,40.00);
     
	wfcSectionEntityLine_ptr lineEnt4 = wfcSectionEntityLine::Create(line4);
	entity = wfcSectionEntity::cast(lineEnt4);
	sketch->AddEntity(entity);

    /*
     Use Section APIs to add entities
     NOTE: Intent Manager must be turn ON after adding entities
     to ensure section resolution done properly 
   */   

	sketch->SetIntentManagerMode(true);
    sketch->SetIntentManagerMode(false);

   /* Redefining the feature */ 
   sweepFeat->RedefineFeature(0,elemTree,featOpts,regenInstr);


   /* Adding one more Rectangle entity to section */
	pfcOutline2D_ptr line5 = wfcCreateOutline2D(-80.00,70.00,80.00,70);
	wfcSectionEntityLine_ptr lineEnt5 = wfcSectionEntityLine::Create(line5);
	entity = wfcSectionEntity::cast(lineEnt5);
	sketch->AddEntity(entity);

	pfcOutline2D_ptr line6 = wfcCreateOutline2D(80.00,70.00,80.00,-70.00);
	
	wfcSectionEntityLine_ptr lineEnt6 = wfcSectionEntityLine::Create(line6);
	entity = wfcSectionEntity::cast(lineEnt6);
	sketch->AddEntity(entity);
	
	pfcOutline2D_ptr line7 = wfcCreateOutline2D(80.00,-70.00,-80.00,-70.00);
	
	wfcSectionEntityLine_ptr lineEnt7 = wfcSectionEntityLine::Create(line7);
	entity = wfcSectionEntity::cast(lineEnt7);
	sketch->AddEntity(entity);
	
	pfcOutline2D_ptr line8 = wfcCreateOutline2D(-80.00,-70.00,-80.00,70.00);
     
	wfcSectionEntityLine_ptr lineEnt8 = wfcSectionEntityLine::Create(line8);

	entity = wfcSectionEntity::cast(lineEnt8);
	sketch->AddEntity(entity);

	/* Solving a feature with previous entities and another Rectangle */
	sketch->SetIntentManagerMode(true);
    sketch->SetIntentManagerMode(false);

   /* Redefining the feature */
	sweepFeat->RedefineFeature(0,elemTree,featOpts,regenInstr);

	 /* Calling funtion to add Ellipse */
	SweepEllipseSection(sweepFeat, exception_info);
   
   sketchSpvalue->SetSectionValue(sketch);
   sketchElem->SetSpecialValueElem(sketchSpvalue);
   
   /*
     Get section of the sweep feature and edit it 
     NOTE: Here we are getting section of sweep using OTK Feature-Section methods
     and not through element tree 
   */

   wfcElementTree_ptr final_tree = sweepFeat->GetElementTree(NULL,wfcFEAT_EXTRACT_NO_OPTS);

   final_tree->WriteElementTreeToFile(wfcELEMTREE_XML,"createsweep_data.inf");
  }
  OTK_EXCEPTION_HANDLER(exception_info);  
}

static void SweepEllipseSection(wfcWFeature_ptr &feat, ofstream &exception_info)
{
  
  try
    {
      /*
	Get sections in the sweep feature
	NOTE: We are editing only section which is available to use
	Section like curved trajectory is unavailable.
      */
      
      wfcSections_ptr sections = feat->GetSections();
      
      wfcSection_ptr section = sections->get(sections->getarraysize() - 1);
      
      /* Adding a Ellipse entity to section */
      
      xintsequence_ptr ids = section->GetEntityIds();
      
      pfcPoint2D_ptr center = wfcCreatePoint2D(0.00,0.00);

      wfcSectionEntityEllipse_ptr ellipse = wfcSectionEntityEllipse::Create(center,150,100.00);
	  wfcSectionEntity_ptr ellipseentity = wfcSectionEntity::cast(ellipse);
      
	  section->AddEntity(ellipseentity);

    /* Solving the feature with previous entities and Ellipse */
	  section->SetIntentManagerMode(true);
      section->SetIntentManagerMode(false);

      wfcElementTree_ptr elemTree = feat->GetElementTree(0,wfcFEAT_EXTRACT_NO_OPTS);
      
      /*
	Set modified section back to wfcPRO_E_SKETCHER
	NOTE : Here we are listing elements and filtering the list to get wfcPRO_E_SKECTHER
	once we got the element we are modifying the section value     
      */
      
      wfcElements_ptr elements = elemTree->ListTreeElements();
      
      for(int ii=0;ii<elements->getarraysize();ii++)
	{
	  
	  if(elements->get(ii)->GetId()==wfcPRO_E_SKETCHER && elements->get(ii)->GetLevel()==3)
	    {
	      elements->get(ii)->GetSpecialValueElem()->SetSectionValue(section);
	      break;
	    }
	}
      
      wfcWSession_ptr WSession  = wfcWSession::cast(pfcGetCurrentSession());
      
      wfcElementTree_ptr newElemtree = WSession->CreateElementTree(elements);

	  /*
     Set modified section into wfcPRO_E_SKETCHER element
     NOTE 1: Sweep feature has to be redefined with modified element tree
     
     NOTE 2: Feature must be redefined with option wfcFEAT_CR_NO_OPTS
             to ensure complete feature creation */
      
      
      wfcFeatCreateOptions_ptr featOpts = wfcFeatCreateOptions::create();
      featOpts->append(wfcFEAT_CR_NO_OPTS);
      wfcWRegenInstructions_ptr regenInstr = wfcWRegenInstructions::Create();
      
      feat->RedefineFeature(0,newElemtree,featOpts,regenInstr);
    }
	OTK_EXCEPTION_HANDLER(exception_info); 
}

static wfcElement_ptr GetSketcherElement(wfcElementTree_ptr &elemTree)
{
  /*
    Get wfcPRO_E_SKECTHER element using wfcElementPath_ptr
    
    NOTE: To get an element with level greater than 1 you must
    define element path with respect to wfcPRO_E_FEATURE_TREE 
    
  */	
	
  
  wfcElemPathItems_ptr sketchItems =  wfcElemPathItems::create();
  wfcElemPathItem_ptr sketchItem0 =  wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID,wfcPRO_E_SWEEP_PROF_COMP);
  sketchItems->append(sketchItem0);
  wfcElemPathItem_ptr sketchItem1 =  wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID,wfcPRO_E_SWEEP_SECTION);
  sketchItems->append(sketchItem1);
  wfcElemPathItem_ptr sketchItem2 =  wfcElemPathItem::Create(wfcELEM_PATH_ITEM_TYPE_ID,wfcPRO_E_SKETCHER);
  sketchItems->append(sketchItem2);
  
  wfcElementPath_ptr sketchPath = wfcElementPath::Create(sketchItems);
  
  wfcElement_ptr element = elemTree->GetElement(sketchPath);
  return(element);
  
}

