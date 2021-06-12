///////////////////////////////////////////////////
// Output.cpp -  Definitions for the CViewUserOutput, CContainOutput
//              and CDockUserOutput classes


#include "stdafx.h"
#include "ContainerApp.h"
#include "UserOutput.h"
#include "resource.h"
#include "../../platform/SysGlobal.h"
#include "../../platform/CSector.h"

extern HWND g_hWndFiles;

///////////////////////////////////////////////
// CViewUserOutput functions
CViewUserOutput::CViewUserOutput()
{
    m_hRichEdit = ::LoadLibrary(_T("Riched20.dll")); // RichEdit ver 2.0
    if (!m_hRichEdit)
    {
        ::MessageBox(NULL,_T("CRichView::CRichView  Failed to load Riched20.dll"), _T(""), MB_ICONWARNING);
    }

    

    if (IsWindow()) DeleteAllItems();
    ImageList_Destroy(m_himlSmall);
}

CViewUserOutput::~CViewUserOutput(void)
{
    if (m_hRichEdit)
        ::FreeLibrary(m_hRichEdit);
}
/*
void CViewUserOutput::OnInitialUpdate()
{
    SetWindowText(_T("Output Window"));
}
*/

void CViewUserOutput::InsertItems()
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


void CViewUserOutput::SetColumns()
{
    //empty the list
    DeleteAllItems();

    //initialise the columns
    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    TCHAR szString[3][20] = {_T("被检人员"), _T("检测时间"), TEXT("检测结果")};
    for(int i = 0; i < 3; ++i)
    {
        lvColumn.pszText = szString[i];
       // InsertColumn(i, lvColumn);
    }
}

HTREEITEM CViewUserOutput::AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage, PRLIST_TYPE enType)
{
    TVITEM tvi = {0};
    tvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
    tvi.iImage = iImage;
    tvi.iSelectedImage = iImage;
    tvi.pszText = (LPTSTR)szText;
    tvi.lParam = (LPARAM)enType;
    tvi.stateMask = TVIS_STATEIMAGEMASK;
    //tvi.state = INDEXTOSTATEIMAGEMASK(pNode->getItemCheck()+1);
    
    TVINSERTSTRUCT tvis = {0};
    tvis.hParent = hParent;
    tvis.item = tvi;
    return TreeView_InsertItem( m_hWnd, &tvis );
}

LRESULT   CViewUserOutput::OnLButtonDClick(TVHITTESTINFO  tvhti,   CPoint   point)
{  
    (void)point;

    TVITEM tvItem = {0};
    tvItem.mask = TVIF_PARAM;
    tvItem.hItem = tvhti.hItem;
    //Node node;
    GetItem(tvItem);
    PRLIST_TYPE enPRType = (PRLIST_TYPE)GetItemData(tvItem.hItem);
    CPersonListConfig::getInstance()->set_selectedType(enPRType);
    if(TVHT_ONITEMLABEL & tvhti.flags)
    {
        PostMessage(g_hWndFiles, WM_COMMAND, ISCT_PERSONLIST_MSG001, /*(LPARAM)pDNode*/NULL);
    }
    if(TVHT_ONITEMBUTTON & tvhti.flags)
    {
        //OnLButtonDown(tvhti.hItem , ptScreen);
    }
    
    return LRESULT(0);
}

LRESULT CViewUserOutput::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    /*case WM_RBUTTONDOWN:
        {
            CString str = CString("");
            str.Format("%x\n", WM_RBUTTONDOWN);
            TRACE(str);
            break;
        }
    */
    case WM_LBUTTONDBLCLK:
        {
            CPoint ptScreen;
            ::GetCursorPos(&ptScreen);

            LRESULT lResult = 0;

            TVHITTESTINFO  tvhti;
            tvhti.pt = ptScreen;
            ScreenToClient(m_hWnd, &tvhti.pt);
            tvhti.flags = LVHT_NOWHERE;
            HitTest(tvhti);

            if(TVHT_ONITEM & tvhti.flags)
            {
                //lResult = OnLButtonDown(tvhti , ptScreen);
                lResult = OnLButtonDClick(tvhti , ptScreen);
            }
            if (lResult == 3)
                return LRESULT(0);
            break;
        }
    
    }

    return WndProcDefault(uMsg, wParam, lParam);
}

void CViewUserOutput::OnInitialUpdate()
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
    DeleteAllItems();
#endif
    // Adjust style to show lines and [+] button
    DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
    dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetWindowLongPtr(GWL_STYLE, dwStyle);

    DeleteAllItems();
    
    //BuildTree(PersonTreeBuild());
    HTREEITEM htiRoot = AddItem(NULL, _T("人员列表"), 0, PRLIST_INVALID);

    AddItem(htiRoot, _T(PRLIST_DETECT_NAME), 0, PRLIST_DETECTED);
    AddItem(htiRoot, _T(PRLIST_SUSPECTED_NAME), 0, PRLIST_SUSPECTED);
    AddItem(htiRoot, _T(PRLIST_NORMAL_NAME), 0, PRLIST_NORMAL);
    AddItem(htiRoot, _T(PRLIST_EXCEPTION_NAME), 0, PRLIST_EXCEPTION);
    Expand(htiRoot, TVE_EXPAND);

    //SetColumns();
    //InsertItems();
    //g_hWndFiles = m_hWnd;
}



/*void CViewUserOutput::PreCreate(CREATESTRUCT &cs)
{
    cs.style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD | 
                WS_CLIPCHILDREN | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL;

    cs.lpszClass = RICHEDIT_CLASS; // RichEdit ver 2.0
}*/


//////////////////////////////////////////////
//  Definitions for the CDockUserOutput class
CDockUserOutput::CDockUserOutput()
{
    // Set the view window to our edit control
    SetView(m_View);
}

void CDockUserOutput::OnInitialUpdate()
{
    // Set the width of the splitter bar
    SetBarWidth(8);
}


