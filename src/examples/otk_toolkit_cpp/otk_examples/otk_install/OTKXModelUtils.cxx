/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Utilites for wfcWModel
  
*/
#include <fstream>
#include <pfcExceptions.h>
#include <wfcGlobal.h>
#include <wfcSession.h>
#include <wfcModel.h>

#include <OTKXUtils.h> 

int OtkUtilWriteAssemblyInfo (
       wfcWSession_ptr wSession, // input - Session Pointer
       wfcWModel_ptr   wModel,   // input - Model Pointer
       ofstream&       InfoFile, // input - InfoFile
       ofstream&       LogFile,  // input - LogFile
       int             level )   // input - level details of info to be written
//------------------------------------------------------------------------------
// Purpose: Writes Assembly Information
//------------------------------------------------------------------------------
{
  int status = 0;

  if( wSession == 0)
    return -1;

  if( wModel == 0)
    return -2;

  try
  {
    pfcModelType wModelType = wModel->GetType();

    if ( wModelType != pfcMDL_ASSEMBLY )
       return -3;

    wfcWAssembly_ptr wAssembly = wfcWAssembly::cast(wModel);

    InfoFile << "|                  Assembly Info                   |" << endl;

    xbool wIsExploded = wAssembly->GetIsExploded ();
    if ( xtrue == wIsExploded )
    {
      InfoFile << "\t - This is an Exploded Assembly." << endl;
    }

    xbool wDynamicPositioning = wAssembly->GetDynamicPositioning ();

    if ( xtrue == wDynamicPositioning )
    {
      InfoFile << "\t - Assembly uses dynamic positioning." << endl;
    }

    wfcConnectors_ptr wConnectors = wAssembly->GetConnectors ();

    if ( NULL != wConnectors )
    {
      xint size = wConnectors->getarraysize ();
    
      InfoFile << "\t - There are " << size << " Connectors in the assembly." 
               << endl;
    }

    wfcHarnesses_ptr wHarnesses = wAssembly->GetHarnesses ();

    if ( NULL != wHarnesses )
    {
      xint size = wHarnesses->getarraysize ();
    
      InfoFile << "\t - There are " << size << " Harnesses in the assembly." 
               << endl;
    }

    wfcLineStocks_ptr wLinestocks = wAssembly->GetLinestocks ();

    if ( NULL != wLinestocks )
    {
      xint size = wLinestocks->getarraysize ();
    
      InfoFile << "\t - There are " << size << " Linestocks in the assembly." 
               << endl;

      for ( int Idx = 0; Idx < size ; Idx++ )
      {
        xstring wLinestockName = (wLinestocks->get(Idx))->GetName ();
        InfoFile << "\t - Linestock [" << Idx << "] = "  << wLinestockName << endl;
      }
    }

    wfcSpools_ptr wSpools = wAssembly->GetSpools ();

    if ( NULL != wSpools )
    {
      xint wSpoolsSize = wSpools->getarraysize ();
    
      InfoFile << "\t - There are " << wSpoolsSize << " Spools in the assembly." 
               << endl;

      for ( int Idx = 0; Idx < wSpoolsSize ; Idx++ )
      {
        xstring wSpoolName = (wSpools->get(Idx))->GetName ();
        InfoFile << "\t - Spool [" << Idx << "] = "  << wSpoolName << endl;
      }
    }

    wfcWComponentPaths_ptr wDispComps = wAssembly->ListDisplayedComponents ();

    if ( NULL != wDispComps )
    {
      xint wDispCompsSize = wDispComps->getarraysize ();
    
      InfoFile << "\t - There are " << wDispCompsSize 
               << " Displayed Components in the assembly." << endl;

      for ( int Idx = 0; Idx < wDispCompsSize ; Idx++ )
      {
        xstring wFileName = ( (wDispComps->get(Idx))->GetLeaf() )->GetFileName ();
        InfoFile << "\t - Displayed Component [" << Idx << "] = " 
                 << wFileName << endl;
      }
    }

    return status;
  }
  OTK_EXCEPTION_HANDLER(LogFile);

  return 0;
}

int OtkUtilWriteModelInfo (
       wfcWSession_ptr wSession, // input - Session Pointer
       wfcWModel_ptr   wModel,   // input - Model Pointer
       ofstream&       InfoFile, // input - InfoFile
       ofstream&       LogFile,  // input - LogFile
       int             level )   // input - level details of info to be written
