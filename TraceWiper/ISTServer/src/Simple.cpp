//////////////////////////////////////////////////////////////
// Simple.cpp -  Definitions for the CViewSimple, 
//                and CDockSimple classes

#include "stdafx.h"
#include "ContainerApp.h"
#include "Simple.h"
#include "resource.h"



///////////////////////////////////////////////
// CViewSimple functions
void CViewSimple::OnPaint(HDC hDC)
{
	//Centre some text in the window
	RECT r;
	::GetClientRect(m_hWnd, &r);
	::DrawText(hDC, _T("Simple View"), -1, &r, DT_CENTER|DT_VCENTER|DT_SINGLELINE);


	/*CListCtrl& lc = GetListCtrl();
	

	lc.InsertItem(0,(LPCTSTR)"aaaa");
    lc.SetItemText(0,1,(LPCTSTR)"aaaa");
    lc.SetItemText(0,2,(LPCTSTR)"aaaa"));
    lc.SetItemText(0,3,(LPCTSTR)"aaaa");*/
}

LRESULT CViewSimple::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_RBUTTONUP:
		{

			POINT stPos;
			LVITEM lvItem;

			GetItem(lvItem);
			
			stPos.x = LOWORD (lParam) ;
			stPos.y = HIWORD (lParam) ;

			HMENU hMenu,hCMenu1,hCMenu2,hCMenu3;

			hMenu=CreatePopupMenu();
			hCMenu1=CreatePopupMenu();
			hCMenu2=CreatePopupMenu();
			hCMenu3=CreatePopupMenu();
			InsertMenu(hMenu,0,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu1,"File");
			InsertMenu(hMenu,1,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu2,"View");
			InsertMenu(hMenu,2,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu3,"Edit");
			AppendMenu(hMenu,MF_STRING,(UINT)43001,"About");
			AppendMenu(hCMenu1,MF_STRING,(UINT)43002,"ÐÂÔö");
			AppendMenu(hCMenu1,MF_STRING,(UINT)43003,"É¾³ý");
			ClientToScreen(m_hWnd,&stPos); 
			TrackPopupMenu(hCMenu1, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x,
				stPos.y, NULL, m_hWnd, NULL);
			break;
		}

	case WM_SIZE:
		Invalidate();
		break;
	
	case WM_WINDOWPOSCHANGED:
		Invalidate();
		break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}

//////////////////////////////////////////////
//  Definitions for the CDockSimple class
CDockSimple::CDockSimple()
{
	// Set the view window to our edit control
	SetView(m_View);
}

CDocker* CDockSimple::NewDockerFromID(int nID)
{
	CDocker* pDock = NULL;
	switch(nID)
	{
	case ID_DOCK_CLASSES1:
		pDock = new CDockClasses;
		break;
	case ID_DOCK_CLASSES2:
		pDock = new CDockClasses;					
		break;
	case ID_DOCK_FILES1:
		pDock = new CDockFiles;
		break;
	case ID_DOCK_FILES2:
		pDock = new CDockFiles;
		break;
	case ID_DOCK_OUTPUT1:
		pDock = new CDockOutput;
		break;
	case ID_DOCK_OUTPUT2:
		pDock = new CDockOutput;
		break;
	case ID_DOCK_TEXT1:
		pDock = new CDockText;
		break;
	case ID_DOCK_TEXT2:
		pDock = new CDockText;
		break;
	default:
		TRACE(_T("Unknown Dock ID\n"));
		break;
	}

	return pDock;
}

void CDockSimple::OnInitialUpdate()
{
	// Set the width of the splitter bar
	SetBarWidth(8);
}


