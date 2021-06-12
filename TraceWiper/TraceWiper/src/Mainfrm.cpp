////////////////////////////////////////////////////
// Mainfrm.cpp

#include "stdafx.h"
#include "resource.h"
#include "ContainerApp.h"
#include "mainfrm.h"
#include "nodesetdialog.h"
#include "authsetdialog.h"
#include "../../platform/sysGlobal.h"
#include "softsetdialog.h"
#include "SecretEraserdialog.h"

#include "resrc1.h"
#include "../../platform/RegisterOps.h"

//extern bool CrnGetUSBDiskID(char cDiskID, LPSTR lpPID);
extern bool LocalAuth();
extern int Client(char *server, char *user, char *password, char *secret);
extern bool LocalAuth();
// Definitions for the CMainFrame class
extern bool SaveConfig();
CString CMainFrame::m_Server;
CString CMainFrame::m_Secret;
CString CMainFrame::m_User;
CString CMainFrame::m_Password;


CMainFrame::CMainFrame()
{
	// Constructor for CMainFrame. Its called after CFrame's constructor

	m_Server = CString("");
	m_Secret = CString("");
	m_User = CString("");
	m_Password = CString("");

	//Set m_DockView as the view window of the frame
	SetView(m_DockView);

	// Set the registry key name, and load the initial window position
	// Use a registry key name like "CompanyName\\Application"
	LoadRegistrySettings(_T("Win32++\\Container Docking"));
}

CMainFrame::~CMainFrame()
{
	// Destructor for CMainFrame.
	RegisterOps::getInstance()->RegUnLoad(HKEY_LOCAL_MACHINE);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	bool bAuthResult = false;
	UNREFERENCED_PARAMETER(lParam);

	// OnCommand responds to menu and and toolbar input
	switch(LOWORD(wParam))
	{
	case IDM_FILE_EXIT:
		// End the application
		::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		return TRUE;
	case IDM_DOCK_DEFAULT:
		m_DockView.CloseAllDockers();
		LoadDefaultDockers();
		return TRUE;
	case IDM_DOCK_CLOSEALL:
		m_DockView.CloseAllDockers();
		return TRUE;
	case IDW_VIEW_STATUSBAR:
		OnViewStatusbar();
		return TRUE;
	case IDW_VIEW_TOOLBAR:
		OnViewToolbar();
		return TRUE;
	case IDM_HELP_ABOUT:
		// Display the help dialog
		OnHelp();
		return TRUE;
	case ID_Menu://Start
#ifndef _DEBUG
		if (LocalAuth() == true)
		{
			bAuthResult = true;
		}
		else if (Client(m_Server.GetBuffer(), m_User.GetBuffer(), m_Password.GetBuffer(), m_Secret.GetBuffer()))
		{
			bAuthResult = true;
		}
		else
		{
			bAuthResult = false;
		}
#else
		bAuthResult = true;
#endif
		if (bAuthResult == true)
			((EraseNode*)(pRoot->getNodeChilds().at(0)))->erase();
		else
			::MessageBox(m_hWnd, "鉴权失败，请检查您的系统设置！", "鉴权失败，请检查您的系统设置！", MB_OK);

		return TRUE; 
		
	case ID_ERASER_STOP://Stop
		pRoot->eraserEnd();

		Sleep(1000);
		pRoot->eraserDestroy();
		
		return TRUE; 
		
	case ID_AUTH_SETTING:
		{
			CAuthSetDlg* m_pAboutDialog;
			m_pAboutDialog = new CAuthSetDlg(IDD_DIALOG_AUTH, m_hWnd, m_Server, m_Secret, m_User, m_Password);
			m_pAboutDialog->DoModal();
			
			m_Server = m_pAboutDialog->m_Server;
			m_Secret = m_pAboutDialog->m_Secret;
			m_User = m_pAboutDialog->m_User;
			m_Password = m_pAboutDialog->m_Password;
			
			// Clean up
			delete m_pAboutDialog;
			m_pAboutDialog = NULL;
						
			return TRUE; 
		}
		break;
	case ID_SOFTWARE_SETTINGS:
		{
			CSoftSetDlg* m_pDialog;
			m_pDialog = new CSoftSetDlg(IDD_SOFTWARE_SETTING, m_hWnd);
			m_pDialog->DoModal();
			
			// Clean up
			delete m_pDialog;
			m_pDialog = NULL;
						
			return TRUE; 
		}
		break;
		
	case ID_ERASER_DST:
		{
			RegLoadDlg* m_pDialog;
			m_pDialog = new RegLoadDlg(IDD_TARGET_DLG, m_hWnd);
			m_pDialog->DoModal();
			
			// Clean up
			delete m_pDialog;
			m_pDialog = NULL;
						
			return TRUE; 
		}
		break;
	case ID_SECRET_ERASER:
		{
			CSecretEraserDlg* m_pDialog;


			m_pDialog = new CSecretEraserDlg(IDD_DIALOG_SECRET_ERASER, m_hWnd);
			m_pDialog->DoModal();
			// Clean up
			delete m_pDialog;
			m_pDialog = NULL;
						
			return TRUE; 
		}
		break;

	case ID_NODE_CFG:
		{
			CNodeSetDlg* m_pAboutDialog;
			m_pAboutDialog = new CNodeSetDlg(IDD_DIALOG_NODECFG, m_hWnd);
			m_pAboutDialog->DoModal();

			// Clean up
			delete m_pAboutDialog;
			m_pAboutDialog = NULL;
						
			return TRUE; 
		}
	case ID_SAVECFG:
		{
			SaveConfig();
						
			return TRUE; 
		}
	case ID_RESTORECFG:
		{
			BOOL   bFailIfExists = 0;
			CString srcFile = CString("eraserCfg.xml");
			CString dstFile = CString("curCfg.xml");
			CopyFile(srcFile,  dstFile,  bFailIfExists);   
			MessageBox("配置恢复，需要重新启动程序!", "提示", MB_OK);
			return TRUE;
		}
	}
	return FALSE;
}

