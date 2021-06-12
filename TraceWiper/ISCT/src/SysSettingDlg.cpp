
#include "stdafx.h"
#include   <sstream> 
#include "atlstr.h"
//#include "resource.h"
#include "Resrc1.h"
#include "shlobj.h"
#include <iosfwd>
#include "../../platform/SysGlobal.h"
//#include "../../platform/CSector.h"
#include "../../platform/RegisterOps.h"
#include "SysSettingDlg.h"
#include "../../platform/FileOps.h"
#include "CfgFile.h"
#include "../../platform/stringEx.h"


using namespace std;

extern DWORD g_currentDetectIndex;
extern DWORD g_totalSelectedNodes;
extern HINSTANCE g_hInstance;

extern int ShowDataGridDlg(HINSTANCE hInstance, HWND hwnd, LPSTR lpszCmdLine, int nCmdShow);

HWND g_hWndProgress = NULL;


PersonListSettingDlg* PersonListSettingDlg::m_pPersonListSettingDlgInstance = NULL;
DetectProgressDlg* DetectProgressDlg::m_pDetectProgressDlgInstance = NULL;

DWORD WINAPI Thread_excuteAllNodes(void* param)
{
    DetectProgressDlg* p = (DetectProgressDlg*)param;
#if 1
        /*create folder first*/
    string lName, lDepart, lIpAddr, lHostName,lStartTime;

    p->get_personParams(lName, lDepart, lIpAddr, lHostName, lStartTime);
    //lDirectoryName = lName + "-" + lStartTime;
    //FileOps::getInstance()->createFolder("", lDirectoryName);
#endif

    pDetectorRoot->allChildrenExecute();
    
    string lEndTime = FileOps::getInstance()->get_currentTime();
    PRLIST_TYPE enType = PRLIST_INVALID;

    int scores = 0;
    pDetectorRoot->get_sumEvaScores(scores, enType);

    PRLISTDATA stInfo;
    stInfo.pr_type = enType;
    stInfo.pr_name = lName;
    stInfo.pr_depart = lDepart;
    stInfo.pr_ipaddr = lIpAddr;
    stInfo.pr_hostname = lHostName;
    stInfo.pr_starttime = lStartTime;
    stInfo.pr_endtime = lEndTime;

    CPersonListConfig::getInstance()->remove_configList(/*PRLIST_DETECTED*/enType, stInfo);
    CPersonListConfig::getInstance()->add_configList(/*PRLIST_DETECTED*/enType, stInfo);

    /*also need add to the detected person list*/
    CPersonListConfig::getInstance()->remove_configList(PRLIST_DETECTED, stInfo);
    CPersonListConfig::getInstance()->add_configList(PRLIST_DETECTED, stInfo);

    /*after finish, end the trhead*/
/*
    if (NULL != pFrame)
    {
        //pFrame->endThread();
        pFrame->release_Thread();
    }
*/
    ::MessageBox(p->GetHwnd(), _T("检测完成"), _T("提示"), MB_OK);
    HWND hwnd = p->GetParent();
    SendMessage(p->GetHwnd(), WM_CLOSE, 0, 0);
    CScoreConfig::getInstance()->set_DisplayType(EN_SCORE_PREVIEW);
    ShowDataGridDlg(g_hInstance,hwnd,"",0);
    CScoreConfig::getInstance()->set_DisplayType(EN_SCORE_SETTING);
    return 0;
}

int AddItem(HWND hList1,LPCTSTR szText, int nImage)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

SysSettingDlg::SysSettingDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
}

SysSettingDlg::~SysSettingDlg()
{

}

BOOL SysSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void SysSettingDlg::BuildData()
{
    HWND phFilePath = GetDlgItem(IDC_EDIT_FILEPATH);
    HWND phKeywords = GetDlgItem(IDC_EDIT_KEYWORDS);
    HWND phRadioPC    = GetDlgItem(IDC_RADIO_PC);
    HWND phRadioNetwork = GetDlgItem(IDC_RADIO_NETWORK);
    
    string lFilepath;
    string lKeywords;
    CSysConfig::getInstance()->loadFromFile();
    //CSysConfig::getInstance()->get_filePath(lFilepath);
    FileOps::getInstance()->getFolder(lFilepath); 
    //CSysConfig::getInstance()->get_keyWords(lKeywords);
    ::SendMessage(phFilePath,WM_SETTEXT, lFilepath.length(), (LPARAM)lFilepath.c_str());

    if (!CSysConfig::getInstance()->get_authType())
    {
        ::SendMessage(phRadioPC,  BM_SETCHECK,   1,   0);
    }
    else
    {
        ::SendMessage(phRadioNetwork,  BM_SETCHECK,   1,   0);
    }
}

BOOL SysSettingDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg)
        {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_BROWSE:
                {
                    char   Path[MAX_PATH] = {0}; 
                    LPITEMIDLIST     ret; 
                    BROWSEINFO   lpbi; 
                    lpbi.hwndOwner=m_hWnd; 
                    lpbi.pidlRoot=NULL; 
                    lpbi.ulFlags=BIF_STATUSTEXT; 
                    lpbi.pszDisplayName=Path; 
                    lpbi.lpszTitle= "选择文件夹 "; 
                    lpbi.lpfn=BrowseCallbackProc; 
                    ret=SHBrowseForFolder(&lpbi); 
                    if(ret!=NULL){ 
                    SHGetPathFromIDList(ret,Path); 
                    sprintf(Path, "%s",Path); 
                    } 
                    
                 //将取到的文件夹字符串放入编辑框中 
                    HWND pText = GetDlgItem(IDC_EDIT_FILEPATH);
                    ::SendMessage(pText,WM_SETTEXT, MAX_PATH, (LPARAM)Path);
                }
                break;
                case IDOK:
                {
                    char   Path[MAX_PATH] = {0}; 
                    int    lCheckState = 0;
                    
                    HWND phText = GetDlgItem(IDC_EDIT_FILEPATH);
                    HWND phRadioPC = GetDlgItem(IDC_RADIO_PC);
                    ::SendMessage(phText,WM_GETTEXT, MAX_PATH, (LPARAM)Path);

                    if (strlen(Path) > 0)
                    {
                        CSysConfig::getInstance()->set_filePath(Path, true);
                    }
                    else
                    {
                        CSysConfig::getInstance()->set_filePath("");
                    }

                    /*set the auth type*/
                    lCheckState = ::SendMessage(phRadioPC,  BM_GETCHECK,   0,   0);
                    if (BST_CHECKED == lCheckState)
                    {
                        CSysConfig::getInstance()->set_authType(false);
                    }
                    else
                    {
                        CSysConfig::getInstance()->set_authType(true);
                    }
                    //CSysConfig::getInstance()->saveToFile();
                }
                break;
                case IDCANCEL:
                {
                    OnCancel();
                } 
                break;
            }
        case WM_NOTIFY:
        {
        }
        break;
        case WM_INITDIALOG:
        {
            //初始化显示数据内容
            BuildData();
        }
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}

