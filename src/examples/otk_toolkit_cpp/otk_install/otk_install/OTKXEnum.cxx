/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Methods to return enum as string.
  
*/

#include <OTKXEnum.h>
#include <wfcCombState.h>
#include <wfcAssembly.h>
#include <pfcSimpRep.h>



char* otkxEnums::wfcCurveCollectionInstrAttributeGet (int id)
{
	switch (id)
	{
		case 	wfcCURVCOLL_NO_ATTR	: return ((char*)"wfcCURVCOLL_NO_ATTR");
		case 	wfcCURVCOLL_ALL	: return ((char*)"wfcCURVCOLL_ALL");
		case 	wfcCURVCOLL_CONVEX	: return ((char*)"wfcCURVCOLL_CONVEX");
		case 	wfcCURVCOLL_CONCAVE	: return ((char*)"wfcCURVCOLL_CONCAVE");
		case 	wfcCURVCOLL_RESERVED_ATTR	: return ((char*)"wfcCURVCOLL_RESERVED_ATTR");
	}

	char *str = (char *) malloc(100);
	sprintf (str, "%d", id);
	
	return (str);
}


char* otkxEnums::wfcCurveCollectionInstrTypeGet (int id)
{
	switch (id)
	{
		case 	wfcCURVCOLL_EMPTY_INSTR 	: return ((char*)"wfcCURVCOLL_EMPTY_INSTR");
		case 	wfcCURVCOLL_ADD_ONE_INSTR	: return ((char*)"wfcCURVCOLL_ADD_ONE_INSTR");
		case 	wfcCURVCOLL_TAN_INSTR	: return ((char*)"wfcCURVCOLL_TAN_INSTR");
		case 	wfcCURVCOLL_CURVE_INSTR	: return ((char*)"wfcCURVCOLL_CURVE_INSTR");
		case 	wfcCURVCOLL_SURF_INSTR	: return ((char*)"wfcCURVCOLL_SURF_INSTR");
		case 	wfcCURVCOLL_BNDRY_INSTR	: return ((char*)"wfcCURVCOLL_BNDRY_INSTR");
		case 	wfcCURVCOLL_LOG_OBJ_INSTR	: return ((char*)"wfcCURVCOLL_LOG_OBJ_INSTR");
		case 	wfcCURVCOLL_PART_INSTR	: return ((char*)"wfcCURVCOLL_PART_INSTR");
		case 	wfcCURVCOLL_FEATURE_INSTR	: return ((char*)"wfcCURVCOLL_FEATURE_INSTR");
		case 	wfcCURVCOLL_FROM_TO_INSTR	: return ((char*)"wfcCURVCOLL_FROM_TO_INSTR");
		case 	wfcCURVCOLL_EXCLUDE_ONE_INSTR	: return ((char*)"wfcCURVCOLL_EXCLUDE_ONE_INSTR");
		case 	wfcCURVCOLL_TRIM_INSTR	: return ((char*)"wfcCURVCOLL_TRIM_INSTR");
		case 	wfcCURVCOLL_EXTEND_INSTR	: return ((char*)"wfcCURVCOLL_EXTEND_INSTR");
		case 	wfcCURVCOLL_START_PNT_INSTR	: return ((char*)"wfcCURVCOLL_START_PNT_INSTR");
		case 	wfcCURVCOLL_ADD_TANGENT_INSTR	: return ((char*)"wfcCURVCOLL_ADD_TANGENT_INSTR");
		case 	wfcCURVCOLL_ADD_POINT_INSTR	: return ((char*)"wfcCURVCOLL_ADD_POINT_INSTR");
		case 	wfcCURVCOLL_OPEN_CLOSE_LOOP_INSTR	: return ((char*)"wfcCURVCOLL_OPEN_CLOSE_LOOP_INSTR");
		case 	wfcCURVCOLL_QUERY_INSTR	: return ((char*)"wfcCURVCOLL_QUERY_INSTR");
		case 	wfcCURVCOLL_RESERVED_INSTR	: return ((char*)"wfcCURVCOLL_RESERVED_INSTR");

	}

	char *str = (char *) malloc(100);
	sprintf (str, "%d", id);
	
	return (str);
}

char* otkxEnums::wfcSurfaceCollectionRefTypeGet (int id)
{
	switch (id)
	{
		case	wfcSURFCOLL_REF_SINGLE	: return ((char*)"wfcSURFCOLL_REF_SINGLE");
		case	wfcSURFCOLL_REF_SINGLE_EDGE	: return ((char*)"wfcSURFCOLL_REF_SINGLE_EDGE");
		case	wfcSURFCOLL_REF_SEED 	: return ((char*)"wfcSURFCOLL_REF_SEED");
		case	wfcSURFCOLL_REF_BND  	: return ((char*)"wfcSURFCOLL_REF_BND");
		case	wfcSURFCOLL_REF_SEED_EDGE	: return ((char*)"wfcSURFCOLL_REF_SEED_EDGE");
		case	wfcSURFCOLL_REF_NEIGHBOR 	: return ((char*)"wfcSURFCOLL_REF_NEIGHBOR");
		case	wfcSURFCOLL_REF_NEIGHBOR_EDGE	: return ((char*)"wfcSURFCOLL_REF_NEIGHBOR_EDGE");
		case	wfcSURFCOLL_REF_GENERIC	: return ((char*)"wfcSURFCOLL_REF_GENERIC");
	}

	char *str = (char *) malloc(100);
	sprintf (str, "%d", id);
	
	return (str);
}

