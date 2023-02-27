/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#include <uifcDefaultListeners.h>
#include <cipxx.h>
#include <pfcCommand.h>


/*
	Create Naked window in empty Dialog for SPR 13256762
*/

class OTKNakedWinCreate : public pfcUICommandActionListener
{
public:
	static xstring NAKEDWINEXAMPLE_DIALOG;
	static xstring NAKEDWINEXAMPLE_MYOK;
	static xstring NAKEDWINEXAMPLE_MYCANCEL;
	static xstring NAKEDWINEXAMPLE_DRAWAREA;
	static xstring NAKEDWINEXAMPLE_NAKEDWINDOW;
	void OnCommand();
};

