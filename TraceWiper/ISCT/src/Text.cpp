///////////////////////////////////////////////////
// Text.cpp -  Definitions for the CViewText, 
//              and CDockText classes

#include "stdafx.h"
#include "ContainerApp.h"
#include "Text.h"
#include "resource.h"
#include "CfgFile.h"
#include <comutil.h>

HWND g_hWndText;

DWORD g_currentDetectIndex = 0;
extern DWORD g_totalSelectedNodes;

///////////////////////////////////////////////
// CViewText functions
CViewText::CViewText()
{
    m_hRichEdit = ::LoadLibrary(_T("Riched20.dll")); // RichEdit ver 2.0
    if (!m_hRichEdit)
    {
        ::MessageBox(NULL,_T("CRichView::CRichView  Failed to load Riched20.dll"), _T(""), MB_ICONWARNING);
    }
}

CViewText::~CViewText(void)
{
    // Free the DLL
    if (m_hRichEdit)
        ::FreeLibrary(m_hRichEdit);

    if (IsWindow()) DeleteAllItems();
    ImageList_Destroy(m_himlSmall);
}

/*void CViewText::OnInitialUpdate()
{
    SetWindowText(_T("Text Edit Window\r\n\r\n You can type some text here ..."));
}
*/

void CViewText::SetSubItem(int nItem, int nSubItem, int curVal, int TotalVal)
{
    char lBuf[32] = {0};
    DWORD percent = curVal*100/TotalVal;
    
    sprintf(lBuf, "%2d%%", percent);

    CViewFiles::SetSubItem(nItem, nSubItem, lBuf);
}

void CViewText::InsertItems(DNode* pDNode)
{
    if (NULL != pDNode)
    {
        ValueList lTitleList;
        ValueList lRowList;
        DWORD lTimeCnt = 0;
        char lBuf[32] = {0};

        pDNode->getTitleList(lTitleList);
        pDNode->getLineResult(lRowList);
        if (0 != lTitleList.size())
        {
            lTimeCnt = lRowList.size()/lTitleList.size();
        }
        sprintf(lBuf, "%d", lTimeCnt);
        
        ++g_currentDetectIndex;
        int item = AddItem(pDNode->getName().c_str(), 2);
        CViewFiles::SetSubItem(item, 1, _T("完成"));
        SetSubItem(item, 2, g_currentDetectIndex, g_totalSelectedNodes);
        CViewFiles::SetSubItem(item, 3, lBuf);
        if (g_currentDetectIndex == g_totalSelectedNodes)
        {
            g_currentDetectIndex = 0;
        }
    }
}

void CViewText::SetColumns()
{
    //empty the list
    DeleteAllItems();

    //initialise the columns
    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    TCHAR szString[4][20] = {_T("检测项"), _T("当前状态"),_T("总进度"),_T("违规次数")};
    for(int i = 0; i < 4; ++i)
    {
        lvColumn.pszText = szString[i];
        InsertColumn(i, lvColumn);
    }
}

LRESULT CViewText::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
      case WM_RBUTTONDOWN:
      {
            POINT stPos;
            stPos.x = LOWORD (lParam) ;
            stPos.y = HIWORD (lParam) ;

            HMENU hMenu;

            hMenu=CreatePopupMenu();

            AppendMenu(hMenu,MF_STRING,(UINT)ISCT_CLEAR_BOTTOM_RSULT, "清除结果");
            
            ClientToScreen(m_hWnd,&stPos); 
            TrackPopupMenu(hMenu, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x, stPos.y, NULL, m_hWnd, NULL);
      }
      break;
      //added by johnz
      case WM_COMMAND:
      {    
          switch(wParam)
          {
              case ISCT_DETECT_PROGRESS:/*Display the detect progess*/
              {
                  DNode* pDNode = (DNode*)lParam;
                  InsertItems(pDNode);
              }
              break;
              case ISCT_CLEAR_BOTTOM_RSULT:
              {
                  DeleteAllItems();
              }
              break;
          }
      }
    }
    return WndProcDefault(uMsg, wParam, lParam);
}
void CViewText::OnInitialUpdate()
{
    g_hWndText = m_hWnd;
    // Set the image lists
    m_himlSmall = ImageList_Create(16, 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    HBITMAP hbm = LoadBitmap(MAKEINTRESOURCE(IDB_FILEVIEW));
    ImageList_AddMasked(m_himlSmall, hbm, RGB(255, 0, 255));
    SetImageList(m_himlSmall, LVSIL_SMALL);
    ::DeleteObject(hbm);

    // Set the report style
    DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
    SetWindowLongPtr(GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
    SetExtendedStyle(LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT/* | LVS_EX_CHECKBOXES*/ );

    SetColumns();
    //InsertItems();
    //g_hWndFiles = m_hWnd;
}


/*void CViewText::PreCreate(CREATESTRUCT &cs)
{
    cs.style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD | 
                WS_CLIPCHILDREN | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL;

    cs.lpszClass = RICHEDIT_CLASS; // RichEdit ver 2.0
}*/


//////////////////////////////////////////////
//  Definitions for the CContainText class
CContainText::CContainText() 
{
    SetDockCaption (_T("检测进度- 显示检测进度信息"));
    SetTabText(_T("检测进度"));
    SetTabIcon(IDI_TEXT);
    SetView(m_ViewText);
} 


//////////////////////////////////////////////
//  Definitions for the CDockText class
CDockText::CDockText()
{
    // Set the view window to our edit control
    SetView(m_View);
}

void CDockText::OnInitialUpdate()
{
    // Set the width of the splitter bar
    SetBarWidth(8);
}


