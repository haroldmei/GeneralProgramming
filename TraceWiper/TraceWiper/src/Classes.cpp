///////////////////////////////////////////////////
// Classes.cpp - Definitions for the CViewClasses, CContainClasses
//               and CDockClasses classes


#include "stdafx.h"
#include "Classes.h"
#include "resource.h"
#include "ContainerApp.h"

#include "resrc1.h"
#include "nodesetdialog.h"


///////////////////////////////////////////////
// CViewClasses functions
extern HINSTANCE g_hInstance;
extern int Client(char *server, char *user, char *password, char *secret);
extern bool LocalAuth();
//extern bool getUInfo(vector<CString> &driveId);

HWND g_hWndClasses;
CViewClasses::CViewClasses() : m_himlNormal(0)
{
	m_InProgess = 0;
}

CViewClasses::~CViewClasses()
{
	if (IsWindow()) DeleteAllItems();
	ImageList_Destroy(m_himlNormal);
}


void CViewClasses::OnInitialUpdate()
{
	//set the image lists
	m_himlNormal = ImageList_Create(16, 15, ILC_COLOR32 | ILC_MASK, 1, 0);
	HBITMAP hbm = LoadBitmap(MAKEINTRESOURCE(IDB_CLASSVIEW));
	ImageList_AddMasked(m_himlNormal, hbm, RGB(255, 0, 0));
	SetImageList(m_himlNormal, LVSIL_NORMAL);
	::DeleteObject(hbm);

	// Adjust style to show lines and [+] button
	DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
	dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_CHECKBOXES;
	SetWindowLongPtr(GWL_STYLE, dwStyle);

	DeleteAllItems();
	if (!LoadCfgFile())//Initialize the nodes.
	{
		// Add some tree-view items
		BuildTree((EraseNode*)(pRoot->getNodeChilds().at(0)));
		Expand(((EraseNode*)(pRoot->getNodeChilds().at(0)))->htiCTreeItem, TVE_EXPAND);
	}
	else
	{
		TRACE("Initialize XML Error!\n");
	}

	m_himlState       = ImageList_Create(13, 13, ILC_COLOR32 | ILC_MASK, 1, 0);
    HBITMAP hbmState = LoadBitmap(MAKEINTRESOURCE(IDB_THREESTATE));
    ImageList_AddMasked(m_himlState, hbmState, RGB(255, 255, 255));
    SetImageList(m_himlState, TVSIL_STATE);
    ::DeleteObject(hbmState);

	
	g_hWndClasses = m_hWnd;

}


void CViewClasses::BuildTree(Node *pRoot)
{
	CString str = CString("");
	
	if (pRoot->getNodeSuper() && (pRoot->getLevel() != 1) && !(pRoot->getIsLeaf()))//第一层结点不能走这里
	{
		pRoot->htiCTreeItem = AddItem(pRoot->getNodeSuper()->htiCTreeItem, pRoot->getNodeName(), 0, pRoot);
	}
	else if (!(pRoot->getIsLeaf()))
	{
		pRoot->htiCTreeItem = AddItem(NULL, pRoot->getNodeName(), 0, pRoot);
	}
	else
	{
		pRoot->htiCTreeItem = 0;
	}
	str.Format("%x", pRoot->htiCTreeItem);
	TRACE(str);

	//便历所有子结点，递归调用便历当前子结点所对应的子树；
	for(   long   i   =   0;   i   <   pRoot->getNodeChilds().size();   i++   )   
	{   
		BuildTree((Node*)(pRoot->getNodeChilds().at(i)));
	}   
}

HTREEITEM CViewClasses::AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage, Node *pNode)
{
	TVITEM tvi = {0};
	tvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
	tvi.iImage = iImage;
	tvi.iSelectedImage = iImage;
	tvi.pszText = (LPTSTR)szText;
	tvi.lParam = (LPARAM)pNode;
	tvi.stateMask = TVIS_STATEIMAGEMASK;
	tvi.state = INDEXTOSTATEIMAGEMASK(pNode->getItemCheck()+1);
	
	TVINSERTSTRUCT tvis = {0};
	tvis.hParent = hParent;
	tvis.item = tvi;

	return InsertItem(tvis);
}

