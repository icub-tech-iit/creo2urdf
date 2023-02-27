/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Utilites for Family Table.
  
*/

#include <pfcExceptions.h>
#include <pfcGlobal.h>
#include <wfcSolid.h>
#include <OTKXUtils.h> 
#include <fstream>

// Utility to write Family Table Row info
int OtkUtilFamilyTableRowInfo(wfcWFamilyTableRow_ptr FamTableRow, ofstream& infoFile, ofstream&  fp_out_familytable)
{
	try
	{
		xbool flat = FamTableRow->IsFlatState();
		infoFile << "\t++ Flat State: "<<(flat == xtrue ? "TRUE" : "FALSE")<<endl;

		xbool IsLocked = FamTableRow->GetIsLocked();
		infoFile << "\t++ Is Locked: "<<(IsLocked == xtrue ? "TRUE" : "FALSE")<<endl;

		wfcWModel_ptr ModelfromDisk = FamTableRow->GetModelFromDisk();
		infoFile << "\t++ Model from Disk Name: "<<ModelfromDisk->GetFullName()<<endl;

		xbool is_mod = FamTableRow->IsModifiable(xtrue);
		infoFile << "\t++ Table Row Modifiable: "<<(is_mod == xtrue ? "TRUE" : "FALSE")<<endl;

		wfcWModel_ptr ModelfromSession = FamTableRow->GetModelFromSession();
		infoFile << "\t++ Model from Session Name: "<<ModelfromSession->GetFullName()<<endl<<endl;

  } // try
  OTK_EXCEPTION_HANDLER(fp_out_familytable);
  return (0);
}

// Utility to show/write Family Table info
int OtkUtilFamilyTableInfo (wfcWSolid_ptr pFTModel, ofstream& infoFile, ofstream&  fp_out_familytable)
{
  try
  {	
	  pfcFamilyMember_ptr FMemb = pfcFamilyMember::cast(pFTModel);
	  		
	  pfcFamilyTableRows_ptr FTRows = FMemb->ListRows();
	
	  int NumberOfRows = FTRows->getarraysize();

	  if(NumberOfRows == 0)
	  {
		  infoFile << "++ Family table is not found"<<endl;
		  return (0);
	  }
	  infoFile << "++ Number of instance Rows: " <<NumberOfRows<<endl;

	  if(NumberOfRows > 0)
	  {
		  wfcWSolid_ptr WSolid = wfcWSolid::cast(pFTModel);
		  wfcWFamilyMember_ptr FamMemb = wfcWFamilyMember::cast(WSolid);

		  xbool is_mod_2 = FamMemb->IsModifiable(xtrue);
		  infoFile << "++ Family Member Modifiable: "<<(is_mod_2 == xtrue ? "TRUE" : "FALSE")<<endl;

		  for(int i=0; i<NumberOfRows; i++)
		  {
		    infoFile << "  ++ Instance: "<<(i+1) <<endl;

			pfcFamilyTableRow_ptr pThisRow = FTRows->get(i);
			xstring InstName = pThisRow->GetInstanceName();
			
			infoFile << "\t++ Instance Name: "<<InstName<<endl;
			wfcWFamilyTableRow_ptr wThisRow = wfcWFamilyTableRow::cast(pThisRow);
			
			OtkUtilFamilyTableRowInfo(wThisRow, infoFile, fp_out_familytable);
		  }
	  }
  } // try
  OTK_EXCEPTION_HANDLER(fp_out_familytable);
  return (0);
}

// Utility to Erase Family Table
int OtkUtilFamilyTableErase (wfcWSolid_ptr wSolid, ofstream&  fp_out_familytable)
{
  try
  {	
	wfcWFamilyMember_ptr FamMemb = wfcWFamilyMember::cast(wSolid);

	wfcFamilyTableStatus FTStatus = FamMemb->GetFamilyTableStatus();

	if(FTStatus == wfcFAMILY_TABLE_VALID_AND_NOT_EMPTY)
		FamMemb->EraseFamilyTable();
  } // try
  OTK_EXCEPTION_HANDLER(fp_out_familytable);
  return (0);
}
