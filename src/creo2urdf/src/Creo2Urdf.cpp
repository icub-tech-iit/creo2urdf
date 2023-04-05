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
#include <pfcShrinkwrap.h>
#include <pfcAssembly.h>

#include <wfcGeometry.h> 

#include <string>

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

		
		// TODO Principal units probably to be changed from MM to M before getting the model properties
		//auto length_unit = solid_ptr->GetPrincipalUnits()->GetUnit(pfcUnitType::pfcUNIT_LENGTH);
		// length_unit->Modify(pfcUnitConversionFactor::Create(0.001), length_unit->GetReferenceUnit()); // IT DOES NOT WORK
		
		// Export stl of the model

		auto asm_csys_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);
		if (asm_csys_list->getarraysize() == 0) {
			printToMessageWindow(session_ptr, "There are no CYS in the asm");
			return;
		}

		// TODO We assume to have just one csys in the ASM
		//asm_csys_list->get(0)->

		auto partModels = session_ptr->ListModelsByType(pfcModelType::pfcMDL_PART);
		if (!partModels || partModels->getarraysize() == 0) {
			printToMessageWindow(session_ptr, "There are no parts in the session");
			return;
		}
		printToMessageWindow(session_ptr, "We have " + to_string(partModels->getarraysize()) + " parts");
		
		
		auto csys_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);
		if (csys_list->getarraysize() == 0) {
			return;
		}

		auto asm_component_list = model_ptr->ListItems(pfcModelItemType::pfcITEM_FEATURE);
		if (asm_component_list->getarraysize() == 0) {
			printToMessageWindow(session_ptr, "There are no CYS in the asm");
			return;
		}

		for (int i = 0; i < asm_component_list->getarraysize(); i++)
		{
			auto comp = asm_component_list->get(i);

			auto feat = pfcFeature::cast(comp);

			if (feat->GetFeatType() == pfcFeatureType::pfcFEATTYPE_COMPONENT)
			{
				auto feat_id = feat->GetId();
				xintsequence_ptr seq = xintsequence::create();

				seq->append(feat_id);
				
				pfcComponentPath_ptr comp_path = pfcCreateComponentPath(pfcAssembly::cast(model_ptr), seq);

				auto transform = comp_path->GetTransform(xfalse);

				auto m = transform->GetMatrix();
				auto o = transform->GetOrigin();
				printToMessageWindow(session_ptr, "feat name: id: " + to_string(feat_id));

				printToMessageWindow(session_ptr, "origin x: " + to_string(o->get(0)) + " y: " + to_string(o->get(1)) + " z: " + to_string(o->get(2)));
				printToMessageWindow(session_ptr, "transform:");
				printToMessageWindow(session_ptr, to_string(m->get(0, 0)) + " " + to_string(m->get(0, 1)) + " " + to_string(m->get(0, 2)));
				printToMessageWindow(session_ptr, to_string(m->get(1, 0)) + " " + to_string(m->get(1, 1)) + " " + to_string(m->get(1, 2)));
				printToMessageWindow(session_ptr, to_string(m->get(2, 0)) + " " + to_string(m->get(2, 1)) + " " + to_string(m->get(2, 2)));
			}

		}

		
		// Get all parts in the model
		for (int i = 0; i < partModels->getarraysize(); i++) {
			auto modelhdl   = partModels->get(i);
			auto name       = modelhdl->GetFullName();
			auto massProp   = pfcSolid::cast(modelhdl)->GetMassProperty();
			auto com        = massProp->GetGravityCenter();
			auto princAxis  = massProp->GetPrincipalAxes();
			auto comInertia = massProp->GetCenterGravityInertiaTensor(); // TODO GetCoordSysInertia ?
		
			printToMessageWindow(session_ptr, "Model name is " + std::string(name) + " and weighs " + to_string(massProp->GetMass()));
			printToMessageWindow(session_ptr, "Center of mass: x: " + to_string(com->get(0)) + " y: " + to_string(com->get(1)) + " z: "+ to_string(com->get(2)));
			printToMessageWindow(session_ptr, "Inertia tensor:");
			printToMessageWindow(session_ptr, to_string(comInertia->get(0, 0)) + " " + to_string(comInertia->get(0, 1)) + " " + to_string(comInertia->get(0, 2)));
			printToMessageWindow(session_ptr, to_string(comInertia->get(1, 0)) + " " + to_string(comInertia->get(1, 1)) + " " + to_string(comInertia->get(1, 2)));
			printToMessageWindow(session_ptr, to_string(comInertia->get(2, 0)) + " " + to_string(comInertia->get(2, 1)) + " " + to_string(comInertia->get(2, 2)));


			auto csys_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_COORD_SYS);
			if (csys_list->getarraysize() == 0) {
				printToMessageWindow(session_ptr, "There are no CYS in the part "+string(name));
				return;
			}

			auto axes_list = modelhdl->ListItems(pfcModelItemType::pfcITEM_AXIS);
			printToMessageWindow(session_ptr, "There are " + to_string(axes_list->getarraysize()) + " axes");
			if (axes_list->getarraysize() !=0 ) {
				for (int i = 0; i < axes_list->getarraysize(); i++)
				{
					auto axis = pfcAxis::cast(axes_list->get(i));
					printToMessageWindow(session_ptr, "The axis is called " + string(axis->GetName()) + " axes");

					auto axis_data = wfcWAxis::cast(axis)->GetAxisData();

					auto axis_line = pfcLineDescriptor::cast(axis_data); // cursed cast from hell

					// There are just two points in the array
					pfcPoint3D_ptr point = axis_line->GetEnd1();

					printToMessageWindow(session_ptr, "Start point coords of " + string(axis->GetName()) + " are (x, y, z) : (" + std::to_string(point->get(0)) + ", " + std::to_string(point->get(1)) + ", " + std::to_string(point->get(2)) + ")");

					point = axis_line->GetEnd2();
					printToMessageWindow(session_ptr, "End point coords of " + string(axis->GetName()) + " are (x, y, z) : (" + std::to_string(point->get(0)) + ", " + std::to_string(point->get(1)) + ", " + std::to_string(point->get(2)) + ")");
				}

			}

			// Getting just the first csys is a valid assumption for the MVP-1, for more complex asm we will need to change it
			modelhdl->Export(name + ".stl", pfcExportInstructions::cast(pfcSTLBinaryExportInstructions().Create(csys_list->get(0)->GetName())));
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

	session->RibbonDefinitionfileLoad("tool.rbn");


	return (0);
}

/*====================================================================*\
FUNCTION : user_terminate()
PURPOSE  : To handle any termination actions
\*====================================================================*/
extern "C" void user_terminate()
{

}