int   CALLBACK   BrowseCallbackProc(HWND   hwnd,   UINT   uMsg,   LPARAM   lParam,   LPARAM   lpData) 
{
    (void)lpData;

    switch(uMsg)
    { 
        case   BFFM_INITIALIZED: 
        {
            //默认为可执行文件当前路径 
            string Path = FileOps::getInstance()->get_currentPath(); 
            ::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM) Path.c_str()); 
        }
        break; 
        case   BFFM_SELCHANGED: 
        { 
            char   curr[MAX_PATH]; 
            SHGetPathFromIDList((LPCITEMIDLIST)lParam,curr); 
            sprintf(curr, "%s",curr); 
            ::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)curr); 
        } 
        break;
        default:
        {
            char lMsg[100] = {0};
            sprintf(lMsg, "0x%x",uMsg);
            TRACE(lMsg);
        }
         break;
    } 
    return   0; 
} 

PersonListSettingDlg* PersonListSettingDlg::getInstance(UINT id, HWND hwnd)
{
    if(NULL == m_pPersonListSettingDlgInstance)
    {
        m_pPersonListSettingDlgInstance = new PersonListSettingDlg(id, hwnd);
    }
    return m_pPersonListSettingDlgInstance;
}


PersonListSettingDlg::PersonListSettingDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
    m_StartTime = FileOps::getInstance()->get_currentTime();
    m_bCancelFlag = false;
}

PersonListSettingDlg::~PersonListSettingDlg()
{

}

void PersonListSettingDlg::setParams(std::string name, std::string depart, std::string ipaddr, std::string hostName, std::string startTime)
{
    m_Name = name;
    m_Depart = depart;
    m_IpAddr = ipaddr;
    m_HostName = hostName;
    m_StartTime = startTime;
}

void PersonListSettingDlg::getParams(std::string& name, std::string& lDepart, std::string& lIpAddr, std::string& lHostName, std::string& startTime)
{
    name = m_Name;
    lDepart = m_Depart;
    lIpAddr = m_IpAddr;
    lHostName = m_HostName;
    startTime = m_StartTime;
}

void PersonListSettingDlg::BuildData()
{
    HWND phName = GetDlgItem(IDC_EDIT_NAME);
    HWND phDepart = GetDlgItem(IDC_EDIT_DEPART);
    HWND phIPAddr = GetDlgItem(IDC_EDIT_IPADDR);
    HWND phHostName = GetDlgItem(IDC_EDIT_HOSTNAME);
    HWND phStartTime = GetDlgItem(IDC_EDIT_STARTTIME);
    if (CPersonListConfig::getInstance()->get_actionType() == PRLIST_ACTION_MODIFY)
    {
        ::SendMessage(phStartTime,EM_SETREADONLY, true, 0);
    }
    ::SendMessage(phName,WM_SETTEXT, MAX_PATH, (LPARAM)m_Name.c_str());
    ::SendMessage(phDepart,WM_SETTEXT, MAX_PATH, (LPARAM)m_Depart.c_str());
    ::SendMessage(phIPAddr,WM_SETTEXT, MAX_PATH, (LPARAM)FileOps::getInstance()->get_IpAddr().c_str());
    ::SendMessage(phHostName,WM_SETTEXT, MAX_PATH, (LPARAM)FileOps::getInstance()->get_computerName().c_str());
    if (CPersonListConfig::getInstance()->get_actionType() == PRLIST_ACTION_ADD)
    {
        m_StartTime = FileOps::getInstance()->get_currentTime();
        ::SendMessage(phStartTime,WM_SETTEXT, MAX_PATH, (LPARAM)m_StartTime.c_str());
    }
    else
    {
        ::SendMessage(phStartTime,WM_SETTEXT, MAX_PATH, (LPARAM)m_StartTime.c_str());
    }
}