//------------------------------------------------------------------------------
// Purpose: Writes Model Information
//------------------------------------------------------------------------------
{
  int status = 0;

  if( wSession == 0)
    return -1;

  if( wModel == 0)
    return -2;

  try
  {
    InfoFile << "|                    Model Info                    |" << endl;

    xstring wFileName = wModel->GetFileName();
    InfoFile << "\t - Model File Name : " << wFileName << endl;

    pfcModelType wModelType = wModel->GetType();
    InfoFile << "\t - Model Type : ";

    switch  (wModelType )
    {
      case pfcMDL_ASSEMBLY :
        InfoFile << "Assembly" << endl;
        break;

      case pfcMDL_PART :
        InfoFile << "Part" << endl;
        break;

      case pfcMDL_DRAWING :
        InfoFile << "Drawing" << endl;
        break;

      case pfcMDL_2D_SECTION :
        InfoFile << "2D Section" << endl;
        break;

      case pfcMDL_LAYOUT :
        InfoFile << "Notebook" << endl;
        break;

      case pfcMDL_DWG_FORMAT :
        InfoFile << "Drawing Format" << endl;
        break;

      case pfcMDL_MFG :
        InfoFile << "Manufacturing" << endl;
        break;

      case pfcMDL_REPORT :
        InfoFile << "Report" << endl;
        break;

      case pfcMDL_MARKUP :
        InfoFile << "Markup" << endl;
        break;

      case pfcMDL_DIAGRAM :
        InfoFile << "Diagram" << endl;
        break;

      case pfcModelType_nil :
        InfoFile << "Nil" << endl;
        break;

      default:
        InfoFile << "Unknown" << endl;
        break;
    }

    xstring wFullName = wModel->GetFullName();
    InfoFile << "\t - Model Full Name : " << wFullName << endl;

    xstring wCommonName = wModel->GetCommonName ();
    if ( xstringnil != wCommonName )
    {
      InfoFile << "\t - Common Name : " << wCommonName << endl;
    }

    xstring wGenericName = wModel->GetGenericName ();
    if ( xstringnil != wGenericName )
    {
      InfoFile << "\t - Generic Name : " << wGenericName << endl;
    }

    xstring wInstanceName = wModel->GetInstanceName ();
    InfoFile << "\t - Instance Name : " << wInstanceName << endl;

    xstring wVersion = wModel->GetVersion ();
    if ( xstringnil != wVersion )
    {
      InfoFile << "\t - Version : " << wVersion << endl;
    }

    xstring wReleaseLevel = wModel->GetReleaseLevel ();
    if ( xstringnil != wReleaseLevel )
    {
      InfoFile << "\t - Release Level : " << wReleaseLevel << endl;
    }

    xstring wRevision = wModel->GetRevision ();
    if ( xstringnil != wRevision )
    {
      InfoFile << "\t - Revision Number : " << wRevision << endl;
    }
	
    xstring wVersionStamp = wModel->GetVersionStamp();
    InfoFile << "\t - Version Stamp : " << wVersionStamp << endl;

    if ( wModelType == pfcMDL_ASSEMBLY )
    {
       status = OtkUtilWriteAssemblyInfo ( wSession, wModel, InfoFile,
                                           LogFile, level);
       LogFile << "status " << status
               << " returned from OtkUtilWriteAssemblyInfo()" << endl;
    }

    return status;
  }
  OTK_EXCEPTION_HANDLER(LogFile);

  return status;
}
//------------------------------------------------------------------------------


// Command Action Listener for Dump Model Info
extern "C" wfcStatus otkDumpModelInfoCmd ()
{
      ofstream LogFile;
      try
      {
        ofstream InfoFile;
        InfoFile.open( "DumpModelInfo.inf", ios::out);
        LogFile.open ( "DumpModelInfo.log", ios::out);

        pfcSession_ptr Session = pfcGetCurrentSession ();
        wfcWSession_ptr wSession = wfcWSession::cast(Session);

        pfcModel_ptr wCurrentModel = wSession->GetCurrentModel();

        wfcWModel_ptr wModel = wfcWModel::cast(wCurrentModel);

        if ( 0 == wCurrentModel )
        {
          InfoFile << "\t - There is no Current Model. "  << endl;
        }
        else
        {
          int status =  OtkUtilWriteModelInfo ( wSession,
                                                wModel,
                                                InfoFile,
                                                LogFile,
                                                3 );
           LogFile << "status " << status
                   << " returned from OtkUtilWriteModelInfo ()" << endl;
        }
        InfoFile.close();
        LogFile.close();

        wSession->DisplayInformationWindow("DumpModelInfo.inf", 0, 0, 0, 0);
      }
      OTK_EXCEPTION_HANDLER(LogFile);
    
      return wfcTK_NO_ERROR;
}
//------------------------------------------------------------------------------

