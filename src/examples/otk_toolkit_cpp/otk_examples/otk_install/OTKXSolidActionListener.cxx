/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/


#include <pfcSession.h>
#include <pfcSolid.h>
#include <OTKXUtils.h>

#ifndef OTK_USE_TKOUT
#define tkout cout
#else
extern std::ofstream tkout;
#endif

class AutoNameListener : pfcDefaultSolidActionListener {

public: 
  AutoNameListener ()  {  }

  void OnAfterFeatureCreate(pfcSolid_ptr Solid, pfcFeature_ptr Feat)
  {
	ofstream exception_info;
	exception_info.open("exception_info.inf",ios::out);

    try {
      if (Feat->GetName() == xstringnil || Feat->GetName().IsEmpty() )
      {
        pfcSession_ptr Session = pfcGetCurrentSession ();
        Session->UIDisplayMessage("feat_ops.txt", "USER Submit feature name", 0);

        xstring newName = Session->UIReadStringMessage();
        Feat->SetName(newName);
      }
      else 
        cout << Feat->GetName() << endl;
    }
    OTK_EXCEPTION_HANDLER(exception_info);
  }

};