BOOL PersonListSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL PersonListSettingDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDOK:
                {
                    char lName[128] = {0}; 
                    char lDepart[128] = {0};
                    char lStartTime[128] = {0};
                    
                    HWND phName = GetDlgItem(IDC_EDIT_NAME);
                    HWND phDepart = GetDlgItem(IDC_EDIT_DEPART);
                    HWND phStartTime = GetDlgItem(IDC_EDIT_STARTTIME);
                    ::SendMessage(phName,WM_GETTEXT, 128, (LPARAM)lName);
                    ::SendMessage(phDepart,WM_GETTEXT, 128, (LPARAM)lDepart);
                    ::SendMessage(phStartTime,WM_GETTEXT, 128, (LPARAM)lStartTime);
                    
                    if (strlen(lName) > 0)
                    {
                        if (strlen(lName) > 20)
                        {
                            ::MessageBox(m_hWnd, _T("姓名长度不能大于20"), _T("提示"), MB_OK);
                            memset(lName, 0, 128);
                            ::SendMessage(phName,WM_SETTEXT, 128, (LPARAM)lName);
                            return TRUE;
                        }
                    }
                    else
                    {
                            ::MessageBox(m_hWnd, _T("姓名不能为空"), _T("提示"), MB_OK);
                            return TRUE;
                    }
                    
                    if (strlen(lDepart) > 0)
                    {
                        if (strlen(lDepart) > 100)
                        {
                            ::MessageBox(m_hWnd, _T("部门长度不能大于100"), _T("提示"), MB_OK);
                            memset(lDepart, 0, 128);
                            ::SendMessage(phDepart,WM_SETTEXT, 128, (LPARAM)lDepart);
                            return TRUE;
                        }
                    }
                    else
                    {
                            ::MessageBox(m_hWnd, _T("部门不能为空"), _T("提示"), MB_OK);
                            return TRUE;
                    }
                    
                    if (strlen(lStartTime) > 0)
                    {
                        if (strlen(lStartTime) > 20)
                        {
                            ::MessageBox(m_hWnd, _T("请输入正确时间"), _T("提示"), MB_OK);
                            memset(lStartTime, 0, 128);
                            ::SendMessage(phDepart,WM_SETTEXT, 128, (LPARAM)lStartTime);
                            return TRUE;
                        }
                    }
                    else
                    {
                            ::MessageBox(m_hWnd, _T("开始检测时间不能为空"), _T("提示"), MB_OK);
                            return TRUE;
                    }
                    
                    PRLISTDATA stInfo;
                    stInfo.pr_type = (int)PRLIST_DETECTED;
                    stInfo.pr_name = lName;
                    stInfo.pr_depart = lDepart;
                    stInfo.pr_ipaddr = FileOps::getInstance()->get_IpAddr();
                    stInfo.pr_hostname = FileOps::getInstance()->get_computerName();
                    stInfo.pr_starttime   = lStartTime;
                    stInfo.pr_endtime   = " ";
                    PRLIST_ACTION enAction = CPersonListConfig::getInstance()->get_actionType();
                    switch(enAction)
                    {
                        case PRLIST_ACTION_ADD:
                        {
                            if (CPersonListConfig::getInstance()->isExists(PRLIST_DETECTED, stInfo))
                            {
                                ::MessageBox(m_hWnd, _T("此记录已存在"), _T("提示"), MB_OK);
                                return TRUE;
                            }
                            else
                            {
                                /*create folder first*/
                                string lDirectoryName = stInfo.pr_name + "-" + stInfo.pr_starttime;
                                FileOps::getInstance()->createFolder("", lDirectoryName);
                                
                                if (CSysConfig::getInstance()->get_FolderChange())
                                {
                                    
                                    lDirectoryName = RegisterOps::getInstance()->getCheckingIpAddress() + "-" + RegisterOps::getInstance()->getComputerName()  
                                                              + "-" + stInfo.pr_name + "-" + stInfo.pr_starttime;
                                    
                                    CSysConfig::getInstance()->set_subfilePath(lDirectoryName);
                                }
                                
                                CPersonListConfig::getInstance()->add_configList(PRLIST_DETECTED, stInfo);
                                setParams(stInfo.pr_name, stInfo.pr_depart, stInfo.pr_ipaddr, stInfo.pr_hostname, stInfo.pr_starttime);
                                m_bCancelFlag = false;
                            }
                        }
                            break;
                        case PRLIST_ACTION_MODIFY:
                        {
                            int selectIndex = CPersonListConfig::getInstance()->get_selectIndex();
                            PRLISTDATA stTmpInfo;
                            CPersonListConfig::getInstance()->findByIndex(PRLIST_DETECTED, selectIndex, stTmpInfo);
                            CPersonListConfig::getInstance()->remove_configList(PRLIST_DETECTED, selectIndex);
                            stInfo.pr_endtime = stTmpInfo.pr_endtime;
                            CPersonListConfig::getInstance()->add_configList(PRLIST_DETECTED, stInfo);
                        }
                            break;
                        default:
                            break;
                    }
                }
                break;
                case IDCANCEL:
                {
                    m_bCancelFlag = true;
                    OnCancel();
                } 
                break;
            }
        case WM_NOTIFY:
        {
        }
        break;
        case WM_INITDIALOG:
        {
            //初始化显示数据内容
            BuildData();
        }
        break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}



SysReportDlg::SysReportDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
}

SysReportDlg::~SysReportDlg()
{

}

