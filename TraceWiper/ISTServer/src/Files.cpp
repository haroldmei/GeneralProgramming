//////////////////////////////////////////////
// Files.cpp - Definitions for CViewFiles, CContainFiles
//             and DockFiles classes

#include "stdafx.h"
#include "Files.h"
#include "resource.h"

#include "atlstr.h"
#include "cfgfile.h"
#include <vector>
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

	// Set the report style
	DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
	SetWindowLongPtr(GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
	SetExtendedStyle(LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT );

	SetColumns();
	InsertItems();
}

int CViewFiles::AddItem(LPCTSTR szText, int nImage)
{
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT|LVIF_IMAGE;
	lvi.iImage = nImage;
	lvi.pszText = (LPTSTR)szText;

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
	TCHAR szString[8][20] = {_T("用户名称"), TEXT("部门信息"), _T("IP地址"), _T("MAC地址"), _T("硬盘序列号"), _T("密码"), _T("密钥"), _T("客户端状态")};
	for(int i = 0; i < 8; ++i)
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
	for(int i = 0; i < LegalItems.size(); ++i)
	{
		LegalItem *item = LegalItems.at(i);

		int index = AddItem(item->name, 2);
		SetSubItem(index, 1, item->dep);
		SetSubItem(index, 2, item->ipaddr);
		SetSubItem(index, 3, item->macaddr);
		SetSubItem(index, 4, item->harddrive);
		SetSubItem(index, 5, item->password);
		SetSubItem(index, 6, item->key);
		//SetSubItem(index, 5, item->);
	}
}

LRESULT CViewFiles::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_RBUTTONDOWN:
		{

			POINT stPos;
			stPos.x = LOWORD (lParam) ;
			stPos.y = HIWORD (lParam) ;

			HMENU hMenu,hCMenu1,hCMenu2,hCMenu3;

			hMenu=CreateMenu();

			hCMenu1=CreatePopupMenu();

			hCMenu2=CreatePopupMenu();

			hCMenu3=CreatePopupMenu();

			InsertMenu(hMenu,0,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu1,"File");

			InsertMenu(hMenu,1,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu2,"View");

			InsertMenu(hMenu,2,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu3,"Edit");

			AppendMenu(hMenu,MF_STRING,(UINT)43001,"About");

			AppendMenu(hCMenu1,MF_STRING,(UINT)43002,"Open");

			AppendMenu(hCMenu1,MF_STRING,(UINT)43003,"Close");

			AppendMenu(hCMenu2,MF_STRING,(UINT)43004,"Big");

			AppendMenu(hCMenu2,MF_STRING,(UINT)43005,"Small");

			AppendMenu(hCMenu3,MF_STRING,(UINT)43006,"Copy");

			AppendMenu(hCMenu3,MF_STRING,(UINT)43007,"Paste");
			//HMENU hSysMenu;
			//GetCursorPos(&stPos);
			ClientToScreen(m_hWnd,&stPos); 
			TrackPopupMenu(hMenu, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x,
				stPos.y, NULL, m_hWnd, NULL);
			break;
		}
	case WM_DESTROY:
		{
			SetImageList(NULL, LVSIL_SMALL);
			break;
		}
}

return WndProcDefault(uMsg, wParam, lParam);
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


