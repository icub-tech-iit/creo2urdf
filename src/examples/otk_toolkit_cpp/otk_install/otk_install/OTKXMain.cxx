/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <wfcSession.h>
#include <wfcGlobal.h>
#include <pfcCommand.h>
#include <pfcUI.h>
#include <pfcExceptions.h>
#include <OTKXCrossSection.h>
#include <OTKXGraphics.h>
#include <OTKXSaveIntfDataCxx.h>
#include <OTKXUtils.h>
#include <OTKXUIFCDialog.h>
#include <OTKMultiBody.h>
#include <OTKXUIFCNakedWin.h>


FILE *logfile;

extern "C" wfcStatus otkSaveAllFeatsAsCxx ();
extern "C" wfcStatus otkSaveFeatAsCxx ();
extern "C" wfcStatus otkListFeatures ();
extern "C" wfcStatus otkDrawingCreateSheetAndViews_top ();
extern "C" wfcStatus otkCreateNodeUDFInPart_top ();
extern "C" wfcStatus otkExportToRaster();
extern "C" wfcStatus addHoleDiameterColumns_top ();
extern "C" wfcStatus otkFeatureOperations ();
extern "C" wfcStatus otkServerOperations ();
extern "C" wfcStatus otkSweepCreate ();
extern "C" wfcStatus otkSection2DCreate ();
extern "C" wfcStatus otkSection3DCreate ();
extern "C" wfcStatus otkQuickPrint ();
extern "C" wfcStatus otkCreateNewCombState();
extern "C" wfcStatus otkCreateLayerState();
extern "C" wfcStatus otkUpdateLayerState();
extern "C" wfcStatus otkLayerStateInfo();
extern "C" wfcStatus otkDeleteLayerState();
extern "C" wfcStatus otkCombinedStateInfo();
extern "C" wfcStatus otkDeleteCombinedState();
extern "C" wfcStatus otkCreateIntfData();
extern "C" wfcStatus otkFamilyTableInfo();
extern "C" wfcStatus otkFamilyTableErase();
extern "C" wfcStatus otkFamilyTableShow();
extern "C" wfcStatus otkFamilyTableEdit();
extern "C" wfcStatus otkFamilyTableRowSelect();
extern "C" wfcStatus otkAssemblyItemOperations();
extern "C" wfcStatus otkDumpSessionInfoCmd();
extern "C" wfcStatus otkDumpModelInfoCmd();
extern "C" wfcStatus otkVisitQuilts_top();


extern "C" {
typedef wfcStatus (*myCallback) ();
}

class MycallbackClass : public virtual pfcUICommandActionListener
{
  public:
    MycallbackClass(myCallback func) { m_func = func; }

    void OnCommand() {
      m_func();
    }
  private:
    myCallback m_func;
};