BOOL SysReportDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SysReportDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_RP_BROWSE:
                {
                    char   Path[MAX_PATH] = {0}; 
                    LPITEMIDLIST     ret; 
                    BROWSEINFO   lpbi; 
                    lpbi.hwndOwner=m_hWnd; 
                    lpbi.pidlRoot=NULL; 
                    lpbi.ulFlags=BIF_STATUSTEXT; 
                    lpbi.pszDisplayName=Path; 
                    lpbi.lpszTitle= "选择文件夹"; 
                    lpbi.lpfn=BrowseCallbackProc; 
                    ret=SHBrowseForFolder(&lpbi); 
                    if(ret!=NULL){ 
                    SHGetPathFromIDList(ret,Path); 
                    sprintf(Path, "%s",Path); 
                    } 
                    
                 //将取到的文件夹字符串放入编辑框中 
                    HWND pText = GetDlgItem(IDC_PRSAVEPATH);
                    ::SendMessage(pText,WM_SETTEXT, MAX_PATH, (LPARAM)Path);
                }
                break;
                case IDOK:
                {
                    char lDate[128] = {0};
                    char lLocation[128] = {0};
                    char lPath[MAX_PATH] = {0};

                    bool bState = false;
                    bool bBit     = false;
                    IntList iList;
                    PersonList lPList;
                    DWORD totalItems = ListView_GetItemCount( m_hPrList );
                    for(DWORD i = 0; i < totalItems; i++)
                    {
                        bState = ListView_GetCheckState( m_hPrList, i );
                        if (bState)
                        {
                            iList.push_back(i);
                        }
                        bBit |= bState;
                    }

                    if (!bBit)
                    {
                        ::MessageBox(m_hWnd, _T("请选择人员"), _T("提示"), MB_OK);
                        return FALSE;
                    }
                    else
                    {
                        CPersonListConfig::getInstance()->get_configList(PRLIST_DETECTED, lPList);
                        if (lPList.size() == 0)
                        {
                            //Error Log.
                            ::MessageBox(m_hWnd, _T("人员为空"), _T("程序异常"), MB_ABORTRETRYIGNORE);
                            return FALSE;
                        }
                    }

                    m_hDate = GetDlgItem(IDC_REPORTDATE);
                    m_hLocation = GetDlgItem(IDC_LOCATION);
                    m_hPath = GetDlgItem(IDC_PRSAVEPATH);
                    ::SendMessage(m_hDate,WM_GETTEXT, 128, (LPARAM)lDate);
                    ::SendMessage(m_hLocation,WM_GETTEXT, 128, (LPARAM)lLocation);
                    ::SendMessage(m_hPath,WM_GETTEXT, MAX_PATH, (LPARAM)lPath);
                    m_date = lDate;
                    m_location = lLocation;
                    m_path = lPath;
                    if (0 == m_location.length())
                    {
                        ::MessageBox(m_hWnd, _T("请填写导出地点"), _T("提示"), MB_OK);
                        return FALSE;
                    }
                    if (0 == m_path.length())
                    {
                        ::MessageBox(m_hWnd, _T("请选择保存路径"), _T("提示"), MB_OK);
                        return FALSE;
                    }

                    /*export the report to the local file.*/
                    SetCursor(LoadCursor(NULL, IDC_WAIT)); /*set cursor to waiting state*/
                    string reportName;
                    for (DWORD idx = 0; idx < iList.size(); idx++)
                    {
                        int num = iList[idx];
                        reportName = lPList[num].pr_ipaddr + "-" + lPList[num].pr_hostname + "-" + lPList[num].pr_name + "-" + lPList[num].pr_starttime + ".txt";
                        ValueList lTestList;
                        string statusInfo = "导出状态:正在导出"+ reportName;
                        ::SendMessage(m_hStatusBar,SB_SETTEXT,1024,(LPARAM)statusInfo.c_str());
                        exportResultToTXT(lPList[num], m_path, reportName, lTestList);
                    }
                    SetCursor(LoadCursor(NULL, IDC_ARROW)); /*resume the cursor*/
                    //ValueList lTestList;
                    //FileOps::getInstance()->exportResultToDoc(m_path, "test.doc", lTestList);
                    OnOK();
                    return TRUE;
                }
                break;
                case IDCANCEL:
                {
                    OnCancel();
                    return TRUE;
                } 
                break;
            }
        case WM_NOTIFY:
        {
            if(IDC_PRLIST == wParam)
            {
                LPNMHDR lpnm_hdr = LPNMHDR(lParam);
                switch(lpnm_hdr->code)
                {
                    case WM_LBUTTONDOWN:
                    {
                        int selectRowIndex = getSelectedRowNum();
                        CPersonListConfig::getInstance()->findByIndex(PRLIST_DETECTED, selectRowIndex, m_personInfo);/*保存当前选择人员信息*/
                        return TRUE;
                    }
                    break;
                   default:
                   return FALSE;
                }
            }
        }
        break;
        case WM_INITDIALOG:
        {
            INITCOMMONCONTROLSEX   init; 
            init.dwSize   =   sizeof(INITCOMMONCONTROLSEX); 
            init.dwICC   =   ICC_BAR_CLASSES; 
            InitCommonControlsEx(&init); 

            m_hStatusBar   =   CreateStatusWindow(WS_CHILD   |   WS_VISIBLE,   "导出状态: ",   m_hWnd,   IDC_STATUS); 
            //::SendMessage(m_hStatusBar,SB_SETTEXT,1024,(LPARAM)TEXT("信息一"));

            BuildPRListData();
            //SendDlgItemMessage(IDOK,   WM_ENABLE,   FALSE,   0);

            //默认为可执行文件当前路径 
            string Path = FileOps::getInstance()->get_currentPath(); 
            
            HWND pText = GetDlgItem(IDC_PRSAVEPATH);
            ::SendMessage(pText,WM_SETTEXT, MAX_PATH, (LPARAM)Path.c_str());

        }
        break;
        default:
        break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}

