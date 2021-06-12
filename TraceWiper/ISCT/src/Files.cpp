//////////////////////////////////////////////
// Files.cpp - Definitions for CViewFiles, CContainFiles
//             and DockFiles classes

#include "stdafx.h"

#include "resource.h"

#include <atlstr.h>
#include <sys/types.h>
#include <time.h>
#include "../../platform/SysGlobal.h"
#include "../../platform/RegisterOps.h"
#include "../../platform/CSector.h"
#include "../../platform/FileOps.h"
#include "cfgfile.h"
#include "Files.h"

#include "SysSettingDlg.h"

extern DWORD g_firefoxTaskId;
extern HANDLE g_firefoxThreadHandle;

extern "C" char *utf8ToMbcs(const char *zFilename);
HWND g_hWndFiles;

ValueList g_FireFoxValueList;
///////////////////////////////////////////////
// CViewFiles functions
CViewFiles::CViewFiles() : m_himlSmall(0)
{

}

CViewFiles::~CViewFiles()
{
    if (IsWindow()) DeleteAllItems();
    ImageList_Destroy(m_himlSmall);
}

void CViewFiles::OnInitialUpdate()
{
    // Set the image lists
    m_himlSmall = ImageList_Create(16, 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    HBITMAP hbm = LoadBitmap(MAKEINTRESOURCE(IDB_FILEVIEW));
    ImageList_AddMasked(m_himlSmall, hbm, RGB(255, 0, 255));
    SetImageList(m_himlSmall, LVSIL_SMALL);
    ::DeleteObject(hbm);
#if 0
    // Set the report style
    DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
    SetWindowLongPtr(GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
    SetExtendedStyle(LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES );

    SetColumns();
    InsertItems();
#endif
    g_hWndFiles = m_hWnd;

    /*read the personList info from the local file.*/
    FileOps::getInstance()->setupPRListFromFile(PRLIST_DETECTED);
    FileOps::getInstance()->setupPRListFromFile(PRLIST_SUSPECTED);
    FileOps::getInstance()->setupPRListFromFile(PRLIST_NORMAL);
    FileOps::getInstance()->setupPRListFromFile(PRLIST_EXCEPTION);
}


int CViewFiles::AddItem(LPCTSTR szText, int nImage)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.iItem = GetItemCount();

    return InsertItem(lvi);
}

void CViewFiles::SetColumns()
{
    //empty the list
    DeleteAllItems();

    //initialise the columns
    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    TCHAR szString[9][20] = {_T("目标ID"), _T("清除项名称"), TEXT("清除项类型"), _T("清除项值"), _T("清除时间"), _T("剩余时间"), _T("进度1"), _T("进度2"), _T("清除结果")};
    for(int i = 0; i < 9; ++i)
    {
        lvColumn.pszText = szString[i];
        InsertColumn(i, lvColumn);
    }
}

BOOL CViewFiles::SetSubItem(int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    return (BOOL)SendMessage(LVM_SETITEM, 0L, (LPARAM)&lvi1);
}

void CViewFiles::InsertItems()
{
    // Add 4th item
    /*int item = AddItem(_T("IE地址栏痕迹"), 2);
    SetSubItem(item, 1, _T("注册表类"));
    SetSubItem(item, 2, _T("HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\TypedUrls"));
    SetSubItem(item, 3, _T("2010-09-10 12:55"));
    SetSubItem(item, 4, _T("0"));
    SetSubItem(item, 5, _T("0"));
    SetSubItem(item, 6, _T("0"));
    SetSubItem(item, 7, _T("成功"));*/
}

LRESULT CViewFiles::OnActionNotify(WPARAM wParam, LPARAM lParam)
{
    (void)lParam;
    CString str1 = CString("");
    CString str2 = CString("");
    CString str3 = CString("");
    CString str0 = CString("");
#if 0
    str0.Format("%d", eraserRetrieveValueContextID(lParam));
    str1.Format("%d", eraserRetrieveValueProgressTimeLeft(lParam));
    str2.Format("%d", eraserRetrieveValueProgressPercent(lParam));
    str3.Format("%d", eraserRetrieveValueProgressTotalPercent(lParam));

    int item = eraserRetrieveValueListIndex(lParam);
#endif

    switch (wParam)
    {
    
#if 0
    case ERASER_WIPE_BEGIN:
        {
            item = AddItem(str0, 2);
            //SetSubItem(item, 1, str0);
            SetSubItem(item, 1, _T("注册表类"));
            SetSubItem(item, 2, _T("注册表类"));
            SetSubItem(item, 3, _T("HKEY_CURRENT_USER\\Software\\Microsoft\\Internet Explorer\\TypedUrls"));
            SetSubItem(item, 4, _T("2010-09-10 12:55"));
            SetSubItem(item, 5, str1);
            SetSubItem(item, 6, str2);
            SetSubItem(item, 7, str3);
            SetSubItem(item, 8, _T("清除..."));
            eraserRetrieveValueListIndexSet(lParam, item);
            break;
        }
    case ERASER_WIPE_UPDATE:
        {
            /*LVITEM  lvi = {0};

            lvi.mask  = LVIF_PARAM;
            lvi.iItem = item;
            GetItem(&lvi);*/

            
            SetSubItem(item, 5, str1);
            SetSubItem(item, 6, str2);
            SetSubItem(item, 7, str3);

            
            break;
        }
    case ERASER_WIPE_DONE:
        //EraserWipeDone();
        //TRACE("ERASER_WIPE_DONE\n");
        break;
#endif
    default:
        break;
    }

    return TRUE;
}


LRESULT CViewFiles::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        //added by johnz
        case WM_COMMAND:
        {    
            switch(wParam)
            {
            case 0xfedc:
            {
                int nCol;
                char **azVals;
                char **azCols;
                int *aiTypes;
                char lBuffer[128] = {0};
                double lLongTime = 0;
                g_FireFoxValueList.clear();
                
                nCol = (int)(((DWORD*)lParam)[1]);
                azVals = (char**)(((DWORD*)lParam)[2]);
                azCols = (char**)(((DWORD*)lParam)[3]);
                aiTypes = (int*)(((DWORD*)lParam)[4]);
                g_FireFoxValueList.push_back(utf8ToMbcs(azVals[0]));
#if 0
                if (azVals && azCols)
                {
                    for (int ii = 0; ii < nCol; ii++)
                    {
                        g_FireFoxValueList.push_back(azVals[ii]);
                        //int item = AddItem((LPCTSTR)utf8ToMbcs(azVals[ii]), 3); //添加行
                        ii++;
                        for(int col = 1; col < 2; col++) //在行中设置列
                        {
                        #if 1
                           FILETIME   ftime;
                           SYSTEMTIME   stime;

                           if (NULL != azVals[ii])
                           {
                               lLongTime = atoi(azVals[ii]);
                               VariantTimeToSystemTime(lLongTime, &stime);
                               sprintf(lBuffer,"%04d年%02d月%02d日 %02d:%02d:%02d",stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute,stime.wSecond);
                               g_FireFoxValueList.push_back(lBuffer);
                           }
                           else
                           {
                               g_FireFoxValueList.push_back("Error Time");
                           }
                       #else
                           char* ascmodtime = NULL;
                           time_t modtime = 127521288564UL;
                           ascmodtime = ctime( &modtime);
                           SetSubItem(item, col, (LPCTSTR)ascmodtime);
                       #endif
                        }
                    }
                }
#endif
                int   flagThread; 
                DWORD   ExitCode   =   0; 
                flagThread   =   GetExitCodeThread(g_firefoxThreadHandle,&ExitCode); 
                flagThread   =   TerminateThread(g_firefoxThreadHandle,ExitCode); 
                flagThread   =   CloseHandle(g_firefoxThreadHandle); 

                break;
            }
            case ISCT_DETECT_DISPLAYRESULT:/*Display the detect result*/
            {
                SetCursor(LoadCursor(NULL, IDC_WAIT)); /*set cursor to waiting state*/

                //empty the list
                DeleteAllItems();

                DNode* pDNode = (DNode*)lParam;
                if (NULL == pDNode)
                {
                    break;
                }

                string lFolderName;

                if (CSysConfig::getInstance()->get_FolderChange()) /*日志路径已经改变*/
                {
                    string lsubFolderName;
                    CSysConfig::getInstance()->get_filePath(lFolderName);
                    CSysConfig::getInstance()->get_subfilePath(lsubFolderName);
                    lFolderName += "\\" + lsubFolderName;
                }
                else
                {
                    FileOps::getInstance()->getFolder(lFolderName); 
                }
                
                string lFileName =lFolderName + "\\" + pDNode->getName() + FILE_POSTFIX_NAME;

                /*if detect result does exists in the log file, only need to get from it.*/
                bool bExists = FileOps::getInstance()->isFileExists((char*)lFileName.c_str());
                if (bExists)
                {
                    ValueList lList;
                    FileOps::getInstance()->readFromFile((char*)lFileName.c_str(), lList);
                    pDNode->setLineResult(lList);

                    /*get the column titles*/
                    ValueList lTitleList;
                    ValueList rowList;
                    pDNode->getTitleList(lTitleList);
                    pDNode->getLineResult(rowList);
                    DisplayResult(lTitleList, rowList, pDNode);
                }
                
                SetCursor(LoadCursor(NULL, IDC_ARROW)); /*resume the cursor*/
            }
            break;
            case ISCT_DETECT_0:
                {
                                        
                    DNode* pDNode = (DNode*)lParam;
                    int lcfgType = pDNode->getCfg()->getCfgType();
                    if (TYPE_ACTION_NODE == lcfgType)
                    {
                        //Delete all the columns firstly
                        for(int i = 0;i < 100;i++)
                        {
                            DeleteColumn(0);
                        }
                        DeleteAllItems();

                        SetCursor(LoadCursor(NULL, IDC_WAIT)); /*set cursor to waiting state*/
                        string lFolderName;
                        FileOps::getInstance()->getFolder(lFolderName); 
                        string lFileName =lFolderName + "\\" + pDNode->getName() + FILE_POSTFIX_NAME;

                        /*if detect result does exists in the log file, only need to get from it.*/
                        bool bExists = FileOps::getInstance()->isFileExists((char*)lFileName.c_str());
                        if (bExists)
                        {
                            ValueList lList;
                            FileOps::getInstance()->readFromFile((char*)lFileName.c_str(), lList);
                            pDNode->setLineResult(lList);
                        }
                        else
                        {
                            pDNode->excute(pDNode->getName(),CMD_START);
                        }

                        /*get the column titles*/
                        ValueList lTitleList;
                        ValueList rowList;
                        pDNode->getTitleList(lTitleList);
                        pDNode->getLineResult(rowList);
                        DisplayResult(lTitleList, rowList, pDNode);
                        //FileOps::getInstance()->writeToExcel(pDNode->getName(), pDNode->getName(), lTitleList, rowList);
                        if (!bExists)
                        {
                            /*write to log file*/
                            FileOps::getInstance()->writeToFile((char*)lFileName.c_str(), rowList);
                        }
                        SetCursor(LoadCursor(NULL, IDC_ARROW)); /*resume the cursor*/
                    }
                    break;
                }
                case ISCT_PERSONLIST_MSG001:
                {

                    PRLIST_TYPE enPRType = CPersonListConfig::getInstance()->get_selectedType();
                    ValueList titleList;
                    ValueList rowList; /*the person List information for display*/

                    switch(enPRType)
                    {
                        case PRLIST_DETECTED:
                        case PRLIST_SUSPECTED:
                        case PRLIST_NORMAL:
                        case PRLIST_EXCEPTION:
                        {
                            ValueList titleList;
                            ValueList rowList;
                            CPersonListConfig::getInstance()->get_titleList(titleList);
                            FileOps::getInstance()->setupPRListFromFile(enPRType);
                            CPersonListConfig::getInstance()->convertListToLine(enPRType, rowList); 
                            DisplayResult(titleList, rowList);
                        }
                        break;
                        default:
                            break;
                    }
                }
                break;
        case ISCT_PERSONLIST_MSG_CREATE:
            {
                HWND hPrevFocus = ::GetFocus();
                PersonListSettingDlg* pPerson = new PersonListSettingDlg(IDD_PERSONLIST, m_hWnd);

                if (NULL == pPerson)
                    throw std::bad_alloc();

                CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_ADD);//设置操作类型

                pPerson->DoModal();

                // Clean up
                delete pPerson;
                pPerson = NULL;
                ::SetFocus(hPrevFocus);
                
                //显示结果
                ValueList titleList;
                ValueList rowList;
                CPersonListConfig::getInstance()->get_titleList(titleList);
                CPersonListConfig::getInstance()->convertListToLine(PRLIST_DETECTED, rowList); 
                DisplayResult(titleList, rowList);
            }
            break;
        //修改名单
        case ISCT_PERSONLIST_MSG_MODIFY:
            {
                int selectRowIndex = getSelectedRowNum();
                if (selectRowIndex >= 0)
                {
                    PRLISTDATA stInfo;
                    PRLIST_TYPE enType = CPersonListConfig::getInstance()->get_selectedType();
                    CPersonListConfig::getInstance()->findByIndex(enType, selectRowIndex, stInfo);
                    CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_MODIFY);
                    CPersonListConfig::getInstance()->set_selectIndex(selectRowIndex);
                    /*显示修改信息*/
                    HWND hPrevFocus = ::GetFocus();
                    PersonListSettingDlg* pPerson = new PersonListSettingDlg(IDD_PERSONLIST, m_hWnd);
                    if (NULL == pPerson)
                        throw std::bad_alloc();

                    pPerson->setParams(stInfo.pr_name, stInfo.pr_depart, stInfo.pr_ipaddr, stInfo.pr_hostname, stInfo.pr_starttime);

                    pPerson->DoModal();

                    // Clean up
                    delete pPerson;
                    pPerson = NULL;
                    ::SetFocus(hPrevFocus);
                    
                    //显示结果
                    ValueList titleList;
                    ValueList rowList;
                    CPersonListConfig::getInstance()->get_titleList(titleList);
                    CPersonListConfig::getInstance()->convertListToLine(PRLIST_DETECTED, rowList); 
                    DisplayResult(titleList, rowList);
                }
            }
            break;
        case ISCT_PERSONLIST_MSG_DELETE: /*删除人员*/
        {
            IntList selectIndexes = getMultiSelectedRowNum();
            //int selectRowIndex = getSelectedRowNum();
            PRLIST_TYPE enType = CPersonListConfig::getInstance()->get_selectedType();
            int totalNum = selectIndexes.size();
            if (totalNum <= 0)
            {
                break;
            }
            
            PRLISTDATA* pData = new PRLISTDATA[totalNum];
            memset(pData, 0, sizeof(PRLISTDATA) * totalNum);

            /*以下批量删除*/
            for (int i = 0; i < totalNum; i++)
            {
                DWORD selectRowIndex = selectIndexes[i];
                PRLISTDATA stInfo;
                
                CPersonListConfig::getInstance()->findByIndex(enType, selectRowIndex, stInfo);
                CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_DELETE);
                CPersonListConfig::getInstance()->set_selectIndex(selectRowIndex);
                //CPersonListConfig::getInstance()->remove_configList(enType, stInfo);
                pData[i] = stInfo;
            }

            for (int j = 0; j < totalNum; j++)
            {
                CPersonListConfig::getInstance()->remove_configList(enType, pData[j]);
            }
            delete []pData;
            pData = NULL;
            
            //显示结果
            ValueList titleList;
            ValueList rowList;
            CPersonListConfig::getInstance()->get_titleList(titleList);
            CPersonListConfig::getInstance()->convertListToLine(enType, rowList); 
            DisplayResult(titleList, rowList);
        }
        break;
        case ISCT_PERSONLIST_MSG_TONORMAL:
        case ISCT_PERSONLIST_MSG_TOSUSPECT:
        case ISCT_PERSONLIST_MSG_TOEXCEPTION:
        {
            /*支持批量转移*/
            IntList selectIndexes = getMultiSelectedRowNum();
            int totalNum = selectIndexes.size();
            if (totalNum <= 0)
            {
                break;
            }

            PRLIST_TYPE enType;

            if (wParam == ISCT_PERSONLIST_MSG_TONORMAL)
            {
                CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_SETNORMAL);
                enType = PRLIST_NORMAL;
            }
            else if (wParam == ISCT_PERSONLIST_MSG_TOSUSPECT)
            {
                CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_SETSUSPECT);
                enType = PRLIST_SUSPECTED;
            }
            else if (wParam == ISCT_PERSONLIST_MSG_TOEXCEPTION)
            {
                CPersonListConfig::getInstance()->set_actionType(PRLIST_ACTION_SETEXCEPTION);
                enType = PRLIST_EXCEPTION;
            }
            
            for (int i = 0; i < totalNum; i++)
            {
                DWORD selectRowIndex = selectIndexes[i];
                PRLISTDATA stInfo;

                CPersonListConfig::getInstance()->findByIndex(PRLIST_DETECTED, selectRowIndex, stInfo);
                CPersonListConfig::getInstance()->set_selectIndex(selectRowIndex);

                if (CPersonListConfig::getInstance()->isExists(enType, stInfo))
                {
                    char lTmp[128] = {0};
                    sprintf(lTmp, "第 %d 记录已存在", selectRowIndex);
                    ::MessageBox(m_hWnd, lTmp, _T("提示"), MB_OK);
                    continue; 
                }
                else
                {
                    CPersonListConfig::getInstance()->add_configList(enType, stInfo);
                }
            }
        }
        break;
        case ISCT_PERSONLIST_MSG_VIEWRESULT:
        {
            int selectRowIndex = getSelectedRowNum();
            if (selectRowIndex >= 0)
            {
                PRLISTDATA stInfo;
                PRLIST_TYPE enType;
                CPersonListConfig::getInstance()->findByIndex(PRLIST_DETECTED, selectRowIndex, stInfo);
                CPersonListConfig::getInstance()->set_selectIndex(selectRowIndex);

                string lDirectoryName = stInfo.pr_name + "-" + stInfo.pr_starttime;;

                if (CSysConfig::getInstance()->get_FolderChange())
                {
                    string folder;
                    CSysConfig::getInstance()->get_filePath(folder);
                    lDirectoryName = RegisterOps::getInstance()->getCheckingIpAddress()
                                            + "-" + RegisterOps::getInstance()->getComputerName() + "-" + lDirectoryName;
                    folder += "\\" + lDirectoryName;
                    CSysConfig::getInstance()->set_subfilePath(lDirectoryName);
                    if (!FileOps::getInstance()->checkFolderExist(folder))
                    {
                        ::MessageBox(m_hWnd, _T("本地历史文件不存在,请确认是否已被删除或更改"), _T("错误"), MB_OK);
                    }
                }
                else
                {
                    if (!FileOps::getInstance()->setFolder(lDirectoryName))
                    {
                        ::MessageBox(m_hWnd, _T("本地历史文件不存在,请确认是否已被删除或更改"), _T("错误"), MB_OK);
                    }
                }
            }

        }
        break;
        default:
        break;
            }
            break;
        }
    case WM_CREATE:
        TRACE("Files\n");
            break;
    case WM_RBUTTONDOWN:
        {
            PRLIST_TYPE enType = CPersonListConfig::getInstance()->get_selectedType();

            /*禁止在其它窗口显示弹出菜单*/
            if (PRLIST_INVALID ==  enType)
            {
                break;
            }
            POINT stPos;
            stPos.x = LOWORD (lParam) ;
            stPos.y = HIWORD (lParam) ;

            HMENU hMenu;

            hMenu=CreatePopupMenu();
            
            if (PRLIST_DETECTED == enType)
            {
                //AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_CREATE, "新建名单");
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_MODIFY, "修改名单");
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_DELETE, "删除名单");
                AppendMenu(hMenu,MF_SEPARATOR, 0, 0);
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_TONORMAL, "列入正常");
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_TOSUSPECT, "列入可疑");
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_TOEXCEPTION, "列入异常");
                AppendMenu(hMenu,MF_SEPARATOR, 0, 0);
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_VIEWRESULT, "读取检测记录");
            }
            else
            {
                AppendMenu(hMenu,MF_STRING,(UINT)ISCT_PERSONLIST_MSG_DELETE, "删除名单");
            }
            ClientToScreen(m_hWnd,&stPos); 
            TrackPopupMenu(hMenu, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x,
                stPos.y, NULL, m_hWnd, NULL);
            break;
        }
    case WM_LBUTTONDOWN:
    {
        TRACE("WM_LBUTTONDOWN\n");
    }
        break;
    case WM_LBUTTONUP:
    {
        TRACE("WM_LBUTTONUP\n");
        break;
    }
    case WM_DESTROY:
        {
            SetImageList(NULL, LVSIL_SMALL);
            break;
        }
    case WM_NOTIFY:
    {
        LPNMHDR lpnm_hdr = LPNMHDR(lParam);
        switch(lpnm_hdr->code)
        {
        case NM_RELEASEDCAPTURE:
        {

                CPoint ptScreen;
                ::GetCursorPos(&ptScreen);
                char aaa[48] = {0};
                sprintf(aaa,"HELLO %x\n", lpnm_hdr->code);
                TRACE(aaa);

                LRESULT lResult = 0;

                LVHITTESTINFO  tvhti;
                tvhti.pt = ptScreen;

                HitTest( tvhti);

                DWORD dwpos = GetMessagePos();
                ScreenToClient(m_hWnd, &tvhti.pt);
                tvhti.flags = LVHT_NOWHERE;
                tvhti.pt.x = GET_X_LPARAM(dwpos);
                tvhti.pt.y = GET_Y_LPARAM(dwpos);
                MapWindowPoints(HWND_DESKTOP, m_hWnd, &tvhti.pt, 1);
                SubItemHitTest(tvhti);

#if 0
                if(LVHT_ONITEM & tvhti.flags)
                {
                    sprintf(aaa,"HELLO  HELLO %x\n", tvhti.flags);
                    TRACE(aaa);
                    LVITEM tvItem = {0};
                    tvItem.mask = LVIF_PARAM;
                    tvItem.iItem = tvhti.iItem;
                    //Node node;
                    ListView_GetItem(m_hWnd, &tvItem);
                    int x = 0;
                    /*if(TVHT_ONITEMSTATEICON & tvhti.flags) 
                    {
                        lResult = setupDrive(m_hLeftTree, tvhti , ptScreen);
                    }*/


            }
#endif
                break;
            }
            break;
        default:
            break;
        }
#if 0
        LPNMHDR lpnmh;
        lpnmh = (NMHDR*)lParam;
        LPNMLISTVIEW    lpnlv;
        char lBuf[32] = {0};
        sprintf(lBuf, "--------->:0x%x\n", lpnmh->code);
        TRACE(lBuf);
        switch (lpnmh->code) 
        {

        case LVN_COLUMNCLICK: // 点击标题列
            {
#if 0
                lpnlv = (LPNMLISTVIEW)lParam;

                if (0 == dwSort[lpnlv->iSubItem]) 
                {
                    dwSort[lpnlv->iSubItem] = 1;
                }
                else 
                {
                    dwSort[lpnlv->iSubItem] = 0;
                }
                SendMessage(lpnmh->hwndFrom, LVM_SORTITEMSEX, lpnlv->iSubItem, (LPARAM)SortProc);
#endif
                ::MessageBox(m_hWnd, _T("test"), _T("提示"), MB_OK);
            }
            break;
        }
#endif
    }
    break;
    }

    return WndProcDefault(uMsg, wParam, lParam);
}

