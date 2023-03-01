/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <ciplib.h>
#include <pfcGlobal.h>
#include <pfcBase.h>
#include <wfcSession.h>
#include <fstream>
#include <wfcModel.h>
#include <OTKXSaveIntfDataCxx.h>
#include <OTKXUtils.h>


/**********************************************************************
In this Example :The code gets the interfaceData handle using otk method.  
		With this handle it creates and dump the interface data information 
		in following files
		InterfaceData_<modelName>.cxx
		EdgeDescs_<modelName>.cxx
		QuiltDatas_<modelName>.cxx
		SurfaceDescs_<modelName>.cxx

		After building above files, when we run the application 
		it will create the new part with the imported feature.

**********************************************************************/

extern "C" wfcStatus otkCreateIntfData ()
{
	ofstream f_out;
	try
	{

	CreateIntfData cIntfData;

	pfcSession_ptr		Session = pfcGetCurrentSession ();		
	pfcModels_ptr		Models = Session->ListModels ();
	
		pfcModel_ptr		Model = Models->get (0);
		xstring ModelName = Model->GetInstanceName();		
		xstring Lname = ModelName.ToLower();

		char buffer [100];
	    /* Saving the cxx file in the form:	<moduleName>_modelName.cxx */
		sprintf (buffer, "InterfaceData_%s.cxx", const_cast<char*>((cStringT)Lname));
		
		f_out.open(buffer, ios::out);
		pfcBaseSession_ptr bSession = pfcBaseSession::cast(Session);
		wfcWSession_ptr wSession = wfcWSession::cast(Session);	
		wfcIntfDataSourceType SrcType = wSession->GetDataSourceType(pfcINTF_NEUTRAL_FILE);

		pfcSolid_ptr pSolid = pfcSolid::cast(Model);
		wfcWSolid_ptr wSolid = wfcWSolid::cast(pSolid);
		wfcWPart_ptr wPart = wfcWPart::cast(wSolid);
		
		wfcEdgeRepresentation_ptr wEdgeRep = wfcEdgeRepresentation::Create(1,1,1);				
		wfcConversionOptions_ptr wConverOption = wfcConversionOptions::Create(wEdgeRep, wfcPART_CURVES_AS_DEFAULT, wfcPART_SURFACES_AS_DEFAULT);		
		wfcInterfaceData_ptr wIntfData = wPart->GetInterfaceData(wConverOption);				
		cIntfData.CreateIntfDataFile (wIntfData, Lname, f_out);

		f_out.close();	

		return wfcTK_NO_ERROR;
	} //try
	OTK_EXCEPTION_HANDLER(f_out);

	return wfcTK_GENERAL_ERROR;
}
