/*
 * Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <creo2urdf/Creo2Urdf.h>
#include <creo2urdf/Validator.h>

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