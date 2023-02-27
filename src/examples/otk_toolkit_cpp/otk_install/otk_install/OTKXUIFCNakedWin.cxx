/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


/*
Click on "OTK->UI Examples->Naked Window Create". In opened dialog, click on OK button.
 1. "NakedWindow1" is existing naked window in the dialog. 'OK' buttons action changes
     its background to Blue.
 2.  Further its actions functioncreates new  "NakedWindow2" .
     Its background colour is set to Yellow.

*/

#include <OTKXUIFCNakedWin.h>
#include <OTKXUtils.h>
#include <pfcGlobal.h>
#include <uifcNakedWindow.h>


static ofstream uifcNakedWinLog;
class myOkPushButtonListenerNakedWin : public uifcDefaultPushButtonListener
{
public:
	void OnActivate(uifcPushButton_ptr handle);
};


class myCancelPushButtonListenerNakedWin : public uifcDefaultPushButtonListener
{
public:
	void OnActivate(uifcPushButton_ptr handle);
};

class NAKEDWINEXAMPLEDialogListenerNakedWin : public uifcDefaultDialogListener
{
public:
	void OnClose(uifcDialog_ptr handle);
};


xstring OTKNakedWinCreate::NAKEDWINEXAMPLE_DIALOG = "emptyDialog";
xstring OTKNakedWinCreate::NAKEDWINEXAMPLE_MYOK = "myOk";
xstring OTKNakedWinCreate::NAKEDWINEXAMPLE_MYCANCEL = "myCancel";
xstring OTKNakedWinCreate::NAKEDWINEXAMPLE_DRAWAREA = "myDrawArea";
xstring OTKNakedWinCreate::NAKEDWINEXAMPLE_NAKEDWINDOW = "myNakedWindow";


void OTKNakedWinCreate::OnCommand()
{
	uifcNakedWinLog.open("uifc.log", ios::out);
	try
	{
		xint status;
		status = uifcCreateDialog(NAKEDWINEXAMPLE_DIALOG, NAKEDWINEXAMPLE_DIALOG);

		if (status == 0)
		{
			uifcDialog_ptr NAKEDWINEXAMPLE = uifcDialogFind(NAKEDWINEXAMPLE_DIALOG, NAKEDWINEXAMPLE_DIALOG);

			NAKEDWINEXAMPLEDialogListenerNakedWin* NAKEDWINEXAMPLELis = new NAKEDWINEXAMPLEDialogListenerNakedWin();

			NAKEDWINEXAMPLE->AddActionListener(NAKEDWINEXAMPLELis);

			uifcPushButton_ptr myok = uifcPushButtonFind(NAKEDWINEXAMPLE_DIALOG, NAKEDWINEXAMPLE_MYOK);

			myOkPushButtonListenerNakedWin* myokLis = new myOkPushButtonListenerNakedWin();

			myok->AddActionListener(myokLis);

			uifcPushButton_ptr mycancel = uifcPushButtonFind(NAKEDWINEXAMPLE_DIALOG, NAKEDWINEXAMPLE_MYCANCEL);

			myCancelPushButtonListenerNakedWin* mycancelLis = new myCancelPushButtonListenerNakedWin;

			mycancel->AddActionListener(mycancelLis);

			status = uifcActivateDialog(NAKEDWINEXAMPLE_DIALOG);

			status = uifcDestroyDialog(NAKEDWINEXAMPLE_DIALOG);

			delete NAKEDWINEXAMPLELis;
			delete myokLis;
			delete mycancelLis;
		}

	}
	OTK_EXCEPTION_HANDLER(uifcNakedWinLog);

	uifcNakedWinLog.close();
}


void NAKEDWINEXAMPLEDialogListenerNakedWin::OnClose(uifcDialog_ptr handle)
{
	try
	{
		xint status;
		// sending PRO_TK_ABORT if user clicks close
		// this status is sent to uifcActivateDialog
		status = uifcExitDialog(OTKNakedWinCreate::NAKEDWINEXAMPLE_DIALOG, -25);
	}
	OTK_EXCEPTION_HANDLER(uifcNakedWinLog)
}

void myCancelPushButtonListenerNakedWin::OnActivate(uifcPushButton_ptr handle)
{
	try
	{
		xint status;
		//xint ret; sending  PRO_TK_E_FOUND if user clicks cancel
		//this status is sent to uifcActivateDialog
		status = uifcExitDialog(OTKNakedWinCreate::NAKEDWINEXAMPLE_DIALOG, -5);
	}
	OTK_EXCEPTION_HANDLER(uifcNakedWinLog);
}



// 1. "NakedWindow1" is existing naked window in the dialog. Following code
//     changes its colour to Blue.
// 2.  "NakedWindow2" is created in following code and is added in the dialog. 
//     Its background colour is set to Yellow.
void myOkPushButtonListenerNakedWin::OnActivate(uifcPushButton_ptr handle)
{
	try
	{
		uifcColor_ptr backGroundColor1 = uifcColor::Create(0, 0, 255);

		uifcNakedWindow_ptr nakedWindow1 = uifcNakedWindowFind(OTKNakedWinCreate::NAKEDWINEXAMPLE_DIALOG, "NakedWindow1");
		nakedWindow1->SetBackgroundColor(backGroundColor1);


		uifcColor_ptr backGroundColor2 = uifcColor::Create(254, 221, 0);
		uifcNakedWindow_ptr nakedWindow2 = uifcNakedWindowDefine("NakedWindow2");

		uifcDialog_ptr dlg = uifcDialogFind(OTKNakedWinCreate::NAKEDWINEXAMPLE_DIALOG, OTKNakedWinCreate::NAKEDWINEXAMPLE_DIALOG);

		uifcGridData_ptr gdData = uifcGridData::Create(1, 1);
		dlg->InsertChild(uifcComponent::cast(nakedWindow2), gdData);
		nakedWindow2->SetBackgroundColor(backGroundColor2);
	}
	OTK_EXCEPTION_HANDLER(uifcNakedWinLog);
}

