
#include "stdafx.h"
#include "Resrc1.h"
#include "atlstr.h"
#include<io.h>
#include <fstream> 
#include "Resource.h"
#include "../../platform/SysGlobal.h"
#include "../../platform/CSector.h"
#include "../../platform/stringEx.h"
#include "../../platform/Ffsco.h"
#include "../../platform/FileOps.h"
#include "DiskScanDlg.h"

extern HINSTANCE g_hInstance;

ValueList gDriveList;
string gStrRootDrive = ROOT_DRIVE;
CString gSecStr;


/* c:\\ or d:\\
*/
__int64 getDriverSizeSector(string pDriver)
{
    ULARGE_INTEGER TotalNumberofBytes;
    TotalNumberofBytes.QuadPart = 0;
    GetDiskFreeSpaceEx(/*TEXT("C:\\")*/pDriver.c_str(),NULL,&TotalNumberofBytes,NULL);

    return TotalNumberofBytes.QuadPart/512;
}


DWORD WINAPI FileSearchTask(LPVOID lParam)
{
    BasicDiskScanDlg* pBasicDlg = (BasicDiskScanDlg*)lParam;
    helper_coffs::ffsco lFindFileObj;
    
    ValueList lTitleList;
    CBasicDiskScan::getInstance()->get_titleList(lTitleList);
    pBasicDlg->clearResult(pBasicDlg->get_ListHandle());
    ValueList lDriveList = pBasicDlg->getDriveList().get_list();
    string lpath;
    for(int i = 0; i < lDriveList.size(); i++)
    {
        lpath = lDriveList[i] + "\\";
        pBasicDlg->findFilesByKeywords(lpath, pBasicDlg->get_keywords());
    }
    //pBasicDlg->DisplayResult(pBasicDlg->get_ListHandle(), lTitleList);
    pBasicDlg->set_runState(SRCH_STOP);
    return 0;
}

DWORD WINAPI sectorScanTask(LPVOID lParam)
{
    unsigned char buffer[512] = {0};
    char lTmp[512] = {0};
    __int64 lPartBegin =0;
    __int64 lPartEnd = 0;
    DWORD percent=0;
    DWORD percent2=0;
    int len = 0;
    char lKeywords[MAX_SIZE] = {0};
    
    char symbol[] = "\\\\.\\";
    char devName[20] = {0};
    LPWSTR tranDevName;

    FullDiskScanDlg* pFullDlg = (FullDiskScanDlg*)lParam;

    ValueList lDriveList = pFullDlg->getDriveList().get_list();
    for(int i = 0; i < lDriveList.size(); i++)
    {
        memset(devName, 0, 20);
        sprintf(devName, "%s%s",symbol, lDriveList[i].c_str());    
    //sprintf(devName, "%s%s",symbol, pFullDlg->getDrive().c_str());

        pFullDlg->set_fmtDevName(devName);

        string driverName = lDriveList[i] + "\\";
        
        if (!pFullDlg->getSectorAddress(devName, lPartBegin, lPartEnd))
        {
            return 1;
        }

//        __int64 lEndSector = getDriverSizeSector(driverName);

        if (!pFullDlg->ReadSectors(i, devName, lPartBegin, lPartEnd))
        {
            return 1;
        }
    }
    pFullDlg->set_runState(SRCH_STOP);
    return 0;
}

DWORD WINAPI SectorURLScanTask(LPVOID  lParam)
{
        DeepDiskScanDlg* pDeepDlg = (DeepDiskScanDlg*)lParam;
        pDeepDlg->clearResult();
        ValueList lList;
        char szBuf[0x10000]; 
        DWORD tick1,tick2;
        LARGE_INTEGER Distance;
        ULARGE_INTEGER TotalNumberofBytes;
        DWORD TotalCnt=0;
        DWORD n=0;//循环计数
        DWORD dwReadCnt=0;
        DWORD percent=0,percent2=0;
        
        string lpath;
        
        ValueList lDriveList = pDeepDlg->getDriveList().get_list();
        int len = lDriveList.size();
        for(int i = 0; i < len; i++)
        {
            lpath = lDriveList[i] + "\\";
            GetDiskFreeSpaceEx(/*TEXT("C:\\")*/lpath.c_str(),NULL,&TotalNumberofBytes,NULL);
            TotalCnt=TotalNumberofBytes.QuadPart>>16;

            tick1=GetTickCount();
            lpath.clear();
            lpath = "\\\\.\\" + lDriveList[i];
            HANDLE hFile=CreateFile(/*TEXT("\\\\.\\C:")*/lpath.c_str(),
               GENERIC_READ|GENERIC_WRITE,
               FILE_SHARE_READ|FILE_SHARE_WRITE,
               0,OPEN_EXISTING,
               FILE_FLAG_NO_BUFFERING,//表示不缓存
               0);
            if (INVALID_HANDLE_VALUE == hFile)
            {
                ::MessageBox(pDeepDlg->GetHwnd(), _T("访问扇区失败"), _T("提示"), MB_OK);
                //pDeepDlg->set_runState(SRCH_STOP);
                return 1;
            }
            pDeepDlg->set_hdSector(hFile);
            Distance.QuadPart=0;
            Distance.LowPart=SetFilePointer(hFile, Distance.LowPart, &Distance.HighPart, FILE_BEGIN);
            for ( ; n< TotalCnt; ++n )
            {
               memset(szBuf, 0, 0x10000);
               ReadFile(hFile, szBuf, 0x10000, &dwReadCnt, 0);
               pDeepDlg->CheckBuffer(szBuf);

               percent2=n*100/TotalCnt;
               if(percent!=percent2)
               {
                    PostMessage(pDeepDlg->m_hProgress, PBM_SETPOS, (WPARAM)percent2, 0L);
                    percent=percent2;
               }
               //Sleep(500);
            }
            pDeepDlg->closeHandleSector();
            tick2=GetTickCount();
            DWORD minute=(tick2-tick1)/1000/60;
            DWORD second=(tick2-tick1)/1000%60;
        }

        pDeepDlg->set_runState(SRCH_STOP);
        
        return 0;

        //printf("共查到上网记录%d条，用时%d分%d秒！\n",m_URLCount,minute,second);
}

DiskScanDlg::DiskScanDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
    m_hwnd     = hwnd;
    m_enState = SRCH_STOP;
    m_hThreadId = NULL;
}

DiskScanDlg::~DiskScanDlg()
{
    m_hLeftTree      = NULL;
    m_hwnd        = NULL;

    if (NULL != m_hThreadId)
    {
        endThread(m_hThreadId);
        m_hThreadId = NULL;
    }
}
BOOL DiskScanDlg::OnInitDialog()
{
#if 0
    CRect rc; 
    rc = GetWindowRect(); 
    int  x,y;  
    float  multiple,H;      
            //取得分辨率 
    x=GetSystemMetrics(  SM_CXSCREEN  );  
    y=GetSystemMetrics(  SM_CYSCREEN  );      
    multiple=(float)x/1024;  //如果你布局时的分辨率不是1027*768，就改一下就行了。 
    H=(float)y/768; 
            //改变对话框的大小  
    this->MoveWindow((int)(rc.top*H),(int)(rc.left*multiple),(int)(rc.Width()*multiple),(int)(rc.Height()*H));  
#endif

    CDialog::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL DiskScanDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DialogProcDefault(uMsg, wParam, lParam);
}

HTREEITEM DiskScanDlg::AddItem(HWND hwnd, HTREEITEM hParent, LPCTSTR szParam, int iImage, LPCTSTR szText)
{
    TVITEM tvi = {0};
    tvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
    tvi.iImage = iImage;
    tvi.iSelectedImage = iImage;
    tvi.lParam = (LPARAM)szParam;
    tvi.pszText = (LPTSTR)szText;
    
    TVINSERTSTRUCT tvis = {0};
    tvis.hParent = hParent;
    tvis.item = tvi;
    return TreeView_InsertItem( hwnd, &tvis );
}

void DiskScanDlg::BuildLeftTree(HWND hWnd)
{
    //BuildTree(PersonTreeBuild());
    ValueList lDTypeList;
    string lDriveName;
    m_diskList.clear();
    CDiskScan::getInstance()->getLogicalDrive(m_diskList, lDTypeList);
    HTREEITEM htiRoot = AddItem(hWnd, NULL, gStrRootDrive.c_str(), 0, gStrRootDrive.c_str());
    for(int i = 0; i < m_diskList.size(); i++)
    {
        lDriveName = lDTypeList[i] + "(" + m_diskList[i] + ")";
        AddItem(hWnd, htiRoot, m_diskList[i].c_str(), 0, lDriveName.c_str());
    }
    TreeView_Expand(hWnd, htiRoot, TVE_EXPAND);

}

void DiskScanDlg::clearResult(HWND hwnd)
{
    ListView_DeleteAllItems(hwnd);
}

