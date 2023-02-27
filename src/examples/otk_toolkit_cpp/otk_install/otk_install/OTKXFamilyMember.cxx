/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <wfcGlobal.h>
#include <wfcSession.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h>

#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif

extern "C" wfcStatus addHoleDiameterColumns (pfcSolid_ptr solid)
{
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);

  try {
    pfcFeatures_ptr hole_features = solid->ListFeaturesByType (true, pfcFEATTYPE_HOLE);
    for (int ii =0; ii < hole_features->getarraysize(); ii++)
    {
      pfcFeature_ptr hole_feat = hole_features->get(ii);
      // list all dimensions int the feature
      pfcModelItems_ptr dimensions = hole_feat->ListSubItems(pfcITEM_DIMENSION);
	
      for (int jj = 0; jj<dimensions->getarraysize(); jj++)
      {
        pfcDimension_ptr dim = pfcDimension::cast (dimensions->get(jj));
        /* determine if the dimension is a diameter dimension */
        if (pfcDIM_DIAMETER == dim->GetDimType()) 
        {
          /* create the family table column */
          pfcFamColDimension_ptr dim_column = solid->CreateDimensionColumn(dim);
          /* add the column to the solid
	     instead of NULL, you could pass any array of pfcParamValues 
	     for the initial column values 		  
          */
	  solid->AddColumn(pfcFamilyTableColumn::cast(dim_column), NULL);
        }
      }
    }
	return wfcTK_NO_ERROR;
  }
  OTK_EXCEPTION_HANDLER(exception_info);
  
  return wfcTK_GENERAL_ERROR;
}

extern "C" wfcStatus addHoleDiameterColumns_top ()
{
  ofstream exception_info;
  exception_info.open("exception_info.inf",ios::out);

  try {
    wfcWSession_ptr ses = wfcWSession::cast (pfcGetCurrentSession() );
    pfcSolid_ptr solid = pfcSolid::cast(ses->GetCurrentModel());

    return addHoleDiameterColumns(solid);
  }
  OTK_EXCEPTION_HANDLER(exception_info);
  
  return wfcTK_GENERAL_ERROR;
}
