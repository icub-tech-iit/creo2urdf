/******************************************************************

FILE:      PTTestBug.c
PURPOSE:   Pro/TOOLKIT application template file.

HISTORY:
Date       Build     Modifier   Rev#   Changes
--------------------------------------------------------------------
10-Oct-08  L-03-18   SDJ        $$1    Created.

*******************************************************************/

#include <ProToolkit.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProSelection.h>
#include <ProIntfData.h>
#include <ProArray.h>
#include <ProDrawing.h>
#include <ProDimension.h>

FILE* errlog_fp;
FILE* fp;

#define PT_TEST_LOG(func,status,err) \
	if (err) \
{ \
	printf (" LOG Error: %s\t%d\n", func, status); \
	fprintf (errlog_fp, " LOG Error: %s\t%d\n", func, status); \
} \
else \
{ \
	printf (" LOG %s\t%d\n", func, status); \
	fprintf (errlog_fp, " LOG %s\t%d\n", func, status); \
}

#define PT_TEST_LOG_SUCC(func) \
	PT_TEST_LOG (func, status, status != PRO_TK_NO_ERROR)

#define PTTestResfileWrite(str) \
{\
	printf(str);\
	printf("\n");\
}

static char line [500];

/*--------------------------------------------------------------------*\
ProAppData used while visiting Csys
\*--------------------------------------------------------------------*/
typedef struct {
	ProMdl		model;
	ProCsys		p_csys;
	ProName		csys_name;
	ProModelitem *items;
}UserCsysAppData ;

ProError ProUtilCsysFind(ProMdl		p_model,
						 ProName		csys_name, ProCsys		*p_csys);

static uiCmdAccessState TestAccessDefault(uiCmdAccessMode access_mode)
{
	return (ACCESS_AVAILABLE);
}

static ProError status;

ProError PTTestRunBug ();