void CViewFiles::DisplayResult(ValueList& titleList,  ValueList& rowList, DNode* pDNode)
{
    if (titleList.empty())
    {
        return;
    }

    //Delete all the columns firstly
    for(int i = 0;i < 100;i++)
    {
        DeleteColumn(0);
    }
    DeleteAllItems();

    DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
    SetWindowLongPtr(GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
    SetExtendedStyle(LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
    
    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
    lvColumn.fmt = LVCFMT_LEFT;
    
    
    /*create column titles*/
    int colNum = titleList.size();
    for(int i = 0; i < colNum; i++)
    {
        if (NULL == pDNode)
        {
            lvColumn.cx = 120;
        }
        else
        {
            lvColumn.cx = pDNode->getNodeTitle(i)->getTitleWidth();
        }
        lvColumn.pszText = (char*)titleList[i].c_str();
        InsertColumn(i, lvColumn);
    }

    /*create row data*/
    int rowNum = rowList.size() / colNum;
    int row = 0;
    for(row = 0; row < rowNum; row++)
    {
        int item = AddItem(rowList[row*colNum].c_str(), 3);
        for(int col = 1; col < colNum; col++)
        {
            SetSubItem(item, col, rowList[col+row*colNum].c_str());
        }
    }
}

int CViewFiles::getSelectedRowNum()
{
    DWORD i = 0;
    DWORD n = 0;
    n   =   GetItemCount(); 
    for   (i   =   0;   i   <   n;   i++) 
    {
        if (GetItemState(i,   LVIS_FOCUSED)   ==   LVIS_FOCUSED) 
        {
            return i;
        }
    }

    return -1;
}

IntList CViewFiles::getMultiSelectedRowNum()
{
    IntList lList;
    DWORD i = 0;
    DWORD n = 0;
    n   =   GetItemCount(); 
    for   (i   =   0;   i   <   n;   i++) 
    {
        if (GetItemState(i,   /*LVIS_FOCUSED*/LVIS_SELECTED)   ==   /*LVIS_FOCUSED*/LVIS_SELECTED) 
        {
            lList.push_back(i);
        }
    }

    return lList;
}

///////////////////////////////////////////////
// CContainFiles functions
CContainFiles::CContainFiles() 
{
    SetTabText(_T("FileView"));
    SetTabIcon(IDI_FILEVIEW);
    SetDockCaption (_T("File View - Docking container"));
    SetView(m_ViewFiles);
}

/////////////////////////////////////////////////
//  Definitions for the CDockFiles class
CDockFiles::CDockFiles() 
{ 
    SetView(m_Files); 
}

void CDockFiles::OnInitialUpdate()
{
    // Set the width of the splitter bar
    SetBarWidth(8);
}
