/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/



#include <OTKXCrossSection.h>

static int xsectionCommandCounter = 0;
static wfcWSession_ptr wSession=0;
static ofstream xsectionException;

/**********************************************************************
Action-Object approach to create planar xsection
Overrides onCommand method from pfcUICommandActionListener.
**********************************************************************/
void XSectionExamples::OnCommand()
{	
  xsectionException.open("Exception.inf",ios::out);
  
  try
    {	  
      wSession = wfcWSession::cast( pfcGetCurrentSession ());
      
      /*
	NOTE 1 : If command is invoked using Object-Action way datum plane selection will be
	extracted from selection buffer
      */
      
      if(mObjectAction== xtrue)
	{
	  pfcSelectionBuffer_ptr selBuffer = wSession->GetCurrentSelectionBuffer();
	  
	  pfcSelections_ptr sels = selBuffer->GetContents();
	  
	  xint numSels=0;
	  
	  if(sels != 0)
	    numSels = sels->getarraysize();
	  
	  if(numSels != 1)
	    return;
	  
	  pfcModelItem_ptr selItem = sels->get(0)->GetSelItem();
	  
	  if(selItem->GetType() == pfcITEM_SURFACE)
	  {
	    pfcSelection_ptr surface_sels = sels->get(0);
	    CreatePlanarXSection(surface_sels);	  
	  }
	}
      
      else
	{	  
	  pfcModel_ptr currModel = wSession->GetCurrentModel();
	  
	  pfcModelType mdlType = currModel->GetType();
	  
	  if(!currModel)
	    {
	      wSession->UIDisplayMessage("xsection_example_msg.txt","USER Not in session",NULL);
	      pfcXToolkitBadContext::Throw("pfcSession::GetCurrentModel"," XSectionExamples::OnCommand"); 	
	    }      
	  
	  else if((mdlType != pfcMDL_ASSEMBLY) && (mdlType != pfcMDL_PART))
	    {
	      wSession->UIDisplayMessage("xsection_example_msg.txt","USER Invalid model type",NULL);
	      pfcXToolkitBadContext::Throw("pfcSession::GetType"," XSectionExamples::OnCommand"); 	
	    }
	  
	  else
	    {		
	      
	      if(mMethod.compare("Edit Hatching") == 0)
		EditSectionHatching();
	      
	      else
	      {
		pfcSelection_ptr no_sel = 0;
		CreatePlanarXSection(no_sel); 
	      }
	      
	    }
	}
    }
  
  OTK_EXCEPTION_HANDLER(xsectionException);
  xsectionException.close();  
    
  return;
}

/**********************************************************************
Object-Action approach to create planar xsection

Overrides OnPopupmenuCreate() method from pfcPopupmenuListener.
When "Sel Obj Menu" is created in session it adds a button to it.
After clicking the button it gets processed through XSectionExamples::onCommand()

**********************************************************************/

void XSectionExamplesPopupMenu::OnPopupmenuCreate (pfcPopupmenu_ptr Menu)
{
  
  wSession = wfcWSession::cast( pfcGetCurrentSession ());
  
  try
    {
      
      /* NOTE 2: Check if popupmenu is selection object menu */		
      if(strcmp(Menu->GetName(),"Sel Obj Menu") != 0)	
		return; 	        
      
      char cmdName[32];
      sprintf( cmdName,"XSectionCascade%d",xsectionCommandCounter);
      pfcPopupmenuOptions_ptr cascademenuOpts = pfcPopupmenuOptions::Create(cmdName);     
      cascademenuOpts->SetLabel("OTK XSection Examples");		
      cascademenuOpts->SetHelptext("Examples for OTK XSection");		
      
      pfcPopupmenu_ptr cascadeMenu = Menu->AddMenu(cascademenuOpts);      
      xsectionCommandCounter++; 
     
      pfcPopupmenuOptions_ptr  menuOpts1 = pfcPopupmenuOptions::Create("XSectionPlaner");
      menuOpts1->SetLabel("Create Planar Section");		
      menuOpts1->SetHelptext("Creates a planar section with currently selected item");
      menuOpts1->SetPositionIndex(1);    
     
      try
	{
	  
	  cascadeMenu->AddButton(Command1,menuOpts1);
	}
      
      OTK_EXCEPTION_HANDLER(xsectionException);
      
      
    }
  
  OTK_EXCEPTION_HANDLER(xsectionException);
  
  return;
  
  
}