void DiskScanDlg::DisplayResult(HWND hwnd, ValueList& lTitleList, ValueList& lRowList)
{
        DWORD dwStyle = (DWORD)::GetWindowLongPtr(hwnd, GWL_STYLE);
        ::SetWindowLongPtr(hwnd, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
        ListView_SetExtendedListViewStyle(hwnd, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

        LV_COLUMN lvColumn = {0};
        lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = 120;

        int colNum = /*lTitleList.size()*/m_TitleColNum; //列数

        if (lRowList.empty())
        {
            return;
        }
        
        /*create row data*/
        int rowNum = lRowList.size() / colNum;
        int row = 0;
        for(row = 0; row < rowNum; row++)
        {
            if (lRowList.empty())  /*to avoid the thread is stopped by user*/
            {
                return;
            }
            int item = AddItem(hwnd, lRowList[row*colNum].c_str(), 3);
            for(int col = 1; col < colNum; col++)
            {
                if (lRowList.empty()) /*to avoid the thread is stopped by user*/
                {
                    return;
                }
                SetSubItem(hwnd, item, col, lRowList[col+row*colNum].c_str());
            }
        }
}

LRESULT   DiskScanDlg::setupDrive(HWND hwnd, TVHITTESTINFO  tvhti,   CPoint   point)
{  
    (void)point;
    string lDrive;
    TVITEM tvItem = {0};
    ItemCheck check = UNCHECKED;
    
    tvItem.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_STATE;
    tvItem.hItem = tvhti.hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;
    //tvItem.pszText = hStr;
    //Node node;
    if (!TreeView_GetItem( hwnd, &tvItem ))
    {
        return LRESULT(-1);
    }

    lDrive = (char*)tvItem.lParam;
    if (((BOOL)(tvItem.state >> 12) -1)) /*UNCHECKED*/
    {
        if (lDrive == ROOT_DRIVE)
        {
            m_driveList.clear();
        }
        else
        {
            m_driveList.remove_value(lDrive);
        }
        check = UNCHECKED;
    }
    else /*CHECKED*/
    {
        if (lDrive == ROOT_DRIVE)
        {
            ValueList lList, lTmpList;
            
            CDiskScan::getInstance()->getLogicalDrive(lList, lTmpList);
            m_driveList.set_list(lList);
        }
        else
        {
            m_driveList.add_value(lDrive);
        }
        check = CHECKED;
    }
    SetItemChecked(hwnd, tvhti.hItem, check, 1);
    return LRESULT(3);
}

int DiskScanDlg::AddItem(HWND hwnd,LPCTSTR szText, int nImage)
{
    LVITEM lvi = {0};
    lvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.iItem = ListView_GetItemCount(hwnd);

    return ListView_InsertItem( hwnd, &lvi );
}

void DiskScanDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

void DiskScanDlg::endThread(HANDLE lThread)
{
    int           flagThread = 0; 
    DWORD   ExitCode   =   0; 
    flagThread   =   GetExitCodeThread(lThread,&ExitCode); 
    flagThread   =   TerminateThread(lThread,ExitCode); 
    flagThread   =   CloseHandle(lThread); 
    m_hThreadId = NULL;
}

void DiskScanDlg::set_runState(SEARCH_STATE enState)
{       
    m_enState = enState;
}

bool DiskScanDlg::SetItemChecked(HWND hwnd, HTREEITEM hItem, ItemCheck check, bool isRoot)
{
    //First Child
    HTREEITEM hChild = TreeView_GetChild(hwnd, hItem);   

    TVITEM tvItem = {0};
    tvItem.mask = TVIF_STATE|TVIF_HANDLE;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;
    if(!TreeView_GetItem( hwnd, &tvItem ))
        return FALSE;
    //GetSelectedItem
    if (!isRoot)
    {
        tvItem.state = INDEXTOSTATEIMAGEMASK((check ? 2 : 1));
        TreeView_SetItem( hwnd, &tvItem);
    }
    //::SendMessage(hwnd, TVM_SETITEM, 0, (LPARAM)(&tvItem));

    //SelectItem(hItem);
    while (hChild != NULL)
    {
        SetItemChecked(hwnd, hChild, check, 0);

        //Next Sibling
        hChild = TreeView_GetNextSibling(hwnd, hChild);
    }
    //tvItem.state = INDEXTOSTATEIMAGEMASK((check ? 1 : 2));
    //TreeView_SetItem( hwnd, &tvItem);
    return TRUE;
}

void DiskScanDlg::clearAll()
{
    m_driveList.clear();
    m_enState = SRCH_STOP;
    m_resultList.clear();
    m_oneRowList.clear();
}

BasicDiskScanDlg::BasicDiskScanDlg(UINT id, HWND hwnd): DiskScanDlg(id, hwnd)
{
    m_hwnd = hwnd;
    ValueList lTitleList;
    CBasicDiskScan::getInstance()->get_titleList(lTitleList);
    m_TitleColNum = lTitleList.size();
    m_bSaveResult = false;

    m_saveName = "基本涉密.log";

    if (CSysConfig::getInstance()->get_FolderChange())
    {
        string lFolderName;
        CSysConfig::getInstance()->get_filePath(lFolderName);
        m_saveName = lFolderName + "\\" + m_saveName;
    }
}

BasicDiskScanDlg::~BasicDiskScanDlg()
{
    m_hBasicKeywords    = NULL;
    //m_hLeftTree      = NULL;
    m_hList             = NULL;
    m_hSResultText = NULL;
    m_hSearch        = NULL;
}

void BasicDiskScanDlg::setupHandle()
{
    m_hBasicKeywords    = CWnd::GetDlgItem(IDC_BASIC_KEYWORDS);
    m_hLeftTree      = CWnd::GetDlgItem(IDC_BASIC_LEFTTREE);
    m_hList             = CWnd::GetDlgItem(IDC_BASIC_LIST);
    m_hSResultText = CWnd::GetDlgItem(IDC_BASIC_SRESULTTXT);
    m_hSearch        = CWnd::GetDlgItem(IDC_BASIC_SEARCH);
    m_hSaveResult = GetDlgItem(IDC_SAVELOG);
}

void BasicDiskScanDlg::BuildLeftTree(HWND hWnd)
{
    //BuildTree(PersonTreeBuild());
    ValueList lDTypeList;
    string lDriveName;
    m_diskList.clear();
    CBasicDiskScan::getInstance()->getLogicalDrive(m_diskList, lDTypeList);
    HTREEITEM htiRoot = AddItem(hWnd, NULL, gStrRootDrive.c_str(), 0, gStrRootDrive.c_str());
    for(int i = 0; i < m_diskList.size(); i++)
    {
        lDriveName = lDTypeList[i] + "(" + m_diskList[i] + ")";
        AddItem(hWnd, htiRoot, m_diskList[i].c_str(), 0, lDriveName.c_str());
    }
    TreeView_Expand(hWnd, htiRoot, TVE_EXPAND);

}

void BasicDiskScanDlg::BuildColumn()
{
        //DWORD dwStyle = (DWORD)GetWindowLongPtr(hList, GWL_STYLE);
        //SetWindowLongPtr(hList, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
        ListView_SetExtendedListViewStyle(m_hList, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

        LV_COLUMN lvColumn = {0};
        lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = 120;

        //get the title list
        ValueList titleList;
        //ValueList rowList;
        CBasicDiskScan::getInstance()->get_titleList(titleList);
        //CPersonListConfig::getInstance()->convertListToLine(PRLIST_DETECTED, rowList); 
        
        int colNum = titleList.size(); //列数
        for(int i = 0; i < colNum; i++)
        {
            lvColumn.pszText = (char*)titleList[i].c_str();
            ListView_InsertColumn( m_hList, i, &lvColumn);
        }
#if 0
        if (rowList.empty())
        {
            return;
        }
        
        /*create row data*/
        int rowNum = rowList.size() / colNum;
        int row = 0;
        for(row = 0; row < rowNum; row++)
        {
            int item = AddItem(m_hPrList, rowList[row*colNum].c_str(), 3);
            for(int col = 1; col < colNum; col++)
            {
                SetSubItem(m_hPrList, item, col, rowList[col+row*colNum].c_str());
            }
        }
#endif
}

bool BasicDiskScanDlg::set_keywords()
{
    if (NULL != m_hBasicKeywords)
    {
            KeyList lKeyList;
            char lKeywords[MAX_SIZE] = {0};
            
            /*是否输入关键字*/
            TCHAR tmp[MAX_SIZE] = {0};
            
            ::CWnd::SendMessage(m_hBasicKeywords,WM_GETTEXT, MAX_SIZE, (LPARAM)tmp);
            m_keywords = (char*)tmp;
            if (0 == m_keywords.length())
            {
                CWnd::MessageBox(_T("请输入搜索关键字"), _T("提示"), MB_OK);
                return false;
            }
    }
    else
    {
        m_keywords = "";
        return false;
    }
    return true;
}


BOOL BasicDiskScanDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_KEYDOWN:
        {
            if (VK_ESCAPE == wParam)
            {
                return TRUE;
            }
        }
        break;
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_BASIC_SEARCH:
                {
                    if (set_keywords())
                    {
                        if (m_driveList.length() == 0)
                        {
                            CWnd::MessageBox(_T("请选择磁盘"), _T("提示"), MB_OK);
                             return FALSE;
                        }

                        m_bSaveResult = (::SendMessage(m_hSaveResult,  BM_GETCHECK,   0,   0) == BST_CHECKED)?true:false;
                        
                        if (m_enState == SRCH_STOP)
                        {
                            FileOps::getInstance()->remove(m_saveName);
                            DWORD dwThreadId = 0;
                            m_hThreadId=::CreateThread(NULL,0,FileSearchTask,this,0,&dwThreadId);
                            set_runState(SRCH_START);
                        }
                        else if (m_enState == SRCH_START)
                        {
                            endThread(m_hThreadId);
                            m_hThreadId = NULL;
                            set_runState(SRCH_STOP);
                        }
                       //CloseHandle(gThread);
                    }
                }
                break;
                case IDC_BASIC_CANCEL:
                {
                    if (NULL != m_hThreadId)
                    {
                        endThread(m_hThreadId);
                        m_hThreadId = NULL;
                    }
                    OnCancel();
                }
                break;
                case IDC_BASIC_KEYWORDS:
                {
                    TRACE("aaaa\n");
                    return 0;
                    switch (lParam)
                    {
                    default:
                        break;
                    }
                }
                break;
            }
            break;
        case WM_NOTIFY:
        {
            if(IDC_BASIC_LEFTTREE == wParam)
            {
                LPNMHDR lpnm_hdr = LPNMHDR(lParam);

                switch(lpnm_hdr->code)
                {
                    case NM_CLICK:
                    {
                        CPoint ptScreen;
                        ::GetCursorPos(&ptScreen);

                        LRESULT lResult = 0;

                        TVHITTESTINFO  tvhti;
                        tvhti.pt = ptScreen;
                        ScreenToClient(m_hLeftTree, &tvhti.pt);
                        tvhti.flags = LVHT_NOWHERE;
                        TreeView_HitTest(m_hLeftTree, &tvhti);
                        
                        if(TVHT_ONITEM & tvhti.flags)
                        {
                            TVITEM tvItem = {0};
                            tvItem.mask = TVIF_PARAM;
                            tvItem.hItem = tvhti.hItem;
                            //Node node;
                            TreeView_GetItem(m_hLeftTree, &tvItem);
                            
                            if(TVHT_ONITEMSTATEICON & tvhti.flags) /*点击的是节点前面的checkbox */
                            {
                                lResult = setupDrive(m_hLeftTree, tvhti , ptScreen);
                            }
                        }
                        if (lResult == 3)
                            return LRESULT(0);
                    }
                    break;
                   default:
                   return FALSE;
                }
            }

            return TRUE;
        }
        break;
        case WM_INITDIALOG:
        {
            //初始化显示数据内容
            setupHandle();
            BuildLeftTree(m_hLeftTree);
            BuildColumn();
        }
        break;
        default:
            break;
    }
    char aaa[48] = {0};
    sprintf(aaa, "%x-%x-%x\n", uMsg, lParam, wParam);
    TRACE(aaa);
    // Always pass unhandled messages on to DialogProcDefault
    return DialogProcDefault(uMsg, wParam, lParam);
}

