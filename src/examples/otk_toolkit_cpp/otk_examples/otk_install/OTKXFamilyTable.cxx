/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/* 
 
 Example for Family Table.
  
*/


#include <pfcSession.h>
#include <pfcModel.h>
#include <pfcGlobal.h>
#include <pfcFeature.h>
#include <pfcSolid.h>
#include <pfcUI.h>

#include <ciplib.h>

#include <wfcSolid.h>
#include <wfcPart.h>
#include <wfcAssembly.h>
#include <pfcExceptions.h>
#include <wfcGlobal.h> 
#include <OTKXUtils.h> 

#include <fstream>



// Family Table Info
extern "C" wfcStatus otkFamilyTableInfo ()
{
	ofstream fp_out_familytable;
	try
	{
		xstring line;
		xstring InfoFile_name;
        
		fp_out_familytable.open("FamilyTable.txt", ios::out);
		fp_out_familytable << "- Family Table Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		if(WSolid != NULL)
		{
			InfoFile_name = "FamilyTable_info.txt";
			ofstream fp_InfoFile;

			fp_InfoFile.open((const char *)InfoFile_name, ios::out);
			OtkUtilFamilyTableInfo(WSolid, fp_InfoFile, fp_out_familytable);
			fp_InfoFile.close();

			// Code to display info file into Pro/E browser window
			pfcWindow_ptr current_win = Session->GetCurrentWindow ();
			line = "file://";
			line += Session->GetCurrentDirectory();
			line += InfoFile_name;
			current_win->SetURL(line);
		} // try
		fp_out_familytable.close();
		return wfcTK_NO_ERROR;
	}
	OTK_EXCEPTION_HANDLER(fp_out_familytable);
	return wfcTK_NO_ERROR;
}

// Family Table Erase
extern "C" wfcStatus otkFamilyTableErase()
{
  ofstream fp_out_familytable;
  try
  {
		fp_out_familytable.open("FamilyTable.txt", ios::out);
		fp_out_familytable << "- Family Table Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		OtkUtilFamilyTableErase(WSolid, fp_out_familytable);

		fp_out_familytable.close();
  } // try
  OTK_EXCEPTION_HANDLER(fp_out_familytable);
  return wfcTK_NO_ERROR;
}

// Family Table Show
extern "C" wfcStatus otkFamilyTableShow ()
{
	ofstream fp_out_familytable;
	try
	{
		fp_out_familytable.open("FamilyTable.txt", ios::out);
		fp_out_familytable << "- Family Table Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcWFamilyMember_ptr FamMemb = wfcWFamilyMember::cast(WSolid);

		FamMemb->ShowFamilyTable();
		fp_out_familytable.close();
	} // try
	OTK_EXCEPTION_HANDLER(fp_out_familytable);
	return wfcTK_NO_ERROR;
}

// Family Table Edit
extern "C" wfcStatus otkFamilyTableEdit ()
{
	ofstream fp_out_familytable;
	try
	{
		fp_out_familytable.open("FamilyTable.txt", ios::out);
		fp_out_familytable << "- Layer State Example -" << endl;

		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcWFamilyMember_ptr FamMemb = wfcWFamilyMember::cast(WSolid);

		FamMemb->EditFamilyTable();
		fp_out_familytable.close();
	}
	OTK_EXCEPTION_HANDLER(fp_out_familytable);
	return wfcTK_NO_ERROR;
}


// Family Table Single Row select
extern "C" wfcStatus otkFamilyTableRowSelect ()
{
	ofstream fp_out_familytable;
	ofstream info_file;
	fp_out_familytable.open("FamilyTableRowSelect.txt", ios::out);
	fp_out_familytable << "- Layer State Example -" << endl;
	try
	{
		pfcSession_ptr Session = pfcGetCurrentSession ();
		wfcWSolid_ptr WSolid = wfcWSolid::cast(Session->GetCurrentModel());

		wfcWFamilyMember_ptr FamMemb = wfcWFamilyMember::cast(WSolid);

		pfcFamilyTableRows_ptr FTRows = FamMemb->ListRows();
		int NumberOfRows = FTRows->getarraysize();

		if(NumberOfRows <= 0) // Family table is not found
			return wfcTK_NO_ERROR;

		// Through UI select 1 for for single row selection and 2 for multiple
		// row selection.

		Session->UIDisplayMessage("family_table.txt", "USER Enter index for row selection 1:Single Row, 2:Multiple Row", 0);
		int SelectedCombStateNo = Session->UIReadIntMessage(1, 2);
		if(SelectedCombStateNo == 1)
		{
			info_file.open("FamilyTableSingleRowInfo.inf", ios::out);
			info_file << "- FamilyTable Single Row info:" << endl;

			wfcWFamilyTableRows_ptr SelectedRows = FamMemb->SelectRows(xfalse);
			if(SelectedRows != NULL)
			{
				OtkUtilFamilyTableRowInfo(SelectedRows->get(0), info_file, fp_out_familytable);
			}
			info_file.close();
		}

		else if(SelectedCombStateNo == 2)
		{
			info_file.open("FamilyTableMultipleRowInfo.inf", ios::out);
			info_file << "- FamilyTable Multiple Rows info:" << endl;

			wfcWFamilyTableRows_ptr SelectedRows = FamMemb->SelectRows(xtrue);
			if(SelectedRows != NULL)
			{
				int NumberOfSelectedRows = SelectedRows->getarraysize();
				for(int i = 0; i < NumberOfSelectedRows; i++)
				{
					info_file << "-  Row Number: "<<(i+1)<< endl;
					OtkUtilFamilyTableRowInfo(SelectedRows->get(i), info_file, fp_out_familytable);
				}
			}
			info_file.close();
		}
		fp_out_familytable.close();
		
	}
	OTK_EXCEPTION_HANDLER(fp_out_familytable);
	return wfcTK_NO_ERROR;
}
