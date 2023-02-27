/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <wfcGlobal.h>
#include <wfcSession.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h>

static xstring dwgtxt = "dwg.txt";

#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif

extern "C" wfcStatus otkDrawingCreateSheetAndViews (pfcModel_ptr model, xstring solidName)
{  
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);

  wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );
  try {
    if (!model || model->GetType() != pfcMDL_DRAWING)
      return wfcTK_GENERAL_ERROR;

    pfcDrawing_ptr drawing = pfcDrawing::cast(model);

    tkout << "The drawing has " << drawing->List2DViews()->getarraysize() << " views" << endl;

    int sheetNo = drawing->AddSheet();
    drawing->SetCurrentSheetNumber (sheetNo);

    pfcModelDescriptor_ptr mdlDescr = 0;
    try {
      mdlDescr = pfcModelDescriptor::CreateFromFileName (solidName);
    }
    xcatchbegin
    xcatch (pfcXInvalidFileName, xfn) 
      tkout << "pfcXInvalidFileName in otkDrawingCreateSheetAndViews - no extension?" << endl;
      return wfcTK_INVALID_FILE;
    xcatch (pfcXUnknownModelExtension, xume) 
      tkout << "pfcXUnknownModelExtension in otkDrawingCreateSheetAndViews - wrong extension?" << endl;
          return wfcTK_INVALID_FILE;
    xcatchend

    pfcModel_ptr solidMdl = ses->GetModelFromDescr (mdlDescr);

    if (!solidMdl) {
      solidMdl = ses->RetrieveModel (mdlDescr);
    }
    try {
          drawing->AddModel (solidMdl);
	}
	OTK_EXCEPTION_HANDLER(exception_info);

    pfcMatrix3D_ptr matrix = pfcMatrix3D::create();
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++) {
        if (i == j)
          matrix->set (i, j, 1.0);
        else
          matrix->set (i, j, 0.0);
      }

    pfcTransform3D_ptr transf = pfcTransform3D::Create (matrix);

    pfcPoint3D_ptr pos = pfcPoint3D::create();
    pos->set (0, 200.0);
    pos->set (1, 600.0);
    pos->set (2, 0.0);

    pfcGeneralViewCreateInstructions_ptr instrs =
          pfcGeneralViewCreateInstructions::Create (solidMdl, sheetNo,
                                                    pos, transf);

    pfcView2D_ptr genView = 
            drawing->CreateView (pfcView2DCreateInstructions::cast(instrs));

    pfcOutline3D_ptr outline = genView->GetOutline();

    pos->set (0,  outline->get(1)->get(0) + 
                 (outline->get(1)->get(0) - outline->get(0)->get(0))  );
    pos->set (1, (outline->get(0)->get(1) + outline->get(1)->get(1))/2);

    pfcProjectionViewCreateInstructions_ptr pInstrs =
        pfcProjectionViewCreateInstructions::Create (genView, pos);

    drawing->CreateView (pfcView2DCreateInstructions::cast(pInstrs));

    pos->set (0, (outline->get(0)->get(0) + outline->get(1)->get(0))/2);
    pos->set (1,  outline->get(0)->get(1) - 
                 (outline->get(1)->get(1) - outline->get(0)->get(1))  );

    pInstrs =
         pfcProjectionViewCreateInstructions::Create (genView, pos);

    drawing->CreateView (pfcView2DCreateInstructions::cast(pInstrs));

    tkout << "Now the drawing has " << drawing->List2DViews()->getarraysize() << " views" << endl;

    return wfcTK_NO_ERROR;
  }
  OTK_EXCEPTION_HANDLER(exception_info);

  return wfcTK_GENERAL_ERROR;

}

extern "C" wfcStatus otkDrawingCreateSheetAndViews_top ()
{
    wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );
    pfcModel_ptr model = ses->GetCurrentModel();

    ses->UIDisplayMessage (dwgtxt, "OTK_DWG1", NULL);

    xstring solidName = ses->UIReadStringMessage (NULL);

    return otkDrawingCreateSheetAndViews(model, solidName);
}
