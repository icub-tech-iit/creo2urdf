/*
	Copyright (c) 2013 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

//#include <wfcSession.h>
//#include <pfcGlobal.h>
//#include <pfcCommand.h>
// #include <pfcUI.h>
#include <pfcExceptions.h>
// #include <wfcClient.h>
#include <fstream>



static ofstream logFile;

//class OTKTestCallBack : public virtual pfcUICommandActionListener
//{
//public:
//	void OnCommand();
//};


/* Entry */
extern "C" int user_initialize(
	int argc,
	char *argv[],
	char *version,
	char *build,
	wchar_t errbuf[80])
{  
	// pfcUICommand_ptr command;

	logFile.open("logmio.txt", ios::out);
	logFile << "- LOGS "<<endl;
	logFile<< "============="<<endl;


	//try
	//{
	//	pfcSession_ptr		Session = pfcGetProESession ();

	//	wfcWSession_ptr wSession = wfcWSession::cast (Session);

	//	try {
	//		wSession->RibbonDefinitionfileLoad("otk_install_test.rbn");
	//	}
	//	xcatchbegin
	//		xcatch (pfcXToolkitError, xtoole) {
	//			xint err = xtoole->GetErrorCode();
	//			xstring method = xtoole->GetMethodName();
	//			logFile << "pfcXToolkitError in method " << method << " with error " << err << "\n" << endl;
	//			logFile << "continuing..\n";
	//	} xcatchend

	//		pfcUICommand_ptr InputCommand1 = wSession->UICreateCommand("OTKInstallTest", new OTKTestCallBack() );        
	//	InputCommand1->Designate ("msg_user.txt", "OTK Install Test", "OTK Install Test", "OTK Install Test");

	//	return (wfcTK_NO_ERROR);
	//}
	//xcatchbegin
	//	xcatchcip (Ex)
	//	logFile << "Exception:\n" << Ex << endl;
	//return (wfcTK_NO_ERROR);
	//xcatchend

	return (0);
}

/* Exit*/
extern "C" void user_terminate()
{
	logFile<< endl;
	logFile<< "================================================" << endl;
	logFile<< "OTK application terminated successfully."<<endl;
	logFile<< "================================================" << endl;

	logFile.close();
}

//void OTKTestCallBack::OnCommand()
//{
//	try
//	{
//		pfcSession_ptr		Session = pfcGetProESession ();
//
//		Session->UIShowMessageDialog("OTK Install Test succeeded!!", NULL);
//	}
//	xcatchbegin
//		xcatchcip (Ex)
//		logFile << "Exception:\n" << Ex << endl;
//	xcatchend
//}