void CMainFrame::OnCreate()
{
	// OnCreate controls the way the frame is created.
	// Overriding CFrame::Oncreate is optional.
	// The default for the following variables is TRUE

	// m_bShowIndicatorStatus = FALSE;	// Don't show statusbar indicators
	// m_bShowMenuStatus = FALSE;		// Don't show toolbar or menu status
	// m_bUseRebar = FALSE;				// Don't use rebars
	// m_bUseThemes = FALSE;            // Don't use themes
	// m_bUseToolbar = FALSE;			// Don't use a toolbar

	// call the base class function
	CFrame::OnCreate();
}

void CMainFrame::OnInitialUpdate()
{
	m_DockView.SetDockStyle(DS_CLIENTEDGE);

	// Load dock settings
	if (!m_DockView.LoadRegistrySettings(GetRegistryKeyName()))
		LoadDefaultDockers();

	// PreCreate initially set the window as invisible, so show it now.
	ShowWindow(SW_SHOWMAXIMIZED);
}

void CMainFrame::LoadDefaultDockers()
{
	// Note: The  DockIDs are used for saving/restoring the dockers state in the registry

	DWORD dwStyle = DS_CLIENTEDGE; // The style added to each docker

	// Add the parent dockers
	CDocker* pDockRight  = m_DockView.AddDockedChild(new CDockClasses, DS_DOCKED_LEFT | dwStyle, 200, ID_DOCK_CLASSES1);	
	CDocker* pDockBottom = m_DockView.AddDockedChild(new CDockText, DS_DOCKED_BOTTOM | dwStyle, 100, ID_DOCK_TEXT1);

	// Add the remaining dockers
	pDockRight->AddDockedChild(new CDockUserOutput, DS_DOCKED_CONTAINER | dwStyle, 200, ID_DOCK_FILES1);
	//pDockRight->AddDockedChild(new CDockClasses, DS_DOCKED_CONTAINER | dwStyle, 200, ID_DOCK_CLASSES2);
	//pDockRight->AddDockedChild(new CDockFiles, DS_DOCKED_CONTAINER | dwStyle, 200, ID_DOCK_FILES2);

	pDockBottom->AddDockedChild(new CDockOutput, DS_DOCKED_CONTAINER | dwStyle, 100, ID_DOCK_OUTPUT1);
	//pDockBottom->AddDockedChild(new CDockText, DS_DOCKED_CONTAINER | dwStyle, 100, ID_DOCK_TEXT2);
	//pDockBottom->AddDockedChild(new CDockOutput, DS_DOCKED_CONTAINER | dwStyle, 100, ID_DOCK_OUTPUT2);
}

void CMainFrame::PreCreate(CREATESTRUCT &cs)
{
	// Call the base class function first
	CFrame::PreCreate(cs);
	
	// Hide the window initially by removing the WS_VISIBLE style
	cs.style &= ~WS_VISIBLE;
}

void CMainFrame::SaveRegistrySettings()
{
	CFrame::SaveRegistrySettings();
	m_DockView.SaveRegistrySettings(GetRegistryKeyName());
}

void CMainFrame::SetupToolbar()
{
	// Set the Resource IDs for the toolbar buttons
	AddToolbarButton( ID_Menu,   TRUE );
	AddToolbarButton( ID_ERASER_STOP,  TRUE );
	AddToolbarButton( ID_SAVECFG,  TRUE );
	
	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( ID_AUTH_SETTING,   TRUE );
	AddToolbarButton( ID_SOFTWARE_SETTINGS,  TRUE );
	AddToolbarButton( ID_ERASER_DST, TRUE );
	
	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( ID_SECRET_ERASER, TRUE );
	
	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( IDM_HELP_ABOUT );
}