char* otkxEnums::wfcSurfaceCollectionInstrTypeGet (int id)
{
	switch (id)
	{
		case	wfcSURFCOLL_SINGLE_SURF	: return ((char*)"wfcSURFCOLL_SINGLE_SURF");
		case	wfcSURFCOLL_SEED_N_BND	: return ((char*)"wfcSURFCOLL_SEED_N_BND");
		case	wfcSURFCOLL_QUILT_SRFS	: return ((char*)"wfcSURFCOLL_QUILT_SRFS");
		case	wfcSURFCOLL_ALL_SOLID_SRFS	: return ((char*)"wfcSURFCOLL_ALL_SOLID_SRFS");
		case	wfcSURFCOLL_NEIGHBOR	: return ((char*)"wfcSURFCOLL_NEIGHBOR");
		case	wfcSURFCOLL_NEIGHBOR_INC	: return ((char*)"wfcSURFCOLL_NEIGHBOR_INC");
		case	wfcSURFCOLL_ALL_QUILT_SRFS	: return ((char*)"wfcSURFCOLL_ALL_QUILT_SRFS");
		case	wfcSURFCOLL_ALL_MODEL_SRFS	: return ((char*)"wfcSURFCOLL_ALL_MODEL_SRFS");
		case	wfcSURFCOLL_LOGOBJ_SRFS	: return ((char*)"wfcSURFCOLL_LOGOBJ_SRFS");
		case	wfcSURFCOLL_DTM_PLN	: return ((char*)"wfcSURFCOLL_DTM_PLN");
		case	wfcSURFCOLL_DISALLOW_QLT	: return ((char*)"wfcSURFCOLL_DISALLOW_QLT");
		case	wfcSURFCOLL_DISALLOW_SLD	: return ((char*)"wfcSURFCOLL_DISALLOW_SLD");
		case	wfcSURFCOLL_DONT_MIX	: return ((char*)"wfcSURFCOLL_DONT_MIX");
		case	wfcSURFCOLL_SAME_SRF_LST	: return ((char*)"wfcSURFCOLL_SAME_SRF_LST");
		case	wfcSURFCOLL_USE_BACKUP	: return ((char*)"wfcSURFCOLL_USE_BACKUP");
		case	wfcSURFCOLL_DONT_BACKUP	: return ((char*)"wfcSURFCOLL_DONT_BACKUP");
		case	wfcSURFCOLL_DISALLOW_LOBJ	: return ((char*)"wfcSURFCOLL_DISALLOW_LOBJ");
		case	wfcSURFCOLL_ALLOW_DTM_PLN	: return ((char*)"wfcSURFCOLL_ALLOW_DTM_PLN");
		case	wfcSURFCOLL_SEED_N_BND_INC_BND	: return ((char*)"wfcSURFCOLL_SEED_N_BND_INC_BND");
	}

	char *str = (char *) malloc(100);
	sprintf (str, "%d", id);
	
	return (str);
}

char* otkxEnums::pfcSimpRepActionTypeGet (int id)
{
  switch(id)
  {
    case pfcSIMPREP_REVERSE: return  ((char*)"pfcSIMPREP_REVERSE");  
    case pfcSIMPREP_INCLUDE: return  ((char*)"pfcSIMPREP_INCLUDE");
    case pfcSIMPREP_EXCLUDE: return  ((char*)"pfcSIMPREP_EXCLUDE");
    case pfcSIMPREP_SUBSTITUTE: return  ((char*)"pfcSIMPREP_SUBSTITUTE");
    case pfcSIMPREP_GEOM: return  ((char*)"pfcSIMPREP_GEOM");  
    case pfcSIMPREP_GRAPHICS: return  ((char*)"pfcSIMPREP_GRAPHICS");
    case pfcSIMPREP_SYMB: return  ((char*)"pfcSIMPREP_SYMB");
    case pfcSIMPREP_NONE: return  ((char*)"pfcSIMPREP_NONE");
    case pfcSIMPREP_BOUNDBOX: return  ((char*)"pfcSIMPREP_BOUNDBOX");  
    case pfcSIMPREP_DEFENV: return  ((char*)"pfcSIMPREP_DEFENV");
    case pfcSIMPREP_LIGHT_GRAPH: return  ((char*)"pfcSIMPREP_LIGHT_GRAPH");
    case pfcSIMPREP_AUTO: return  ((char*)"pfcSIMPREP_AUTO");
    case pfcSimpRepActionType_nil: return  ((char*)"pfcSimpRepActionType_nil");

    default: return (char*)"*** Unknown enum in method pfcSimpRepActionTypeGet";  
  }
}

char* otkxEnums::wfcExplodedAnimationMoveTypeGet (int id)
{
  switch(id)
  {
    case wfcEXPLDANIM_MOVE_TRANSLATE: return  ((char*)"wfcEXPLDANIM_MOVE_TRANSLATE");  
    case wfcEXPLDANIM_MOVE_ROTATE: return  ((char*)"wfcEXPLDANIM_MOVE_ROTATE");
    case wfcExplodedAnimationMoveType_nil: return  ((char*)"wfcExplodedAnimationMoveType_nil");
    
    default: return (char*)"*** Unknown enum in method wfcExplodedAnimationMoveTypeGet";  
  
  }
}
