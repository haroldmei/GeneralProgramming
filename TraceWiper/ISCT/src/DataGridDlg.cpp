/****************************************************************************
 *                                                                          *
 * File    : main.c                                                         *
 *                                                                          *
 * Purpose : Generic dialog based Win32 application.                        *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

/* 
 * Either define WIN32_LEAN_AND_MEAN, or one or more of NOCRYPT,
 * NOSERVICE, NOMCX and NOIME, to decrease compile time (if you
 * don't need these defines -- see windows.h).
 */
#define UNICODE

#define WIN32_LEAN_AND_MEAN
/* #define NOCRYPT */
/* #define NOSERVICE */
/* #define NOMCX */
/* #define NOIME */
#include "../../platform/libxml/xmlTree.h"

//#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "comutil.h"
#include "resource.h"
#include "DataGridView.h"
#include "CfgFile.h"
#include "../../platform/CSector.h"

void getAllNodesName(NameList &pAllNodes)
{
    NameList lTreeNodeName;
    XmlTree::getInstance()->treeNodes(&lTreeNodeName);
    for(unsigned int i = 0; i < lTreeNodeName.size(); i++)
    {
        BoolList lChecked;
        NameList lSubTreeName;
        XmlTree::getInstance()->subTreeNodes(lTreeNodeName[i], &lSubTreeName);

        for(unsigned int j = 0; j < lSubTreeName.size(); j++)
        {
            NameList lNodeNames;
            XmlTree::getInstance()->nodes(lTreeNodeName[i], lSubTreeName[j], &lNodeNames, &lChecked);
            for(unsigned int k = 0; k < lNodeNames.size(); k++)
            {
                pAllNodes.push_back(lNodeNames[k]);
            }
        }
    }
}


#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

#define Refresh(A) RedrawWindow(A,NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_UPDATENOW);

/** Global variables ********************************************************/

int gsLineNum = 0;

static HANDLE ghInstance;
HWND hList1;

//A nice assortment of pastel custom colors
static COLORREF gaCustomColors[] = {
        0xDCDCDC, 0xCDCDDB, 0xBDBED2, 0xE3C4C3,
        0xFFE0C0, 0xE0C8AB, 0xD7AFB0, 0xF0BFEB,
        0xE2A7DA, 0x9697FF, 0xBFBFFF, 0xBBDBFF,
        0xCBFFFF, 0xBFEFCB, 0xA6DEB3, 0x82D28B };

void MainDlg_OnClose(HWND hwnd)
{
    ghInstance = NULL;
    gsLineNum = 0;
    CEditor::getInstance()->set_previewFlag(false);

    EndDialog(hwnd, 0);
}

void MainDlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    static int RowHeight = 20;
    switch(id)
    {
        case IDC_GRIDLINES:
        {
            ListView_SetExtendedListViewStyle(hList1,LVS_EX_GRIDLINES ^ ListView_GetExtendedListViewStyle(hList1));
        }
        break;

        case IDC_RESIZE_COL:
        {
            static BOOL fSet = FALSE;

            DataGridView_SetResizableHeader(hList1,fSet);
            fSet = !fSet;
        }
        break;

        case IDC_COLOR_BK:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = ListView_GetBkColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetBkColor(hList1,cc.rgbResult);
                Refresh(hList1);
            }
        }
        break;

        case IDC_ALT_CLR_ROW:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = DataGridView_GetAltBkColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetAltBkColor(hList1,cc.rgbResult,TRUE); 
                Refresh(hList1);
            }
        }
        break;

        case IDC_ALT_CLR_COL:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = DataGridView_GetAltBkColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetAltBkColor(hList1,cc.rgbResult,FALSE); 
                Refresh(hList1);
            }
        }
        break;

        case IDC_TXT_CLR:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = ListView_GetTextColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetTextColor(hList1,cc.rgbResult); 
                Refresh(hList1);
            }
        }
        break;

        case IDC_ALT_TXT_CLR:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = DataGridView_GetAltTextColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetAltTextColor(hList1,cc.rgbResult); 
                Refresh(hList1);
            }
        }
        break;

        case IDC_COLOR_EDIT:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = DataGridView_GetEditorBkColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetEditorBkColor(hList1,cc.rgbResult); 
                Refresh(hList1);
            }
        }
        break;

        case IDC_TXTCOLOR_EDIT:
        {
            CHOOSECOLOR cc; 
            memset(&cc, 0, sizeof(cc));
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = hwnd;
            cc.hInstance = (HWND)ghInstance;
            cc.rgbResult = DataGridView_GetEditorTxtColor(hList1);
            cc.lpCustColors = (LPDWORD) gaCustomColors;
            cc.Flags = CC_FULLOPEN | CC_RGBINIT;
            if (ChooseColor(&cc))
            {
                DataGridView_SetEditorTxtColor(hList1,cc.rgbResult); 
                Refresh(hList1);
            }
        }
        break;

        case IDC_ROWHDR:
        {
            static BOOL fRwHdr = TRUE;
            DataGridView_DisplayRowHeaders(hList1, fRwHdr);
            fRwHdr = !fRwHdr;
        }
        break;

        case IDC_PLUS:
        {
            RowHeight++;
            DataGridView_SetRowHeight(hList1,RowHeight);
        }
        break;

        case IDC_MINUS:
        {
            RowHeight--;
            DataGridView_SetRowHeight(hList1,RowHeight);
        }
        break;
        case IDC_SCORE_SAVE:
        {
            //TO DO...
            SetCursor(LoadCursor(NULL, IDC_WAIT));
            pDetectorRoot->treeSave(); /*cannot save to file, need to be fixed.*/
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            ::MessageBox(hwnd, _T("保存完成"), _T("提示"), MB_OK);
        }
        break;
        case IDC_SCORE_CLOSE:
        {
            MainDlg_OnClose(hwnd);
        }
        break;
        case IDC_SCORE_DEFAULT:
        {
            //TO DO...
        }
        break;
        case IDC_SCORE_CALC:
        {
            PreviousResult(hwnd);
        }
        break;
        case IDC_SCORE_DISPLAY:
        {
            DisplaySetting(hwnd);
        }
        break;
    }
    // TODO: Add your message processing code here...
}

void MainDlg_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rcDlg, rcGrid;
    GetClientRect(hwnd,&rcDlg);
    GetClientRect(hList1,&rcGrid);
    MapWindowPoints(hList1,hwnd,(LPPOINT)&rcGrid.left,2);

    SetWindowPos(hList1,NULL,0, 0,rcDlg.right - rcGrid.left,
        rcDlg.bottom - rcGrid.top,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
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

int AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.lParam = (LPARAM)pNode;
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

void SetSubItem(HWND hList1, int nItem, int nSubItem, char* szText)
{
    wchar_t   *wsir;
    _bstr_t   str=szText;
    wsir=str;
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)wsir;
    ListView_SetItem(hList1,&lvi1);
}

void SetSubItem(HWND hList1, int nItem, int nSubItem, int val)
{
    char lBuf[32] = {0};
    sprintf(lBuf, "%d", val);
    wchar_t   *wsir;
    _bstr_t   str=lBuf;
    wsir=str;
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)wsir;
    ListView_SetItem(hList1,&lvi1);
}

LPWSTR string2WCHAR(const char* srcStr)
{
        wchar_t   *wsir;
        _bstr_t   str=srcStr;
        wsir=str;
        return wsir;
}

void TC2C(const PTCHAR tc, char* pBuf) 
{ 
           _bstr_t b = tc;
           char* lBuf  = NULL;
           lBuf = b;
          sprintf(pBuf, lBuf);
}

