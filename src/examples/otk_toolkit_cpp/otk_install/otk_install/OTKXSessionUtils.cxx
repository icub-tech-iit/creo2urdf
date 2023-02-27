/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Utilites for wfcWSession
  
*/
#include <fstream>
#include <pfcExceptions.h>
#include <wfcGlobal.h>
#include <wfcSession.h>

#include <OTKXUtils.h> 


int OtkUtilWriteSessionInfo (
       wfcWSession_ptr wSession, // input
       ofstream&       InfoFile, // input
       ofstream&       LogFile,  // input
       xint             level )   // input - level details of info to be written
//------------------------------------------------------------------------------
// Purpose: Writes Session Information
//------------------------------------------------------------------------------
{
  if( wSession == 0)
    return -1;

  try
  {
    InfoFile << "|                   Session Info                   |" << endl;

    xint wNumericVersion = wSession->GetReleaseNumericVersion ();
    InfoFile << "\t - integer major version number of the Creo executable : "
             << wNumericVersion << endl;

    xstring wDateCode = wSession->GetDisplayDateCode ();
    InfoFile << "\t - customer visible date code of the Creo executable : "
             << wDateCode << endl;

    xstring wCurrentDir = wSession->GetCurrentDirectory ();
    InfoFile << "\t - Curent Directory : " << wCurrentDir << endl;

    xstring wCurrentWS = wSession->GetCurrentWS  ();

    if ( xstringnil != wCurrentWS )
    {
      InfoFile << "\t - Curent Workspace : " << wCurrentWS << endl;
    }

    pfcModel_ptr wCurrentModel = wSession->GetCurrentModel();

    if ( 0 == wCurrentModel )
    {
      InfoFile << "\t - There is no Current Model. "  << endl;
    }
    else
    {
      xstring wCurrentModelName = wCurrentModel->GetFileName();

      InfoFile << "\t - Curent Model Name : " << wCurrentModelName << endl;
    }

    pfcModel_ptr wActiveModel = wSession->GetActiveModel();

    if ( 0 == wActiveModel )
    {
      InfoFile << "\t - There is no Active Model. "  << endl;
    }
    else
    {
      xstring wActiveModelName = wActiveModel->GetFileName();

      InfoFile << "\t - Active Model Name : " << wActiveModelName << endl;
    }


    return 0;
  }
  OTK_EXCEPTION_HANDLER(LogFile);

  return 0;
}
//------------------------------------------------------------------------------


// Command Action Listener for Dump Session Info
extern "C" wfcStatus otkDumpSessionInfoCmd ()
{
      ofstream LogFile;
      try
      {
        ofstream InfoFile;
        InfoFile.open( "DumpSessionInfo.inf", ios::out);
        LogFile.open( "DumpSessionInfo.log", ios::out);

        pfcSession_ptr Session = pfcGetCurrentSession ();
        wfcWSession_ptr wSession = wfcWSession::cast(Session);

        int status =  OtkUtilWriteSessionInfo ( wSession,
                                                InfoFile,
                                                LogFile,
                                                3 );
        LogFile << "status " << status
                << " returned from OtkUtilWriteSessionInfo ()" << endl;

        InfoFile.close();
        LogFile.close();

        wSession->DisplayInformationWindow("DumpSessionInfo.inf", 0, 0, 0, 0);
      }
      OTK_EXCEPTION_HANDLER(LogFile);

      return wfcTK_NO_ERROR;
}



