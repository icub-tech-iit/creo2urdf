/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/



#include <OTKXGraphics.h>
#include <OTKXUtils.h>

static  wfcWSession_ptr wSession;
static ofstream lightsException;

/**********************************************************************
Overrides onCommand method from pfcUICommandActionListener depending upon 
private members in the LightingExamples class 
Executes any one of the following operation
  1.Add Light
  2.Remove Light
  3. Display Light Information

**********************************************************************/
void LightingExamples::OnCommand()
{	
	lightsException.open("Exception.inf",ios::out);
  try
    {	  
	  wSession = wfcWSession::cast(pfcGetCurrentSession()); 
	  
	  wfcWWindow_ptr window = wfcWWindow::cast(wSession->GetCurrentWindow());
	  
	  if(mRemoveLight == xtrue)
	    RemoveLight(window);
	  
	  else if(mGetLightInformation == xtrue)
	    DisplayLightInformation(window);
	  
	  else
	    AddLight(window,mLightType);	  
	  
    }
	OTK_EXCEPTION_HANDLER(lightsException);
   
    return;
}

/**********************************************************************
Adds a light source to the current window.
Method takes all inputs required for a light source.
Following type of lights are supported
  1.Point light (equivalent to lightbulb in Creo)
  2.Direction light (equivalent to distant in Creo)
  3.Spot light (equivalent to spot in Creo)

**********************************************************************/

void LightingExamples::AddLight(wfcWWindow_ptr windowObject,wfcLightType type)
{
  
  try
    {
      wfcLightSourceInstructions_ptr newLightInstrs = GetExistingLights(windowObject,true);
      
      /*
	NOTE 1 : User can add maximum 5 lights in any window
	SetLightInstructions() will throws pfcXToolkitUnsupported exception 
	if number of lights are greater than 5
	
      */
      
      if(newLightInstrs->getarraysize() >= 5)
	{
	  wSession->UIDisplayMessage("lights_example_msg.txt","USER Too many lights",NULL);
	  return;
	}
      
      /*
	NOTE 2 : Method uses a point to determine source & aim location
	of light source.
	
      */
      
      
      wSession->UIDisplayMessage("lights_example_msg.txt","USER Select light position",NULL);
      pfcSelectionOptions_ptr refSelopts = pfcSelectionOptions::Create("point");
      refSelopts->SetMaxNumSels(1);
      
      pfcSelections_ptr refSels = wSession->Select(refSelopts);
      
      pfcPoint3D_ptr position = refSels->get(0)->GetPoint();   
      
      refSels->clear();
      
      wSession->UIDisplayMessage("lights_example_msg.txt","USER Select aim point",NULL);
      refSels = wSession->Select(refSelopts);
      
      pfcPoint3D_ptr directionPnt = refSels->get(0)->GetPoint();	   
      
      wSession->UIDisplayMessage("lights_example_msg.txt","USER RGB Red value",NULL);      
      xreal red = wSession->UIReadRealMessage(0.00,1.00);
      
      wSession->UIDisplayMessage("lights_example_msg.txt","USER RGB Green value",NULL);      
      xreal green = wSession->UIReadRealMessage(0.00,1.00);
      
      wSession->UIDisplayMessage("lights_example_msg.txt","USER RGB Blue value",NULL);
      xreal blue = wSession->UIReadRealMessage(0.00,1.00);
      
      wSession->UIDisplayMessage("lights_example_msg.txt","USER Light name",NULL);
      xstring lightName = wSession->UIReadStringMessage(false);
      
      pfcColorRGB_ptr color = pfcColorRGB::Create(red,green,blue);
      
      wfcLightSourceInstruction_ptr spotLightinstr = 
	wfcLightSourceInstruction::Create(lightName,type,color,true);
      
      spotLightinstr->SetPosition(position);
      
      pfcVector3D_ptr direction = wfcCreateVector3D(directionPnt->get(0),directionPnt->get(1),directionPnt->get(2));
      spotLightinstr->SetDirection(direction);
      
      /*
	NOTE 3 : Spread angle is required only for spot light
      */
      
      if(type == wfcLIGHT_SPOT)
	{
	  
	  wSession->UIDisplayMessage("lights_example_msg.txt","USER Spot Light Angle",NULL);
	  xreal spotAngle = wSession->UIReadRealMessage(0,180);
	  
	  spotLightinstr->SetSpreadAngle(spotAngle);	   
	}
	    
      wSession->UIDisplayMessage("lights_example_msg.txt","USER Activate Light",NULL);
      xint activateIp = wSession->UIReadIntMessage(0,1);

	  activateIp ? (spotLightinstr->SetIsActive(xtrue)) : (spotLightinstr->SetIsActive(xfalse));
      
      newLightInstrs->append(spotLightinstr);
      
      windowObject->SetLightInstructions(newLightInstrs);
      
      /*
	NOTE 4 : A window repaint or refresh is required to render an object after lights have been set
      */
      
      windowObject->Refit();
      
    }
	OTK_EXCEPTION_HANDLER(lightsException);
}

