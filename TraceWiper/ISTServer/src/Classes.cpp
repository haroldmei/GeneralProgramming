///////////////////////////////////////////////////
// Classes.cpp - Definitions for the CViewClasses, CContainClasses
//               and CDockClasses classes


#include "stdafx.h"
#include "Classes.h"
#include "resource.h"
#include "ContainerApp.h"

#include "resrc1.h"


///////////////////////////////////////////////
// CViewClasses functions
CViewClasses::CViewClasses() : m_himlNormal(0)
{
}

CViewClasses::~CViewClasses()
{
	if (IsWindow()) DeleteAllItems();
	ImageList_Destroy(m_himlNormal);
}

extern HINSTANCE g_hInstance;

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

	// Add some tree-view items
	HTREEITEM htiRoot = AddItem(NULL, _T("�����ʹ�úۼ����"), 0);
	
	HTREEITEM htiCTreeViewApp = AddItem(htiRoot, _T("Windows���кۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("��ʼ/���кۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("��ʼ/�����ۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("��ʼ/����������ۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("��ʼ/�ĵ��ۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("windows���ʹ���ļ��б�"), 0);
	AddItem(htiCTreeViewApp, _T("������������ĺۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("���г����������ʱ�ļ�"), 0);
	AddItem(htiCTreeViewApp, _T("windows�¼���Ӧ�ó�����־"), 0);
	AddItem(htiCTreeViewApp, _T("xpԤ���ļ��ۼ�"), 0);
	AddItem(htiCTreeViewApp, _T("regedit�����ۼ�"), 0);
	
	HTREEITEM htiMainFrame = AddItem(htiRoot, _T("������ۼ�"), 0);
	HTREEITEM htiMainFrameIE = AddItem(htiMainFrame, _T("IE������ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE��ַ���ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE���������ҳ�ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE�����cookies�ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE�������ʱ�ļ��ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE������Զ���ɱ��ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE������Զ��������ۼ�"), 0);
	AddItem(htiMainFrameIE, _T("IE���������������վ��ۼ�"), 0);
	HTREEITEM htiMainFrameFireFox = AddItem(htiMainFrame, _T("firefox�ۼ�"), 0);
	AddItem(htiMainFrameFireFox, _T("FireFox���������ҳ�ۼ�"), 0);
	AddItem(htiMainFrameFireFox, _T("FireFox�����cookies�ۼ�"), 0);
	AddItem(htiMainFrameFireFox, _T("FireFox�������ʱ�ļ��ۼ�"), 0);
	AddItem(htiMainFrameFireFox, _T("FireFox����������ļ��ۼ�"), 0);
	AddItem(htiMainFrameFireFox, _T("FireFox������Զ��������ۼ�"), 0);
	
	HTREEITEM htiView = AddItem(htiRoot, _T("�ִ�������ۼ�"), 0);
	AddItem(htiView, _T("Windowsд�ְ�ۼ�"), 0);
	AddItem(htiView, _T("officeʹ�úۼ�"), 0);
	AddItem(htiView, _T("Acrobat reader�ۼ�"), 0);
	AddItem(htiView, _T("ultraedit�ۼ�"), 0);

	HTREEITEM htiView1 = AddItem(htiRoot, _T("ͼ�δ�������ۼ�"), 0);
	AddItem(htiView1, _T("windows������ͼʹ�úۼ�"), 0);
	AddItem(htiView1, _T("ACDSee�ۼ�"), 0);
	AddItem(htiView1, _T("PhotoShop�ۼ�"), 0);


	HTREEITEM htiView2 = AddItem(htiRoot, _T("ý�岥������ۼ�"), 0);
	AddItem(htiView2, _T("Windows media player�ۼ�"), 0);
	AddItem(htiView2, _T("Quik Time�ۼ�"), 0);
	AddItem(htiView2, _T("ReadPlayer�ۼ�"), 0);
	AddItem(htiView2, _T("winamp�ۼ�"), 0);
	AddItem(htiView2, _T("gom�ۼ�"), 0);

	HTREEITEM htiView3 = AddItem(htiRoot, _T("ѹ����������ۼ�"), 0);
	AddItem(htiView3, _T("winrar�ۼ�"), 0);
	AddItem(htiView3, _T("winzip�ۼ�"), 0);
	AddItem(htiView3, _T("7zip�ۼ�"), 0);

	HTREEITEM htiView4 = AddItem(htiRoot, _T("ɱ���ͷ���ǽ����ۼ�"), 0);
	AddItem(htiView4, _T("ŵ�ٺۼ�"), 0);
	AddItem(htiView4, _T("antivir�ۼ�"), 0);
	AddItem(htiView4, _T("Zone alarm�ۼ�"), 0);

	HTREEITEM htiView5 = AddItem(htiRoot, _T("��������ۼ�"), 0);
	AddItem(htiView5, _T("Net Transport�ۼ�"), 0);
	AddItem(htiView5, _T("FlashGet�ۼ�"), 0);
	AddItem(htiView5, _T("NetAnts�ۼ�"), 0);
	AddItem(htiView5, _T("Internet Download Manager�ۼ�"), 0);

	HTREEITEM htiView6 = AddItem(htiRoot, _T("���̿�¼����ۼ�"), 0);
	AddItem(htiView6, _T("Nero�ۼ�"), 0);
	AddItem(htiView6, _T("Alcohol�ۼ�"), 0);
	AddItem(htiView6, _T("Easy CD Creator�ۼ�"), 0);
	AddItem(htiView6, _T("UltraISO�ۼ�"), 0);

	HTREEITEM htiView7 = AddItem(htiRoot, _T("����վ����"), 0);

	HTREEITEM htiView8 = AddItem(htiRoot, _T("Windowsϵͳ��ԭ�ۼ�"), 0);

	HTREEITEM htiView9 = AddItem(htiRoot, _T("Windows���ز��ͼ��ۼ�"), 0);

	HTREEITEM htiViewa = AddItem(htiRoot, _T("�����ļ�Ŀ¼���"), 0);

	HTREEITEM htiViewb = AddItem(htiRoot, _T("����ȫ�������"), 0);

	HTREEITEM htiViewc = AddItem(htiRoot, _T("���������������"), 0);

	HTREEITEM htiViewd = AddItem(htiRoot, _T("���������������"), 0);

	// Expand some tree-view items
	Expand(htiRoot, TVE_EXPAND);
	Expand(htiCTreeViewApp, TVE_EXPAND);
	Expand(htiMainFrame, TVE_EXPAND);
	Expand(htiView, TVE_EXPAND);
	Expand(htiView1, TVE_EXPAND);
	/*Expand(htiView2, TVE_EXPAND);
	Expand(htiView3, TVE_EXPAND);
	Expand(htiView4, TVE_EXPAND);
	Expand(htiView5, TVE_EXPAND);
	Expand(htiView6, TVE_EXPAND);
	Expand(htiView7, TVE_EXPAND);
	Expand(htiView8, TVE_EXPAND);
	Expand(htiView9, TVE_EXPAND);
	Expand(htiViewa, TVE_EXPAND);
	Expand(htiViewb, TVE_EXPAND);
	Expand(htiViewc, TVE_EXPAND);
	Expand(htiViewd, TVE_EXPAND);*/
	
}

HTREEITEM CViewClasses::AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage)
{
	TVITEM tvi = {0};
	tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvi.iImage = iImage;
	tvi.iSelectedImage = iImage;
	tvi.pszText = (LPTSTR)szText;

	TVINSERTSTRUCT tvis = {0};
	tvis.hParent = hParent;
	tvis.item = tvi;

	return InsertItem(tvis);
}

void   CViewClasses::OnRButtonDown(UINT   nFlags,   CPoint   point)    
{  
#if 0
	CPoint   pt;  
	CMenu   menu;  
	/*CTreeCtrl&   cThisTree   =   GetTreeCtrl();  

	HTREEITEM   SelItem   =   cThisTree.HitTest(   point,     &nFlags   );  

	if   (SelItem   ==   NULL)     return   ;//δѡ�����οؼ����򷵻�  

	cThisTree.SelectItem(SelItem)   ;//ȡ�õ�ǰ��ѡ�ڵ�  
*/

	menu.CreatePopupMenu();  
	menu.AppendMenu(0,ID_USER1,"�˵�һ");  
	menu.AppendMenu(0,ID_USER2,"�˵���");  
	GetCursorPos(&pt);  
	menu.TrackPopupMenu(TPM_RIGHTBUTTON,pt.x,pt.y,this);  
	//ptAction=pt;  
#endif
   
}   


LRESULT CViewClasses::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		//
		
		/*
		hSysMenu = GetSystemMenu((HWND)wParam,FALSE);
	    AppendMenu(hSysMenu,MF_SEPARATOR,0,NULL);
	    AppendMenu(hSysMenu,0,ID_NODE_ADD,"szMenuHelp");
	    AppendMenu(hSysMenu,0,ID_NODE_DELETE,"szMenuAbout");*/
		break;
	
		
	case WM_DESTROY:
		SetImageList(NULL, LVSIL_SMALL);
		break;
		
	case WM_RBUTTONDOWN:
		{
			HMENU hSysMenu;
		HINSTANCE hInstance = GetApp()->GetInstanceHandle();
		g_hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(ID_Menu));
		g_hSubMenu = GetSubMenu(g_hMenu,0);

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
		//MessageBox("WM_LBUTTONDOWN", "WM_LBUTTONDOWN", MB_OK);
		break;
	}

	return WndProcDefault(uMsg, wParam, lParam);
}


///////////////////////////////////////////////
// CContainClasses functions
CContainClasses::CContainClasses() 
{
	SetTabText(_T("�ۼ�����б�"));
	SetTabIcon(IDI_CLASSVIEW);
	SetDockCaption (_T("�ۼ�����б� - �ۼ�����б�"));
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
	}

	return FALSE;
}

void CContainClasses::SetupToolbar()
{
	// Set the Bitmap resource for the toolbar
	GetToolbar().SetImages(RGB(192,192,192), IDW_MAIN, 0, 0);
	
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
	GetToolbar().SetToolbarTheme(tt);
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