void SysReportDlg::BuildPRListData()
{
        m_hPrList = GetDlgItem(IDC_PRLIST);
        //DWORD dwStyle = (DWORD)GetWindowLongPtr(hList, GWL_STYLE);
        //SetWindowLongPtr(hList, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
        ListView_SetExtendedListViewStyle(m_hPrList, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

        LV_COLUMN lvColumn = {0};
        lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = 120;

        //获取人员列表信息
        ValueList titleList;
        ValueList rowList;
        ValueList lPersonData;
        
        CPersonListConfig::getInstance()->get_titleList(titleList);
        
        int colNum = titleList.size(); //列数

        lvColumn.pszText = _T("人员类型");
        ListView_InsertColumn( m_hPrList, 0, &lvColumn);

        for(int i = 0; i < colNum; i++)
        {
            lvColumn.pszText = (char*)titleList[i].c_str();
            ListView_InsertColumn( m_hPrList, (i+1), &lvColumn);
        }

        lPersonData = CPersonListConfig::getInstance()->get_personData();

        for (int iType = (int)PRLIST_SUSPECTED; iType <= (int)PRLIST_EXCEPTION; iType++)
        {
            CPersonListConfig::getInstance()->convertListToLine((PRLIST_TYPE)iType, rowList); 

            if (rowList.empty())
            {
                continue;
            }
            
            /*create row data*/
            int rowNum = rowList.size() / colNum;
            int row = 0;

            for(row = 0; row < rowNum; row++)
            {
                //int item = AddItem(m_hPrList, rowList[row*colNum].c_str(), 3);
                int item = AddItem(m_hPrList, lPersonData[iType].c_str(), 3);
                for(int col = 1; col <= colNum; col++)
                {
                    SetSubItem(m_hPrList, item, col, rowList[(col-1)+row*colNum].c_str());
                }
            }
        }
}

int SysReportDlg::getSelectedRowNum()
{
    DWORD i = 0;
    DWORD n = 0;
    n   =   ListView_GetItemCount(m_hPrList); 
    for   (i   =   0;   i   <   n;   i++) 
    {
        if (ListView_GetItemState( m_hPrList, i,   LVIS_FOCUSED)   ==   LVIS_FOCUSED) 
        {
            return i;
        }
    }

    return -1;
}

#if 0
void SysReportDlg::exportResultToDoc(PRLISTDATA stInfo, std::string lpath, std::string docName, ValueList& lList)
{
    string ExportFilename = lpath + "\\" + docName;
    ValueList lHeaderList;
    ValueList lTitleList;
    StringEx oStr;

    ostringstream ostr;
    ostr<<create_new();
    ostr<<put_title("检测结果", "标题 ", ALIGN_CENTER);
    ostr<<put_title("", "正文 ", ALIGN_LEFT);
    lHeaderList.push_back("被检人员: " + stInfo.pr_name);
    lHeaderList.push_back("部门:" + stInfo.pr_depart);
    lHeaderList.push_back("IP地址:" + stInfo.pr_ipaddr);
    lHeaderList.push_back("主机名:" + stInfo.pr_hostname);
    lHeaderList.push_back("检测开始时间: " + stInfo.pr_starttime);
    lHeaderList.push_back("检测结束时间:" + stInfo.pr_endtime);
    lHeaderList.push_back("导出时间:" + m_date);
    lHeaderList.push_back("导出地点:" + m_location);
    lHeaderList.push_back("安全评估结果:");
    string str;
    for (int i = 0; i < lHeaderList.size(); i++)
    {
        ostr << put_title(lHeaderList[i].c_str(), "正文", ALIGN_LEFT);
    }

    ostr << put_Paragraph();
    ostr << move_down();
    DNode * pDNodeLevel1 = NULL;
    DNode* pDNodeLevel2 = NULL;
    DNode* pDNodeLevel3 = NULL;
    string logFolder;
    string filename;
    ValueList lResultList;
    
    logFolder = stInfo.pr_ipaddr + "-" + stInfo.pr_hostname + "-" + stInfo.pr_name + "-" + stInfo.pr_starttime;
    if (!FileOps::getInstance()->checkFolderExist(logFolder))
    {
        ::MessageBox(m_hWnd, _T("日志不存在"), _T("提示"), MB_OK);
        return;
    }
    
    for(DWORD i = 0; i < pDetectorRoot->getNodeChilds().size(); i++)
    {
        pDNodeLevel1 = (DNode*)(pDetectorRoot->getNodeChilds()[i]);
        char lTmp[32] = {0};
        sprintf(lTmp, "%d \t", i + 1);
        string s = lTmp;

        ostr<<put_title1((s + pDNodeLevel1->getName()).c_str());
        for (DWORD j = 0; j < pDNodeLevel1->getNodeChilds().size(); j++)
        {
            pDNodeLevel2 = (DNode*)(pDNodeLevel1->getNodeChilds()[j]);
            memset(lTmp, 0, 32);
            sprintf(lTmp, "%d.%d\t", i+1, j+1);
            s.clear();
            s = lTmp;
            ostr<<put_title2((s + pDNodeLevel2->getName()).c_str());
            for (DWORD k = 0; k < pDNodeLevel2->getNodeChilds().size(); k++)
            {
                pDNodeLevel3 = (DNode*)(pDNodeLevel2->getNodeChilds()[k]);

                memset(lTmp, 0, 32);
                sprintf(lTmp, "%d.%d.%d\t", i+1, j+1, k+1);
                s.clear();
                s = lTmp;

                ostr<<put_title3((s + pDNodeLevel3->getName()).c_str());

                filename = lpath + "\\" + logFolder + "\\" + pDNodeLevel3->getName() + FILE_POSTFIX_NAME;
                pDNodeLevel3->getTitleList(lTitleList);
                string sTitle;
                int lColCnt = lTitleList.size();
                for (int lNum = 0; lNum < lColCnt; lNum++)
                {
                    sTitle += lTitleList[lNum] + "\t";
                }
                ostr << put_title(sTitle.c_str(), "正文", ALIGN_LEFT);
                
                if (FileOps::getInstance()->readFromFile((char*)filename.c_str(), lResultList))
                {
                    int rowCnt = lResultList.size()/lColCnt;
                    string s;
                    
                    for (int rowIndex = 0; rowIndex < rowCnt; rowIndex++)
                    {
                        for (int col = 0; col < lColCnt; col++)
                        {
                            s += lResultList[rowIndex * lColCnt + col]/* +"\t"*/;
                            s += "    ";
                            s += "\n";
                        }
                        oStr.string_replace(s,"\"","");
                        //oStr.string_replace(s,"\"","");
                    }
                    ostr << put_title(s.c_str(), "正文", ALIGN_LEFT);

                    lResultList.clear();
                }
            }            
        }
    }
    ostr<<close_save(/*"d:\\test3.doc"*/(const char*)ExportFilename.c_str());
    ValueList lList;
    lList.push_back(ostr.str());
    FileOps::getInstance()->writeToFile("xxx.log", lList);
    RunScript( ostr.str().c_str() );//运行生成的脚本

}
#endif

void SysReportDlg::exportResultToTXT(PRLISTDATA stInfo, std::string lpath, std::string docName, ValueList& lList)
{
    string ExportFilename = lpath + "\\" + docName;
    ValueList lTitleList;
    StringEx oStr;
    ValueList outList;
    
    outList.push_back("\t\t\t检测结果\n");

    outList.push_back("被检人员: " + stInfo.pr_name + "\n");
    outList.push_back("部门:" + stInfo.pr_depart + "\n");
    outList.push_back("IP地址:" + stInfo.pr_ipaddr + "\n");
    outList.push_back("主机名:" + stInfo.pr_hostname + "\n");
    outList.push_back("检测开始时间: " + stInfo.pr_starttime + "\n");
    outList.push_back("检测结束时间:" + stInfo.pr_endtime + "\n");
    outList.push_back("导出时间:" + m_date + "\n");
    outList.push_back("导出地点:" + m_location + "\n");
    outList.push_back("安全评估结果:");
    
    DNode * pDNodeLevel1 = NULL;
    DNode* pDNodeLevel2 = NULL;
    DNode* pDNodeLevel3 = NULL;
    string logFolder;
    string filename;
    ValueList lResultList;
    
    logFolder = stInfo.pr_ipaddr + "-" + stInfo.pr_hostname + "-" + stInfo.pr_name + "-" + stInfo.pr_starttime;
    string sTmp;
    if (CSysConfig::getInstance()->get_FolderChange())
    {
        CSysConfig::getInstance()->get_filePath(sTmp);
        sTmp += "\\" + logFolder;
    }
    else
    {
        sTmp = logFolder;
    }
    if (!FileOps::getInstance()->checkFolderExist(sTmp))
    {
        ::MessageBox(m_hWnd, _T("日志不存在"), _T("提示"), MB_OK);
        return;
    }
    
    for(DWORD i = 0; i < pDetectorRoot->getNodeChilds().size(); i++)
    {
        pDNodeLevel1 = (DNode*)(pDetectorRoot->getNodeChilds()[i]);
        char lTmp[32] = {0};
        sprintf(lTmp, "%d \t", i + 1);
        string s = lTmp;
        s += pDNodeLevel1->getName()+"\n";
        outList.push_back(s);
        for (DWORD j = 0; j < pDNodeLevel1->getNodeChilds().size(); j++)
        {
            pDNodeLevel2 = (DNode*)(pDNodeLevel1->getNodeChilds()[j]);
            memset(lTmp, 0, 32);
            sprintf(lTmp, "%d.%d\t", i+1, j+1);
            s.clear();
            s = lTmp;
            s += pDNodeLevel2->getName()+ "\n";
            outList.push_back(s);
            for (DWORD k = 0; k < pDNodeLevel2->getNodeChilds().size(); k++)
            {
                pDNodeLevel3 = (DNode*)(pDNodeLevel2->getNodeChilds()[k]);

                memset(lTmp, 0, 32);
                sprintf(lTmp, "%d.%d.%d\t", i+1, j+1, k+1);
                s.clear();
                s = lTmp;
                s += pDNodeLevel3->getName()+"\n";
                outList.push_back(s);
                if (CSysConfig::getInstance()->get_FolderChange())
                {
                    filename = sTmp + "\\" + pDNodeLevel3->getName() + FILE_POSTFIX_NAME;
                }
                else
                {
                    filename = lpath + "\\" + logFolder + "\\" + pDNodeLevel3->getName() + FILE_POSTFIX_NAME;
                }
                pDNodeLevel3->getTitleList(lTitleList);
                string sTitle;
                int lColCnt = lTitleList.size();
                for (int lNum = 0; lNum < lColCnt; lNum++)
                {
                    sTitle += lTitleList[lNum] + "\t";
                }

                s += sTitle + "\n";
                outList.push_back(s);
                
                if (FileOps::getInstance()->readFromFile((char*)filename.c_str(), lResultList))
                {
                    int rowCnt = lResultList.size()/lColCnt;
                    string s2;
                    
                    for (int rowIndex = 0; rowIndex < rowCnt; rowIndex++)
                    {
                        for (int col = 0; col < lColCnt; col++)
                        {
                            s2 += lResultList[rowIndex * lColCnt + col] +"\t";
                        }
                        outList.push_back(s2 + "\n");
                        s2.clear();
                        //oStr.string_replace(s,"\"","");
                        //oStr.string_replace(s,"\"","");
                    }
                    

                    lResultList.clear();
                }
            }            
        }
    }

    FileOps::getInstance()->writeToFile((char*)ExportFilename.c_str(), outList);
}

DetectProgressDlg* DetectProgressDlg::getInstance(UINT id, HWND hwnd)
{
    if(NULL == m_pDetectProgressDlgInstance)
    {
        m_pDetectProgressDlgInstance = new DetectProgressDlg(id, hwnd);
    }
    return m_pDetectProgressDlgInstance;
}

DetectProgressDlg* DetectProgressDlg::getInstance()
{
    return m_pDetectProgressDlgInstance;
}

DetectProgressDlg::DetectProgressDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
}

