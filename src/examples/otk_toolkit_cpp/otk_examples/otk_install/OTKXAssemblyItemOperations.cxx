/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
   
This example will demonstrate basic operations using 
Assembly Item/Flexible Components.

*/

#include <pfcSession.h>
#include <pfcModel.h>
#include <ciplib.h>
#include <pfcGlobal.h>
#include <wfcSolid.h>
#include <wfcPart.h>
#include <wfcAssembly.h>
#include <wfcComponentFeat.h>
#include <wfcFeature.h>
#include <pfcExceptions.h>
#include <OTKXUtils.h> 
#include <OTKXAssemblyItemUtils.h>


#include <fstream>

/* ---------------------------------------------------
    * ASSEMBLY STRUCTURE USED FOR THE EXAMPLE *
---------------------------------------------------
							---------		-------
							 FEAT ID 		 FEAT #
							---------		-------
FLEX_TOP_ASM.asm
|
|---ASM_RIGHT						    1  	 		   1
|---ASM_TOP						    3			   2
|---ASM_FRONT						    5			   3
|---ASM_DEF_CSYS					    7			   4
|
|===FLEX_PART2.prt					   39			   5
|
|===FLEX_SUB_ASM1.asm					   42			   6
	|
	|---ASM_RIGHT				    	    1			   1
	|---ASM_TOP					    3			   2
	|---ASM_FRONT					    5			   3
	|---ASM_DEF_CSYS				    7			   4
	|
	|===FLEX_PART1.prt				   39			   5
	|
	|===FLEX_SUB_ASM2.asm				   40			   6
		|
		|---ASM_RIGHT				    1			   1
		|---ASM_TOP				    3			   2
		|---ASM_FRONT				    5			   3
		|---ASM_DEF_CSYS			    7			   4
		|
		|===FLEX_PART1.prt			   39			   5
		|===FLEX_PART2.prt			   40			   5
			
---------------------------------------------------
    * ASSEMBLY STRUCTURE USED FOR THE EXAMPLE *
--------------------------------------------------- */


//***********************************************************
extern "C" wfcStatus otkAssemblyItemOperations ()
{
  ofstream fp_out_asmitem;
  
  try
    {			
      fp_out_asmitem.open("AssemblyItemExample.txt", ios::out);
      fp_out_asmitem << "- Assembly Item Example Example -" << endl;
      
      pfcSession_ptr Session = pfcGetCurrentSession ();
      
      /* Open top assembly FLEX_TOP_ASM.asm in Creo Session*/
      pfcModel_ptr CurrModel = Session->GetCurrentModel();
      
      pfcAssembly_ptr PAsm = pfcAssembly::cast(CurrModel);
      
      wfcWAssembly_ptr WAsm = wfcWAssembly::cast(CurrModel);
      
      pfcModelDescriptor_ptr Part_MDesc = pfcModelDescriptor::CreateFromFileName("flex_part2.prt");
      pfcModel_ptr PartOwner = Session->GetModelFromDescr(Part_MDesc);
      
      /* Populate component Ids for Component Path */
      xintsequence_ptr int_id_array = xintsequence::create();
      int_id_array->set(0, 40);
      
      /* Create Component Path with respect to top assembly*/
      pfcComponentPath_ptr AsmCompPath = pfcCreateComponentPath (PAsm, int_id_array);
      wfcWComponentPath_ptr WAsmCompPath = wfcWComponentPath::cast(AsmCompPath);
      
      wfcAssemblyItems_ptr AsmItemArray = wfcAssemblyItems::create();
      
      /* Populate dimension Ids (from flex_part2.prt) for AssemblyItem array */
      xintsequence_ptr dim_id_array = xintsequence::create();
      dim_id_array->set(0, 0);
      dim_id_array->set(1, 1);
      dim_id_array->set(2, 2);
      
      /* Populate AssemblyItem array from Dimensions */
      AsmItemArray = OTKXUtilPopulateAssemblyItemsDimensionArray(
	 AsmItemArray, WAsm, PartOwner, dim_id_array, WAsmCompPath, fp_out_asmitem);
      
      
      pfcModelDescriptor_ptr Asm_MDesc = pfcModelDescriptor::CreateFromFileName("flex_sub_asm1.asm");
      pfcModel_ptr AsmOwner = Session->GetModelFromDescr(Asm_MDesc);
      pfcSolid_ptr asmSolid = pfcSolid::cast(AsmOwner);
      
      /*Set Component Flexible using AssemblyItem Array*/
      wfcWComponentFeat_ptr WCFeat = OTKXUtilSetFlexibleComponent(asmSolid, 40, AsmItemArray, fp_out_asmitem);
      
      int_id_array->set(0, 40);
      int_id_array->set(1, 39);
      
      /* Create Component Path with respect to sub assembly */
      pfcAssembly_ptr PAsm_flex1 = pfcAssembly::cast(AsmOwner);
      pfcComponentPath_ptr AsmCompPath2 = pfcCreateComponentPath (PAsm_flex1, int_id_array);
      wfcWComponentPath_ptr WAsmCompPath2 = wfcWComponentPath::cast(AsmCompPath2);
      
      /* Create Flexible Model using AssemblyItem Array */
      pfcModel_ptr FlexModel = OTKXUtilCreateFlexibleModel(asmSolid, 40, WAsmCompPath2, AsmItemArray, fp_out_asmitem);
      
      fp_out_asmitem << "Unsetting Component Flexible..." << endl; 
      
      /* Unset component flexible */
      WCFeat->UnsetAsFlexible();
      
      xbool is_flex = WCFeat->IsFlexible();
      
      fp_out_asmitem << " Is Component Flexible ? : " << (is_flex ? "YES" : "NO") << endl;
      
      wfcAssemblyItems_ptr AsmItemArray2 = wfcAssemblyItems::create();
      
      /* Populate dimension Ids (from flex_part2.prt) for AssemblyItem array */
      xintsequence_ptr dim_id_array2 = xintsequence::create();
      dim_id_array2->set(0, 0);
      dim_id_array2->set(1, 1);
      
      /* Populate AssemblyItem array from Dimensions */
      AsmItemArray2 = OTKXUtilPopulateAssemblyItemsDimensionArray(
	  AsmItemArray2, WAsm, PartOwner, dim_id_array2, 0, fp_out_asmitem);
      
      /* Populate feature Ids (from flex_part2.prt) for AssemblyItem array */
      xintsequence_ptr feat_id_array = xintsequence::create();
      feat_id_array->set(0, 39);
      
      /* Populate AssemblyItem array from Features */
      AsmItemArray2 = OTKXUtilPopulateAssemblyItemsFeatureArray(
	AsmItemArray2, WAsm, PartOwner, feat_id_array, 0, fp_out_asmitem);
      
      pfcSolid_ptr pSolid = pfcSolid::cast(CurrModel);
      
      /*Create Predefined Flexibility Component using AssemblyItem Array*/
      OTKXUtilCreatePredefinedFlexibilityComponent(pSolid, 39, AsmItemArray2, PartOwner, fp_out_asmitem);		
      
      return wfcTK_NO_ERROR;
    }
  
  OTK_EXCEPTION_HANDLER(fp_out_asmitem);
  
  return wfcTK_GENERAL_ERROR;
  
}