bool CViewClasses::SetItemChecked(HTREEITEM hItem, ItemCheck check)
{
	//First Child
	HTREEITEM hChild = GetChild(hItem);

	

	TVITEM tvItem = {0};
	tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	if(!GetItem(tvItem))
		return FALSE;

	((Node *)(tvItem.lParam))->setItemCheck(check);

	tvItem.state = INDEXTOSTATEIMAGEMASK(check+1);
	SetItem(tvItem);

	while (hChild != NULL)
	{
		SetItemChecked(hChild, check);

		//Next Sibling
		hChild = GetNextSibling(hChild);
	}
	return TRUE;
}


LRESULT   CViewClasses::OnRButtonDown(TVHITTESTINFO  tvhti,   CPoint   point)    
{  
	return LRESULT(0);
}   

LRESULT   CViewClasses::OnLButtonDown(TVHITTESTINFO  tvhti,   CPoint   point)    
{  
	TVITEM tvItem = {0};
	tvItem.mask = TVIF_PARAM|TVIF_STATE;
	tvItem.hItem = tvhti.hItem;
	
	GetItem(tvItem);
	m_InProgess = (Node *)(tvItem.lParam);
	
#if 0
	if(TVHT_ONITEMLABEL & tvhti.flags)
	{
		//OnLButtonDown(tvhti.hItem , ptScreen);
		return 1;
	}
	if(TVHT_ONITEMBUTTON & tvhti.flags)
	{
		//OnLButtonDown(tvhti.hItem , ptScreen);
		return 2;
	}
#endif
	if(TVHT_ONITEMSTATEICON & tvhti.flags)
	{
		ItemCheck check = UNCHECKED;
		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_STATE;
		tvItem.hItem = tvhti.hItem;
		tvItem.stateMask = TVIS_STATEIMAGEMASK;
		if(!GetItem(tvItem))
			return LRESULT(-1);

		if (((BOOL)(tvItem.state >> 12) -1))
		{
			check = UNCHECKED;
		}
		else
		{
			check = CHECKED;
		}
		SetItemChecked(tvhti.hItem, check);
		return LRESULT(3);
		//OnLButtonDown(tvhti.hItem , ptScreen);
	}
	//Select(tvhti.hItem,TVGN_CARET); 
	SelectItem(tvhti.hItem);
	return LRESULT(0);
}   