void BasicDiskScanDlg::set_runState(SEARCH_STATE enState)
{
    if (m_enState == SRCH_STOP)
    {
        ::CWnd::SendMessage(m_hSearch,WM_SETTEXT, 128, (LPARAM)TEXT("停止"));
    }
    else if (m_enState == SRCH_START)
    {
        ::CWnd::SendMessage(m_hSearch,WM_SETTEXT, 128, (LPARAM)TEXT("搜索"));
    }
    
    m_enState = enState;
}

void BasicDiskScanDlg::DisplayResult(HWND hwnd, ValueList& lTitleList, ValueList lRowList)
{
    DiskScanDlg::DisplayResult(hwnd, lTitleList, lRowList);

    if (m_bSaveResult) /*记录结果*/
    {
        string s;
        ValueList lList;
        for (int i = 0; i < lRowList.size(); i++)
        {
            s += lRowList[i] + "\t";
        }

        s+= "\n";
        lList.push_back(s);
        FileOps::getInstance()->writeToFile((char*)m_saveName.c_str(), lList, true); /*append*/
    }

    //set_runState(SRCH_STOP);
}
void BasicDiskScanDlg::fileFound(struct _finddata_t filefind, string path, string matchkeyword, ValueList &lTitleList)
{
    char tmpBuf[1024] = {0};
    helper_coffs::ffsco ffo;

    m_resultList.push_back(filefind.name);         /*file name*/
    m_oneRowList.push_back(filefind.name);

    m_resultList.push_back(path);                     /*directory*/
    m_oneRowList.push_back(path);

    sprintf(tmpBuf, "%d KB", filefind.size/1024); /*size*/
    m_resultList.push_back(tmpBuf);
    m_oneRowList.push_back(tmpBuf);

    memset(tmpBuf, 0, 1024);
    ffo.getLocalTime(filefind.time_create, tmpBuf);
    m_resultList.push_back(tmpBuf);                     /*create time*/
    m_oneRowList.push_back(tmpBuf);

    memset(tmpBuf, 0, 1024);
    ffo.getLocalTime(filefind.time_write, tmpBuf);
    m_resultList.push_back(tmpBuf);                 /*modified time */
    m_oneRowList.push_back(tmpBuf);

    memset(tmpBuf, 0, 1024);
    ffo.getLocalTime(filefind.time_access, tmpBuf);
    m_resultList.push_back(tmpBuf);                 /*access time*/
    m_oneRowList.push_back(tmpBuf);

    m_resultList.push_back(matchkeyword);      /*matched key word*/
    m_oneRowList.push_back(matchkeyword);
    //                if ((m_oneRowList.size()/m_TitleColNum)== 10)
    {
        DisplayResult(m_hList, lTitleList, m_oneRowList);
        m_oneRowList.clear();
    }
}
void BasicDiskScanDlg::findFilesByKeywords(string path, string keywords)
{
//    char tmpBuf[1024] = {0};
    helper_coffs::ffsco ffo;
    string matchkeyword;        
    ValueList keywordsList;
    ValueList lTitleList;
    ffo.splitStr(keywords, ";", keywordsList);

    struct _finddata_t filefind;
    if(path[path.size()-1]=='\\')
    {
        path.resize(path.size()-1);
    }
    string curr=path+"\\*.*";

    m_resultList.clear(); /*clear the previous result when scan files each time.*/

    int done=0;
    int handle = 0;
    if((handle=_findfirst(curr.c_str(),&filefind))==-1)
    {
        return ;
    }
    
    do
    {
        if((0 == strcmp(filefind.name,"..")) ||(0 == strcmp(filefind.name,".")))
        {
            continue;
        }
        
        curr=path+"\\"+filefind.name;
        SendMessage(m_hSResultText, WM_SETTEXT, MAX_PATH, (LPARAM)curr.c_str());
        
        //if(strstr(filefind.name,keywords.c_str()))
        if (_A_SUBDIR==filefind.attrib)
        {
            findFilesByKeywords(curr,keywords);
        }
        else
        {
            if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
            {
                fileFound(filefind, curr, matchkeyword, lTitleList);
            }
            else // match the content
            {
                ifstream   ifile(curr.c_str());   
                string   line;   
                if(!ifile.is_open())   
                {

                }
                else
                {
                    while (!ifile.eof())     
                    {
                        getline(ifile,   line);
                        //if(strstr(line.c_str(),keywords.c_str()) /*&& (FILE_ATTRIBUTE_DIRECTORY != filefind.attrib)*/)
                        if (hasKeywordsInString(line, keywordsList, matchkeyword))
                        {
                            fileFound(filefind, curr, matchkeyword, lTitleList);
                            break;
                        }
                    }   
                    ifile.close();
                }
            }
        }
        #if 0
        if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
        {
            //if(FILE_ATTRIBUTE_DIRECTORY != (filefind.attrib & FILE_ATTRIBUTE_DIRECTORY))
            {                
                m_resultList.push_back(filefind.name);         /*file name*/
                m_oneRowList.push_back(filefind.name);
                
                m_resultList.push_back(curr);                     /*directory*/
                m_oneRowList.push_back(curr);
                
                sprintf(tmpBuf, "%d KB", filefind.size/1024); /*size*/
                m_resultList.push_back(tmpBuf);
                m_oneRowList.push_back(tmpBuf);
                
                memset(tmpBuf, 0, 1024);
                ffo.getLocalTime(filefind.time_create, tmpBuf);
                m_resultList.push_back(tmpBuf);                     /*create time*/
                m_oneRowList.push_back(tmpBuf);

                memset(tmpBuf, 0, 1024);
                ffo.getLocalTime(filefind.time_write, tmpBuf);
                m_resultList.push_back(tmpBuf);                 /*modified time */
                m_oneRowList.push_back(tmpBuf);

                memset(tmpBuf, 0, 1024);
                ffo.getLocalTime(filefind.time_access, tmpBuf);
                m_resultList.push_back(tmpBuf);                 /*access time*/
                m_oneRowList.push_back(tmpBuf);
                
                m_resultList.push_back(matchkeyword);      /*matched key word*/
                m_oneRowList.push_back(matchkeyword);
                if ((m_oneRowList.size()/m_TitleColNum)== 10)
                {
                    DisplayResult(m_hList, lTitleList, m_oneRowList);
                    m_oneRowList.clear();
                }
            }
        }
        else
        {
            ifstream   ifile(filefind.name);   
            string   line;   
            if(!ifile.is_open())   
            {
                if (_A_SUBDIR==filefind.attrib)
                {
                    ifile.close();
                    //findFilesByKeywords(curr,keywords);
                    continue;
                }
            }
            else
            {
                while (!ifile.eof())     
                {
                    getline(ifile,   line);
                    //if(strstr(line.c_str(),keywords.c_str()) /*&& (FILE_ATTRIBUTE_DIRECTORY != filefind.attrib)*/)
                    if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
                    {
                        m_oneRowList.clear();
                        
                        m_resultList.push_back(filefind.name);         /*file name*/
                        m_oneRowList.push_back(filefind.name);
                        
                        m_resultList.push_back(curr);                     /*directory*/
                        m_oneRowList.push_back(curr);
                        
                        sprintf(tmpBuf, "%d KB", filefind.size/1024); /*size*/
                        m_resultList.push_back(tmpBuf);
                        m_oneRowList.push_back(tmpBuf);
                        
                        memset(tmpBuf, 0, 1024);
                        ffo.getLocalTime(filefind.time_create, tmpBuf);
                        m_resultList.push_back(tmpBuf);                     /*create time*/
                        m_oneRowList.push_back(tmpBuf);

                        memset(tmpBuf, 0, 1024);
                        ffo.getLocalTime(filefind.time_write, tmpBuf);
                        m_resultList.push_back(tmpBuf);                 /*modified time */
                        m_oneRowList.push_back(tmpBuf);

                        memset(tmpBuf, 0, 1024);
                        ffo.getLocalTime(filefind.time_access, tmpBuf);
                        m_resultList.push_back(tmpBuf);                 /*access time*/
                        m_oneRowList.push_back(tmpBuf);
                        
                        m_resultList.push_back(matchkeyword);      /*matched key word*/
                        m_oneRowList.push_back(matchkeyword);

                        if ((m_oneRowList.size()/m_TitleColNum)== 10)
                        {
                            DisplayResult(m_hList, lTitleList, m_oneRowList);
                            m_oneRowList.clear();
                        }
                        break;
                    }
                }   
            }
            ifile.close();
        }

        if (_A_SUBDIR==filefind.attrib)
            findFilesByKeywords(curr,keywords);
        #endif
    }while(!(done=_findnext(handle,&filefind))); 
    _findclose(handle);      
}

