///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include <atlstr.h>
#include "../../platform/sysGlobal.h"
#include "SoftSetDialog.h"
#include "resrc1.h"
#include "resource.h"
#include "cfgfile.h"
#include "shlobj.h"
#include "../../platform/registerops.h"
#include "../../platform/FileOps.h"


// Definitions for the CSoftSetDlg class
int   CALLBACK   BrowseCallbackProc(HWND   hwnd,   UINT   uMsg,   LPARAM   lParam,   LPARAM   lpData) 
{
    (void)lpData;

    switch(uMsg)
    { 
        case   BFFM_INITIALIZED: 
        {
            //默认为可执行文件当前路径 
            string Path = FileOps::getInstance()->get_currentPath(); 
            ::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM) Path.c_str()); 
        }
        break; 
        case   BFFM_SELCHANGED: 
        { 
            char   curr[MAX_PATH]; 
            SHGetPathFromIDList((LPCITEMIDLIST)lParam,curr); 
            sprintf(curr, "%s",curr); 
            ::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)curr); 
        } 
        break;
        default:
        {
            char lMsg[100] = {0};
            sprintf(lMsg, "0x%x",uMsg);
            TRACE(lMsg);
        }
         break;
    } 
    return   0; 
}


CSoftSetDlg::CSoftSetDlg(UINT nResID, HWND hWndParent/*, CString Server, CString Secret, CString User, CString Password*/)
	: CDialog(nResID, hWndParent)
{
	//m_ptrEntry = ptrEntry;
	/*m_Server = Server;
	m_Secret = Secret;
	m_User = User;
	m_Password = Password;
	*/
	g_eEraseMethodDisk = (ERASER_METHOD)1;
	g_eEraseMethodFiles = (ERASER_METHOD)1;
	
	m_hInstRichEdit = ::LoadLibrary(_T("RICHED32.DLL"));
    if (!m_hInstRichEdit)
 		::MessageBox(NULL, _T("CSoftSetDlg::CRichView  Failed to load RICHED32.DLL"), _T(""), MB_ICONWARNING);
}

void CSoftSetDlg::UnsetDlgBtn1()
{
	switch(g_eEraseMethodFiles)
	{
		case 1:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_1, 0);
			break;
		case 2:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_2, 0);
			break;
		case 3:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_3, 0);
			break;
		case 4:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_4, 0);
			break;
		default:
			break;
	}
	return;
}

void CSoftSetDlg::UnsetDlgBtn2()
{
	switch(g_eEraseMethodDisk)
	{
		case 1:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_1, 0);
			break;
		case 2:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_2, 0);
			break;
		case 3:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_3, 0);
			break;
		case 4:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_4, 0);
			break;
		default:
			break;
	}
	return;
}

void CSoftSetDlg::SetDlgBtn1()
{
	::CheckDlgButton(m_hWnd, IDC_CHECK_2_1, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK_2_2, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK_2_3, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK_2_4, 0);
	
	switch(g_eEraseMethodFiles)
	{
		case 1:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_1, 1);
			break;
		case 2:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_2, 1);
			break;
		case 3:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_3, 1);
			break;
		case 4:
			::CheckDlgButton(m_hWnd, IDC_CHECK_2_4, 1);
			break;
		default:
			break;
	}
	return;
}

void CSoftSetDlg::SetDlgBtn2()
{
	
	::CheckDlgButton(m_hWnd, IDC_CHECK_3_1, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK_3_2, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK_3_3, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK_3_4, 0);
	switch(g_eEraseMethodDisk)
	{
		case 1:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_1, 1);
			break;
		case 2:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_2, 1);
			break;
		case 3:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_3, 1);
			break;
		case 4:
			::CheckDlgButton(m_hWnd, IDC_CHECK_3_4, 1);
			break;
		default:
			break;
	}
	return;
}

CSoftSetDlg::~CSoftSetDlg()
{
	::FreeLibrary(m_hInstRichEdit);
}