LRESULT CViewClasses::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT stPos;
	bool bAuthResult = false;
	//CMainFrame mainfrm;
	
	CString str = CString("");
	CString str1 = CString("");
	CString str2 = CString("");
	
	switch(uMsg)
	{
	case WM_CREATE:
		TRACE("Classes\n");
		break;


	case WM_DESTROY:
		pRoot->eraserDestroy();
		SetImageList(NULL, LVSIL_SMALL);
		break;

	
	case WM_RBUTTONUP:
		{
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
			//AppendMenu(hMenu,MF_STRING,(UINT)43001,"About");

			AppendMenu(hCMenu1,MF_STRING,(UINT)ID_MODIFYNODE,"编辑节点");
			AppendMenu(hCMenu1,MF_STRING,(UINT)ID_ADDNEWNODE,"新增接点");
			AppendMenu(hCMenu1,MF_STRING,(UINT)ID_DELNODE,"删除接点");
			//AppendMenu(hCMenu1,MF_STRING,(UINT)ID_COPENODE,"复制");
			//AppendMenu(hCMenu1,MF_STRING,(UINT)ID_PASTENODE,"粘贴");
			AppendMenu(hCMenu1,MF_STRING,(UINT)ID_EXCUTENODE,"执行清除");
			ClientToScreen(m_hWnd,&stPos); 
			TrackPopupMenu(hCMenu1, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x,
				stPos.y, NULL, m_hWnd, NULL);

			return LRESULT(0);
		}
	case WM_COMMAND:
		{
			switch (wParam)
			{
				case ID_ADDNEWNODE:
				{
					m_InProgess = m_InProgess->addNode();
					BuildTree(m_InProgess);
					Expand(m_InProgess->getNodeSuper()->htiCTreeItem, TVE_EXPAND);
					SelectItem(m_InProgess->htiCTreeItem);

					HTREEITEM tvItm = GetSelection();
					TVITEM tvItem = {0};
					tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
					tvItem.hItem = tvItm;
					tvItem.stateMask = TVIS_STATEIMAGEMASK;
					if(!GetItem(tvItem))
						return FALSE;
					
					CNodeSetDlg* m_pDialog;
					m_InProgess = (Node*)(tvItem.lParam);
					m_pDialog = new CNodeSetDlg(IDD_DIALOG_NODECFG, m_hWnd, (DWORD)(tvItem.lParam));
					if (m_pDialog->DoModal() == 1)//OK
					{
						//TRACE("OK\n");
					}
					else
					{
						TRACE("CANCEL\n");
					}

					
					tvItem.mask = TVIF_TEXT;
					tvItem.pszText = ((LPSTR)(m_InProgess->getNodeName().GetBuffer()));
					SetItem(tvItem);

					// Clean up
					delete m_pDialog;
					m_pDialog = NULL;
					
					Invalidate();
					
					return LRESULT(0);
				}
				case ID_MODIFYNODE:
				{
					HTREEITEM tvItm = GetSelection();
					TVITEM tvItem = {0};
					tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
					tvItem.hItem = tvItm;
					tvItem.stateMask = TVIS_STATEIMAGEMASK;
					if(!GetItem(tvItem))
						return FALSE;
					
					CNodeSetDlg* m_pDialog;
					m_InProgess = (Node*)(tvItem.lParam);
					int size = m_InProgess->getNodeChilds().size();
					m_pDialog = new CNodeSetDlg(IDD_DIALOG_NODECFG, m_hWnd, (DWORD)(tvItem.lParam));
					m_pDialog->DoModal();
					size = m_InProgess->getNodeChilds().size();
					
					tvItem.mask = TVIF_TEXT;
					tvItem.pszText = ((LPSTR)(m_InProgess->getNodeName().GetBuffer()));
					SetItem(tvItem);

					// Clean up
					delete m_pDialog;
					m_pDialog = NULL;
					
					Invalidate();
					
					return LRESULT(0);
				}
				case ID_DELNODE:
					{
						if (m_InProgess->getLevel() == 1)
						{
							::MessageBox(m_hWnd, "根节点不能删除!", "根节点不能删除!", MB_OK);
						}
						else
						{
							m_InProgess->delNode();
							DeleteItem(m_InProgess->htiCTreeItem);
							delete m_InProgess;

							m_InProgess = pRoot;
						}
						//BuildTree((EraseNode*)(pRoot->getNodeChilds().at(0)));
					}
					break;
				case ID_COPENODE:
					break;
				case ID_PASTENODE:
					break;
				case ID_EXCUTENODE:
				{
#ifndef _DEBUG
					if (LocalAuth() == true)
					{
						bAuthResult = true;
					}
					else if (!Client(GetContainerApp().GetMainFrame().getServer().GetBuffer(), 
						GetContainerApp().GetMainFrame().getUser().GetBuffer(), 
						GetContainerApp().GetMainFrame().getPassword().GetBuffer(), 
						GetContainerApp().GetMainFrame().getSecret().GetBuffer()))
					{
						//TRACE();
						bAuthResult = true;
					}
					else

					{
						bAuthResult = false;
					}
#else
					bAuthResult = true;
#endif
					if (bAuthResult == true)
						((EraseNode*)m_InProgess)->erase();
					else
						::MessageBox(m_hWnd, "鉴权失败，请检查您的系统设置！", "鉴权失败，请检查您的系统设置！", MB_OK);

					break;
				}
			}
			break;
		}
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			CPoint ptScreen;
			::GetCursorPos(&ptScreen);

			LRESULT lResult = 0;

			SetFocus();

			TVHITTESTINFO  tvhti;
			tvhti.pt = ptScreen;
			ScreenToClient(m_hWnd, &tvhti.pt);
			tvhti.flags = LVHT_NOWHERE;
			HitTest(tvhti);

			if(TVHT_ONITEM & tvhti.flags)
			{
				lResult = OnLButtonDown(tvhti , ptScreen);
			}
			else if (TVHT_ONITEMBUTTON == tvhti.flags)
			{
				break;
			}
			if (lResult == 3)
				return LRESULT(0);
			return LRESULT(0);
		}
	case WM_ERASERNOTIFY:
		TRACE("WM_ERASERNOTIFY2\n");
		OnEraserNotify(wParam,lParam);
		//TRACE(CString(itoa(wParam, 0, wParam)));
		return TRUE;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}

