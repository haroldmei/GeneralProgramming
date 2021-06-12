///////////////////////////////////////////////////
// Output.cpp -  Definitions for the CViewOutput, CContainOutput
//              and CDockOutput classes


#include "stdafx.h"
#include "ContainerApp.h"
#include "Output.h"
#include "resource.h"


///////////////////////////////////////////////
// CViewOutput functions
CViewOutput::CViewOutput()
{
    m_hRichEdit = ::LoadLibrary(_T("Riched20.dll")); // RichEdit ver 2.0
    if (!m_hRichEdit)
    {
        ::MessageBox(NULL,_T("CRichView::CRichView  Failed to load Riched20.dll"), _T(""), MB_ICONWARNING);
    }

    

    if (IsWindow()) DeleteAllItems();
    ImageList_Destroy(m_himlSmall);
}

CViewOutput::~CViewOutput(void)
{
    if (m_hRichEdit)
        ::FreeLibrary(m_hRichEdit);
}
/*
void CViewOutput::OnInitialUpdate()
{
    SetWindowText(_T("Output Window"));
}
*/

int CViewOutput::AddItem(LPCTSTR szText, int nImage)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.iItem = GetItemCount();

    return InsertItem(lvi);
}

BOOL CViewOutput::SetSubItem(int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    return (BOOL)SendMessage(LVM_SETITEM, 0L, (LPARAM)&lvi1);
}

void CViewOutput::InsertItems()
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
    ValueList lList;
    lList.push_back("公司名称:");       lList.push_back("北京X X 信息安全有限公司");
    lList.push_back("技术支持:");       lList.push_back("tech@xxx.com");
    lList.push_back("客户服务部:");  lList.push_back("service@xxx.com");
    lList.push_back("市场部:");             lList.push_back("sales@xxx.com");
    lList.push_back("网址:");                   lList.push_back("http://www.xxx.com");
    lList.push_back("邮编:");                   lList.push_back("10001");
    lList.push_back("传真:");                   lList.push_back("(010)12345678-999");
    lList.push_back("联系电话:");        lList.push_back("(010)12345678");
    lList.push_back("地址:");                   lList.push_back("北京市上地信息路");

    for(int i = 0; i < lList.size();)
    {
        int item = AddItem(lList[i].c_str(), 3);
        i++;
        SetSubItem(item, 1, lList[i].c_str());
        i++;
    }
}


void CViewOutput::SetColumns()
{
    //empty the list
    DeleteAllItems();

    //initialise the columns
    LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    TCHAR szString[2][20] = {_T("标题"), _T("内容")};
    for(int i = 0; i < 2; ++i)
    {
        lvColumn.pszText = szString[i];
        InsertColumn(i, lvColumn);
    }
}


void CViewOutput::OnInitialUpdate()
{
    // Set the image lists
    m_himlSmall = ImageList_Create(16, 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    HBITMAP hbm = LoadBitmap(MAKEINTRESOURCE(IDB_FILEVIEW));
    ImageList_AddMasked(m_himlSmall, hbm, RGB(255, 0, 255));
    SetImageList(m_himlSmall, LVSIL_SMALL);
    ::DeleteObject(hbm);

    // Set the report style
    DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
    SetWindowLongPtr(GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
    SetExtendedStyle(/*LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | */LVS_OWNERDRAWFIXED/*|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT */);

    SetColumns();
    InsertItems();
    //g_hWndFiles = m_hWnd;
}



/*void CViewOutput::PreCreate(CREATESTRUCT &cs)
{
    cs.style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD | 
                WS_CLIPCHILDREN | WS_HSCROLL | WS_VISIBLE | WS_VSCROLL;

    cs.lpszClass = RICHEDIT_CLASS; // RichEdit ver 2.0
}*/


//////////////////////////////////////////////
//  Definitions for the CDockOutput class
CDockOutput::CDockOutput()
{
    // Set the view window to our edit control
    SetView(m_View);
}

void CDockOutput::OnInitialUpdate()
{
    // Set the width of the splitter bar
    SetBarWidth(8);
}


