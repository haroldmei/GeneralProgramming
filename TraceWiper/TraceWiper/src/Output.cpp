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
	TCHAR szString[2][20] = {_T("软件显示项目"), _T("软件信息描述")/*, TEXT("清除项类型"), _T("清除项值"), _T("清除时间"), _T("剩余时间"), _T("进度1"), _T("进度2"), _T("清除结果")*/};
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
	SetExtendedStyle(LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES );

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


