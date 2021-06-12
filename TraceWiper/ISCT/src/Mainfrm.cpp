////////////////////////////////////////////////////
// Mainfrm.cpp

//#include "DNode.h"

#include "stdafx.h"
#include "resource.h"
#include "ContainerApp.h"
#include "mainfrm.h"
#include "resrc1.h"
#include "..\..\sqlite_lib\sqlite3.h"
#include "CfgFile.h"
#include "SysSettingDlg.h"
//#include "DiskScanDlg.h"
#include "../../platform/FileOps.h"
#include "authsetdialog.h"
#include "../../platform/CSector.h"
#include "../../platform/RegisterOps.h"

#if defined(_WIN32) || defined(WIN32)
# include <io.h>
#define isatty(h) _isatty(h)
#define access(f,m) _access((f),(m))
#else
/* Make sure isatty() has a prototype.
*/
extern int isatty();
#endif


extern int InitDataGridDlg(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
extern int ShowDataGridDlg(HINSTANCE hInstance, HWND hwnd, LPSTR lpszCmdLine, int nCmdShow);
extern int InitDiskScanDlg(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
extern int ShowDiskScanDlg(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
extern int ShowDiskScanFullDlg(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
extern int ShowDiskScanDeepDlg(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);

extern int Client(char *server, char *user, char *password, char *secret);
extern bool LocalAuth();

extern DWORD g_totalSelectedNodes;

struct previous_mode_data {
  int valid;        /* Is there legit data in here? */
  int mode;
  int showHeader;
  int colWidth[100];
};

/*
** An pointer to an instance of this structure is passed from
** the main program to the callback.  This is used to communicate
** state and mode information.
*/
struct callback_data {
  sqlite3 *db;           /* The database */
  int echoOn;            /* True to echo input commands */
  int cnt;               /* Number of records displayed so far */
  FILE *out;             /* Write results here */
  int mode;              /* An output mode setting */
  int writableSchema;    /* True if PRAGMA writable_schema=ON */
  int showHeader;        /* True to show column names in List or Column mode */
  char *zDestTable;      /* Name of destination table when MODE_Insert */
  char separator[20];    /* Separator character for MODE_List */
  int colWidth[100];     /* Requested width of each column when in column mode*/
  int actualWidth[100];  /* Actual width of each column */
  char nullvalue[20];    /* The text to print when a NULL comes back from
                         ** the database */
  struct previous_mode_data explainPrev;
                         /* Holds the mode information just before
                         ** .explain ON */
  char outfile[FILENAME_MAX]; /* Filename for *out */
  const char *zDbFilename;    /* name of the database file */
  sqlite3_stmt *pStmt;   /* Current statement if any. */
  FILE *pLog;            /* Write log output here */
};


extern HINSTANCE g_hInstance;
#if 0
extern "C" int do_meta_command(char *zLine, struct callback_data *p);
extern "C" void main_init(struct callback_data *data);
extern "C" char *Argv0;
extern "C" void open_db(struct callback_data *p);
extern "C" int process_sqliterc(
  struct callback_data *p,        /* Configuration data */
  const char *sqliterc_override   /* Name of config file. NULL to use default */
);
extern "C" DWORD db_task();
#endif

//DWORD g_firefoxTaskId;


CString CMainFrame::m_Server;
CString CMainFrame::m_Secret;
CString CMainFrame::m_User;
CString CMainFrame::m_Password;


// Definitions for the CMainFrame class
CMainFrame::CMainFrame(): m_pSysSettingDialog(NULL), m_hDetectThread(NULL),m_pTabDialog(NULL)
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

    delete m_pTabDialog;
    m_pTabDialog = NULL;
}
BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    // OnCommand responds to menu and and toolbar input
    switch(LOWORD(wParam))
    {
        case IDM_EDIT_COPY:
        case ID_SCORE_SETTINGS:
        {
            ShowDataGridDlg(g_hInstance,m_hWnd,"",0);
            return TRUE;
        }
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
        {    // Display the help dialog
            OnHelp();
            return TRUE;
        }
        case IDM_FILE_SAVE:
        case ID_SAVE_CFG:
        {
            SetCursor(LoadCursor(NULL, IDC_WAIT));
            pDetectorRoot->treeSave();
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            MessageBox(_T("保存完成"), _T("提示"), MB_OK);
            return TRUE;
        }
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
        case IDM_FILE_NEW:
        case ID_START:
        {
            if (0 == g_totalSelectedNodes)
            {
                MessageBox(_T("请选择检测项"), _T("提示"), MB_OK);
                return TRUE;
            }
            
            if (NULL != m_hDetectThread)
            {
                MessageBox(_T("程序正在检测..."), _T("提示"), MB_OK);
                return TRUE;
            }
            
            PersonListSettingDlg* pPerson = PersonListSettingDlg::getInstance(IDD_PERSONLIST, m_hWnd);

            if (NULL == pPerson)
            {
                    throw std::bad_alloc();
            }

            if (CSysConfig::getInstance()->get_authType())
            {
                if (Client(m_Server.GetBuffer(), m_User.GetBuffer(), m_Password.GetBuffer(), m_Secret.GetBuffer()))
                {
                    //TRACE();
                    ::MessageBox(m_hWnd, "鉴权失败，请检查您的系统设置！", "鉴权失败，请检查您的系统设置！", MB_OK);
                    return FALSE;
                }
            }
            else
            {
                HWND hPrevFocus = ::GetFocus();

                CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_ADD);//设置操作类型

                pPerson->DoModal();

                // Clean up
                //delete pPerson;
                //pPerson = NULL;
                ::SetFocus(hPrevFocus);

                 PersonList lPersonList;
                 CPersonListConfig::getInstance()->get_configList(PRLIST_DETECTED, lPersonList);

                if (lPersonList.size() == 0 || pPerson->m_bCancelFlag)
                {
                    return TRUE;
                }

                DetectProgressDlg* pProgress = new DetectProgressDlg(IDD_DETECTPROGRESS, m_hWnd);

                if (NULL == pProgress)
                {
                        throw std::bad_alloc();
                }

                string lName, lDepart, lIpAddr, lHostName,lStartTime;
                pPerson->getParams(lName, lDepart, lIpAddr, lHostName, lStartTime);
                
                pProgress->set_personParams(lName, lDepart, lIpAddr, lHostName, lStartTime);
                pProgress->DoModal();
            }
            return TRUE; 
        }
        case ID_STOP:
        {
        }
        break;
        case ID_SECURITY_BASIC:
        {
              HWND hPrevFocus = ::GetFocus();
              BasicDiskScanDlg* pBasicDiskScan = new BasicDiskScanDlg(IDD_BASICDISKSCAN, m_hWnd);
  
              if (NULL == pBasicDiskScan)
                  throw std::bad_alloc();
  
              pBasicDiskScan->DoModal();
  
              // Clean up
              delete pBasicDiskScan;
              pBasicDiskScan = NULL;
              ::SetFocus(hPrevFocus);
              return TRUE;
        }
        break;
        case ID_SECURITY_FULL:
        {
              HWND hPrevFocus = ::GetFocus();
              FullDiskScanDlg* pFullDiskScan = new FullDiskScanDlg(IDD_DISKFULLSCAN, m_hWnd);
  
              if (NULL == pFullDiskScan)
                  throw std::bad_alloc();
  
              pFullDiskScan->DoModal();
  
              // Clean up
              delete pFullDiskScan;
              pFullDiskScan = NULL;
              ::SetFocus(hPrevFocus);
              return TRUE;
        }
        break;
        case ID_SECURITY_DEEP:
        {
              HWND hPrevFocus = ::GetFocus();
              DeepDiskScanDlg* pDeepDiskScan = new DeepDiskScanDlg(IDD_DISKDEEPSCAN, m_hWnd);
  
              if (NULL == pDeepDiskScan)
                  throw std::bad_alloc();
  
              pDeepDiskScan->DoModal();
  
              // Clean up
              delete pDeepDiskScan;
              pDeepDiskScan = NULL;
              ::SetFocus(hPrevFocus);
              return TRUE;
        }
        break;
        case ID_REFRESH_MD5:
        {
            DNode* pDNode = pDetectorRoot->getChildNode("交叉检测");
            if (NULL != pDNode)
            {
                std::string keyString = pDNode->getNodeTitle(0)->getKey(0);
                pDNode->RefreshMD5Key(keyString);
            }
            return TRUE;
        }
        case IDM_EDIT_PASTE:
        case ID_SYS_SETTING:
        {
            OnSysSetting();
            return TRUE;
        }
        break;
        case IDM_EDIT_CUT:
        case ID_EXPORT:
        {
            HWND hPrevFocus = ::GetFocus();
            SysReportDlg* pReport = new SysReportDlg(IDD_REPORT, m_hWnd);

            if (NULL == pReport)
                throw std::bad_alloc();

            pReport->DoModal();

            // Clean up
            delete pReport;
            pReport = NULL;
            ::SetFocus(hPrevFocus);
            return TRUE;
        }
        break;
        case ID_NODE_USERDEFINED:
        {
            HWND hPrevFocus = ::GetFocus();
            UserDefinedDlg* p = new UserDefinedDlg(IDD_USERDEFINED, m_hWnd);

            if (NULL == p)
                throw std::bad_alloc();

            p->DoModal();

            // Clean up
            delete p;
            p = NULL;
            ::SetFocus(hPrevFocus);
            return TRUE;
        }
        break;
        case ID_REGLOAD:
        {
            HWND hPrevFocus = ::GetFocus();
            RegLoadDlg* p = new RegLoadDlg(IDD_REGLOAD, m_hWnd);

            if (NULL == p)
                throw std::bad_alloc();

            p->DoModal();

            // Clean up
            delete p;
            p = NULL;
            ::SetFocus(hPrevFocus);
            return TRUE;
        }
        break;
        case ID_SECURITY_MGR:
        case IDM_FILE_PRINT: /*磁盘管理,临时使用此ID值*/
        {
            //if (NULL == m_pTabDialog)
            {
                m_pTabDialog = new DiskTabDlg(IDD_TAB_SECTOR, m_hWnd);
                if (NULL == m_pTabDialog)
                {
                    throw std::bad_alloc();
                }
                
                m_pTabDialog->Create();
            }
            //else
            {
                //m_pTabDialog->ShowWindow(SW_SHOW);
            }

            return TRUE;
/*            
            HWND hPrevFocus = ::GetFocus();
            
            m_pTabDialog->DoModal();
            //m_Dialog.Create();
            
            ::SetFocus(hPrevFocus);
            return TRUE;
*/
        }
        break;

    }
    return FALSE;
}

void CMainFrame::OnCreate()
{
    // OnCreate controls the way the frame is created.
    // Overriding CFrame::Oncreate is optional.
    // The default for the following variables is TRUE

    // m_bShowIndicatorStatus = FALSE;    // Don't show statusbar indicators
    // m_bShowMenuStatus = FALSE;        // Don't show toolbar or menu status
    // m_bUseRebar = FALSE;                // Don't use rebars
    // m_bUseThemes = FALSE;            // Don't use themes
    // m_bUseToolbar = FALSE;            // Don't use a toolbar

    // call the base class function
    CFrame::OnCreate();
}

void CMainFrame::OnInitialUpdate()
{
#ifdef PRODUCT_U_BINDING
    if (!LocalAuth())
    {
        //::MessageBox(m_hWnd, "U盘未绑定,请对U盘进行授权!", "程序退出", MB_OK);
        exit(1);
    }
#endif

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
    AddToolbarButton( IDM_FILE_NEW,   TRUE );
    AddToolbarButton( IDM_FILE_OPEN,  TRUE );
    AddToolbarButton( IDM_FILE_SAVE,  TRUE );
    
    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_EDIT_CUT,   TRUE );
    AddToolbarButton( IDM_EDIT_COPY,  TRUE );
    AddToolbarButton( IDM_EDIT_PASTE, TRUE );
    
    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_FILE_PRINT, TRUE );
    
    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_HELP_ABOUT );
}

inline void CMainFrame::OnSysSetting()
{
    if (NULL == m_pSysSettingDialog)
    {
        // Store the window handle that currently has keyboard focus
        HWND hPrevFocus = ::GetFocus();
        if (hPrevFocus == GetMenubar())
            hPrevFocus = m_hWnd;

        m_pSysSettingDialog = new SysSettingDlg(IDD_SYSSETTING, m_hWnd);
        // Some MS compilers (including VS2003 under some circumstances) return NULL instead of throwing
        //  an exception when new fails. We make sure an exception gets thrown!
        if (NULL == m_pSysSettingDialog)
            throw std::bad_alloc();

        m_pSysSettingDialog->DoModal();

        // Clean up
        delete m_pSysSettingDialog;
        m_pSysSettingDialog = NULL;
        ::SetFocus(hPrevFocus);
    }
}