DetectProgressDlg::~DetectProgressDlg()
{

}

HWND DetectProgressDlg::GetHwnd()
{
    return m_hWnd;
}

BOOL DetectProgressDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    g_hWndProgress = m_hWnd;

    DWORD dwThreadId;
    m_hDetectThread = ::CreateThread
            (NULL,0,Thread_excuteAllNodes, this,0,&dwThreadId);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL DetectProgressDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            switch (LOWORD (wParam))
            {
                case ID_STOP:
                {
                    endThread();
                    release_Thread();
                    OnCancel();
                    return TRUE;
                } 
                break;
            }
        }
        case ISCT_DETECT_PROGRESS_SHOW:
        {
            DNode* pDNode = (DNode*)lParam;
            if (NULL != pDNode)
            {
                string tmp = "当前检测项:" + pDNode->getName();
                DWORD percent = g_currentDetectIndex*100/g_totalSelectedNodes;
                SendMessage(m_hProgressBar, PBM_SETPOS, (WPARAM)percent, 0L);
                SendMessage(m_hCurItemText,WM_SETTEXT, MAX_PATH, (LPARAM)tmp.c_str());
            }
        }
        break;
        case WM_INITDIALOG:
        {
                m_hProgressBar  = GetDlgItem(IDC_PROGRESSBAR);
                m_hCurItemText = GetDlgItem(IDC_CURITEMTEXT);

                SendMessage(m_hProgressBar, PBM_SETPOS, (WPARAM)0, 0L);
                SendMessage(m_hCurItemText,WM_SETTEXT, MAX_PATH, (LPARAM)"正在检测:");
        }
        break;
        default:
        break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}

void DetectProgressDlg::set_personParams(string& lName, string& lDepart, string& lIpAddr, string& lHostName,string& lStartTime)
{
    m_name = lName;
    m_depart = lDepart;
    m_ipAddr = lIpAddr;
    m_hostName = lHostName;
    m_startTime = lStartTime;
}