void buildtable(int &pRow, int &pColum, std::string pContent, void* pNode )
{
    int item = 0;
    wchar_t   *wsir;
    _bstr_t   str=pContent.c_str();
    wsir=str;
    
    if(1 == (pColum))
    {
        item = AddItem(hList1, wsir, 3, pNode); //添加行
        if(((DNode*)pNode)->getLevel() == 1) 
        {
            SetSubItem(hList1, item, 7, ((DNode*)pNode)->getCfg()->getSumEva());
            ((DNode*)pNode)->getCfg()->setRowCol(item, 7);
        }
    }
    else
    {
        item = AddItem(hList1, TEXT(" "), 3, pNode);
        if (pColum >= 3)
        {
            if(3 == pColum)
            {
                gsLineNum++;                
                SetSubItem(hList1, item, pColum-1, gsLineNum);
            }
            SetSubItem(hList1, item, pColum, (char*)pContent.c_str());
            SetSubItem(hList1, item, pColum+1, ((DNode*)pNode)->getCfg()->getEvaWeigth());
//            SetSubItem(hList1, item, pColum+2, ((DNode*)pNode)->getCfg()->getCalcWeigth());
            SetSubItem(hList1, item, pColum+2, ((DNode*)pNode)->getCalcResult());            
            SetSubItem(hList1, item, pColum+5, (char*)((DNode*)pNode)->getCfg()->getRemark().c_str());
            
        }
        else
        {
            SetSubItem(hList1, item, pColum-1, wsir);
            if(((DNode*)pNode)->getLevel() == 2)
            {
                SetSubItem(hList1, item, 6, ((DNode*)pNode)->getCfg()->getSumEva());
                ((DNode*)pNode)->getCfg()->setRowCol(item, 6);
            }

        }
    }


    
}

BOOL MainDlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    if (EN_SCORE_SETTING == CScoreConfig::getInstance()->get_DisplayType())
    {
        DisplaySetting(hwnd);
    }
    else
    {
        PreviousResult(hwnd);
    }
    return TRUE;
}

/****************************************************************************
 *                                                                          *
 * Function: MainDlg_Proc                                                   *
 *                                                                          *
 * Purpose : Process messages for the Main dialog.                          *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

BOOL CALLBACK MainDlg_Proc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        HANDLE_MSG (hwndDlg, WM_CLOSE, MainDlg_OnClose);
        HANDLE_MSG (hwndDlg, WM_COMMAND, MainDlg_OnCommand);
        HANDLE_MSG (hwndDlg, WM_INITDIALOG, MainDlg_OnInitDialog);
        HANDLE_MSG (hwndDlg, WM_SIZE, MainDlg_OnSize);

        //// TODO: Add dialog message crackers here...

        default: return FALSE;
    }
}

/****************************************************************************
 *                                                                          *
 * Function: WinMain                                                        *
 *                                                                          *
 * Purpose : Initialize the application.  Register a window class,          *
 *           create and display the main window and enter the               *
 *           message loop.                                                  *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

int InitDataGridDlg(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX icc;
    WNDCLASSEX wcx;

    ghInstance = hInstance;

    /* Initialize common controls. Also needed for MANIFEST's */

    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES /*|ICC_COOL_CLASSES|ICC_DATE_CLASSES|ICC_PAGESCROLLER_CLASS|ICC_USEREX_CLASSES*/;
    InitCommonControlsEx(&icc);

    InitDataGridView(hInstance);

    /* Load Rich Edit control support */
 
    // LoadLibrary(_T("riched32.dll"));  // Rich Edit v1.0
    // LoadLibrary(_T("riched20.dll"));  // Rich Edit v2.0, v3.0

    /* Get system dialog information */
    wcx.cbSize = sizeof(wcx);
    if (!GetClassInfoEx(NULL, MAKEINTRESOURCE(32770), &wcx))
        return 0;

    /* Add our own stuff */
    wcx.hInstance = hInstance;
    wcx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDR_ICO_MAIN));
    wcx.lpszClassName = TEXT("SDKDataGClass");
    if (!RegisterClassEx(&wcx))
        return 0;

    /* The user interface is a modal dialog box */
    return 0;//DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)MainDlg_Proc);
}

int ShowDataGridDlg(HINSTANCE hInstance, HWND hwnd, LPSTR lpszCmdLine, int nCmdShow)
{
    if (NULL != ghInstance)
    {
        ::MessageBox(NULL, _T("实例已存在"), _T("提示"), MB_OK);
        return 0;
    }
    InitDataGridDlg(hInstance,hInstance,"",0);
    /* The user interface is a modal dialog box */
    return DialogBox(hInstance, MAKEINTRESOURCE(DLG_MAIN), hwnd, (DLGPROC)MainDlg_Proc);
}

