/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
   
This example will demonstrate creating & manipulating Section2D as model.
It will first allocate a 2D section & set its name.
It will then call Save() on it to save it as .sec file.
It will then retrieve the .sec file & add/delete entities to/from it.


*/

#include <pfcSession.h>
#include <wfcSession.h>
#include <pfcGlobal.h>
#include <pfcExceptions.h>
#include <pfcUI.h>
#include <pfcSelect.h>
#include <wfcSolid.h>
#include <wfcModel.h>
#include <wfcAssembly.h>
#include <OTKXUtils.h>


static void AddEntitiestoSection(wfcSection_ptr &section, ofstream &section_info);

//*********************************************
extern "C" wfcStatus otkSection2DCreate ()
{
  ofstream section_info;
  section_info.open("section.inf",ios::out);

  try
    {	  

      pfcSession_ptr Session = pfcGetCurrentSession ();      
      wfcWSession_ptr WSession = wfcWSession::cast(Session);
      
      /*
	Create a new Section2D file with extension .sec
	
	NOTE 1: New file will be saved as otk_section_2d.sec
	
	NOTE 2: 2D Sections are always represented by .sec file
      */
      wfcSection_ptr section2D = WSession->CreateSection2D();
      
      section2D->SetName("otk_section_2d");
      
      /*
	Add 2D Entities to the section
	
	NOTE : Before manipulating any section we must turn ON intent manager
	to ensure section resolution done properly
	
      */
      
      section2D->SetIntentManagerMode(true);
      
      AddEntitiestoSection(section2D, section_info);
      
      section2D->SetIntentManagerMode(false);
      
      /*
	Interface wfcSection inherits from wfcWModel hence method pfcModel::Save()
	will save otk_section_2d.sec to current directory
	
	NOTE : only 2D Sections are allowed to use wfcWModel methods
	They should not be used for 3D Sections
	
      */
      section2D->Save();
      
      Session->EraseUndisplayedModels();
      
      /*
	Retrieve a 2D Section as pfcModel object
	Display it using pfcModel::Display()
      */
      
      pfcModelDescriptor_ptr desc = pfcModelDescriptor::CreateFromFileName("otk_section_2d.sec");	  
      
      pfcModel_ptr sectionModel = Session->RetrieveModel(desc);
      
      sectionModel->Display();
      
      /*
	Convert pfcModel object to wfcSection
	
	NOTE : We can cast pfcModel or wfcWModel object whose type is pfcMDL_2D_SECTION
	           to wfcSection object       
		   
      */
      
      wfcSection_ptr section2d = wfcSection::cast(sectionModel);
      
      section_info<<"\nName of the Section "<<section2d->GetName()<<endl;
      
      /*
	List Entities in the Section
	Delete an entity
      */
      
      wfcSectionEntities_ptr entities = section2d->ListSectionEntities();
      section_info<<"\nNumber of entities in the section "<<entities->getarraysize()<<endl;
      
      for(int ii =0;ii<entities->getarraysize();ii++)
	section_info<<"\n\t-Id of entity"<<(ii+1)<<" is "<<section2d->GetEntityIds()->get(ii)<<endl;
      
      section2d->DeleteEntity(section2d->GetEntityIds()->get(1));
      
      section_info.close();
      
    }   
	OTK_EXCEPTION_HANDLER(section_info);
   
    return wfcTK_NO_ERROR;
}



static void AddEntitiestoSection(wfcSection_ptr &section, ofstream &section_info)
{
  try
    {
      /*
	wfcSectionEntity is parent class for all kind of entities
	
	NOTE : We should always cast child entitiy object e.g.wfcSectionEntityPoint
	to wfcSectionEntity before passing it to AddEntity()
	
      */
      
      pfcPoint2D_ptr point = wfcCreatePoint2D(100.00,100.00);
	  
      wfcSectionEntityPoint_ptr  pointEnt = wfcSectionEntityPoint::Create(point);
      wfcSectionEntity_ptr entity = wfcSectionEntity::cast(pointEnt);
      section->AddEntity(entity);
      
      pfcOutline2D_ptr line = wfcCreateOutline2D(10.00,10.00,150.00,150.00);
      
      wfcSectionEntityLine_ptr lineEnt = wfcSectionEntityLine::Create(line);
      entity = wfcSectionEntity::cast(lineEnt);
      section->AddEntity(entity);
      
      wfcSectionEntityCircle_ptr circleEnt = wfcSectionEntityCircle::Create(point,100.00);
      entity = wfcSectionEntity::cast(circleEnt);
      section->AddEntity(entity);
      
      wfcSectionEntityEllipse_ptr ellipseEnt = wfcSectionEntityEllipse::Create(point,50.00,20.00);
      entity = wfcSectionEntity::cast(ellipseEnt);
      section->AddEntity(entity);
    }
  	OTK_EXCEPTION_HANDLER(section_info);   
}
