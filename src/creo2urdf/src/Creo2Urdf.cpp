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


void printToMessageWindow(pfcSession_ptr session, std::string message)
{
	xstringsequence_ptr msg_sequence = xstringsequence::create();
	msg_sequence->append(xstring(message.c_str()));
	session->UIClearMessage();
	session->UIDisplayMessage("creo2urdf.txt", "DEBUG %0s", msg_sequence);
}


class Creo2UrdfActionListerner : public pfcUICommandActionListener {
public:
	void OnCommand() override {
		pfcSession_ptr session_ptr = pfcGetProESession();

		pfcModel_ptr model_ptr = session_ptr->GetCurrentModel();


		pfcSolid_ptr solid_ptr = pfcSolid::cast(session_ptr->GetCurrentModel());
		// Export stl of the model
		auto partModels = session_ptr->ListModelsByType(pfcMDL_PART);
		if (!partModels || partModels->getarraysize() == 0) {
			printToMessageWindow(session_ptr, "There are no parts in the session");
			return;
		}
		//message << "We have " << partModels->getarraysize() << " parts" << std::endl;
		printToMessageWindow(session_ptr, "We have " + to_string(partModels->getarraysize()) + " parts");
		// Get all parts in the model
		for (int i = 0; i < partModels->getarraysize(); i++) {
			ProMdlName mdlname;
			auto modelhdl = partModels->get(i);// = partModels->getl; How to transform it to ProModel?
			//message << "model name is " << modelhdl->GetFullName() << " and weighs " << pfcSolid::cast(modelhdl)->GetMassProperty()->GetMass()<< std::endl;
			printToMessageWindow(session_ptr, "model name is " + std::string(modelhdl->GetFullName()) + " and weighs " + to_string(pfcSolid::cast(modelhdl)->GetMassProperty()->GetMass()));
			auto name = modelhdl->GetFullName();
			modelhdl->Export(name + ".stl", pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create("CSYS")));
		}

		return;
	}
};


class Creo2UrdfAccessListener : public pfcUICommandAccessListener {
public:
	pfcCommandAccess OnCommandAccess(xbool AllowErrorMessages) override {
		auto model = pfcGetProESession()->GetCurrentModel();
		if (!model) {
			return pfcCommandAccess::pfcACCESS_AVAILABLE;
		}
		auto type = model->GetType();
		if (type != pfcMDL_PART && type != pfcMDL_ASSEMBLY) {
			return pfcCommandAccess::pfcACCESS_UNAVAILABLE;
		}
		return pfcCommandAccess::pfcACCESS_AVAILABLE;

	}
};

static ProError status;

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
	auto session = pfcGetProESession();

	auto cmd = session->UICreateCommand("Creo2Urdf", new Creo2UrdfActionListerner());
	cmd->AddActionListener(new Creo2UrdfAccessListener()); // To be checked it is odd
	cmd->Designate("ui.txt", "Run Creo2Urdf", "Run Creo2Urdf", "Run Creo2Urdf");
	uiCmdCmdId	cmd_id;

	return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{

}