/**********************************************************************
Get all existing lights in a window.
Method uses boolean SkipReadonlyLights to determine whether to collect readonly lights or not

**********************************************************************/

wfcLightSourceInstructions_ptr LightingExamples::GetExistingLights(wfcWWindow_ptr windowObject,xbool SkipReadonlyLights)
{
  wfcLightSourceInstructions_ptr lightInstrs = windowObject->GetLightInstructions();	
  
  wfcLightSourceInstructions_ptr newLightInstrs = wfcLightSourceInstructions::create();
  
  for(int ii=0;ii<lightInstrs->getarraysize();ii++)
    {
      if(SkipReadonlyLights == xtrue)
	{
	  if(lightInstrs->get(ii)->GetType() == wfcLIGHT_AMBIENT || lightInstrs->get(ii)->GetType() == wfcLIGHT_HDRI)
	    continue;
	  
	  if(lightInstrs->get(ii)->GetType() == wfcLIGHT_DIRECTION && lightInstrs->get(ii)->GetName().Match("default distant_*") )
	    continue;
	}
      
      newLightInstrs->append(lightInstrs->get(ii));
      
    }
  
  return (newLightInstrs);
}


/**********************************************************************
Removes a light from window.
It asks user to enter name of the light to be removed.

**********************************************************************/

void LightingExamples::RemoveLight(wfcWWindow_ptr windowObject)
{
  
  try
    {
      wSession->UIDisplayMessage("lights_example_msg.txt","USER Light name",NULL);
      xstring lightName = wSession->UIReadStringMessage(false);
      
      
      /*
	NOTE 5: Two default lights (distant1 and distant2) will always append some string
	to its end.To remove the strings use xstring::Match with wildcard
      */
      
      
      if(lightName.Match("distant1") || lightName.Match("distant2"))
	lightName.operator+=("_*");
      
      wfcLightSourceInstructions_ptr lights = GetExistingLights(windowObject,xtrue);
      
      for(int ii=0;ii<lights->getarraysize();ii++)
	{
	  if(lights->get(ii)->GetName().Match(lightName) )
	    {
	      lights->removerange(ii,ii+1);
	      break;
	    }
	}
      
      /*
	NOTE 6: SetLightInstructions() will always remove all (non readonly)
	lights & add lights passed to it. So remove desired light from a sequence 
	and set that light sequence again into window
	
      */
      
      windowObject->SetLightInstructions(lights);
      
    }
	OTK_EXCEPTION_HANDLER(lightsException);
}

/**********************************************************************
Dump information of the light into a file.
Displays the file using DisplayInformationWindow()
**********************************************************************/

void LightingExamples::DisplayLightInformation(wfcWWindow_ptr windowObject)
{
  fstream lightFile;	
  
  wfcLightSourceInstructions_ptr lights = GetExistingLights(windowObject,xfalse);
  
  lightFile.open("Lights.inf",ios::out);
  
  for(int ii=0;ii<lights->getarraysize();ii++)
    {
      wfcLightSourceInstruction_ptr light = lights->get(ii);
      
      lightFile<<"Name of the light "<<light->GetName()<<endl;
      lightFile<<"Type of the light "<<light->GetType()<<endl;
      
      lightFile<<"Position of source {"<<light->GetPosition()->get(0)<<"," 
	       <<light->GetPosition()->get(1)<<","<<light->GetPosition()->get(2)<<"}\n";
      
      lightFile<<"Aim point {"<<light->GetDirection()->get(0)<<","
	       <<light->GetDirection()->get(1)<<","<<light->GetDirection()->get(2)<<"}\n";
      
      lightFile<<"RGB Color is {" <<light->GetColor()->GetRed()<<","
	       <<light->GetColor()->GetBlue()<<","<<light->GetColor()->GetGreen()<<"}\n";
      
      if(light->GetType() == wfcLIGHT_SPOT)
	lightFile<<"Spread Angle is "<<light->GetSpreadAngle()<<endl;

    lightFile<<" Light is "<<(light->GetIsActive() ? "active" : "NOT active")<<endl;
      
      lightFile<<endl;
      
    }
  
  wSession->DisplayInformationWindow("./Lights.inf",0,0,0,0);
}
