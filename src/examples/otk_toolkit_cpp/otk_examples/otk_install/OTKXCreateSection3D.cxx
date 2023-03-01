/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
   This example will demonstrate manipulating a 3D Section.
   It will first get section present in sketcher session.
   It will then add/delete entities to/from it.
   It will finally set it active to reflect the changes.
   
   
*/

#include <pfcSession.h>
#include <wfcSession.h>
#include <pfcGlobal.h>
#include <pfcExceptions.h>
#include <pfcUI.h>
#include <pfcSelect.h>
#include <wfcSolid.h>
#include <wfcModel.h>
#include <OTKXUtils.h>


static void AddEntitiestoSection(wfcSection_ptr &section, ofstream &section_info);

//*********************************************
extern "C" wfcStatus otkSection3DCreate ()
{	
  ofstream section_info;
  section_info.open("section3d.inf",ios::out);

  try
    {
      pfcSession_ptr Session = pfcGetProESession ();      
      wfcWSession_ptr WSession = wfcWSession::cast(Session);
      
      /*
	Get an active section from current sketcher session
	
	NOTE 1: We must be in sketcher mode to get an active section
	otherwise method will throw an pfcXToolkitBadContext exception
	
	NOTE 2: GetActiveSection() always return a copy of the section object
		       
	NOTE 3: If you modify this section and wish to apply changes
	you must set it back using method SetActiveSection()	   
      */
      
      wfcSection_ptr section3d = WSession->GetActiveSection();
      
      /*
	Modify section by adding 2D entities to the section
	
	NOTE : Before manipulating any section we must turn ON intent manager
	to ensure section resolution done properly
	
      */      
      section3d->SetIntentManagerMode(true);
      
      section3d->SetEpsilon(0.50);
      
      AddEntitiestoSection(section3d, section_info);
      
      section3d->SetActive();
      
      section3d = WSession->GetActiveSection();
      
      /*
	List entities in the section.
	Filter list to get ARC entity 
	
	NOTE : To delete or get any entity we must know its identifier
	GetEntityIds() returns identifier of all entities  
	present in the section
	
      */
      
      wfcSectionEntities_ptr entities = section3d->ListSectionEntities();
      
      for(int ii=0;ii<entities->getarraysize();ii++)
	{
	  if(entities->get(ii)->GetSectionEntityType() == wfcSEC_ENTITY_2D_ARC)
	    {
	      section3d->DeleteEntity(section3d->GetEntityIds()->get(ii));
	      break;
	    }
	}
      
      /*
	Set modified section as an active section
	
	NOTE 1: We must take care of validity of section
	
	NOTE 2: Copy of the section is set as active
	
	NOTE 3: Section will also be added to UNDO/REDO stack
      */
      section3d->SetActive();
      
      section3d->SetIntentManagerMode(false);
      
    }   
  
  xcatchbegin
    
    xcatch(pfcXToolkitBadContext,badConEx)
    {
      pfcMessageDialogOptions_ptr msgOpts = pfcMessageDialogOptions::Create();
      pfcMessageButtons_ptr buttons = pfcMessageButtons::create();
      buttons->append(pfcMESSAGE_BUTTON_OK);
      msgOpts->SetButtons(buttons);
      msgOpts->SetMessageDialogType(pfcMESSAGE_WARNING);
      msgOpts->SetDialogLabel("OTK 3D Section Operations ERROR");
      pfcSession_ptr Session = pfcGetCurrentSession ();
      
      Session->UIShowMessageDialog("You must be in active sketcher mode to proceed",msgOpts);
    }
  
  xcatchend
    
    
    return wfcTK_NO_ERROR;
  
}



static void AddEntitiestoSection(wfcSection_ptr &section, ofstream &section_info)
{
  try
    {

      pfcPoint2D_ptr point = wfcCreatePoint2D(100.00,100.00);
      
      wfcSectionEntityPoint_ptr  pointEnt = wfcSectionEntityPoint::Create(point);
      wfcSectionEntity_ptr entity = wfcSectionEntity::cast(pointEnt);
      section->AddEntity(entity);
      
      pfcOutline2D_ptr line = wfcCreateOutline2D(10.00,10.00,150.00,150.00);
      
      wfcSectionEntityLine_ptr lineEnt = wfcSectionEntityLine::Create(line);
      entity = wfcSectionEntity::cast(lineEnt);
      section->AddEntity(entity);
      
      wfcSectionEntityCircle_ptr circleEnt = wfcSectionEntityCircle::Create(point,200.50);
      entity = wfcSectionEntity::cast(circleEnt);
      section->AddEntity(entity);
      
      wfcSectionEntityEllipse_ptr ellipseEnt = wfcSectionEntityEllipse::Create(point,150.00,200.00);
      entity = wfcSectionEntity::cast(ellipseEnt);
      section->AddEntity(entity);
      
      wfcSectionEntityArc_ptr arcEnt = wfcSectionEntityArc::Create(point,1.57,3.14,50.00);
      entity = wfcSectionEntity::cast(arcEnt);
      section->AddEntity(entity);
      
      line = wfcCreateOutline2D(100.00,100.00,150.00,150.00);
      
      wfcSectionEntityCenterLine_ptr cLine = wfcSectionEntityCenterLine::Create(line);
      entity = wfcSectionEntity::cast(cLine);
      section->AddEntity(entity);
      
    }
	OTK_EXCEPTION_HANDLER(section_info);
}
  

