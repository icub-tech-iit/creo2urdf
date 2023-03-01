/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/

#include <uifcDefaultListeners.h>
#include <cipxx.h>
#include <pfcCommand.h>

/*
  Create and Activate Server Registry Dialog
*/
class OTKServerRegistryDialog : public pfcUICommandActionListener
{	
 public:
  static xstring OTK_SERVER_DIALOG;
  static xstring OTK_CLOSE_BUTTON;
  static xstring OTK_SERVER_TABLE;
  static xstring OTK_WS_TABLE;	
  
  void showDialog();
  void OnCommand();
};
/*
  Create and Initialize Server Table
*/
class OTKServerManager
{
 private:
  uifcTable_ptr serverTable;
  xstringsequence_ptr serverNames;
  xstringsequence_ptr serverStatus;
  xstringsequence_ptr serverIcons;	
  
 public:
  OTKServerManager();
  void populateServers();		
  
};
/*
 * Listener class for  Server Table 
 */
class OTKWorkspaceManager : public uifcDefaultTableListener
{
 private:
  uifcTable_ptr workspaceTable;
 public:		
  OTKWorkspaceManager();	
  void OnCellSelect(uifcTable_ptr handle);
};
/*
  Create and Initialize Server Table
*/
class OTKCacheManager
{
 public:
  OTKCacheManager();
};

class OTKCacheTools : public uifcDefaultPushButtonListener
{	
 public:
  void OnActivate(uifcPushButton_ptr handle);
};

class OTKDialogExit : public uifcDefaultPushButtonListener
{	
  public :
    void OnActivate(uifcPushButton_ptr handle);
};

class OTKServerCacheTab : public uifcDefaultTabListener
{
 public:
  void OnItemSelect(uifcTab_ptr handle);	
};