/*====================================================================*\
FUNCTION : user_initialize()
PURPOSE  : 
\*====================================================================*/
extern "C" int user_initialize(
	int argc,			
	char *argv[],
	char *version, 
	char *build,
	wchar_t errbuf[80])
{
	uiCmdCmdId	cmd_id;

	errlog_fp = fopen ("PTTestBug.log", "w");

	status = ProCmdActionAdd("RunBug", (uiCmdCmdActFn)PTTestRunBug,
		uiProe2ndImmediate, TestAccessDefault,
		PRO_B_TRUE, PRO_B_TRUE, &cmd_id);
	PT_TEST_LOG_SUCC ("ProCmdActionAdd");

	status = ProMenubarmenuPushbuttonAdd(
		"File", "RunBug", "-Run Bug", "Run bug code",
		"File.psh_rename", PRO_B_TRUE, cmd_id, L"pt_bug.txt");
	PT_TEST_LOG_SUCC("ProMenubarmenuPushbuttonAdd");

	fflush(errlog_fp);

	return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{
	fflush(errlog_fp);
	fclose (errlog_fp);
}


ProError PTTestRunBug()
{
	ProError status;
	ProMdl drwModel, solid;
	ProDimAttachment *attachmentsArr;
	ProDimSense *senseArray;
	ProDimOrient orient;

	ProDimension dimension;
	int attachSize, i, senseSize, j;
	ProModelitem mdlItem1;

	int nSels, nSels1;
	ProSelection *sels, *sels1;
	Pro3dPnt loc = { 173.066548, 630.577482, 0.000000 };

	fp = fopen("dimension_info_file.log", "w+");

	status = ProMdlCurrentGet(&drwModel);
	PT_TEST_LOG_SUCC("ProMdlCurrentGet");

	status = ProDrawingCurrentsolidGet((ProDrawing)drwModel, (ProSolid*)&solid);
	PT_TEST_LOG_SUCC("ProDrawingCurrentsolidGet");

	status = ProSelect("datum,edge,edge_end,curve_end", -1, NULL, NULL, NULL, NULL, &sels, &nSels);
	PT_TEST_LOG_SUCC("ProSelect");

	if (status == PRO_TK_NO_ERROR && nSels >= 2)
	{
		status = ProArrayAlloc(0, sizeof(ProDimAttachment), 1, (ProArray*)&attachmentsArr);
		PT_TEST_LOG_SUCC("ProArrayAlloc");

		status = ProArrayAlloc(0, sizeof(ProDimSense), 1, (ProArray *)&senseArray);
		PT_TEST_LOG_SUCC("ProArrayAlloc");

		for (int i = 1; i < nSels; i++)
		{
			ProDimAttachment* oneAttach;
			status = ProArrayAlloc(2, sizeof(ProDimAttachment), 1, (ProArray*)&oneAttach);
			status = ProSelectionCopy(sels[0], &oneAttach[0][0]);
			PT_TEST_LOG_SUCC("ProSelectionCopy");

			status = ProSelectionCopy(sels[i], &oneAttach[1][0]);
			PT_TEST_LOG_SUCC("ProSelectionCopy");

			oneAttach[0][1] = NULL;
			oneAttach[1][1] = NULL;

			status = ProArrayObjectAdd((ProArray*)&attachmentsArr, -1, 1, oneAttach);

			ProDimSense *Onesense;
			status = ProArrayAlloc(2, sizeof(ProDimSense), 1, (ProArray *)&Onesense);
			PT_TEST_LOG_SUCC("ProArrayAlloc");

			Onesense[0].type = PRO_DIM_SNS_TYP_PNT;
			Onesense[0].sense = 4;
			Onesense[0].orient_hint = PRO_DIM_ORNT_NONE;

			Onesense[1].type = PRO_DIM_SNS_TYP_PNT;
			Onesense[1].sense = 2;
			Onesense[1].orient_hint = PRO_DIM_ORNT_NONE;

			status = ProArrayObjectAdd((ProArray*)&senseArray, -1, 1, Onesense);
		}

	}

	status = ProDrawingOrdinateDimensionsCreate((ProDrawing)drwModel, PRO_DIMENSION, attachmentsArr, senseArray, loc, &dimension);
	PT_TEST_LOG_SUCC("ProDrawingOrdinateDimensionsCreate");

	fclose(fp);
	return (PRO_TK_NO_ERROR);
}


/*====================================================================*\
FUNCTION : PTTestRunBug
PURPOSE  : Execute the bug code.
\*====================================================================*/
//ProError PTTestRunBug ()
//{	
//	ProError status;	
//	ProMdl drwModel, solid;
//	ProDimAttachment *attachmentsArr;
//	ProDimSense *senseArray;
//	ProDimOrient orient;
//	
//	ProDimension dimension;
//	int attachSize, i, senseSize, j;
//	ProModelitem mdlItem1;
//
//	int nSels, nSels1;
//	ProSelection *sels, *sels1;
//	Pro3dPnt loc = {173.066548, 630.577482, 0.000000};
//
//	fp = fopen ("dimension_info_file.log", "w+");
//
//	status = ProMdlLoad(L"drw0002", PRO_MDL_DRAWING, PRO_B_FALSE, &drwModel);
//	PT_TEST_LOG_SUCC("ProMdlLoad");
//
//	status = ProMdlDisplay(drwModel);
//	PT_TEST_LOG_SUCC("ProMdlDisplay");
//
//	status = ProDrawingCurrentsolidGet((ProDrawing)drwModel,(ProSolid*)&solid);
//	PT_TEST_LOG_SUCC("ProDrawingCurrentsolidGet");
//	
//	status = ProArrayAlloc(2,sizeof(ProDimAttachment),1,(ProArray*)&attachmentsArr);
//	PT_TEST_LOG_SUCC("ProArrayAlloc");
//
///*	status = ProModelitemInit((ProMdl)solid, 131, PRO_EDGE, &mdlItem1);
//	PT_TEST_LOG_SUCC("ProModelitemInit");
//
//	status = ProSelectionAlloc(NULL, &mdlItem1, &attachmentsArr[0][0]);
//	PT_TEST_LOG_SUCC("ProSelectionAlloc");
//*/
//	status = ProSelect("edge", 2, NULL, NULL, NULL, NULL, &sels, &nSels);
//	PT_TEST_LOG_SUCC("ProSelect");
//
//	status = ProSelectionCopy(sels[0], &attachmentsArr[0][0]);
//	PT_TEST_LOG_SUCC("ProSelectionCopy");
//
//	status = ProSelectionCopy(sels[1], &attachmentsArr[1][0]);
//	PT_TEST_LOG_SUCC("ProSelectionCopy");
//
//	attachmentsArr[0][1] = NULL;
//	attachmentsArr[1][1] = NULL;
//
//	status = ProArrayAlloc (2, sizeof (ProDimSense), 1, (ProArray *) &senseArray);
//	PT_TEST_LOG_SUCC("ProArrayAlloc");
//
//	senseArray[0].type = PRO_DIM_SNS_TYP_PNT;
//	senseArray[0].sense = 4;
//	senseArray[0].orient_hint = PRO_DIM_ORNT_NONE;
//
//	senseArray[1].type = PRO_DIM_SNS_TYP_PNT;
//	senseArray[1].sense = 2;
//	senseArray[1].orient_hint = PRO_DIM_ORNT_NONE;
//
//	status = ProDrawingOrdinateDimensionsCreate((ProDrawing)drwModel, PRO_DIMENSION, attachmentsArr, senseArray, loc, &dimension);
//	PT_TEST_LOG_SUCC("ProDrawingOrdinateDimensionsCreate");
//	
//	fclose(fp);
//  return (PRO_TK_NO_ERROR);
//}