BOOL CSoftSetDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//char aa[64] = {0};
	//sprintf(aa, "0x%x-0x%x-0x%x\n", uMsg, wParam, lParam);
	switch (uMsg)
	{
	default:
		break;
	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CSoftSetDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	LPTSTR str = new TCHAR[10];
	switch (LOWORD(wParam))
    {
	case IDC_BUTTON2:
		/*::GetDlgItemText(m_hWnd, IDC_EDIT_SERVER, str, 32);
		m_Server = CString(str);

		::GetDlgItemText(m_hWnd, IDC_EDIT_SECRET, str, 32);
		m_Secret = CString(str);
		
		::GetDlgItemText(m_hWnd, IDC_EDIT_USER, str, 32);
		m_User = CString(str);
		
		::GetDlgItemText(m_hWnd, IDC_EDIT_PASSWORD, str, 32);
		m_Password = CString(str);*/

		CDialog::OnOK();
	case IDC_CHECK1:
		break;
	case IDC_CHECK2:
		break;
	case IDC_CHECK3:
		break;
	case IDC_CHECK_2_1:
		UnsetDlgBtn1();
		g_eEraseMethodFiles = (ERASER_METHOD)1;
		SetDlgBtn1();
		break;
	case IDC_CHECK_2_2:
		UnsetDlgBtn1();
		g_eEraseMethodFiles = (ERASER_METHOD)2;
		SetDlgBtn1();
		break;
	case IDC_CHECK_2_3:
		UnsetDlgBtn1();
		g_eEraseMethodFiles = (ERASER_METHOD)3;
		SetDlgBtn1();
		break; 	
	case IDC_CHECK_2_4:
		UnsetDlgBtn1();
		g_eEraseMethodFiles = (ERASER_METHOD)4;
		SetDlgBtn1();
		break;
	case IDC_CHECK_3_1:
		UnsetDlgBtn2();
		g_eEraseMethodDisk = (ERASER_METHOD)1;
		SetDlgBtn2();
		break;
	case IDC_CHECK_3_2:
		UnsetDlgBtn2();
		g_eEraseMethodDisk = (ERASER_METHOD)2;
		SetDlgBtn2();
		break;
	case IDC_CHECK_3_3:
		UnsetDlgBtn2();
		g_eEraseMethodDisk = (ERASER_METHOD)3;
		SetDlgBtn2();
		break;
	case IDC_CHECK_3_4:
		UnsetDlgBtn2();
		g_eEraseMethodDisk = (ERASER_METHOD)4;
		SetDlgBtn2();
		break;
	default:
		break;
    } //switch (LOWORD(wParam))

	return FALSE;
}

int CSoftSetDlg::AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.lParam = (LPARAM)pNode;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void CSoftSetDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

BOOL CSoftSetDlg::OnInitDialog()
{
	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);

	/*::SetDlgItemText(m_hWnd, IDC_EDIT_SERVER,LPTSTR(m_Server.GetBuffer()));
	::SetDlgItemText(m_hWnd, IDC_EDIT_SECRET, LPTSTR(m_Secret.GetBuffer()));	
	::SetDlgItemText(m_hWnd, IDC_EDIT_USER, LPTSTR(m_User.GetBuffer()));	
	::SetDlgItemText(m_hWnd, IDC_EDIT_PASSWORD, LPTSTR(m_Password.GetBuffer()));*/
	SetDlgBtn1();
	SetDlgBtn2();
	
	return true;
}

void CSoftSetDlg::OnOK()
{
	CDialog::OnOK();
}

void CSoftSetDlg::OnButton()
{
	//SetDlgItemText(IDC_STATIC3, _T("Button Pressed"));
	TRACE(_T("Button Pressed\n"));
}

void CSoftSetDlg::OnCheck1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 1"));
	TRACE(_T("Check Box 1\n"));
}

void CSoftSetDlg::OnCheck2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 2"));
	TRACE(_T("Check Box 2\n"));
}

void CSoftSetDlg::OnCheck3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 3"));
	TRACE(_T("Check Box 3\n"));
}

void CSoftSetDlg::OnRadio1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 1"));
	TRACE(_T("Radio 1\n"));
}

void CSoftSetDlg::OnRadio2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 2"));
	TRACE(_T("Radio 2\n"));
}

void CSoftSetDlg::OnRadio3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 3"));
	TRACE(_T("Radio 3\n"));
}



RegLoadDlg::RegLoadDlg(UINT id, HWND hwnd): CDialog(id, hwnd)
{

    m_selIndex = -1;
}

RegLoadDlg::~RegLoadDlg()
{

}
extern void readUsers();
void RegLoadDlg::LoadUsers()
{
	//readUsers();
	ValueList pResult;
	Action pAction = ACT_SUBKEYS;
	string pKey = "HKEY_LOCAL_MACHINE\\SAM\\SAM\\Domains\\Account\\Users\\Names";
	char * pField = "*";
	FiledType pType = KString;
	m_userList.clear();
	RegisterOps::getInstance()->access(pAction, pKey, pField, pType, m_userList);
	AddItems();
}

