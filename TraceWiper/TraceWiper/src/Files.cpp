//////////////////////////////////////////////
// Files.cpp - Definitions for CViewFiles, CContainFiles
//             and DockFiles classes

#include "stdafx.h"
#include "Files.h"
#include "resource.h"

#include <atlstr.h>
#include "cfgfile.h"

HWND g_hWndFiles;
///////////////////////////////////////////////
// CViewFiles functions
CViewFiles::CViewFiles() : m_himlSmall(0),m_ItemCnt(0)
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
	SetExtendedStyle(LVS_SHOWSELALWAYS | LVS_REPORT| LVS_EDITLABELS | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES );

	SetColumns();
	InsertItems();
	g_hWndFiles = m_hWnd;
}


int CViewFiles::AddItem(LPCTSTR szText, int nImage)
{
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT|LVIF_IMAGE;
	lvi.iImage = nImage;
	lvi.pszText = (LPTSTR)szText;
	lvi.iItem = GetItemCount();
	m_ItemCnt++;

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
	TCHAR szString[6][20] = {_T("序列号"), /*_T("清除项名称"), TEXT("清除项类型"), */_T("清除项值"), _T("清除时间"), _T("剩余时间"), _T("进度1"),/* _T("进度2"), */_T("清除结果")};
	for(int i = 0; i < 6; ++i)
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

LRESULT CViewFiles::OnEraserNotify(WPARAM wParam, LPARAM lParam)
{
	CString str1 = CString("");
	CString str2 = CString("");
	CString str3 = CString("");
	CString str0 = CString("");
	CString str4 = CString("");	
	
    switch (wParam)
    {
    case ERASER_WIPE_BEGIN:
		{
			str0.Format("%d", /*eraserRetrieveValueContextID(lParam)*/m_ItemCnt);
			str1.Format("%d", eraserRetrieveValueProgressTimeLeft(lParam));
			str2.Format("%d", eraserRetrieveValueProgressPercent(lParam));
			str3.Format("%d", eraserRetrieveValueProgressTotalPercent(lParam));
			str4 = CString(eraserRetrieveValueStrData(lParam));

			int item = eraserRetrieveValueListIndex(lParam);

			
			item = AddItem(str0, 2);
			//SetSubItem(item, 1, str0);
			//SetSubItem(item, 1, _T("注册表类"));
			//SetSubItem(item, 2, _T("注册表类"));
			SetSubItem(item, 1, _T(str4));
			SetSubItem(item, 2, _T("2010-09-10 12:55"));
			SetSubItem(item, 3, str1);
			SetSubItem(item, 4, str2);
			//SetSubItem(item, 5, str3);
			SetSubItem(item, 5, _T("清除..."));
			eraserRetrieveValueListIndexSet(lParam, item);
			TRACE("ERASER_WIPE_BEGIN\n");
			break;
		}
    case ERASER_WIPE_UPDATE:
        {
			str0.Format("%d", /*eraserRetrieveValueContextID(lParam)*/m_ItemCnt);
			str1.Format("%d", eraserRetrieveValueProgressTimeLeft(lParam));
			str2.Format("%d", eraserRetrieveValueProgressPercent(lParam));
			str3.Format("%d", eraserRetrieveValueProgressTotalPercent(lParam));
			str4 = CString(eraserRetrieveValueStrData(lParam));

			int item = eraserRetrieveValueListIndex(lParam);
			
			/*LVITEM  lvi = {0};

		    lvi.mask  = LVIF_PARAM;
		    lvi.iItem = item;
			GetItem(&lvi);*/

			
			SetSubItem(item, 3, str1);
			SetSubItem(item, 4, str2);
			//SetSubItem(item, 5, str3);
			
			TRACE("ERASER_WIPE_UPDATE\n");

			
			break;
		}
    case ERASER_WIPE_DONE:
        //EraserWipeDone();
		TRACE("ERASER_WIPE_DONE\n");
        break;

	case ERASER_TEST_PAUSED:
		TRACE("ERASER_TEST_PAUSED\n");
		
    }

    return TRUE;
}


LRESULT CViewFiles::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		TRACE("Files\n");
			break;
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
			ClientToScreen(HWND(),&stPos); 
			TrackPopupMenu(hMenu, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x,
				stPos.y, NULL, HWND(), NULL);
			break;
		}
	case WM_LBUTTONDOWN:
		TRACE("WM_LBUTTONDOWN\n");
		break;
	case WM_LBUTTONUP:
		TRACE("WM_LBUTTONUP\n");
		break;
		
	case WM_DESTROY:
		{
			SetImageList(NULL, LVSIL_SMALL);
			break;
		}
	case WM_ERASERNOTIFY:
		{
			TRACE("WM_ERASERNOTIFY1\n");
			OnEraserNotify(wParam,lParam);
			//TRACE(CString(itoa(wParam, 0, wParam)));
			return TRUE;
		}
	case WM_REGNOTIFY:
		{
			TRACE("WM_REGNOTIFY\n");

			CString regType = CString("");
			switch (wParam)
			{
				case 0:
					regType = CString("注册表键");
					break;
				case 1:
					regType = CString("注册表键值");
					break;
				default:
					regType = CString("注册表");
					break;
			}

			
			CString str1 = CString("");
			CString str2 = CString("");
			CString str3 = CString("");
			CString str0 = CString("");
			CString str4 = CString("");

			str0.Format("%d", /*eraserRetrieveValueContextID(lParam)*/m_ItemCnt);
			str1.Format("%d", 0);
			str2.Format("%d", 0);
			//str3.Format("%d", eraserRetrieveValueProgressTotalPercent(lParam));
			str4 = *((CString*)lParam);
			
			int item = AddItem(str0, 2);
			//SetSubItem(item, 1, str0);
			//SetSubItem(item, 1, _T("注册表类"));
			//SetSubItem(item, 2, _T("注册表类"));
			SetSubItem(item, 1, _T(str4));
			SetSubItem(item, 2, _T("2010-09-10 12:55"));
			SetSubItem(item, 3, str1);
			SetSubItem(item, 4, str2);
			//SetSubItem(item, 5, str3);
			SetSubItem(item, 5, _T("清除..."));
	
			return TRUE;
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