bool BasicDiskScanDlg::hasKeywordsInString(string strSrc, ValueList keywordsList, string& keyword)
{
    for(int i = 0; i < keywordsList.size(); i++)
    {
        if (strstr(strSrc.c_str(),keywordsList[i].c_str()))
        {
            keyword = keywordsList[i];
            return true;
        }
    }
    return false;
}

void BasicDiskScanDlg::clearAll()
{
    DiskScanDlg::clearAll();
    ::CWnd::SendMessage(m_hBasicKeywords,WM_SETTEXT, MAX_SIZE, (LPARAM)"");
}

FullDiskScanDlg::FullDiskScanDlg(UINT id, HWND hwnd): DiskScanDlg(id, hwnd)
{
    m_hwnd = hwnd;
    m_hThreadId = NULL;
    m_hdSector   = NULL;
    m_pArrySecInfo = NULL;
    
    ValueList lTitleList;
    CFullDiskScan::getInstance()->get_titleList(lTitleList);
    m_TitleColNum = lTitleList.size();

    m_pSecBuffer = new char[512 * MAX_SECTOR_NUM];

    m_pDeepDlg = NULL;

    m_bCheckURL = false;
    m_bSaveResult = false;

    m_saveName = "全面扫描.log";

    if (CSysConfig::getInstance()->get_FolderChange())
    {
        string lFolderName;
        CSysConfig::getInstance()->get_filePath(lFolderName);
        m_saveName = lFolderName + "\\" + m_saveName;
    }
}

FullDiskScanDlg::~FullDiskScanDlg()
{
    //m_hLeftTree      = NULL;
    m_hKeywords  = NULL;
    m_hStart         = NULL;
    m_hList           = NULL;
    m_hEdit1         = NULL;
    m_hEdit2         = NULL;
    m_hProgress   = NULL;
    m_hThreadId  = NULL;
    m_hdSector    = NULL;
    m_secSize      = 0;
    FreeSectorInfo();

    if (NULL != m_pSecBuffer)
    {
        delete m_pSecBuffer;
        m_pSecBuffer = NULL;
    }
}

void FullDiskScanDlg::AllocateSectorInfo()
{
    FreeSectorInfo();
    
    m_secSize = m_driveList.length();
    m_pArrySecInfo = new ST_SECINFO[m_secSize];

    if (NULL == m_pArrySecInfo)
    {
        throw std::bad_alloc();
    }

}

void FullDiskScanDlg::FreeSectorInfo()
{
    if (NULL != m_pArrySecInfo)
    {
        delete []m_pArrySecInfo;
        m_pArrySecInfo = NULL;
    }
}