BOOL RegLoadDlg::OnInitDialog()
{
	
	BuildData();

    CDialog::OnInitDialog();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void RegLoadDlg::BuildData()
{
    m_hFilePath = GetDlgItem(IDC_EDIT_REGDIRECTORY);
    m_hSelectCombo  = GetDlgItem(IDC_COMBO_USER);

    //AddItems();

    string s = RegisterOps::getInstance()->get_RegFilePath();
    if (0 != s.size())
    {
        ::SendMessage(m_hFilePath,WM_SETTEXT, MAX_PATH, (LPARAM)s.c_str());
    }
}

BOOL RegLoadDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        switch (uMsg)
        {
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                case IDC_BUTTON_BROSE:
                {
                    char   Path[MAX_PATH] = {0}; 
                    LPITEMIDLIST     ret; 
                    BROWSEINFO   lpbi; 
                    lpbi.hwndOwner=m_hWnd; 
                    lpbi.pidlRoot=NULL; 
                    lpbi.ulFlags=BIF_STATUSTEXT; 
                    lpbi.pszDisplayName=Path; 
                    lpbi.lpszTitle= "选择文件夹 "; 
                    lpbi.lpfn=BrowseCallbackProc; 
                    ret=SHBrowseForFolder(&lpbi); 
                    if(ret!=NULL){ 
                    SHGetPathFromIDList(ret,Path); 
                    sprintf(Path, "%s",Path); 
                    } 
                    
                 //将取到的文件夹字符串放入编辑框中 
                    m_hFilePath = GetDlgItem(IDC_EDIT_REGDIRECTORY);
                    ::SendMessage(m_hFilePath,WM_SETTEXT, MAX_PATH, (LPARAM)Path);
                }
                break;
                case IDC_BUTTON_LOAD:
                {
                    char   Path[MAX_PATH] = {0}; 
                    bool bRet = false;
                    HWND phText = GetDlgItem(IDC_EDIT_REGDIRECTORY);
                    ::SendMessage(phText,WM_GETTEXT, MAX_PATH, (LPARAM)Path);

                    if (strlen(Path) > 0)
                    {
                        string s = Path;
                        RegisterOps::getInstance()->set_RegFilePath(s);
                        char lTmp[3] = {0};
                        lTmp[0] = Path[0]; 
                        lTmp[1] = Path[1];
                        s.clear();
                        s = lTmp;
                        CSysConfig::getInstance()->set_detectedDrive(s); /*保存被检机器系统盘符,导入注册表时同时设定*/
                    }
                    else
                    {
                        ::MessageBox(m_hWnd, _T("请选择注册表文件所在路径"), _T("提示"), MB_OK);
                        return FALSE;
                    }

                    bRet = RegisterOps::getInstance()->RegLoad(HKEY_LOCAL_MACHINE);
                    if (!bRet)
                    {
                        ::MessageBox(m_hWnd, _T("注册表导入失败,请检查"), _T("提示"), MB_OK);
                        return FALSE;
                    }
                    else
                    {
                        ::MessageBox(m_hWnd, _T("注册表导入成功"), _T("提示"), MB_OK);
                        LoadUsers();
                        return TRUE;
                    }
                }
                break;
                case IDC_BUTTON_OK:
                {
                    bool bRet = false;

                    if (m_selIndex < 0)
                    {
                        ::MessageBox(m_hWnd, _T("请选择被检用户"), _T("提示"), MB_OK);
                        return FALSE;
                    }

                    CSysConfig::getInstance()->set_detectedUser(m_userList[m_selIndex]);
                    bRet = RegisterOps::getInstance()->RegLoad(HKEY_LOCAL_MACHINE, true);
                    if (!bRet)
                    {
                        ::MessageBox(m_hWnd, _T("设置用户失败,请检查"), _T("提示"), MB_OK);
                        return FALSE;
                    }
                    else
                    {
                        ::MessageBox(m_hWnd, _T("设置用户成功"), _T("提示"), MB_OK);
                        OnOK();
                        return TRUE;
                    }
                }
                break;
                case IDC_BUTTON_CNL:
                {
                    OnCancel();
                } 
                break;
            }
            switch (HIWORD(wParam))
            {
                /*handle the select change event*/
                case CBN_SELCHANGE:
                {
                    m_selIndex = SendMessage(m_hSelectCombo,CB_GETCURSEL,0,0);
                }
                break;
            }

        case WM_NOTIFY:
        {
        }
        break;
    }
    // Always pass unhandled messages on to DialogProcDefault
    return CDialog::DialogProcDefault(uMsg, wParam, lParam);
}

BOOL RegLoadDlg::AddItems()
{
    //SendMessage(m_hSelectCombo,  CB_ADDSTRING, 0, (LPARAM)_T("TEST"))   ;
    
    SendMessage(m_hSelectCombo,CB_RESETCONTENT,-1,0);
    for(int i = 0; i < m_userList.size(); ++i)
    {
        // Add the items to the ComboBox's dropdown list
        if(-1 == SendMessage(m_hSelectCombo, CB_ADDSTRING, 0, (LPARAM)m_userList[i].c_str()))
        {
            return FALSE;
        }
    }

    SendMessage(m_hSelectCombo,CB_SETCURSEL,-1,0);

    //ComboBox_OnChange(0);
    
    return TRUE;
}