extern "C" int user_initialize(
    int argc,
    char *argv[],
    char *version,
    char *build,
    wchar_t errbuf[80])
{  
  pfcUICommand_ptr command;
	
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

    logfile = fopen("logmio.txt", "w");
    fprintf(logfile, "- LOGS -\n");
    fprintf(logfile, "=============\n");
    
	fprintf(logfile, "Trying to load the ribbon\n");

    try
      {

		fprintf(logfile, "Trying to load the ribbon\n");

	pfcSession_ptr		Session = pfcGetCurrentSessionWithCompatibility (pfcC4Compatible);
	wfcWSession_ptr wSession = wfcWSession::cast (Session);
	fprintf(logfile, "Trying to load the ribbon\n");

        try {

	  /* Loading the ribbon */
			fprintf(logfile, "Trying to load the ribbon\n");

			wSession->RibbonDefinitionfileLoad("otkExamples.rbn");

	  fprintf(logfile, "Loaded Ribbon\n");

        } 
		OTK_EXCEPTION_HANDLER(exception_info);
	    
	    /* Create command */
	    pfcUICommand_ptr InputCommand1 = wSession->UICreateCommand("OTKExample1", new MycallbackClass(otkSaveAllFeatsAsCxx) );        
        InputCommand1->Designate ("message.txt", "Save All Features as cxx", "Save All Features as cxx", "Save All Features as cxx");
	
	/* Create command */
	pfcUICommand_ptr InputCommand2 = wSession->UICreateCommand("OTKExample2", new MycallbackClass(otkSaveFeatAsCxx) );
        InputCommand2->Designate ("message.txt", "Save Feature as cxx", "Save Feature as cxx", "Save Feature as cxx");
	
		/* Create command */
	pfcUICommand_ptr InputCommand3 = wSession->UICreateCommand("OTKExample3", new MycallbackClass(otkListFeatures) );
        InputCommand3->Designate ("message.txt", "List Features Info", "List Features Info", "List Features Info");
	
	/* Create command */
	pfcUICommand_ptr InputCommand4 = wSession->UICreateCommand("OTKExample4", new MycallbackClass(otkDrawingCreateSheetAndViews_top) );
        InputCommand4->Designate ("message.txt", "Create Drawing Sheets and Views", "Create Drawing Sheets and Views", "Create Drawing Sheets and Views");
	
	/* Create command */
	pfcUICommand_ptr InputCommand5 = wSession->UICreateCommand("OTKExample5", new MycallbackClass(otkCreateNodeUDFInPart_top) );
        InputCommand5->Designate ("message.txt", "Create Node UDF in Part", "Create Node UDF in Part", "Create Node UDF in Part");
	
	/* Create command */
	pfcUICommand_ptr InputCommand6 = wSession->UICreateCommand("OTKExample6", new MycallbackClass(otkExportToRaster) );
        InputCommand6->Designate ("message.txt", "Export To Raster file type", "Export To Raster file type", "Export To Raster file type");
	
	/* Create command */
	pfcUICommand_ptr InputCommand7 = wSession->UICreateCommand("OTKExample7", new MycallbackClass(addHoleDiameterColumns_top) );
        InputCommand7->Designate ("message.txt", "Find Diameter Dimensions in Family Table", "Find Diameter Dimensions in Family Table", "Find Diameter Dimensions in Family Table");	
	
	/* Create command */
	pfcUICommand_ptr InputCommand8 = wSession->UICreateCommand("OTKExample8", new MycallbackClass(otkFeatureOperations) );
        InputCommand8->Designate ("message.txt", "Do Feature Operations", "Do Feature Operations", "Do Feature Operations");
	
	/* Create command */
	pfcUICommand_ptr InputCommand9 = wSession->UICreateCommand("OTKExample9", new MycallbackClass(otkServerOperations) );
        InputCommand9->Designate ("message.txt", "Do Server Operations", "Do Server Operations", "Do Server Operations");
	
	/* Create command */
	pfcUICommand_ptr InputCommand10 = wSession->UICreateCommand("OTKExample10", new MycallbackClass(otkSweepCreate) );
        InputCommand10->Designate ("message.txt", "Create Sweep Feature", "Create Sweep Feature", "Create Sweep Feature");
	
	/* Create command */
	pfcUICommand_ptr InputCommand11 = wSession->UICreateCommand("OTKExample11", new MycallbackClass(otkSection2DCreate) );
        InputCommand11->Designate ("message.txt", "Create a 2D Section", "Create a 2D Section", "Create a 2D Section");
	
	/* Create command */
	pfcUICommand_ptr InputCommand12 = wSession->UICreateCommand("OTKExample12", new MycallbackClass(otkSection3DCreate) );
        InputCommand12->Designate ("message.txt", "3D Section operations", "3D Section operations", "3D Section operations");
	
	/* Create command */
	pfcUICommand_ptr InputCommand13 = wSession->UICreateCommand("OTKExample13", new MycallbackClass(otkQuickPrint) );
	InputCommand13->Designate ("message.txt", "Quick Print", "Quick Print", "Quick Print");
	
	/* Create command */
	pfcUICommand_ptr InputCommand14 = wSession->UICreateCommand("OTKExample14", new MycallbackClass(otkCreateNewCombState) );
	InputCommand14->Designate ("message.txt", "Create new combined state", "Create new combined state", "Create new combined state");
	
	/* Create command */
	pfcUICommand_ptr InputCommand15 = wSession->UICreateCommand("OTKExample15", new MycallbackClass(otkCreateLayerState) );
	InputCommand15->Designate ("message.txt", "Create new layer state", "Create new layer state", "Create new layer state");

	/* Create command */
	pfcUICommand_ptr InputCommand16 = wSession->UICreateCommand("OTKExample16", new MycallbackClass(otkUpdateLayerState) );
	InputCommand16->Designate ("message.txt", "Update layer state", "Update layer state", "Update layer state");

	/* Create command */
	pfcUICommand_ptr InputCommand17 = wSession->UICreateCommand("OTKExample17", new MycallbackClass(otkLayerStateInfo) );
	InputCommand17->Designate ("message.txt", "Layer state info", "Layer state info", "Layer state info");

	/* Create command */
	pfcUICommand_ptr InputCommand18 = wSession->UICreateCommand("OTKExample18", new MycallbackClass(otkDeleteLayerState) );
	InputCommand18->Designate ("message.txt", "Delete layer state", "Delete layer state", "Delete layer state");

	/* Create command */
	pfcUICommand_ptr InputCommand19 = wSession->UICreateCommand("OTKExample19", new MycallbackClass(otkCombinedStateInfo) );
	InputCommand19->Designate ("message.txt", "Combined state info", "Combined state info", "Combined state info");

	/* Create command */
	pfcUICommand_ptr InputCommand20 = wSession->UICreateCommand("OTKExample20", new MycallbackClass(otkDeleteCombinedState) );
	InputCommand20->Designate ("message.txt", "Delete combined state", "Delete combined state", "Delete combined state");

	/* Create command */
	pfcUICommand_ptr InputCommand21 = wSession->UICreateCommand("OTKExample21", new XSectionExamples("Planar") );
	InputCommand21->Designate ("message.txt", "Create Planar Section", "Create Planar Section", "Create Planar Section");
	
	/* Create command */
	pfcUICommand_ptr InputCommand22 = wSession->UICreateCommand("OTKExample22", new XSectionExamples("Planar with quilt"));
	InputCommand22->Designate ("message.txt", "Create Planar Section with quilt", "Create Planar Section with quilt", "Create Planar Section with quilt");
	
	/* Create command */
	pfcUICommand_ptr InputCommand23_1 = wSession->UICreateCommand("OTKExample23_1", new XSectionExamples("Planar with exclude part"));
	InputCommand23_1->Designate ("message.txt", "Create Planar Section with excluded parts", "Create Planar Section with excluded parts", "Create Planar Section with excluded parts");

	/* Create command */
	pfcUICommand_ptr InputCommand23 = wSession->UICreateCommand("OTKExample23", new XSectionExamples("Planar with part"));
	InputCommand23->Designate ("message.txt", "Create Planar Section with selected part", "Create Planar Section with selected part", "Create Planar Section with selected part");
	
	/* Create command */
	pfcUICommand_ptr InputCommand24 = wSession->UICreateCommand("OTKExample24", new XSectionExamples("Edit Hatching") );
	InputCommand24->Designate ("message.txt", "Section Hatching Multi Edit", "Section Hatching Multi Edit", "Section Hatching Multi Edit");
	
	/*Add popupmenu listner for xsection Object/Action creation*/
	wSession->AddActionListener(new XSectionExamplesPopupMenu());

	/* Create command */
	pfcUICommand_ptr InputCommand25 = wSession->UICreateCommand("OTKExample25", new LightingExamples(wfcLIGHT_SPOT) );
	InputCommand25->Designate ("message.txt", "Add Spot Light", "Add Spot Light", "Add Spot Light");
	
	/* Create command */
	pfcUICommand_ptr InputCommand26 = wSession->UICreateCommand("OTKExample26", new LightingExamples(wfcLIGHT_POINT) );
	InputCommand26->Designate ("message.txt", "Add Point Light", "Add Point Light", "Add Point Light");
	
	/* Create command */
	pfcUICommand_ptr InputCommand27 = wSession->UICreateCommand("OTKExample27", new LightingExamples(wfcLIGHT_DIRECTION) );
	InputCommand27->Designate ("message.txt", "Add Direction Light", "Add Direction Light", "Add Direction Light");
	
	/* Create command */
	pfcUICommand_ptr InputCommand28 = wSession->UICreateCommand("OTKExample28", new LightingExamples(wfcLightType_nil,xtrue) );
	InputCommand28->Designate ("message.txt", "Remove Light", "Remove Light", "Remove Light");
	
	/* Create command */
	pfcUICommand_ptr InputCommand29 = wSession->UICreateCommand("OTKExample29", new LightingExamples(wfcLightType_nil,xfalse,xtrue) );
	InputCommand29->Designate ("message.txt", "Display Light Information", "Display Light Information", "Display Light Information");

	/* Create command */
	pfcUICommand_ptr InputCommand30 = wSession->UICreateCommand("OTKExample30", new MycallbackClass(otkFamilyTableInfo) );
	InputCommand30->Designate ("message.txt", "Family Table Info", "Family Table Info", "Family Table Info");

	/* Create command */
	pfcUICommand_ptr InputCommand31 = wSession->UICreateCommand("OTKExample31", new MycallbackClass(otkFamilyTableErase) );
	InputCommand31->Designate ("message.txt", "Family Table Erase", "Family Table Erase", "Family Table Erase");

	/* Create command */
	pfcUICommand_ptr InputCommand32 = wSession->UICreateCommand("OTKExample32", new MycallbackClass(otkFamilyTableShow) );
	InputCommand32->Designate ("message.txt", "Family Table Show", "Family Table Show", "Family Table Show");

	/* Create command */
	pfcUICommand_ptr InputCommand33 = wSession->UICreateCommand("OTKExample33", new MycallbackClass(otkFamilyTableEdit) );
	InputCommand33->Designate ("message.txt", "Family Table Edit", "Family Table Edit", "Family Table Edit");

	/* Create command */
	pfcUICommand_ptr InputCommand34 = wSession->UICreateCommand("OTKExample34", new MycallbackClass(otkFamilyTableRowSelect) );
	InputCommand34->Designate ("message.txt", "Family Table row selection", "Family Table row selection", "Family Table row selection");
	
	/* Create command */
	pfcUICommand_ptr InputCommand35 = wSession->UICreateCommand("OTKExample35", new MycallbackClass(otkCreateIntfData) );
	InputCommand35->Designate ("message.txt", "Create Interface Data", "Create Interface Data", "Create Interface Data");

	/* Create command */
	pfcUICommand_ptr InputCommand36 = wSession->UICreateCommand("OTKExample36", new MycallbackClass(otkAssemblyItemOperations) );
	InputCommand36->Designate ("message.txt", "Assembly Item Operations", "Assembly Item Operations", "Assembly Item Operations");	

	/* Create command */
	pfcUICommand_ptr InputCommand37 = wSession->UICreateCommand("DumpSessionInfo",new MycallbackClass (otkDumpSessionInfoCmd) );
	InputCommand37->Designate ("message.txt", "Dump Session Info", "Dump Session Info", "Dump Session Info");

	/* Create command */
	pfcUICommand_ptr InputCommand38 = wSession->UICreateCommand("DumpModelInfo",new MycallbackClass (otkDumpModelInfoCmd) );
	InputCommand38->Designate ("message.txt", "Dump Model Info", "Dump Model Info", "Dump Model Info");

	/* Create command */
	pfcUICommand_ptr InputCommand39 = wSession->UICreateCommand("OTKExample39",new MycallbackClass (otkVisitQuilts_top) );
	InputCommand39->Designate ("message.txt", "Visit Model Items", "Visit Model Items", "Visit Model Items");

	/* UIFC Example */
	pfcUICommand_ptr InputCommand40 = wSession->UICreateCommand("OTKExample40",new OTKServerRegistryDialog() );
	InputCommand40->Designate ("message.txt", "UIFC Dialog Demo", "UIFC Dialog Demo", "UIFC Dialog Demo");

	/* MultiBody to Assembly Example */
	pfcUICommand_ptr InputCommand41 = wSession->UICreateCommand("OTKExample41", new OTKMultiBody("MultiBody To Assembly"));
	InputCommand41->Designate("message.txt", "MultiBody To Assembly", "MultiBody To Assembly", "MultiBody To Assembly");

	/* Assembly to MultiBody Example */
	pfcUICommand_ptr InputCommand42 = wSession->UICreateCommand("OTKExample42", new OTKMultiBody("Assembly To MultiBody"));
	InputCommand42->Designate("message.txt", "Assembly To MultiBody", "Assembly To MultiBody", "Assembly To MultiBody");	

	/*MultiBody Apply Appearance Example */
	if (wSession->GetCreoType() == wfcCREO_PARAMETRIC) {
		OtkAppearUIAccessListener* accessPermIdentify = new OtkAppearUIAccessListener(true);
		OtkAppearUIAccessListener* accessPerm = new OtkAppearUIAccessListener(false);

		pfcUICommand_ptr InputCommand43 = wSession->UICreateCommand("OTKExample43", new OTKMultiBody("Identify Bodies", accessPermIdentify, accessPerm));
		InputCommand43->Designate("message.txt", "Identify Bodies", "Identify Bodies", "Identify Bodies");

		pfcUICommand_ptr InputCommand44 = wSession->UICreateCommand("OTKExample44", new OTKMultiBody("Restore Bodies appearance", accessPermIdentify, accessPerm));
		InputCommand44->Designate("message.txt", "Restore Bodies appearance", "Restore Bodies appearance", "Restore Bodies appearance");

		if (InputCommand43 != NULL && InputCommand44 != NULL)
		{
			InputCommand43->AddActionListener(accessPermIdentify);
			InputCommand44->AddActionListener(accessPerm);
		}
	}

	/* UIFC Namke window create Example */
	pfcUICommand_ptr InputCommand45 = wSession->UICreateCommand("OTKExample45", new OTKNakedWinCreate());
	InputCommand45->Designate("message.txt", "UIFC Naked Window Create", "UIFC Naked Window Create", "UIFC Naked Window Create");

	return 0;
      }
	  OTK_EXCEPTION_HANDLER(exception_info);

	  return (int) wfcTK_GENERAL_ERROR;
}

extern "C" void user_terminate()
{
  fprintf(logfile, "\n");
  fprintf(logfile, "OTK application terminated successfully.\n");
  fflush(logfile);
  fclose(logfile);
}


extern "C" void otkXstringDemo ();


extern "C" int main (int argc, char**argv)
{
  if (argc > 2 && strcmp(argv[1], "-cip") == 0)
  {
    int testnum = atoi(argv[2]);
    if (testnum == 1)
      otkXstringDemo();

    return 0;
  }


  wfcOtkMain (argc, argv);
  return 0;
}
