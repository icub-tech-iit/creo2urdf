/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 This example will demonstrate basic Windchill Server operations.
 It will first authenticate the browser followed by server register.
 It will retrieve an assembly (asm0001.asm) from disk, save it to workspace followed by check-in
 It will then download the just checked in assembly.
 It will also checkout the just checked in assembly.

 Change the #define statements appropriately.
 
*/

#include <wfcSession.h>
#include <pfcModel.h>
#include <ciplib.h>
#include <pfcGlobal.h>
#include <pfcServer.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h>


#include <fstream>

#define WC_USERNAME "abc"
#define WC_PASSWORD "xyz"
#define SERVER_ALIAS "OTK_Server"
#define SERVER_URL "http://abcdef.com/Windchill"
#define SERVER_WORKSPACE "Workspace ABC"

ofstream fp_out_server;

pfcServer_ptr otkServerRegister (pfcSession_ptr Session, ofstream &exception_info);
pfcModels_ptr otkServerCheckinOperation (pfcSession_ptr Session, 
	pfcServer_ptr Server, ofstream &exception_info);
void otkServerCheckoutORDownloadOperation (pfcServer_ptr Server, 
	pfcModels_ptr Models, xbool DoCheckout, ofstream &exception_info);

//***********************************************************
extern "C" wfcStatus otkServerOperations ()
{
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

	try
	{		
		fp_out_server.open("ServerOperations.txt", ios::out);
	    fp_out_server << "- Server Operations Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();

		/*Register the Windchill Server*/
		pfcServer_ptr Server = otkServerRegister(Session, exception_info);

		/*Retrieve, Save & Checkin set of models*/
		pfcModels_ptr Models = otkServerCheckinOperation(Session, Server, exception_info);

		/*Download the Checked-in models*/
		otkServerCheckoutORDownloadOperation(Server, Models, xfalse, exception_info);

		/*Checkout the Checked-in models*/
		otkServerCheckoutORDownloadOperation(Server, Models, xtrue, exception_info);

		Server->Unregister();
	
		fp_out_server.close();
		
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(exception_info);

	return wfcTK_GENERAL_ERROR;

}

//*******************************************************
pfcServer_ptr otkServerRegister (pfcSession_ptr Session, ofstream &exception_info)
{
	try
	{
		Session->AuthenticateBrowser(WC_USERNAME, WC_PASSWORD);

		fp_out_server << "Browser Authentication Success" << endl;
		
		pfcServer_ptr Server = Session->RegisterServer(SERVER_ALIAS, SERVER_URL, SERVER_WORKSPACE);
		
		fp_out_server << "Server Register Success" << endl;
		
		return (Server);
	}
	OTK_EXCEPTION_HANDLER(exception_info);

    return 0;
}

//********************************************************
pfcModels_ptr otkServerCheckinOperation (
	pfcSession_ptr Session, pfcServer_ptr Server, ofstream &exception_info)
{
	try
	{
		pfcModelDescriptor_ptr AsmDesc = pfcModelDescriptor::Create(pfcMDL_ASSEMBLY, "asm0001", 0);

		pfcModel_ptr Model = Session->RetrieveModel(AsmDesc);

		fp_out_server << "Model Retrieve Success" << endl;
		
		Server->Activate();

		fp_out_server << "Server Activate Success" << endl;
		
		Model->Save();

		fp_out_server << "Model Save Success" << endl;
		
		Server->UploadObjects(Model);

		fp_out_server << "Model Upload Success" << endl;
		
		pfcCheckinOptions_ptr CheckinOpts = pfcCheckinOptions::Create();

		Server->CheckinObjects(Model, CheckinOpts);	

		fp_out_server << "Model Checkin Success" << endl;
		
		pfcModels_ptr Models = pfcModels::create();

		Models->append(Model);

		return (Models);
		
	}
	OTK_EXCEPTION_HANDLER(exception_info);

	return 0;
}


//********************************************************
void otkServerCheckoutORDownloadOperation (
	pfcServer_ptr Server, pfcModels_ptr Models, 
	xbool DoCheckout, ofstream &exception_info)
{
	try
	{
		pfcCheckoutOptions_ptr CO_Opts = pfcCheckoutOptions::Create();

		CO_Opts->SetDependency(pfcSERVER_DEPENDENCY_ALL);

		for (int i=0 ; i<Models->getarraysize() ; i++)
		{
			Server->CheckoutObjects(Models->get(i), 0, DoCheckout, CO_Opts);

			if (DoCheckout)
			  fp_out_server << "Model Checkout Success" << endl;					
			else
			  fp_out_server << "Model Download Success" << endl;
			
		}
	}
	OTK_EXCEPTION_HANDLER(exception_info);
}