void DetectProgressDlg::get_personParams(string& lName, string& lDepart, string& lIpAddr, string& lHostName,string& lStartTime)
{
    lName = m_name;
    lDepart = m_depart;
    lIpAddr = m_ipAddr;
    lHostName = m_hostName;
    lStartTime = m_startTime;
}

void DetectProgressDlg::endThread()
{
    int           flagThread = 0; 
    DWORD   ExitCode   =   0; 
    if (NULL != m_hDetectThread)
    {
        flagThread   =   GetExitCodeThread(m_hDetectThread,&ExitCode); 
        flagThread   =   TerminateThread(m_hDetectThread,ExitCode); 
        flagThread   =   CloseHandle(m_hDetectThread); 
    }

    g_currentDetectIndex = 0;
}


UserDefinedDlg::UserDefinedDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
}

UserDefinedDlg::~UserDefinedDlg()
{

}

BOOL UserDefinedDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL UserDefinedDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int index = -1;

    switch (uMsg)
    {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDOK:
                {
                    save();
                    OnOK();
                    return TRUE;
                }
                break;
                case IDCANCEL:
                {
                    OnCancel();
                    return TRUE;
                }
                break;
            }
            switch (HIWORD(wParam))
            {
                /*handle the select change event*/
                case CBN_SELCHANGE:
                {
                    index = SendMessage(m_hSelectCombo,CB_GETCURSEL,0,0);
                    ComboBox_OnChange(index);
                }
                break;
            }
        case WM_NOTIFY:
        {
        }
        break;
        case WM_INITDIALOG:
        {
            m_hSelectCombo  = GetDlgItem(IDC_USERDEFINECOMBO);
            m_hStaticText       = GetDlgItem(IDC_USERDEFINESTATIC);
            m_hContent          = GetDlgItem(IDC_EDIT_CONTENT);
            m_hTip                 = GetDlgItem(IDC_USERDEFINETIP);
            
            AddItems();
        }
        break;
        default:
        break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}

void UserDefinedDlg::PreCreate(CREATESTRUCT &cs)
{
    cs.lpszClass = WC_COMBOBOXEX;
    cs.style = WS_VISIBLE | WS_CHILD | CBS_DROPDOWN;

    // Set the hight of the combobox in order to see a dropdown list
    cs.cy = 100;
}
BOOL UserDefinedDlg::AddItems()
{
    
    m_ItemList.push_back("用户自定义进程1");
    m_ItemList.push_back("用户自定义进程2");
    m_ItemList.push_back("自定义下载类软件");
    m_ItemList.push_back("自定义通信类软件");

    //SendMessage(m_hSelectCombo,  CB_ADDSTRING, 0, (LPARAM)_T("TEST"))   ;
    
    for(int i = 0; i < m_ItemList.size(); ++i)
    {
        // Add the items to the ComboBox's dropdown list
        if(-1 == SendMessage(m_hSelectCombo, CB_ADDSTRING, 0, (LPARAM)m_ItemList[i].c_str()))
        {
            return FALSE;
        }
    }

    SendMessage(m_hSelectCombo,CB_SETCURSEL,0,0);

    ComboBox_OnChange(0);
    
    return TRUE;
}

void UserDefinedDlg::ComboBox_OnChange(int index)
{
    DNode* pDNode = NULL;
    string filedString;
    
    pDNode = pDetectorRoot->getChildNode((char*)m_ItemList[index].c_str());
    
    if (NULL == pDNode)
    {
        ::MessageBox(m_hWnd, _T("节点不存在,请检查配置文件"), _T("错误"), MB_OK);
        return;
    }

    if (index == 0 || index == 1)
    {
        filedString = pDNode->getNodeTitle(0)->mKeys[0]->mFields[0].mField;
    }
    else
    {
        filedString = pDNode->getNodeTitle(0)->mKeys[0]->mKey;
    }

    SendMessage(m_hContent,WM_SETTEXT, filedString.size(), (LPARAM)filedString.c_str());

    switch (index)
    {
        case 0:
        {
            SendMessage(m_hStaticText,WM_SETTEXT, 100, (LPARAM)"进程名:");
            SendMessage(m_hTip,WM_SETTEXT, 100, (LPARAM)"(注:进程名以分号分隔)");
        }
        break;
        case 1:
        {                        
            SendMessage(m_hStaticText,WM_SETTEXT, 100, (LPARAM)"进程名:");
            SendMessage(m_hTip,WM_SETTEXT, 100, (LPARAM)"(注:进程名以分号分隔)");
        }
        break;
        case 2:
        case 3:
        {            
            SendMessage(m_hStaticText,WM_SETTEXT, 100, (LPARAM)"注册表键值:");
            SendMessage(m_hTip,WM_SETTEXT, 100, (LPARAM)"(注:注册表键值完整路径)");
        }
        break;
    }
}

void UserDefinedDlg::save()
{
    int index = 0;
    string nodeName;
    DNode* pDNode = NULL;
    string saveValue;
    char lValue[1024] = {0};
    index = SendMessage(m_hSelectCombo,CB_GETCURSEL,0,0);
    SendMessage(m_hContent,WM_GETTEXT, 1024, (LPARAM)lValue);
    saveValue = lValue;
    
    nodeName = m_ItemList[index];
    pDNode = pDetectorRoot->getChildNode((char*)nodeName.c_str());

    if (NULL != pDNode)
    {
        Titles lTitles = pDNode->getNodeTitles();
        Keys lKeys;
        Fileds lFields;
        switch (index)
        {
            case 0:
            case 1:
            {
                for(int i = 0; i < lTitles.size(); i++)
                {
                    lKeys = lTitles[i]->mKeys;
                    for (int j = 0; j < lKeys.size(); j++)
                    {
                        lFields = lKeys[j]->mFields;
                        for (int k = 0; k < lFields.size(); k++)
                        {
                            pDNode->getNodeTitle(i)->mKeys[j]->mFields[k].mField = saveValue;
                        }
                    }
                }
            }
            break;

            case 2:
            case 3:
            {
                for(int i = 0; i < lTitles.size(); i++)
                {
                    lKeys = lTitles[i]->mKeys;
                    for (int j = 0; j < lKeys.size(); j++)
                    {
                        pDNode->getNodeTitle(i)->mKeys[j]->mKey = saveValue;
                    }
                }
            }
            break;
        }
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        pDNode->replaceXmlNode();
        pDetectorRoot->treeSave(); /*cannot save to file, need to be fixed.*/
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        ::MessageBox(m_hWnd, _T("保存完成"), _T("提示"), MB_OK);

    }
}

