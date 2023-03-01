#include <ProToolkit.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProSelection.h>
#include <ProIntfData.h>
#include <ProArray.h>
#include <ProNotify.h>
#include <ProDrawing.h>
#include <ProDrawingView.h>
#include <wfcSession.h>
#include <wfcGlobal.h>

FILE* errlog_fp;

#define PT_TEST_LOG(func,status, model_name,err) \
	if (err) \
{ \
	printf (" LOG Error: %s\t%d\n", func, status); \
	fprintf (errlog_fp, " ciao icub-tech ho trovato un errore in aprire il modello corrente: %s\t%d\n", func, status); \
} \
else \
{ \
	printf (" LOG %s\t%d\n", func, status); \
	fprintf (errlog_fp, " ciao icub-tech sono riuscito ad aprire il modello %s\t%d : %ls \n", func, status, model_name); \
}

#define PT_TEST_LOG_SUCC(func, model_name) \
	PT_TEST_LOG (func, status, model_name, status != PRO_TK_NO_ERROR)

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
	status = ProCmdActionAdd("RunBug", (uiCmdCmdActFn)PTTestRunBug, uiProe2ndImmediate, TestAccessDefault, PRO_B_TRUE, PRO_B_TRUE, &cmd_id);
	status = ProMenubarmenuPushbuttonAdd("File", "RunBug", "-Run Bug", "Run bug code", "File.psh_rename", PRO_B_TRUE, cmd_id, L"pt_bug.txt");	

	return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{

}


/*====================================================================*\
FUNCTION : PTTestRunBug
PURPOSE  : Execute the bug code.
\*====================================================================*/
ProError PTTestRunBug ()
{	
	ProError status = PRO_TK_GENERAL_ERROR;
	ProMdl currMdl;
	ProFeature feat1;
	ProIntfDataSource intfdata;
	ProSelection *sels;
	int nSels = -1;
	ProCsys csys;
	ProModelitem mdlItem;
	ProMdlName model_name;

	errlog_fp = fopen ("PTTestBug.log", "w");

	status = ProMdlCurrentGet(&currMdl);

	status = ProMdlMdlnameGet(currMdl, model_name);

	PT_TEST_LOG_SUCC("ProMdlCurrentGet", model_name);

	if(status != PRO_TK_NO_ERROR)
		return status;	

	fflush(errlog_fp);
	fclose (errlog_fp);
	return status;	
}





