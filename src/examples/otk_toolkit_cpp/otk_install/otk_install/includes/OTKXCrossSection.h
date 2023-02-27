/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXCROSSSECTION_H
#define OTKXCROSSSECTION_H




#include <pfcSelect.h>
#include <wfcSession.h>
#include <pfcUI.h>
#include <wfcSolid.h>
#include <wfcXSection.h>
#include <pfcGlobal.h>
#include <pfcExceptions.h> 
#include <fstream>
#include <OTKXUtils.h>

class XSectionExamples : virtual public pfcUICommandActionListener
{
  
 public:
    XSectionExamples(string inMethod,xbool inObjectAction=xfalse) { mMethod = inMethod; mObjectAction=inObjectAction; }
	
    void OnCommand();

	void OnPopupmenuCreate(pfcPopupmenu_ptr Menu);

	void CreatePlanarXSection(pfcSelection_ptr &refSelection);

	void GetSectionHatching(xrstring fileName);

	void EditSectionHatching();	

			
  private:
    string mMethod;
	xbool mObjectAction;
};


class XSectionExamplesPopupMenu : virtual public pfcPopupmenuListener
{

public:
	void OnPopupmenuCreate (pfcPopupmenu_ptr Menu);

	XSectionExamplesPopupMenu() {  Command1 = pfcGetProESession()->UICreateCommand("OTKPopupMenuXSection", new XSectionExamples("Planar",xtrue) );}		

private:
	 pfcUICommand_ptr Command1;

};

#endif /* OTKXCROSSSECTION_H */
