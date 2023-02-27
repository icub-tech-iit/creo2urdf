/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <wfcGlobal.h>
#include <wfcSession.h>
#include <pfcExceptions.h>
#include <pfcUDFCreate.h>
#include <OTKXUtils.h>

#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif

extern "C" wfcStatus otkCreateNodeUDFInPart (pfcSolid_ptr solid,
                                             xreal diam)
{
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);
  try {
    if (!solid)
      return wfcTK_BAD_INPUTS;

    pfcModelItems_ptr items = solid->ListItems (pfcITEM_COORD_SYS);

    if (!items || items->getarraysize() == 0)
      return wfcTK_BAD_INPUTS;

    pfcCoordSystem_ptr csys = pfcCoordSystem::cast(items->get(0));

    pfcUDFCustomCreateInstructions_ptr instrs =
                       pfcUDFCustomCreateInstructions::Create ("node");

    instrs->SetDimDisplayType (pfcUDFDISPLAY_BLANK);

    pfcSelection_ptr csys_sel = 
            pfcCreateModelItemSelection (pfcModelItem::cast(csys), NULL);

    pfcUDFReference_ptr csys_ref = pfcUDFReference::Create ("REF_CSYS", csys_sel);

    pfcUDFReferences_ptr refs = pfcUDFReferences::create();

    refs->set (0, csys_ref);

    instrs->SetReferences (refs);

    pfcUDFVariantDimension_ptr var_diam =
                                   pfcUDFVariantDimension::Create ("d11", diam);

    pfcUDFVariantValues_ptr vals = pfcUDFVariantValues::create();
    vals->set (0, pfcUDFVariantValue::cast(var_diam));

    instrs->SetVariantValues (vals);

    pfcSolid_ptr placement_model = pfcSolid::cast(csys->GetDBParent());


    pfcFeatureGroup_ptr group = 
       placement_model->CreateUDFGroup (pfcUDFCustomCreateInstructions::cast(instrs));

    return wfcTK_NO_ERROR;
  }
  OTK_EXCEPTION_HANDLER(exception_info);

  return wfcTK_GENERAL_ERROR;
}

extern "C" wfcStatus otkCreateNodeUDFInPart_top ()
{
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);
  try {
    wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );
    pfcSolid_ptr solid = pfcSolid::cast(ses->GetCurrentModel());

    return otkCreateNodeUDFInPart(solid, 1.);
  }
  OTK_EXCEPTION_HANDLER(exception_info);

  return wfcTK_GENERAL_ERROR;
}