/**********************************************************************
 Creates planar section in four ways
	1. Creates a planar section without quilt when input is surface or a datum plane
	   This case uses pfcXSECTYPE_MODEL as cut object type

	2. Creates a planar section with quilt when input is surface, datum plane and a quilt.
	   If quilt is not selected it will include all quilts in model
	   This case uses pfcXSECTYPE_QUILTS or pfcXSECTYPE_MODELQUILTS as cut object type
	
	3. Creates a planar section with selected part when input is surface or datum plane & a part
	   This case uses pfcXSECTYPE_ONEPART

	4. Creates a planar section with excluded components.

**********************************************************************/
void XSectionExamples::CreatePlanarXSection(pfcSelection_ptr &refSelection)
{	
  
  try
    {
      if(refSelection == 0)
	{
	  pfcSelectionOptions_ptr refSelopts = 	pfcSelectionOptions::Create("datum,surface");
	  refSelopts->SetMaxNumSels(1);
	  
	  wSession->UIDisplayMessage("xsection_example_msg.txt","USER Plane Selection",NULL);
	  
	  pfcSelections_ptr refSels = wSession->Select(refSelopts);
	  
	  refSelection = refSels->get(0);			
	}
      
      pfcXSecCutobjType cutObjtype = pfcXSecCutobjType_nil;
      xint quiltID = -1;
      xintsequence_ptr memberIdTable = 0;	
      xint planeId = refSelection->GetSelItem()->GetId();
      wfcXSectionComponents_ptr compExcl = 0;
      
      pfcParent_ptr owner = refSelection->GetSelItem()->GetDBParent();
      wfcWSolid_ptr solidOwner = wfcWSolid::cast(owner);
      
      if(mMethod.compare("Planar") == 0)
	cutObjtype = pfcXSECTYPE_MODEL;		  
      
      /*
	NOTE 3: Asks user to select a quilt to use in the xsection definition 
	If user aborts the operation, the selection ignores the quilt
      */
      else if(mMethod.compare("Planar with quilt") == 0)
	{
	  try
	    {
	      wSession->UIDisplayMessage("xsection_example_msg.txt","USER Quilt Selection",NULL);
	      
	      pfcSelectionOptions_ptr quiltSelopts = 	pfcSelectionOptions::Create("dtmqlt");
	      quiltSelopts->SetMaxNumSels(1);
	      
	      pfcSelections_ptr quiltSels = wSession->Select(quiltSelopts);
	      pfcSelection_ptr selCutobj = quiltSels->get(0);			
	      
	      pfcComponentPath_ptr compPath = selCutobj->GetPath();
	      
	      if(compPath !=0)
		memberIdTable = compPath->GetComponentIds();
	      
	      quiltID = selCutobj->GetSelItem()->GetId();
	      
	      cutObjtype = pfcXSECTYPE_QUILTS;
	      
	    }
	  
	  xcatchbegin 
	    xcatch(pfcXToolkitUserAbort,SelectionAborted)
	    
	    wSession->UIDisplayMessage("xsection_example_msg.txt","USER Quilt Selection aborted",NULL);
	    cutObjtype = pfcXSECTYPE_MODELQUILTS;	  
	  xcatchend
	}
      
      /*
	  NOTE 4: Asks user to select a part to use in the xsection definition 	          
      */
      else if(mMethod.compare("Planar with part") == 0)
	{
	  if(wSession->GetCurrentModel()->GetType() == pfcMDL_ASSEMBLY)			  
	    {
	      wSession->UIDisplayMessage("xsection_example_msg.txt","USER Part Selection",NULL);
	      
	      pfcSelectionOptions_ptr partSelopts = 	pfcSelectionOptions::Create("part");
	      partSelopts->SetMaxNumSels(1);
	      
	      pfcSelections_ptr partSels = wSession->Select(partSelopts);
	      
	      pfcSelection_ptr selCutobj = partSels->get(0);
	      
	      pfcComponentPath_ptr compPath = selCutobj->GetPath();
	      
	      if(compPath !=0)
		memberIdTable = compPath->GetComponentIds();
	      
	      cutObjtype = pfcXSECTYPE_ONEPART;
	    }
	  
	  else
	    cutObjtype = pfcXSECTYPE_MODEL;
	}
      /*
	NOTE 5: Set selection environment to allow box selection and click OK after selecting objects
	Then ask user to select components to exclude from xsection definition
      */
      else if(mMethod.compare("Planar with excluded parts"))
	{
	  if(wSession->GetCurrentModel()->GetType() == pfcMDL_ASSEMBLY)			  
	    {
	      wSession->UIDisplayMessage("xsection_example_msg.txt","USER Part Exclusion",NULL);
	      
	      wfcSelectionEnvironmentOptions_ptr envOpts = wfcSelectionEnvironmentOptions::create();
	      
	      envOpts->append(wfcSelectionEnvironmentOption::Create(wfcSELECT_BY_BOX_ALLOWED,1));
	      envOpts->append(wfcSelectionEnvironmentOption::Create(wfcSELECT_DONE_REQUIRED,1));
	      
	      pfcSelectionOptions_ptr partSelopts = 
		pfcSelectionOptions::cast( wfcWSelectionOptions::Create("part",-1,envOpts) );	      
	      
	      pfcSelections_ptr partSels = wSession->Select(partSelopts);
	      
	      if(partSels != 0)
		{	      
		  int arrSize = partSels->getarraysize();
		  pfcComponentPaths_ptr compPaths = pfcComponentPaths::create();
		  
		  for(int ii = 0;ii<arrSize;ii++)
		    compPaths->append(partSels->get(ii)->GetPath());
		  
		  compExcl = wfcXSectionComponents::Create(compPaths,xtrue);      
		}
	      
	      cutObjtype = pfcXSECTYPE_MODEL;
	    }
	}
      
      wSession->UIDisplayMessage("xsection_example_msg.txt","USER Section Name",NULL);
      
      xstring sectionName = wSession->UIReadStringMessage(false);
      
      /*
	NOTE 6: Finally creates planar section with inputs received so far.
	Keeping Flip as xtrue (i.e. Clip model in the direction of positive normal to xsection plane )
      */
      
      solidOwner->CreatePlanarXSection(sectionName,planeId,cutObjtype,memberIdTable,quiltID,xtrue,compExcl);
      solidOwner->RefreshMdlTree();
    }
  
  OTK_EXCEPTION_HANDLER(xsectionException);
  
  return;
}

