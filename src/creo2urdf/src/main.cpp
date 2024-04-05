/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Creo2Urdf.h>
#include <creo2urdf/Validator.h>
#include <ProTKRunTime.h>
#include <ProCore.h>


 /*! @brief Do batch mode stuff
 */
ProError evaluateBatchMode(const std::string& asm_path, const std::string& yaml_path, const std::string& csv_path, const std::string& output_path) {
    if (asm_path.empty() || yaml_path.empty() || csv_path.empty() || output_path.empty()) { 
        return PRO_TK_BAD_INPUTS; // to be safe
    } 
    ProError err = PRO_TK_NO_ERROR;

    pfcBaseSession* session = pfcGetProESession();
    if (!session) {
        ProTKPrintf("Creo2Urdf: impossible to retrieve the session");
		return PRO_TK_GENERAL_ERROR;
	}

    auto asm_model_ptr = session->RetrieveModel(pfcModelDescriptor::CreateFromFileName(asm_path.c_str()));

    if (!asm_model_ptr) {
		ProTKPrintf("Creo2Urdf: impossible to retrieve the model located in %s", asm_path);
        return PRO_TK_GENERAL_ERROR;
    }


    Creo2Urdf creo2urdfApp(yaml_path, csv_path, output_path, asm_model_ptr);
    creo2urdfApp.OnCommand();
    return err;
}

/**
 * @brief Initializes the buttons by associating them with the corresponding functions
 * Additionally, it loads the ribbon definition file.
 * 
 */
extern "C" int user_initialize(
    int argc,
    char* argv[],
    char* version,
    char* build,
    wchar_t errbuf[80])
{
    auto session = pfcGetProESession();

    if (argc > 4) {
        std::string asm_path    = argv[1];
        std::string yaml_path   = argv[2];
        std::string csv_path    = argv[3];
        std::string output_path = argv[4];
        ProError err = evaluateBatchMode(asm_path, yaml_path, csv_path, output_path);
        ProEngineerEnd();
        return (int)err; // or whatever you want
    }

    ProTKPrintf("Creo2Urdf version %s build %s\n", version, build);

    auto cmd = session->UICreateCommand("Creo2Urdf", new Creo2Urdf());
    cmd->AddActionListener(new Creo2UrdfAccess()); // To be checked it is odd
    cmd->Designate("ui.txt", "Run Creo2Urdf", "Run Creo2Urdf", "Run Creo2Urdf");

    auto cmd_validate = session->UICreateCommand("Validator", new Validator());
    cmd_validate->AddActionListener(new ValidatorAccess()); // To be checked it is odd
    cmd_validate->Designate("ui.txt", "Run Validation", "Run Validation", "Run Validation");

    session->RibbonDefinitionfileLoad("creo2urdf.rbn");

    return (0);
}

/**
 * @brief Handles termination actions.
 * 
 */
extern "C" void user_terminate()
{
}