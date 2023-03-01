/*
	Copyright (c) 2022 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.
*/



#include <OTKXUIFCDialog.h>
#include <OTKXUtils.h>
#include <pfcGlobal.h>
xstring OTKServerRegistryDialog::OTK_SERVER_DIALOG = "otkcx_server_registry";
xstring OTKServerRegistryDialog::OTK_CLOSE_BUTTON = "close_pb";
xstring OTKServerRegistryDialog::OTK_SERVER_TABLE = "server_table";
xstring OTKServerRegistryDialog::OTK_WS_TABLE = "workspace_table";
static ofstream uifcLog;

void OTKServerRegistryDialog::showDialog()
{
  try
    {
      /*
       * Create a dialog from resource file and fill details of the Server , Workspaces and Cache			  
       */			
      uifcCreateDialog(OTK_SERVER_DIALOG,OTK_SERVER_DIALOG);						
      OTKServerManager *serverManager = new OTKServerManager();
      serverManager->populateServers();
      
      new OTKCacheManager();
      
      /*
       * Add Action listeners for Tab and Close PushButton			 
       */
      uifcTab_ptr serverTab = uifcTabFind(OTK_SERVER_DIALOG, "server_cache_tab");
      OTKServerCacheTab *tabListener = new OTKServerCacheTab();
      serverTab->AddActionListener(tabListener);
      
      uifcPushButtonFind(OTK_SERVER_DIALOG, OTK_CLOSE_BUTTON)->AddActionListener(new OTKDialogExit());		
      /*
       * Display and Activate Dialog.
       * Please note that uifcActivateDialog() is a BLOCKING call here.
       * uifcExitDialog() will unblock this
       */
      uifcActivateDialog(OTK_SERVER_DIALOG);	
      uifcDestroyDialog(OTK_SERVER_DIALOG);		
    } 
  OTK_EXCEPTION_HANDLER(uifcLog);		
  
}
void OTKServerRegistryDialog::OnCommand()
{
  uifcLog.open("uifc.log",ios::out);
  showDialog();
  uifcLog.close();
}

OTKServerManager::OTKServerManager()
{		
  try
    {
      serverNames = xstringsequence::create();
      serverNames->append("<No Server>");
      serverNames->append("otk.ptc.com");
      serverNames->append("toolkit.ptc.com");
      serverNames->append("jlink.ptc.com");
      serverNames->append("weblink.ptc.com");
      serverNames->append("vbtoolkit.ptc.com");
      
      serverStatus = xstringsequence::create();
      serverStatus->append("online");
      serverStatus->append("offline");
      serverStatus->append("offline");
      serverStatus->append("offline");
      serverStatus->append("offline");
      serverStatus->append("offline");
      
      serverIcons = xstringsequence::create();
      serverIcons->append("otkcx_arrow");
      serverIcons->append("otkcx_server16x16");
      serverIcons->append("otkcx_server16x16");
      serverIcons->append("otkcx_server16x16");
      serverIcons->append("otkcx_server16x16");
      serverIcons->append("otkcx_server16x16");
      
      /*
       * Set Selection policies for row and cell selections of the table
       */
      serverTable = uifcTableFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG,OTKServerRegistryDialog::OTK_SERVER_TABLE);
      serverTable->AddActionListener(new OTKWorkspaceManager());
      serverTable->SetHeight(serverNames->getarraysize() + 1);
      serverTable->SetRowHighlight(true);
      serverTable->SetRowSelectionPolicy(uifcSELECTION_POLICY_EXTENDED);
      serverTable->SetCellSelectionPolicy(uifcSELECTION_POLICY_EXTENDED);
      serverTable->SetSelectionVisible(true);
    } 
  OTK_EXCEPTION_HANDLER(uifcLog);
  
}	
/*
 *  Populate server details in the table	 
 */
void OTKServerManager::populateServers()
{
  
  for(int ii=0;ii<serverNames->getarraysize();ii++)
    {			
      try
	{
	  /*
	   *  Create a row by the name of the Server and insert it in the table
	   */
	  uifcTableRow_ptr serverRow = uifcTableRowDefine(serverNames->get(ii));				
	  uifcRowPositionData_ptr rowPos = uifcRowPositionData::Create();			
	  rowPos->SetIndex(ii);			
	  serverTable->InsertRow(serverRow, rowPos);	
	  
	  /*
	   *  Locate server cell and fill it with server name
	   */			
	  uifcTableCell_ptr serverCell =  uifcTableCellFind(serverTable->GetDialog(), serverTable->GetComponent(), serverNames->get(ii), "server");
	  serverCell->SetText(serverNames->get(ii));
	  
	  /*
	   *  Define a label and set an icon image to it
	   * Insert that label to the table
	   * Locate the icon cell and set label as its component name
	   */			
	  uifcLabel_ptr iconLabel = uifcLabelDefine(serverRow->GetName() + "_label");
	  iconLabel->SetImage(serverIcons->get(ii));				
	  serverTable->InsertChild(uifcComponent::cast(iconLabel));				
	  
	  uifcTableCell_ptr iconCell =  uifcTableCellFind(serverTable->GetDialog(), serverTable->GetComponent(), serverNames->get(ii), "image");
	  iconCell->SetComponentName(serverRow->GetName() + "_label");				
	  
	  /*
	   *	Locate server status cell and fill it with server status
	   */
	  uifcTableCell_ptr statusCell =  uifcTableCellFind(serverTable->GetDialog(), serverTable->GetComponent(), serverNames->get(ii), "status");
	  statusCell->SetText(serverStatus->get(ii));
	  
	} 
      OTK_EXCEPTION_HANDLER(uifcLog);				
    }
  
  /*		   
   * Set first server row as selected 
   */
  try
    {
      xstringsequence_ptr selectedRow = xstringsequence::create();
      selectedRow->append(serverNames->get(0));
      selectedRow->append("server");		
      
      serverTable->SetSelectedCellNameArray(selectedRow);			
    } 
  OTK_EXCEPTION_HANDLER(uifcLog);			
  
  
}	


