#include <ProToolkit.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProSelection.h>
#include <ProIntfData.h>
#include <ProArray.h>
#include <ProNotify.h>
#include <ProDrawing.h>
#include <ProDrawingView.h>
#include <pfcGlobal.h>
#include <pfcModel.h>
#include <pfcSolid.h>
#include <string>
#include <pfcShrinkwrap.h>

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

ProError Creo2Urdf();

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
	status = ProCmdActionAdd("RunBug", (uiCmdCmdActFn)Creo2Urdf, uiProe2ndImmediate, TestAccessDefault, PRO_B_TRUE, PRO_B_TRUE, &cmd_id);
	status = ProMenubarmenuPushbuttonAdd("File", "Creo2Urdf", "-Run Creo2Urdf", "Run Creo2Urdf code", "File.psh_rename", PRO_B_TRUE, cmd_id, L"creo2urdf.txt");

	return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{

}

void printToMessageWindow(pfcSession_ptr session, std::stringstream & message)
{
	xstringsequence_ptr msg_sequence = xstringsequence::create();
	msg_sequence->append(xstring(message));

	session->UIDisplayMessage("creo2urdf.txt", "DEBUG %0s", msg_sequence);
}


/*====================================================================*\
FUNCTION : Creo2Urdf
PURPOSE  : Execute the creo2urdf code.
\*====================================================================*/
ProError Creo2Urdf()
{
	ProError status = PRO_TK_GENERAL_ERROR;

	pfcSession_ptr session_ptr = pfcGetProESession();

	pfcModel_ptr model_ptr = session_ptr->GetCurrentModel();

	xstring name = model_ptr->GetFullName();

	pfcSolid_ptr solid_ptr = pfcSolid::cast(session_ptr->GetCurrentModel());
	pfcMassProperty_ptr massprop_ptr = solid_ptr->GetMassProperty();
	xreal mass = massprop_ptr->GetMass();

	std::stringstream message;
	message << "model name is " << name << " and weighs " << mass<<"\n";

	// Export stl of the model
	//pfcShrinkwrapSTLInstructions stlExportInstructions("2Bars.stl");
	//auto stlExportInstructions_ptr = stlExportInstructions.Create("optional xrstring CsysName");

	// The model is an assembly, let's navigate its parts.
	if (model_ptr->GetType() == pfcMDL_ASSEMBLY) {
		auto assembly_ptr = pfcAssembly::cast(model_ptr);
		auto items_ptr = assembly_ptr->ListFeaturesByType(pfcFEATTYPE_COMPONENT);//pfcITEM_SIMPREP);

		if (items_ptr) {
			message.clear();
			message << "Number of Items found is " << items_ptr->getarraysize() << "\n";
			for (int i = 0; i < items_ptr->getarraysize(); ++i) {
				auto item_ptr = items_ptr->get(i);
				message<<" "<<item_ptr->GetName()<<" ";

			}
			printToMessageWindow(session_ptr, message);
		}
	}

	model_ptr->Export("2bars.stl", pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create("ASM_CSYS")));



	if(status != PRO_TK_NO_ERROR)
		return status;

	return status;
}