void PreviousResult(HWND hwnd)
{
    //Delete all the columns firstly
    for(int i = 0;i < 100;i++)
    {
        ListView_DeleteColumn(hList1, 0);
    }

    ListView_DeleteAllItems(hList1);
    
    DWORD dwStyle = (DWORD)GetWindowLongPtr(hwnd, GWL_STYLE);
    SetWindowLongPtr(hwnd, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
    ListView_SetExtendedListViewStyle(hwnd, LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

    TCHAR titles[][128] = {TEXT("检测类"), TEXT("检测组"), TEXT("序号"), TEXT("检测项"), TEXT("违规总分%"),TEXT("违规积分")};
    int widths[9] = {100,100,40,100,100,100};
    
    hList1 = GetDlgItem(hwnd,IDC_DATAGRID);

    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
    lvColumn.fmt = LVCFMT_LEFT;
    

    int colNum = 0; //列数
    for(int i = 0; i < NELEMS(titles); i++)
    {
        lvColumn.cx = widths[i];
        lvColumn.pszText = titles[i];
        ListView_InsertColumn(hList1, i, &lvColumn);
    }

    int i = 0, j = 0;
    gsLineNum = 0;
    pDetectorRoot->tableListBuild(i,j,buildtable);

    int item = AddItem(hList1, _T("统计结果"), 3, pDetectorRoot); /*to display the score result...*/
    SetSubItem(hList1, item, 4, /*"103"*/pDetectorRoot->getCfg()->getSumEva());
    SetSubItem(hList1, item, 5, /*"103"*/pDetectorRoot->getCfg()->getSumCalc());
    pDetectorRoot->getCfg()->setRowCol(item, 0);
    // Extend the last column
    DataGridView_ExtendLastColumn(hList1, TRUE);
    //DataGridView_DisplayRowHeaders(hList1, TRUE);
    // Enable Auto Focus

    CEditor::getInstance()->set_previewFlag(true);
}

void DisplaySetting(HWND hwnd)
{
    //Delete all the columns firstly
    for(int i = 0;i < 100;i++)
    {
        ListView_DeleteColumn(hList1, 0);
    }

    ListView_DeleteAllItems(hList1);
    
    TCHAR titles[][128] = {TEXT("检测类"), TEXT("检测组"), TEXT("序号"), TEXT("检测项"), TEXT("违规评估权重%"),TEXT("违规积分权重"), TEXT("组小计"),TEXT("类合计"),TEXT("备注")};
    int widths[9] = {100,100,40,100,100,100,60,60,100};
    hList1 = GetDlgItem(hwnd,IDC_DATAGRID);

    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
    lvColumn.fmt = LVCFMT_LEFT;
    //lvColumn.cx = 100;

    int colNum = 0; //列数
    for(int i = 0; i < NELEMS(titles); i++)
    {
        lvColumn.cx = widths[i];
        lvColumn.pszText = titles[i];
        ListView_InsertColumn(hList1, i, &lvColumn);
    }

    int i = 0, j = 0;
    gsLineNum = 0;
    pDetectorRoot->tableListBuild(i,j,buildtable);

    int item = AddItem(hList1, _T("统计结果"), 3, pDetectorRoot); /*to display the score result...*/
    //for(int idx = 4; idx <= 7; idx++)
    {
        SetSubItem(hList1, item, 4, pDetectorRoot->getCfg()->getSumEva());
        SetSubItem(hList1, item, 5, pDetectorRoot->getCfg()->getSumCalc());
        //SetSubItem(hList1, item, 6, pDetectorRoot->getCfg()->getSumEva());
        //SetSubItem(hList1, item, 7, pDetectorRoot->getCfg()->getSumEva());
    }
    pDetectorRoot->getCfg()->setRowCol(item, 0);
    
    // Extend the last column
    DataGridView_ExtendLastColumn(hList1, TRUE);

    CEditor::getInstance()->set_previewFlag(false);
}