BOOL FullDiskScanDlg::OnInitDialog()
{
    DiskScanDlg::OnInitDialog();

    //初始化显示数据内容
    setupHandle();
    BuildLeftTree(m_hLeftTree);
    BuildColumn();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void FullDiskScanDlg::setupHandle()
{
    m_hLeftTree  = GetDlgItem(IDC_FULL_LEFTTREE);
    m_hKeywords = GetDlgItem(IDC_FULL_KEYWORDS); 
    m_hStart       =  GetDlgItem(IDC_FULL_START);
    m_hList         =  GetDlgItem(IDC_FULL_LIST);
    m_hEdit1       =  GetDlgItem(IDC_FULL_EDIT1);
    m_hEdit2       =  GetDlgItem(IDC_FULL_EDIT2);
    m_hProgress =  GetDlgItem(IDC_FULL_PROGRESS);
    m_hCurSectorText =  GetDlgItem(IDC_FULL_CURSECTEXT);
    m_hCheckURL = GetDlgItem(IDC_CHECKURL);
    m_hSaveResult = GetDlgItem(IDC_SAVELOG);
}


void FullDiskScanDlg::BuildColumn()
{
        ListView_SetExtendedListViewStyle(m_hList, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

        LV_COLUMN lvColumn = {0};
        lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = 300;

        //get the title list
        ValueList titleList;
        CFullDiskScan::getInstance()->get_titleList(titleList);
        
        int colNum = titleList.size(); //列数
        for(int i = 0; i < colNum; i++)
        {
            lvColumn.pszText = (char*)titleList[i].c_str();
            ListView_InsertColumn( m_hList, i, &lvColumn);
        }
}

bool FullDiskScanDlg::set_keywords()
{
    if (NULL != m_hKeywords)
    {
            KeyList lKeyList;
            char lKeywords[MAX_SIZE] = {0};
            
            /*是否输入关键字*/
            TCHAR tmp[MAX_SIZE] = {0};
            
            ::CWnd::SendMessage(m_hKeywords,WM_GETTEXT, MAX_SIZE, (LPARAM)tmp);
            m_keywords.clear();
            m_keywords = (char*)tmp;
            if (0 == m_keywords.length())
            {
                CWnd::MessageBox(_T("请输入搜索关键字"), _T("提示"), MB_OK);
                return false;
            }
    }
    else
    {
        m_keywords = "";
        return false;
    }
    return true;
}

BOOL FullDiskScanDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        if (VK_ESCAPE == wParam)
        {
            return FALSE;
        }

        switch (uMsg)
        {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_FULL_START:
                {
                    if (set_keywords())
                    {
                        if (m_driveList.length() == 0)
                        {
                            CWnd::MessageBox(_T("请选择磁盘"), _T("提示"), MB_OK);
                             return FALSE;
                        }

                       m_bCheckURL = (::SendMessage(m_hCheckURL,  BM_GETCHECK,   0,   0) == BST_CHECKED)?true:false;
                       m_bSaveResult = (::SendMessage(m_hSaveResult,  BM_GETCHECK,   0,   0) == BST_CHECKED)?true:false;
                       
                        if (m_enState == SRCH_STOP) /*START*/
                        {
                            AllocateSectorInfo();
                            FileOps::getInstance()->remove(m_saveName);
                            DWORD dwThreadId = 0;
                            clearResult(m_hList);
                            m_hThreadId=::CreateThread(NULL, 0, sectorScanTask, this, 0, &dwThreadId);
                            set_runState(SRCH_START);
                            m_pDeepDlg->setAction(m_bCheckURL);
                            m_pDeepDlg->clearResult();
                        }
                        else if (m_enState == SRCH_START) /*STOP*/
                        {
                            //endThread(m_hThreadId);
                            set_runState(SRCH_STOP);
                            m_pDeepDlg->setAction(false);
                            m_pDeepDlg->setPercent(0);
                        }
                       //CloseHandle(gThread);
                       
                    }
                }
                break;
                case IDC_FULL_CANCEL:
                {
                    if (NULL != m_hThreadId)
                    {
                        endThread(m_hThreadId);
                        m_hThreadId = NULL;
                    }
                    if (NULL != m_hdSector)
                    {
                        CloseHandle(m_hdSector);
                        m_hdSector = NULL;
                    }
                    OnCancel();
                }
                break;
            }
            break;
        case WM_NOTIFY:
        {
            if(IDC_FULL_LEFTTREE == wParam)
            {
                LPNMHDR lpnm_hdr = LPNMHDR(lParam);
                switch(lpnm_hdr->code)
                {
                    case NM_CLICK:
                    {
                        CPoint ptScreen;
                        ::GetCursorPos(&ptScreen);

                        LRESULT lResult = 0;

                        TVHITTESTINFO  tvhti;
                        tvhti.pt = ptScreen;
                        ScreenToClient(m_hLeftTree, &tvhti.pt);
                        tvhti.flags = LVHT_NOWHERE;
                        TreeView_HitTest(m_hLeftTree, &tvhti);

                        if(TVHT_ONITEM & tvhti.flags)
                        {
                            TVITEM tvItem = {0};
                            tvItem.mask = TVIF_PARAM;
                            tvItem.hItem = tvhti.hItem;
                            //Node node;
                            TreeView_GetItem(m_hLeftTree, &tvItem);
                            
                            if(TVHT_ONITEMSTATEICON & tvhti.flags) /*点击的是节点前面的checkbox */
                            {
                                lResult = setupDrive(m_hLeftTree, tvhti , ptScreen);
                            }
                        }
                        if (lResult == 3)
                            return LRESULT(0);
                        break;
                    }
                    break;
                   default:
                   return FALSE;
                }
            }

            if(IDC_FULL_LIST == wParam)
            {
                LPNMHDR lpnm_hdr = LPNMHDR(lParam);
                switch(lpnm_hdr->code)
                {
                    case NM_CLICK:
                    {
                        if (m_enState == SRCH_START)
                        {
                            CWnd::MessageBox(_T("正在扫描,请稍候"), _T("查看扇区"), MB_OK);
                            return FALSE;
                        }

                        string curSecDevName;
                        int secIndex = 0;
                        int selectSectorNum = getSelectedSectorNum(secIndex);
                        if (-1 != selectSectorNum)
                        {
                            curSecDevName = get_sectorDevNameByIndex(secIndex);
                            if (0 == curSecDevName.size())
                            {
                                CWnd::MessageBox(_T("提取扇区信息错误"), _T("错误"), MB_OK);
                                return FALSE;
                            }
                            DisplaySectorData(selectSectorNum, curSecDevName);
                        }
                        //CPersonListConfig::getInstance()->findByIndex(PRLIST_DETECTED, selectRowIndex, m_personInfo);/*保存当前选择人员信息*/
                        return TRUE;
                    }
                    break;
                   default:
                   return FALSE;
                }
            }
        }
        break;

        case WM_SIZE:
        {
            
        }
        break;
        default:
            break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return DialogProcDefault(uMsg, wParam, lParam);
}

string FullDiskScanDlg::get_sectorDevNameByIndex(int secIndex)
{
    for (int i = 0; i < m_secSize; i++)
    {
        if (secIndex <= m_pArrySecInfo[i].secIndex)
        {
            return m_pArrySecInfo[i].secDevName;
        }
    }

    return "";
}

void FullDiskScanDlg::set_runState(SEARCH_STATE enState)
{
    if (m_enState == SRCH_STOP) /*START*/
    {
        ::CWnd::SendMessage(m_hStart,WM_SETTEXT, 128, (LPARAM)TEXT("停止"));
        m_enState = enState;        
    }
    else if (m_enState == SRCH_START) /*STOP*/
    {
        ::CWnd::SendMessage(m_hStart,WM_SETTEXT, 128, (LPARAM)TEXT("开始"));
        PostMessage(m_hProgress, PBM_SETPOS, (WPARAM)0, 0L);
        SendMessage(m_hCurSectorText,WM_SETTEXT, 128, (LPARAM)TEXT(""));
        m_oneRowList.clear();
        m_enState = enState;

        if (NULL != m_hdSector)
        {
            CloseHandle(m_hdSector);
            m_hdSector = NULL;
        }

        if (NULL != m_hThreadId)
        {
                endThread(m_hThreadId);
        }
    }
    
}

int FullDiskScanDlg::AddItem(HWND hwnd,LPCTSTR szText, int nImage)
{
    LVITEM lvi = {0};
    lvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    
    lvi.iItem = ListView_GetItemCount(hwnd);

    return ListView_InsertItem( hwnd, &lvi );
}

void FullDiskScanDlg::DisplayResult(int lCurDriveIndex, HWND hwnd, ValueList& lTitleList, ValueList& lRowList)
{
    
        DWORD dwStyle = (DWORD)::GetWindowLongPtr(hwnd, GWL_STYLE);
        ::SetWindowLongPtr(hwnd, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
        ListView_SetExtendedListViewStyle(hwnd, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

        LV_COLUMN lvColumn = {0};
        lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = 120;

        int colNum = /*lTitleList.size()*/m_TitleColNum; //列数

        if (lRowList.empty())
        {
            return;
        }
        
        /*create row data*/
        int rowNum = lRowList.size() / colNum;
        int row = 0;
        for(row = 0; row < rowNum; row++)
        {
            if (lRowList.empty())  /*to avoid the thread is stopped by user*/
            {
                return;
            }
            int item = AddItem(hwnd, lRowList[row*colNum].c_str(), 3);
            for(int col = 1; col < colNum; col++)
            {
                if (lRowList.empty()) /*to avoid the thread is stopped by user*/
                {
                    return;
                }
                SetSubItem(hwnd, item, col, lRowList[col+row*colNum].c_str());
            }
            
            /*store the sector info for the display*/
            m_pArrySecInfo[lCurDriveIndex].secIndex = item;
            m_pArrySecInfo[lCurDriveIndex].secDevName = m_fmtSectorDevName;

        }
}

void FullDiskScanDlg::MatchKeywordsInSector(DWORD dwSectorIndex, char* lSectorBuf)
{
    ValueList lTitleList;   //no use, just for the param pass.
    char lTmp[512 * MAX_SECTOR_NUM + 1] = {0};
    char lTmp2[512] = {0};
    
    char firstKey;
    char* pKeywords = (char*)m_keywords.c_str();
    firstKey = pKeywords[0];
    char* p = NULL;
    char* p2 = NULL;
    
    if (NULL == lSectorBuf)
    {
        return;
    }

    memcpy(lTmp, lSectorBuf, 512 * MAX_SECTOR_NUM);

    p = lTmp;
    
    for (int i = 0; i < MAX_SECTOR_NUM; i++)
    {
      p = lTmp + i * 512;
      
      if (NULL == p)
      {
          return;
      }
      memcpy(lTmp2, p, 512);
      
      p2 = lTmp2;
      for(int j = 0; j < 512; j++)
      {
        if (lTmp2[j] == firstKey)
        {
            p2 = lTmp2 + j;
            int len = strlen(pKeywords);
            
            if (len > 512 - j)
            {
                break;
            }
            
            if (compareStrByMem(p2, pKeywords,/*512-j*/len))
            {
                char lstrNum[256] = {0};
                sprintf(lstrNum, "%d", (dwSectorIndex + i));
                m_oneRowList.push_back(lstrNum);
                m_oneRowList.push_back(pKeywords);

                //if ((dwSectorIndex % 1000)== 0)
                {
                    //DisplayResult(m_hList, lTitleList, m_oneRowList);
                    m_oneRowList.clear();
                }
                break;
            }
            else
            {
                continue;
            }
          }
        }
    }
}

bool FullDiskScanDlg::ReadSectors(int lCurDriveIndex, char* devName, __int64 lPartBegin, __int64 lPartEnd)
{
    char firstKey;
    char* pKeywords = NULL;
    char lTmp[128] = {0};
    int len = 0;
    DWORD dwCB = 0;
    DWORD percent=0;
    DWORD percent2=0;
    HANDLE mutexSector;
    ValueList lTitleList;   //no use, just for the param pass.
    
    char lstrNum[32] = {0};

    ValueList keywordsList;
    StringEx oStr;
    oStr.splitStr( m_keywords, ";", keywordsList);
    
    m_hdSector = CreateFile(devName, 
        GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    
    if (m_hdSector == INVALID_HANDLE_VALUE) 
    {
        CWnd::MessageBox(_T("打开句柄失败"), _T("错误"), MB_OK);
        return false;
    }

    DWORD dwClusterSize = MAX_SECTOR_NUM;
    for (__int64 i = lPartBegin; i <= lPartEnd; /*i++*/ i = i + dwClusterSize)
    {
          memset(lTmp, 0, 128);
          sprintf(lTmp, "%d", i);
          string x = lTmp;
          string statusInfo = "正在扫描"+ m_fmtSectorDevName + "磁盘 第 " + x + " 扇区\n";

          SendMessage(m_hCurSectorText,WM_SETTEXT, 128, (LPARAM)statusInfo.c_str());
          memset(m_pSecBuffer, 0, 512 * MAX_SECTOR_NUM);
          getSectorBuffer(m_hdSector, i, MAX_SECTOR_NUM, (char*)/*lpSectBuff*/m_pSecBuffer, false);
//          DWORD dwReadCnt = 0;
//          ReadFile(m_hdSector, m_pSecBuffer, 512 * MAX_SECTOR_NUM, &dwReadCnt, 0);

          char lEachSectorbuf[512] = {0};
          
          for(int idx = 0; idx < MAX_SECTOR_NUM; idx++)
          {
              memset(lEachSectorbuf, 0, 512);
              memcpy(lEachSectorbuf, &m_pSecBuffer[idx * 512], 512);
  
              char* p = NULL;
              p = lEachSectorbuf;

              //pKeywords = (char*)m_keywords.c_str();
              //firstKey = pKeywords[0];

              for(int j = 0; j < 512; j++)
              {
                  for (int k = 0; k < keywordsList.size(); k++) /*匹配每个关键词*/
                 {
                    pKeywords = (char*)keywordsList[k].c_str();
                    firstKey = pKeywords[0];      
                    if (lEachSectorbuf[j] == firstKey)
                    {
                        p = lEachSectorbuf + j;
                        len = strlen(pKeywords);
                        
                        if (len > 512 - j)
                        {
                            break;
                        }
                        
                        if (compareStrByMem(p, pKeywords,/*512-j*/len))
                        {
                            char lstrNum[128] = {0};
                            sprintf(lstrNum, "%d", (i+idx));
                            m_oneRowList.push_back(lstrNum);
                            m_oneRowList.push_back(pKeywords);
                            
                           if (m_bSaveResult) /*记录结果*/
                           {
                               string s;
                               ValueList lList;
                               convertSectorToString(s, (unsigned char*)lEachSectorbuf, j, ((512 - j) > 32)?(j+32):(512 - j)); /*只取32个字节*/
                               s += "\n";
                               lList.push_back(s);
                               FileOps::getInstance()->writeToFile((char*)m_saveName.c_str(), lList, true); /*append*/
                           }

                            //if ((i % 1000)== 0)
                            {
                                DisplayResult(lCurDriveIndex, m_hList, lTitleList, m_oneRowList);
                                m_oneRowList.clear();
                            }
                            break;
                        }
                        else
                        {
                            continue;
                        }
                     }
                  }
                }    
          }
          
          //MatchKeywordsInSector(i, lTmp);
          percent2=i*100/lPartEnd;
          if(percent!=percent2)
          {
               //CWnd::SendMessage(m_hProgress, PBM_SETPOS, (WPARAM)percent2, 0L);
               PostMessage(m_hProgress, PBM_SETPOS, (WPARAM)percent2, 0L);
               percent=percent2;
          }

          if (m_bCheckURL) /*check the url at the same time*/
          {
              m_pDeepDlg->CheckBuffer(m_pSecBuffer);
              m_pDeepDlg->setPercent(percent2);
          }
          
          if (i == ((i/dwClusterSize) * MAX_SECTOR_NUM) && ((lPartEnd%MAX_SECTOR_NUM) != 0))
          {
              dwClusterSize = 1;
          }
      }          

      CloseHandle(m_hdSector);
      m_hdSector = NULL;

      CWnd::PostMessage(m_hProgress, PBM_SETPOS, (WPARAM)0, 0L);
      
      return true;
}

__int64 FullDiskScanDlg::mySetFilePointer (HANDLE hf, __int64 distance, DWORD MoveMethod)

{

   LARGE_INTEGER li;


   li.QuadPart = distance;


   li.LowPart = SetFilePointer (hf,  li.LowPart, &li.HighPart, MoveMethod);


   if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() 

       != NO_ERROR)

   {

      li.QuadPart = -1;

   }


   return li.QuadPart;

} 


void FullDiskScanDlg::getSectorBuffer(HANDLE hDev, __int64 lSectorIndex, __int64 lSectorNum, char* lpSectBuff, bool bcloseFlag)
{
    char lErrorLogBuf[1024] = {0};
    DWORD dwCB = 0;
    PLONG lpDistanceToMoveHigh;
    
    HANDLE mutexSector;

#if 0
    if (NULL == hDev) //若为空，表示扫描完成，重新打开句柄
    {
            hDev = CreateFile(m_fmtSectorDevName.c_str(), GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            if (hDev == INVALID_HANDLE_VALUE) 
            {
                sprintf(lErrorLogBuf,"%s, %s %d",  "磁盘句柄无效,错误位置:", __FILE__, __LINE__);
                ::CWnd::MessageBox(NULL, lErrorLogBuf, _T("内部错误"), MB_OK);
                return;
            }
            m_hdSector = hDev;
    }
#endif

    //访问扇区,进行互斥操作
    mutexSector = CreateMutex(NULL, FALSE, _T("sectorMutex"));

    WaitForSingleObject(mutexSector, INFINITE); //等待互斥量空置（没有线程拥有它）
#if 0    
    DWORD ret = SetFilePointer(hDev, /*512 * lSectorIndex*/512 * lSectorIndex * lSectorNum, lpDistanceToMoveHigh, FILE_BEGIN);
#endif
   __int64 distance = 512 * lSectorIndex;
    __int64 ret = mySetFilePointer(hDev, distance, FILE_BEGIN);
    if(ret == /*INVALID_SET_FILE_POINTER*/-1) 
    {
      sprintf(lErrorLogBuf,"%s, %s %d",  "访问扇区错误,错误位置:", __FUNCTION__, __LINE__);
      CWnd::MessageBox(lErrorLogBuf, _T("内部错误"), MB_OK);

      ReleaseMutex(mutexSector);  //释放信号量
      CloseHandle(hDev);
      return;
    }

    ret = ReadFile(hDev, lpSectBuff, /*512 * 1*/512 * lSectorNum, &dwCB, NULL);

    ReleaseMutex(mutexSector);  //释放信号量
    if (bcloseFlag) //扫描完成后关闭句柄，显示单个扇区完成后关闭，再次显示时重新打开
    {
        CloseHandle(hDev);
    }
}

bool FullDiskScanDlg::getSectorAddress(char* devName, __int64& lPartBegin, __int64& lPartEnd)
{
       DWORD dwOutByte;            // DeviceIoControl 读取的字节数
       DWORD dwSectorByte;
       DISK_GEOMETRY diskGeometry;

       HANDLE hDev = CreateFile(devName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
       if (hDev == INVALID_HANDLE_VALUE) 
       {
           CWnd::MessageBox(_T("打开句柄失败"), _T("错误"), MB_OK);
           return false;
       }
       // 读取每扇区字节数
       BOOL bDeviceRet = DeviceIoControl(hDev, IOCTL_DISK_GET_DRIVE_GEOMETRY,
           NULL, 0, &diskGeometry, sizeof(DISK_GEOMETRY), &dwOutByte, NULL);
   
       // 每扇区字节数
       dwSectorByte = diskGeometry.BytesPerSector;
   
       // 读取分区大小和偏移量
       PARTITION_INFORMATION partInfo;
       bDeviceRet = DeviceIoControl(hDev, IOCTL_DISK_GET_PARTITION_INFO,
           NULL, 0, &partInfo, sizeof(PARTITION_INFORMATION), &dwOutByte, NULL);
   
       __int64 lPartitionSize = partInfo.PartitionLength.QuadPart;        // 分区的大小
       __int64 lOffset = partInfo.StartingOffset.QuadPart;                // 偏移
   
       // StartingOffset 除以每扇区字节数就是开始扇区；
       // PartitionLength 除以每扇区字节数就是扇区总数。
       lPartBegin = lOffset / dwSectorByte;
       lPartEnd = lPartitionSize / dwSectorByte;
   
       // 开始扇区加上扇区总数再减去1就是结束扇区。
       lPartEnd += lOffset;
       lPartEnd -= 1;
       CloseHandle(hDev);

       return true;
}

bool FullDiskScanDlg::compareStrByMem(void* s1, void* s2, int len)
{
    int totalLen = len;

    int i = 0;
    if (s1 == NULL || s2 == NULL)
    {
        return false;
    }

    while(len-- && *(char *)s1 == *(char *)s2)
    {
        s1 = (char *)s1 + 1;
        s2 = (char *)s2 + 1;
        i++;
    }

    if (i < totalLen)
    {
        return false;
    }
    else
    {
        return true;
    }
}

int FullDiskScanDlg::getSelectedSectorNum(int& secIndex)
{
    DWORD i = 0;
    DWORD n = 0;
    n   =   ListView_GetItemCount(m_hList); 
    for   (i   =   0;   i   <   n;   i++) 
    {
        if (ListView_GetItemState( m_hList, i,   LVIS_FOCUSED)   ==   LVIS_FOCUSED) 
        {
#if 0
            LVITEM Item = {0};
            Item.mask = LVIF_PARAM;
            Item.iItem = i;
            Item.iSubItem = 0;

            ListView_GetItem(m_hList, &Item);
            curSecDevName = (char*)Item.lParam;
            CWnd::MessageBox( (char*)Item.lParam, _T("扇区"), MB_OK);
#endif
            LVHITTESTINFO lvi = {0};
            char buf[32] = {0};
            int sectorNum = 0;
            lvi.iItem = i;
            lvi.iSubItem = 0;
            ListView_GetItemText(m_hList, lvi.iItem, /*g_lpInst->hti.iSubItem*/0, buf, sizeof buf);
            sectorNum = atoi(buf);
            secIndex = i;
            return sectorNum;
        }
    }

    return -1;
}

void FullDiskScanDlg::DisplaySectorData(int sectorIndex, string& curSecDevName)
{
    HANDLE hDev = NULL;
    unsigned char sectorBuf[512] = {0};
    char lTmp[128] = {0};

    /*if the scan finished, we should retrieve it again to get the the sector buffer*/
    if (NULL == m_hdSector)
    {
        hDev = CreateFile(curSecDevName.c_str()/*m_fmtSectorDevName.c_str()*/, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
        if (hDev == INVALID_HANDLE_VALUE) 
        {
            CWnd::MessageBox(_T("打开句柄失败"), _T("错误"), MB_OK);
            return;
        }
    }
    
    getSectorBuffer(hDev, sectorIndex, 1, (char*)sectorBuf, false);

    gSecStr.Empty();
    sprintf(lTmp, "偏移      |00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F");
    gSecStr += lTmp;
    gSecStr += "\r\n";
    memset(lTmp, 0, 128);
    sprintf(lTmp, "_______|______________________________________________");
    gSecStr += lTmp;
    gSecStr += "\r\n";
    memset(lTmp, 0, 128);
    sprintf(lTmp, "0x%04x  |", 0);
    gSecStr += lTmp;
    for(int idx = 0; idx < 512; idx++)
    {
        char subStr[128] = {0};
        sprintf(subStr, "%02x  ", sectorBuf[idx]);
        gSecStr += subStr;
        if((idx+1)%16 == 0)
        {
            gSecStr += "\r\n";
            if (idx != 511)
            {
                memset(lTmp, 0, 128);
                sprintf(lTmp, "0x%04x  |", idx+1);
                gSecStr += lTmp;
            }
        }
    }
        
    ::CWnd::SendMessage(m_hEdit1,WM_SETTEXT, gSecStr.GetLength(), (LPARAM)gSecStr.GetBuffer(0));
    gSecStr.Empty();
    for(int idx = 0; idx < 512; idx++)
    {
        char tmp[2] = {0};
        sprintf(tmp, "%c", sectorBuf[idx]);
        gSecStr += tmp;
        if((idx+1)%128 == 0)
        {
            gSecStr += "\r\n";
        }
    }
    ::CWnd::SendMessage(m_hEdit2,WM_SETTEXT, gSecStr.GetLength(), (LPARAM)gSecStr.GetBuffer(0));
    CloseHandle(hDev);
}

void FullDiskScanDlg::convertSectorToString(string& lOutString, unsigned char* pSectorBuf, unsigned int lStartIndex, unsigned int lEndIndex)
{
    while(lStartIndex < lEndIndex)
    {
        char tmp[2] = {0};
        sprintf(tmp, "%c", pSectorBuf[lStartIndex]);
        lOutString += tmp;
        lStartIndex++;
    }
}

void FullDiskScanDlg::set_DeepPointer(DeepDiskScanDlg* p)
{
    if (NULL != p)
    {
        m_pDeepDlg = p;
    }
}

void FullDiskScanDlg::clearAll()
{
    DiskScanDlg::clearAll();
    clearResult(m_hList);
    m_bCheckURL = false;
    m_resultList.clear();
}

/*DeepDiskScanDlg class*/
DeepDiskScanDlg::DeepDiskScanDlg(UINT id, HWND hwnd): DiskScanDlg(id, hwnd)
{
    m_hwnd = hwnd;
    m_urlList.clear();
    m_URLCount = 0;

    ValueList lTitleList;
    CFullDiskScan::getInstance()->get_titleList(lTitleList);
    m_TitleColNum = lTitleList.size();
    m_hdSector = NULL;
    m_bSaveResult = false;

    m_saveName = "深度涉密.log";

    if (CSysConfig::getInstance()->get_FolderChange())
    {
        string lFolderName;
        CSysConfig::getInstance()->get_filePath(lFolderName);
        m_saveName = lFolderName + "\\" + m_saveName;
    }
}

DeepDiskScanDlg::~DeepDiskScanDlg()
{
    //m_hLeftTree      = NULL;
    m_hList            = NULL;
    m_hProgress    = NULL;
    m_hStart         = NULL;
}

void DeepDiskScanDlg::setupHandle()
{
    m_hLeftTree      = CWnd::GetDlgItem(IDC_DEEP_LEFTTREE);
    m_hList             = CWnd::GetDlgItem(IDC_DEEP_LIST);
    m_hProgress     = CWnd::GetDlgItem(IDC_DEEP_PROGRESS);
    m_hStart          = CWnd::GetDlgItem(IDC_DEEP_START);
    m_hSaveResult = GetDlgItem(IDC_SAVELOG);
}


void DeepDiskScanDlg::BuildColumn()
{
        ListView_SetExtendedListViewStyle(m_hList, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

        LV_COLUMN lvColumn = {0};
        lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = 120;

        //get the title list
        ValueList titleList;
        CDeepDiskScan::getInstance()->get_titleList(titleList);
        
        int colNum = titleList.size(); //列数
        for(int i = 0; i < colNum; i++)
        {
            lvColumn.pszText = (char*)titleList[i].c_str();
            ListView_InsertColumn( m_hList, i, &lvColumn);
        }
}

BOOL DeepDiskScanDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg)
        {
        case WM_KEYDOWN:
        {
            if (VK_ESCAPE == wParam)
            {
                return TRUE;
            }
        }
        break;
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_DEEP_START:
                {
                    if (m_driveList.length() == 0)
                    {
                        CWnd::MessageBox(_T("请选择磁盘"), _T("提示"), MB_OK);
                         return FALSE;
                    }

                    m_bSaveResult = (::SendMessage(m_hSaveResult,  BM_GETCHECK,   0,   0) == BST_CHECKED)?true:false;
                    
                    if (m_enState == SRCH_STOP)
                    {
                        FileOps::getInstance()->remove(m_saveName);
                        DWORD dwThreadId = 0;
                        m_hThreadId=::CreateThread(NULL,0,SectorURLScanTask,this,0,&dwThreadId);
                        set_runState(SRCH_START);
                    }
                    else if (m_enState == SRCH_START)
                    {
                        endThread(m_hThreadId);
                         m_hThreadId = NULL;
                        set_runState(SRCH_STOP);
                    }
                }
                break;
                case IDC_DEEP_CANCEL:
                {
                    if (NULL != m_hThreadId)
                    {
                        endThread(m_hThreadId);
                        m_hThreadId = NULL;
                    }                    
                    OnCancel();
                }
                break;
            }
            break;
        case WM_NOTIFY:
        {
            if(IDC_DEEP_LEFTTREE == wParam)
            {
                LPNMHDR lpnm_hdr = LPNMHDR(lParam);

                CPoint ptScreen;
                ::GetCursorPos(&ptScreen);

                LRESULT lResult = 0;

                TVHITTESTINFO  tvhti;
                tvhti.pt = ptScreen;
                ScreenToClient(m_hLeftTree, &tvhti.pt);
                tvhti.flags = LVHT_NOWHERE;
                TreeView_HitTest(m_hLeftTree, &tvhti);

                switch(lpnm_hdr->code)
                {
                    case NM_CLICK:
                    {
                        CPoint ptScreen;
                        ::GetCursorPos(&ptScreen);

                        LRESULT lResult = 0;

                        TVHITTESTINFO  tvhti;
                        tvhti.pt = ptScreen;
                        ScreenToClient(m_hLeftTree, &tvhti.pt);
                        tvhti.flags = LVHT_NOWHERE;
                        TreeView_HitTest(m_hLeftTree, &tvhti);

                        if(TVHT_ONITEM & tvhti.flags)
                        {
                            TVITEM tvItem = {0};
                            tvItem.mask = TVIF_PARAM;
                            tvItem.hItem = tvhti.hItem;
                            //Node node;
                            TreeView_GetItem(m_hLeftTree, &tvItem);
                            
                            if(TVHT_ONITEMSTATEICON & tvhti.flags) /*点击的是节点前面的checkbox */
                            {
                                lResult = setupDrive(m_hLeftTree, tvhti , ptScreen);
                            }
                        }
                        if (lResult == 3)
                            return LRESULT(0);
                        break;
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
            //初始化显示数据内容
            setupHandle();
            BuildLeftTree(m_hLeftTree);
            BuildColumn();
        }
        break;
        default:
            break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return DialogProcDefault(uMsg, wParam, lParam);
}

void DeepDiskScanDlg::set_runState(SEARCH_STATE enState)
{
    if (m_enState == SRCH_STOP)
    {
        ::CWnd::SendMessage(m_hStart,WM_SETTEXT, 128, (LPARAM)TEXT("停止"));
    }
    else if (m_enState == SRCH_START)
    {
        ::CWnd::SendMessage(m_hStart,WM_SETTEXT, 128, (LPARAM)TEXT("开始"));
        CWnd::PostMessage(m_hProgress, PBM_SETPOS, 0, 0L);

        if (NULL != m_hdSector)
        {
            CloseHandle(m_hdSector);
            m_hdSector = NULL;
        }

        if (NULL != m_hThreadId)
        {
                endThread(m_hThreadId);
        }
    }
    
    m_enState = enState;
}

void DeepDiskScanDlg::DisplayResult(HWND hwnd, ValueList& lTitleList, ValueList& lRowList)
{
    DiskScanDlg::DisplayResult(hwnd, lTitleList, lRowList);

    if (m_bSaveResult) /*记录结果*/
    {
        string s;
        ValueList lList;
        for (int i = 0; i < lRowList.size(); i++)
        {
            s += lRowList[i] + "\t";
        }

        lList.push_back(s);
        FileOps::getInstance()->writeToFile((char*)m_saveName.c_str(), lList, true); /*append*/
    }

    //set_runState(SRCH_STOP);
}

BOOL DeepDiskScanDlg::CheckURL(char *szURL)
{
    return FALSE;
}

void DeepDiskScanDlg::CheckBuffer(char* pBuf)
{
char myUrlBuf[256]={0};
char szBuf[300];//代替CString
FILETIME FileTime,LocalFileTime;
SYSTEMTIME SystemTime;
BOOL result;
int i=0;
int offset=0;
do
{
   //Sleep(1);
   if ( strncmp(pBuf+i, "LEAK", 4) && strncmp(pBuf+i, "URL ", 4) )
   {
    if ( !strncmp(pBuf+i, "REDR", 4))
    {
     if ( !strncmp(pBuf + 0x10 + i, "http://", 7))
     {
      if ( strlen(pBuf + 0x10 + i)<= 0xFF )                //这里是判断字符串的长度
      {
       strcpy(myUrlBuf, pBuf + 0x10 + i); //小于255就直接strcpy拷过来
      }
      else
      {
       memcpy(myUrlBuf, pBuf + 0x10 + i, 0xFC); // 大于的话就只拷前252个字符
      }
      result = CheckURL(myUrlBuf);            // 检查得到的URL串
      if ( !result )                          // 若符合要求，则记录！
      {
       //printf("%s\n",myUrlBuf);
       ValueList lList;
       
       m_urlList.push_back(myUrlBuf);

      FileTime.dwHighDateTime = *(DWORD *)(pBuf + 0x14 + i);
      FileTime.dwLowDateTime = *(DWORD *)(pBuf + 0x10 + i);
      FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SystemTime);
      sprintf(szBuf,"%d年%d月%d日 %d:%d:%d",
       SystemTime.wYear,
       SystemTime.wMonth,
       SystemTime.wDay,
       SystemTime.wHour,
       SystemTime.wMinute,
       SystemTime.wSecond);

       m_urlList.push_back(szBuf);

       lList.push_back(myUrlBuf);
       lList.push_back(szBuf);
       m_URLCount++;                          // 计数

       ValueList titleList;
       CDeepDiskScan::getInstance()->get_titleList(titleList);

       DisplayResult(m_hList, titleList, lList);
      }
     }
    }
   }
   else
   {
    if ( !strncmp(pBuf + 0x60 + i, "http://", 7) )
    {
     offset=0x60;
LABEL_29:
     if ( strlen(pBuf + offset+ i)<= 0xFF )
     {
      strcpy(myUrlBuf, pBuf + offset + i);
     }
     else
     {
      memcpy(myUrlBuf, pBuf + offset + i, 0xFC);
     }
     result = CheckURL(myUrlBuf);
     if ( !result )
     {
      //printf("%s\n",myUrlBuf);
      FileTime.dwHighDateTime = *(DWORD *)(pBuf + 0x14 + i);
      FileTime.dwLowDateTime = *(DWORD *)(pBuf + 0x10 + i);
      FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
      FileTimeToSystemTime(&LocalFileTime, &SystemTime);
      sprintf(szBuf,"%d年%d月%d日 %d:%d:%d",
       SystemTime.wYear,
       SystemTime.wMonth,
       SystemTime.wDay,
       SystemTime.wHour,
       SystemTime.wMinute,
       SystemTime.wSecond);
       //printf("[%s]\n",szBuf);
       m_URLCount++;

       ValueList lList;
       
       m_urlList.push_back(myUrlBuf);
       m_urlList.push_back(szBuf);

       lList.push_back(myUrlBuf);
       lList.push_back(szBuf);

       ValueList titleList;
       CDeepDiskScan::getInstance()->get_titleList(titleList);

       DisplayResult(m_hList, titleList, lList);

     }
     goto LABEL_34;
    }
    else if( !strncmp(pBuf + 0x68 + i, "http://", 7) )
    {
     offset=0x68;
     goto LABEL_29;
    }
    else
    {
     if ( strncmp(pBuf + 0x60 + i, "Visited", 7) )
     {
      if ( !strncmp(pBuf + 0x68 + i, "Visited", 7))
      {
       if ( strstr(pBuf + 0x68 + i, "@http:")
        || strstr(pBuf + 0x68 + i, "@ftp:")
        || strstr(pBuf + 0x68 + i, "@java") )
       {
        offset = 0x71;
        goto LABEL_29;
       }
      }
     }
     else
     {
      if ( strstr(pBuf + 0x60 + i, "@http:")
       || strstr(pBuf + 0x60 + i, "@ftp:")
       || strstr(pBuf + 0x60 + i, "@java") )
      {
       offset = 0x69;
       goto LABEL_29;
      }
     }

    }
   }
LABEL_34:
    i += 0x80;
}
while ( i < 0xFF80u );
}

void DeepDiskScanDlg::SectorURLScan(HANDLE hDev, string lpath)
{
        clearResult();
        char szBuf[0x10000]; 
        LARGE_INTEGER Distance;
        ULARGE_INTEGER TotalNumberofBytes;
        DWORD TotalCnt=0;
        DWORD n=0;//循环计数
        DWORD dwReadCnt=0;
        DWORD percent=0,percent2=0;
        char* lTmp = (char*)lpath.c_str();
        char lTmp2[3] = {0};
        lTmp2[0] = lTmp[4]; lTmp2[1] = lTmp[5];
        string path2;
        
        path2 = lTmp2;
        path2 +=  "\\";
        GetDiskFreeSpaceEx(/*TEXT("C:\\")*/path2.c_str(),NULL,&TotalNumberofBytes,NULL);
        TotalCnt=TotalNumberofBytes.QuadPart>>16;

        Distance.QuadPart=0;
        Distance.LowPart=SetFilePointer(hDev, Distance.LowPart, &Distance.HighPart, FILE_BEGIN);
        for ( ; n< TotalCnt; ++n )
        {
           memset(szBuf, 0, 0x10000);
           ReadFile(hDev, szBuf, 0x10000, &dwReadCnt, 0);
           CheckBuffer(szBuf);

           percent2=n*100/TotalCnt;
           if(percent!=percent2)
           {
                PostMessage(m_hProgress, PBM_SETPOS, (WPARAM)percent2, 0L);
                percent=percent2;
           }
           //Sleep(500);
        }
}

void DeepDiskScanDlg::addResult(string& lStr)
{
    m_urlList.push_back(lStr);
}

void DeepDiskScanDlg::clearResult()
{
    m_urlList.clear();
    m_URLCount = 0;
    DiskScanDlg::clearResult(m_hList);
}

void DeepDiskScanDlg::getResult(ValueList& lList)
{
    lList = m_urlList;
}

void DeepDiskScanDlg::setAction(bool bVal)
{
    ::EnableWindow(m_hStart,!bVal);
}

void DeepDiskScanDlg::setPercent(DWORD lVal)
{
    PostMessage(m_hProgress, PBM_SETPOS, (WPARAM)lVal, 0L);
}

void DeepDiskScanDlg::clearAll()
{
    DiskScanDlg::clearAll();
    clearResult();
    m_urlList.clear();
    m_resultList.clear();
}

DiskTabDlg::DiskTabDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{
}

DiskTabDlg::~DiskTabDlg()
{
}

BOOL DiskTabDlg::OnInitDialog()
{
#if 0
    CRect rc; 
    rc = GetWindowRect(); 
    int  x,y;  
    float  multiple,H;      
            //取得分辨率 
    x=GetSystemMetrics(  SM_CXSCREEN  );  
    y=GetSystemMetrics(  SM_CYSCREEN  );      
    multiple=(float)x/1024;  //如果你布局时的分辨率不是1027*768，就改一下就行了。 
    H=(float)y/768; 
            //改变对话框的大小  
    this->MoveWindow((int)(rc.top*H),(int)(rc.left*multiple),(int)(rc.Width()*multiple),(int)(rc.Height()*H));  
#endif

    //CDialog::OnInitDialog();
    m_hWnd = GetActiveWindow();
    BuildData();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void DiskTabDlg::BuildData()
{
    // Set the Icon
    SetIconLarge(IDW_MAIN);
    SetIconSmall(IDW_MAIN);
    m_pBasicDlg = new BasicDiskScanDlg(IDD_BASICDISKSCAN, m_hWnd);
    m_pFullDlg = new FullDiskScanDlg(IDD_DISKFULLSCAN, m_hWnd);
    m_pDeepDlg = new DeepDiskScanDlg(IDD_DISKDEEPSCAN, m_hWnd);

    if (NULL == m_pBasicDlg || NULL == m_pFullDlg || NULL == m_pDeepDlg)
    {
        throw std::bad_alloc();
    }
    
    m_pFullDlg->set_DeepPointer(m_pDeepDlg);
    
    m_Tab.AttachDlgItem(IDC_TAB1, this);
    m_Tab.AddTabPage(m_pBasicDlg, _T("基本涉密检测"));
    m_Tab.AddTabPage(m_pFullDlg,   _T("全面涉密检测"));
    m_Tab.AddTabPage(m_pDeepDlg, _T("深度涉密检测"));

    //m_Tab.AddTabPage(new CComboBoxDialog(IDD_COMBOBOXES), _T("ComboBox Dialog"));
    m_Tab.SelectPage(0);

}

BOOL DiskTabDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg)
        {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDOK:
                {

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
#if 0
        case WM_CLOSE:
        {
            //delete this;
            m_pBasicDlg->clearAll();
            m_pFullDlg->clearAll();
            m_pFullDlg->clearAll();
            ShowWindow(SW_HIDE);
            return TRUE;
        }
        break;
#endif
        case WM_KEYDOWN:
        {
            if (VK_ESCAPE == wParam)
            {
                return TRUE;
            }
        }
        default:
            break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return DialogProcDefault(uMsg, wParam, lParam);
}