/**********************************************************************
 Method visits all cross sections and their geometries
 Then it dumps information about hatch parameters in a file.
**********************************************************************/

void XSectionExamples::GetSectionHatching(xrstring fileName)
{
  
  try
    {
      wfcWSolid_ptr solidOwner = wfcWSolid::cast(wSession->GetCurrentModel());
      
      /*
	NOTE 7: List all cross section in the current model
	Get geometry for valid xsection components
	Get line patterns present on each component
	Print each parameter for every line pattern to file
      */
      
      pfcXSections_ptr sections	= solidOwner->ListCrossSections();
      
      fstream hatchInfoFile;
      
      hatchInfoFile.open(fileName,ios::out);
      
      for(int ii=0;ii<sections->getarraysize();ii++)
	{//loop over XSections
	  wfcWXSection_ptr wSection = wfcWXSection::cast(sections->get(ii));
	  
	  hatchInfoFile<<wSection->GetName()<<endl;
	  
	  wfcXSectionGeometries_ptr sectionGeoms = wSection->CollectGeometry(0);			
	  
	  for(int jj=0;jj<sectionGeoms->getarraysize();jj++)
	    {//loop over XSection Geometries
	      wfcXSectionHatches_ptr sectionHatches = wSection->GetCompXSectionHatches(jj,0);
	      
	      if(sectionHatches != 0)
		{
		  hatchInfoFile<<"\tComponent["<<jj<<"] ";
		  
		  wfcXSectionGeometry_ptr sectionGeom = sectionGeoms->get(jj);
		  
		  for(int kk=0;kk<sectionGeom->GetMemberIdTable()->getarraysize();kk++)
		    {//loop over member id table
		      hatchInfoFile<<"AtPath("<<sectionGeom->GetMemberIdTable()->get(kk);
		      
		      if(kk != (sectionGeom->GetMemberIdTable()->getarraysize() - 1))
			hatchInfoFile<<",";
		      else
			hatchInfoFile<<") ";
		    }//loop over member id table
		  
		  hatchInfoFile<<"{"<<endl;
		  
		  for(int ll=0;ll<sectionHatches->getarraysize();ll++)
		    {//loop over hatches
		      wfcXSectionHatch_ptr sectionHatch = sectionHatches->get(ll);
		      
		      hatchInfoFile<<"\t\t(ANGLE="<<sectionHatch->GetAngle()
			           <<",\tSPACING="<<sectionHatch->GetSpacing()<<")";
		      
		      if(ll != (sectionHatches->getarraysize()-1))
			hatchInfoFile<<","<<endl;
		      else
			hatchInfoFile<<endl;
		    }//loop over hatches			
		  
		  hatchInfoFile<<"\t}"<<endl;
		}
	    }//loop over XSection Geometries
	  
	  hatchInfoFile<<"END"<<endl;
	}//loop over XSections
      
      hatchInfoFile.close();    
    }

  OTK_EXCEPTION_HANDLER(xsectionException);
  return;
  
}


