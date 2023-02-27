/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <wfcGlobal.h>
#include <wfcSession.h>
#include <wfcSolid.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h>


#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif


extern "C" wfcStatus otkQuickPrint ()
{
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

    wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );

	try
	{
		pfcModel_ptr model = ses->GetCurrentModel();

		if (model != NULL)
		{
			tkout << "Model name: " << model->GetInstanceName() <<endl;
		}
		else
			return wfcTK_GENERAL_ERROR;


		if (!model ||( model->GetType() != pfcMDL_PART && model->GetType() != pfcMDL_ASSEMBLY) )
			return wfcTK_GENERAL_ERROR;

		pfcWindow_ptr win = ses->GetModelWindow(model);

		if (win != NULL)
		{

		  pfcSolid_ptr solidModel = pfcSolid::cast(model);
                  
                  wfcWSolid_ptr wSolid = wfcWSolid::cast (solidModel);
                  
                  wfcQuickPrintInstructions_ptr quickPInstr = wfcQuickPrintInstructions::Create();
                  
                  quickPInstr->SetLayoutType(wfcQPRINT_LAYOUT_MANUAL);
                  
                  quickPInstr->SetManualLayoutType (wfcQPRINTMANUAL_3VIEW_1_23VERT);
                  
                  wfcDrawingViewDisplay_ptr viewDisplayStyle1 = wfcDrawingViewDisplay::Create (pfcDISPSTYLE_WIREFRAME, false, wfcTANEDGE_SOLID, wfcCABLEDISP_DEFAULT, false, false);
                  wfcQuickPrintGeneralViewInsructions_ptr generalViewInstruction1v = wfcQuickPrintGeneralViewInsructions::Create(wfcQPRINT_PROJ_GENVIEW_MAIN, "1_VIEW", 0.025, viewDisplayStyle1);
                  quickPInstr->SetGeneralViewInstructions(generalViewInstruction1v);
                                    
                  wfcQuickPrintGeneralViewInsructions_ptr generalViewInstruction2v = wfcQuickPrintGeneralViewInsructions::Create(wfcQPRINT_PROJ_GENVIEW_MAIN, "2_VIEW", 0.025, viewDisplayStyle1);
                  quickPInstr->SetGeneralViewInstructions(generalViewInstruction2v);  
                                  
                  wfcQuickPrintGeneralViewInsructions_ptr generalViewInstruction3v = wfcQuickPrintGeneralViewInsructions::Create(wfcQPRINT_PROJ_GENVIEW_MAIN, "3_VIEW", 0.025, viewDisplayStyle1);
                  quickPInstr->SetGeneralViewInstructions(generalViewInstruction3v);                  
                  
                  wSolid->QuickPrint(quickPInstr, "postscript.pcf");
		}
		else
		{
                  return wfcTK_GENERAL_ERROR;
		}

		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(exception_info);
			
	return wfcTK_GENERAL_ERROR;
}

