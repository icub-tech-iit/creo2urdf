/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#ifndef OTKXGRAPHICS_H
#define OTKXGRAPHICS_H



#include <pfcGlobal.h>
#include <wfcSession.h>
#include <pfcExceptions.h>


class LightingExamples : virtual public pfcUICommandActionListener
{
 public:	
  void OnCommand();
  
  void AddLight(wfcWWindow_ptr windowObject,wfcLightType type);
  
  wfcLightSourceInstructions_ptr GetExistingLights(wfcWWindow_ptr windowObject,xbool SkipReadonlyLights);
  
  void RemoveLight(wfcWWindow_ptr windowObject);
  
  void DisplayLightInformation(wfcWWindow_ptr windowObject);
  
  LightingExamples(wfcLightType inLightType,xbool inRemoveLight = xfalse,xbool inGetLightInformation =xfalse) 
    {
      mLightType = inLightType; 
      mRemoveLight = inRemoveLight;
      mGetLightInformation = inGetLightInformation;
    }
  
 private:
  wfcLightType mLightType;
  xbool mRemoveLight;
  xbool mGetLightInformation;
  
};

#endif /* OTKXGRAPHICS_H */