RegLoadDlg::RegLoadDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{

    m_selIndex = -1;
}

RegLoadDlg::~RegLoadDlg()
{

}

void RegLoadDlg::LoadUsers()
{
    DNode* pDNode = pDetectorRoot->getChildNode("用户信息");
	
	m_userList.clear();
	
    if (NULL != pDNode)
    {
        pDNode->excute( pDNode->getName(), CMD_START);
        pDNode->getLineResult(m_userList);
    }
    
    AddItems();
}

BOOL RegLoadDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void RegLoadDlg::BuildData()
{
    m_hFilePath = GetDlgItem(IDC_EDIT_FILEPATH);
    m_hSelectCombo  = GetDlgItem(IDC_USERS);

    //AddItems();

    string s = RegisterOps::getInstance()->get_RegFilePath();
    if (0 != s.size())
    {
        ::SendMessage(m_hFilePath,WM_SETTEXT, MAX_PATH, (LPARAM)s.c_str());
    }
}

BOOL RegLoadDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg)
        {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_REG_BROWSE:
                {
                    char   Path[MAX_PATH] = {0}; 
                    LPITEMIDLIST     ret; 
                    BROWSEINFO   lpbi; 
                    lpbi.hwndOwner=m_hWnd; 
                    lpbi.pidlRoot=NULL; 
                    lpbi.ulFlags=BIF_STATUSTEXT; 
                    lpbi.pszDisplayName=Path; 
                    lpbi.lpszTitle= "选择文件夹 "; 
                    lpbi.lpfn=BrowseCallbackProc; 
                    ret=SHBrowseForFolder(&lpbi); 
                    if(ret!=NULL){ 
                    SHGetPathFromIDList(ret,Path); 
                    sprintf(Path, "%s",Path); 
                    } 
                    
                 //将取到的文件夹字符串放入编辑框中 
                    m_hFilePath = GetDlgItem(IDC_EDIT_FILEPATH);
                    ::SendMessage(m_hFilePath,WM_SETTEXT, MAX_PATH, (LPARAM)Path);
                }
                break;
                case ID_LOAD:
                {
                    char   Path[MAX_PATH] = {0}; 
                    bool bRet = false;
                    HWND phText = GetDlgItem(IDC_EDIT_FILEPATH);
                    ::SendMessage(phText,WM_GETTEXT, MAX_PATH, (LPARAM)Path);

                    if (strlen(Path) > 0)
                    {
                        string s = Path;
                        RegisterOps::getInstance()->set_RegFilePath(s);
                        char lTmp[3] = {0};
                        lTmp[0] = Path[0]; 
                        lTmp[1] = Path[1];
                        s.clear();
                        s = lTmp;
                        CSysConfig::getInstance()->set_detectedDrive(s); /*保存被检机器系统盘符,导入注册表时同时设定*/
                    }
                    else
                    {
                        ::MessageBox(m_hWnd, _T("请选择注册表文件所在路径"), _T("提示"), MB_OK);
                        return FALSE;
                    }

                    bRet = RegisterOps::getInstance()->RegLoad(HKEY_LOCAL_MACHINE);
                    if (!bRet)
                    {
                        ::MessageBox(m_hWnd, _T("注册表导入失败,请检查"), _T("提示"), MB_OK);
                        return FALSE;
                    }
                    else
                    {
                        ::MessageBox(m_hWnd, _T("注册表导入成功"), _T("提示"), MB_OK);
                        LoadUsers();
                        return TRUE;
                    }
                }
                break;
                case IDOK:
                {
                    bool bRet = false;

                    if (m_selIndex < 0)
                    {
                        ::MessageBox(m_hWnd, _T("请选择被检用户"), _T("提示"), MB_OK);
                        return FALSE;
                    }

                    CSysConfig::getInstance()->set_detectedUser(m_userList[m_selIndex]);
                    bRet = RegisterOps::getInstance()->RegLoad(HKEY_LOCAL_MACHINE, true);
                    if (!bRet)
                    {
                        ::MessageBox(m_hWnd, _T("设置用户失败,请检查"), _T("提示"), MB_OK);
                        return FALSE;
                    }
                    else
                    {
                        ::MessageBox(m_hWnd, _T("设置用户成功"), _T("提示"), MB_OK);
                        OnOK();
                        return TRUE;
                    }
                }
                break;
                case IDCANCEL:
                {
                    OnCancel();
                } 
                break;
            }
            switch (HIWORD(wParam))
            {
                /*handle the select change event*/
                case CBN_SELCHANGE:
                {
                    m_selIndex = SendMessage(m_hSelectCombo,CB_GETCURSEL,0,0);
                }
                break;
            }

        case WM_NOTIFY:
        {
        }
        break;
        case WM_INITDIALOG:
        {
            //初始化显示数据内容
            BuildData();
        }
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}

BOOL RegLoadDlg::AddItems()
{
    //SendMessage(m_hSelectCombo,  CB_ADDSTRING, 0, (LPARAM)_T("TEST"))   ;
    
    SendMessage(m_hSelectCombo,CB_RESETCONTENT,-1,0);
    for(int i = 0; i < m_userList.size(); ++i)
    {
        // Add the items to the ComboBox's dropdown list
        if(-1 == SendMessage(m_hSelectCombo, CB_ADDSTRING, 0, (LPARAM)m_userList[i].c_str()))
        {
            return FALSE;
        }
    }

    SendMessage(m_hSelectCombo,CB_SETCURSEL,-1,0);

    //ComboBox_OnChange(0);
    
    return TRUE;
}