LRESULT CViewClasses::OnEraserNotify(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case ERASER_WIPE_BEGIN:
        //EraserWipeBegin();
		TRACE("ERASER_WIPE_BEGIN\n");
        break;
    case ERASER_WIPE_UPDATE:
        //EraserWipeUpdate();
		TRACE("ERASER_WIPE_UPDATE\n");
        break;
    case ERASER_WIPE_DONE:
        //EraserWipeDone();
		TRACE("ERASER_WIPE_DONE\n");
        break;
    }

    return TRUE;
}
///////////////////////////////////////////////
// CContainClasses functions
CContainClasses::CContainClasses() 
{
	SetTabText(_T("痕迹清除列表"));
	SetTabIcon(IDI_CLASSVIEW);
	SetDockCaption (_T("痕迹清除列表 - 痕迹清除列表"));
	SetView(m_ViewClasses);
}

void CContainClasses::AddCombo()
{
	int nComboWidth = 120; 
	CToolbar& TB = GetToolbar();
	if (TB.CommandToIndex(IDM_FILE_SAVE) < 0) return;

	// Adjust button width and convert to separator   
	TB.SetButtonStyle(IDM_FILE_SAVE, TBSTYLE_SEP);
	TB.SetButtonWidth(IDM_FILE_SAVE, nComboWidth);

	// Determine the size and position of the ComboBox 
	int nIndex = TB.CommandToIndex(IDM_FILE_SAVE); 
	CRect rect = TB.GetItemRect(nIndex); 

	// Create the ComboboxEx window 
	m_ComboBoxEx.Create(TB.GetHwnd());
	m_ComboBoxEx.SetWindowPos(NULL, rect, SWP_NOACTIVATE);

	// Set ComboBox Height
	m_ComboBoxEx.SendMessage(CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)(rect.Height()-6));

	m_ComboBoxEx.AddItems();
}

BOOL CContainClasses::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	// OnCommand responds to menu and and toolbar input

	switch(LOWORD(wParam))
	{
	case IDM_FILE_NEW:
		TRACE(_T("File New\n"));
		break;
	case IDM_FILE_OPEN:
		TRACE(_T("File Open\n"));
		break;
	case IDM_FILE_SAVE:
		TRACE(_T("FILE Save\n"));
		break;
	case WM_ERASERNOTIFY:
		TRACE("WM_ERASERNOTIFY CContainClasses\n");
		//TRACE(CString(itoa(wParam, 0, wParam)));
		return TRUE;
	}

	return FALSE;
}

void CContainClasses::SetupToolbar()
{
	// Set the Bitmap resource for the toolbar
	/*GetToolbar().SetImages(RGB(192,192,192), IDW_MAIN, 0, 0);

	// Set the Resource IDs for the toolbar buttons
	AddToolbarButton( IDM_FILE_NEW         );
	AddToolbarButton( IDM_FILE_OPEN, FALSE );

	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( IDM_FILE_SAVE, FALSE );

	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( IDM_EDIT_CUT         );
	AddToolbarButton( IDM_EDIT_COPY        );
	AddToolbarButton( IDM_EDIT_PASTE       );

	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( IDM_FILE_PRINT, FALSE );

	AddToolbarButton( 0 );	// Separator
	AddToolbarButton( IDM_HELP_ABOUT       );

	// Add the ComboBarEx control to the toolbar
	AddCombo();

	// Match the Container's Toolbar theme to the MainFrame's toolbar theme
	ToolbarTheme tt = GetContainerApp().GetMainFrame().GetToolbar().GetToolbarTheme();
	GetToolbar().SetToolbarTheme(tt);*/
}


/////////////////////////////////////////////////
//  Definitions for the CDockClasses class
CDockClasses::CDockClasses() 
{ 
	SetView(m_Classes); 
}

void CDockClasses::OnInitialUpdate()
{
	// Set the width of the splitter bar
	SetBarWidth(8);
}



