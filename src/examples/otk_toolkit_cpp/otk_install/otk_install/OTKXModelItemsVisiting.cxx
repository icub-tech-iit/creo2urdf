/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 

/* 
   
This example demonstrates the visiting of model items.
It also shows how to use OTK C++ together with Creo Parametric Toolkit:
quilts are visited using OTK C++, surfaces in quilts are visited 
using Creo Parametric Toolkit.

*/

#include <pfcSession.h>
#include <wfcSession.h>
#include <wfcGlobal.h>
#include <pfcExceptions.h>

#include <ProToolkit.h>
#include <ProObjects.h>
#include <ProSurface.h>
#include <ProQuilt.h>
#include <OTKXUtils.h>


#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif

extern "C" ProError otkx_qltsrf_visit (ProSurface surface, ProError status, ProAppData data)
{
  ProError ierr;
  static double total_area =0;
  double surf_area, *srfarea_ptr;

  srfarea_ptr = (double *) data;

  if (*srfarea_ptr == 0)
    total_area = 0;

  ierr = ProSurfaceAreaEval (surface, &surf_area);
  if (PRO_TK_NO_ERROR != ierr)
    return ierr;

  total_area += surf_area;

  *srfarea_ptr = total_area;

  return (PRO_TK_NO_ERROR);
}

extern "C" ProError otkx_qltsrf_filter (ProSurface surface, ProAppData data)
{
  return (PRO_TK_NO_ERROR);
}

class OTKXQuiltVisitor : public wfcDefaultVisitingClient
{
public:
  OTKXQuiltVisitor() : surf_area(0) {}

  wfcStatus ApplyAction (pfcObject_ptr pfc_object, wfcStatus filter_status)
  {
    ProQuilt quilt;
    ProError err;
    quilt = (ProQuilt)wfcGetHandleFromObject( pfcObject::cast(pfc_object) );

//    err = ProGeomitemToQuilt((ProGeomitem*)p_model_item,&quilt);
//    ProError status = (ProError)wfcStatusToPro(filter_status);

    if (wfcTK_NO_ERROR == filter_status)
    {
      err =  ProQuiltSurfaceVisit ( quilt, otkx_qltsrf_visit, otkx_qltsrf_filter,
                                   (ProAppData)&surf_area);
      return wfcStatusFromPro(err, "OTKXQuiltVisitor::ApplyAction");
    }
    else
      return filter_status;
  }

  double GetCombinedSurfArea () { return surf_area; }

private:
  double surf_area;
};

extern "C" wfcStatus otkVisitQuilts (wfcWSolid_ptr solid)
{
  OTKXQuiltVisitor *visitor = 	new OTKXQuiltVisitor();
  wfcStatus status = solid->VisitItems(visitor, pfcITEM_QUILT);

  tkout << "wfcWSolid::VisitItems returned with status " << status << endl;

  if (wfcTK_NO_ERROR != status && wfcTK_E_NOT_FOUND != status)
  {
    if (wfcTK_E_NOT_FOUND == status)
      tkout << "No quilts found\n";
    else
      tkout << "VisitItems returned with error " << status << endl;

    return status;
  }

  tkout.precision(10);
  tkout << "Surface Area : " << visitor->GetCombinedSurfArea() << endl;

  status = wfcWSession::cast(pfcGetCurrentSession())->ReleaseVisitor(visitor);

  return status;
}

extern "C" wfcStatus otkVisitQuilts_top ()
{
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);
  try {
    wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );
    wfcWSolid_ptr solid = wfcWSolid::cast(ses->GetCurrentModel());

    return otkVisitQuilts (solid);
  }
  OTK_EXCEPTION_HANDLER(exception_info);

  return wfcTK_GENERAL_ERROR;
}