void OTKServerCacheTab::OnItemSelect(uifcTab_ptr handle)
{
  /*		 
   * A standard message will be displayed when switching tabs from Server to Cache or vice versa 
   */		
  if(handle->GetSelectedItemNameArray()->get(0).Match("server_lay"))			
    pfcGetCurrentSession()->UIDisplayMessage("TabSwitch.txt","Server Tab",NULL);
  else if (handle->GetSelectedItemNameArray()->get(0).Match("cache_lay"))
    pfcGetCurrentSession()->UIDisplayMessage("TabSwitch.txt","Cache Tab",NULL);
  
}

void OTKDialogExit::OnActivate(uifcPushButton_ptr handle)
{
  uifcExitDialog(handle->GetDialog(),0);
}

void OTKCacheTools::OnActivate(uifcPushButton_ptr handle)
{
  /*
   * On activating "Clear" button value in the cache space is reset to zero
   */		
  if(handle->GetComponent().Match("clear_cache_pb"))
    {	
      uifcInputPanel_ptr cacheIp = uifcInputPanelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "total_cache_ip");
      cacheIp->SetIntegerValue(0);
    }
  
  /*
   * On activating "Cache Tools" button Tab will be switched to Servers
   */
  else if(handle->GetComponent().Match("cache_tools_pb"))
    {			
      uifcTab_ptr serverCacheTab = uifcTabFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG ,"server_cache_tab");
      xstringsequence_ptr selNames = xstringsequence::create();
      selNames->append("server_lay");
      serverCacheTab->SetSelectedItemNameArray(selNames);			
      
    }
}

/*
 * Set values into label (Server Location , Cache Space , Disk Space)
 * 
 * Add Action Listener for PushButtons "Clear" and "Cache Tools"	 * 
 * 
 */
OTKCacheManager::OTKCacheManager()
{
  try
    {
      uifcLabel_ptr cacheLoc = uifcLabelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "location_label");
      cacheLoc->SetText("Location : //home//user//.wf//.cache2");
      
      uifcInputPanel_ptr wsIp = uifcInputPanelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "ws_space_ip");
      wsIp->SetIntegerValue(400);
      
      uifcInputPanel_ptr cacheIp = uifcInputPanelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "total_cache_ip");
      cacheIp->SetIntegerValue(12);
      
      uifcInputPanel_ptr diskIP = uifcInputPanelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "disk_space_ip");
      diskIP->SetIntegerValue(13819);
      
      OTKCacheTools *listener = new OTKCacheTools();
      uifcPushButton_ptr clearPB = uifcPushButtonFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "cache_tools_pb");
      clearPB->AddActionListener(listener);
      
      uifcPushButton_ptr cacheToolsPB = uifcPushButtonFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "clear_cache_pb");
      cacheToolsPB->AddActionListener(listener);
      
    }
  OTK_EXCEPTION_HANDLER(uifcLog);	
  
  
}
OTKWorkspaceManager::OTKWorkspaceManager()
{			
  try
    {
      workspaceTable = uifcTableFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG,OTKServerRegistryDialog::OTK_WS_TABLE);
      workspaceTable->SetFontStyle(uifcFONT_STYLE_BOLD);
      workspaceTable->SetFontSize(10);
      
      uifcTableRow_ptr row = uifcTableRowDefine("workspace_row");
      uifcRowPositionData_ptr rowPos = uifcRowPositionData::Create();
      rowPos->SetIndex(-1);				 
      workspaceTable->InsertRow(row, rowPos);			 
      
    }
  OTK_EXCEPTION_HANDLER(uifcLog);	
  
}		

/*		 
 * Override method OnCellSelect to handle event of selecting a cell of Server Table
 * On each selection in server table fill workspace table with workspace name and context
 * On each selection in server table fill information of server name and location in Description layout
 */		
void OTKWorkspaceManager::OnCellSelect(uifcTable_ptr handle) 
{
  try
    {
      xstringsequence_ptr selNames = handle->GetSelectedCellNameArray();				
      
      uifcTableCell_ptr wsCell =  uifcTableCellFind(workspaceTable->GetDialog(), workspaceTable->GetComponent(),"workspace_row", "workspace");
      wsCell->SetText(selNames->get(0) + " Active WorkSpace");			
      
      uifcTableCell_ptr contextCell =  uifcTableCellFind(workspaceTable->GetDialog(), workspaceTable->GetComponent(),"workspace_row", "context");
      contextCell->SetText("Windchill PDM");			
      
      uifcLabel_ptr serverName = uifcLabelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "server_name_lb");
      serverName->SetText("Server Name : " + selNames->get(0));			
      
      uifcLabel_ptr serverLoc = uifcLabelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "location_lb");
      serverLoc->SetText("Location : https\\\\ " + selNames->get(0) + ": 8080");		
      
      uifcLabel_ptr wsName = uifcLabelFind(OTKServerRegistryDialog::OTK_SERVER_DIALOG, "workspace_lb");
      wsName->SetText("Workspace Name : " + selNames->get(0) + " Active WorkSpace");		
      
    }
  OTK_EXCEPTION_HANDLER(uifcLog);	
  
  
}	