/**********************************************************************
Allows user to edit hatch parameters for all cross section in the current model
Method uses a file returned by GetSectionHatching() 
which has information about all section hatching
Then it asks user to edit the hatch parameters in that file.
Finally when user changes & saves file it will read the file 
and update the hatching for all cross sections

**********************************************************************/

void XSectionExamples::EditSectionHatching()
{
  try
    { //try
      xrstring hatchFileName ("ListHatches.hatch");
      fstream hatchInfoFile;      
      
      wfcWSolid_ptr solidOwner = wfcWSolid::cast(wSession->GetCurrentModel());    
      
      GetSectionHatching(hatchFileName);
      
      xbool fileChanged = wSession->UIEditFile(hatchFileName);		
      
      if(fileChanged == xtrue)
	{ //After Edit file
	  hatchInfoFile.open(hatchFileName,ios::in);
	  char fileLine[80] = "\0";
	  xstring hatchParamline,angleString,spacingString;
	  size_t posStart,posEnd;	  
	  int Angle;
	  double Spacing;
	  
	  while(!hatchInfoFile.eof())
	    {//eof while
	      hatchInfoFile.getline(fileLine,80);				
	      
	      pfcXSection_ptr editSection = solidOwner->GetCrossSection(fileLine);
	      wfcWXSection_ptr WEditsectionhatch = wfcWXSection::cast(editSection);
	      
	      xint compIditer = -1;
	      
	      while(strcmp(fileLine,"END")!=0 && (!hatchInfoFile.eof())  )
		{
		  hatchInfoFile.getline(fileLine,80);			  
		  
		  if(strcmp(fileLine ,"END") != 0)
		    compIditer++;
		  
		  wfcXSectionHatches_ptr hatches = wfcXSectionHatches::create();
		  
		  while(strcmp(fileLine,"\t}")!=0 && strcmp(fileLine,"END")!=0)
		    {				   
		      hatchInfoFile.getline(fileLine,80,')');						
		      
		      hatchParamline = fileLine;
			  posStart = hatchParamline.Pos("=");
			  posEnd = hatchParamline.Pos(",");
			  angleString = hatchParamline.Substring ((posStart+1),posEnd);			  
			  xbool ret = angleString.ToInt(&Angle);			  
			  
			  hatchParamline = hatchParamline.Substring(posEnd);
			  posStart = hatchParamline.Pos("=");
			  posEnd = hatchParamline.Pos(",");
			  spacingString = hatchParamline.Substring((posStart+1),(posEnd-posStart-1));		   
			  ret  = spacingString.ToDouble(&Spacing);		      
			  
			  wfcXSectionHatch_ptr hatch = wfcXSectionHatch::Create(Angle,Spacing,10);
			  hatches->append(hatch);
		      
			  hatchInfoFile.getline(fileLine,80);
			  
			  if(strcmp(fileLine,",")==0)
			continue;
			  else
			    hatchInfoFile.getline(fileLine,80);    
			  
		    }
		  
		  if(strcmp(fileLine ,"END") != 0)
		    WEditsectionhatch->SetCompXSectionHatches(compIditer,0,hatches);
		  
		}		   
	      
	    }//eof while			  
	  
	  hatchInfoFile.close();	 
	  
	}//After Edit file
    }//try	
  
  
  OTK_EXCEPTION_HANDLER(xsectionException);   
  return;
 
}
