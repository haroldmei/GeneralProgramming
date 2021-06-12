///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include "EntrySetDialog.h"
#include "resrc1.h"
#include "resource.h"
#include <atlstr.h>
#include "cfgfile.h"

// Definitions for the CEntrySetDlg class
CEntrySetDlg::CEntrySetDlg(UINT nResID, HWND hWndParent, DWORD ptrEntry)
	: CDialog(nResID, hWndParent)
{
	m_ptrEntry = ptrEntry;
	m_hInstRichEdit = ::LoadLibrary(_T("RICHED32.DLL"));
    if (!m_hInstRichEdit)
 		::MessageBox(NULL, _T("CEntrySetDlg::CRichView  Failed to load RICHED32.DLL"), _T(""), MB_ICONWARNING);
}

CEntrySetDlg::~CEntrySetDlg()
{
	::FreeLibrary(m_hInstRichEdit);
}

BOOL CEntrySetDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		//Additional messages to be handled go here
	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CEntrySetDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	int index = -1;

	Config *pCfg = (Config*)m_ptrEntry;
	tString strGet;
	tString strGet1;
	CString strVal;
	CString strVal1;
	switch (LOWORD(wParam))
    {
	case IDC_COMBO_NODETYPE:
		switch (HIWORD(wParam))
		{
			case CBN_SELCHANGE:
				index = ::SendMessage(GetDlgItem(IDC_COMBO_NODETYPE), CB_GETCURSEL, 0, 0);
				switch (index)
				{
					case 2://注册表值
						::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, TRUE, 0L);
						::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, TRUE, 0L);

						::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, TRUE, 0L);
						::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, FALSE, 0L);
						pCfg->setCfgType(TYPE_REG_VAL);
						break;
					case 1://注册表键
						::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, TRUE, 0L);
						::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, TRUE, 0L);

						::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, FALSE, 0L);
						::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, TRUE, 0L);
						pCfg->setCfgType(TYPE_REG);
						break;
					case 0://文件/目录
						::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, FALSE, 0L);
						::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, FALSE, 0L);

						::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, TRUE, 0L);
						::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, TRUE, 0L);
						pCfg->setCfgType(TYPE_DIR);
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		break;
	case ID_CONFIRM:
		index = ::SendMessage(GetDlgItem(IDC_COMBO_NODETYPE), CB_GETCURSEL, 0, 0);
		switch (index)
		{
			case 2://注册表值
				strGet = GetDlgItemString(IDC_EDIT_REGVAL);
				strVal = CString(strGet.c_str());
				pCfg->setCfgValue(strVal);
				break;
			case 1://注册表键
				strGet = GetDlgItemString(IDC_EDIT_REGKEY);
				strVal = CString(strGet.c_str());
				pCfg->setCfgValue(strVal);
				break;
			case 0://文件/目录
				strGet = GetDlgItemString(IDC_EDIT_DIR);
				strGet1 = GetDlgItemString(IDC_EDIT_FILETYPE);
				strVal = CString(strGet.c_str());
				strVal1 = CString(strGet1.c_str());

				if (!strcmp(strGet1.c_str(), ""))
				{
					pCfg->setCfgValue(strVal);
					pCfg->setCfgType(TYPE_DIR);
				}
				else
				{
					if (!strcmp(strGet.c_str(), ""))
					{
						MessageBox("请指定路径！", "请指定路径！", MB_OK);
						return FALSE;
					}
					pCfg->setCfgValue(strVal + CString("\\") + strVal1);
					pCfg->setCfgType(TYPE_FILE_TYPE);
				}
				break;
			default:
				break;
		}
		OnOK();
		break;
	case ID_ENTRY_CANCEL:
		OnCancel();
		break;
	default:
		break;
    } //switch (LOWORD(wParam))

	return FALSE;
}

int CEntrySetDlg::AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.lParam = (LPARAM)pNode;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void CEntrySetDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

BOOL CEntrySetDlg::OnInitDialog()
{
	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);
	

	::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_ADDSTRING, 0, (LPARAM) _T("注册表键"));//2
	::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_ADDSTRING, 0, (LPARAM) _T("注册表值"));//1
	::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_ADDSTRING, 0, (LPARAM) _T("目录/文件"));//0
	
	Config *pCfg = (Config*)m_ptrEntry;
	if (!pCfg)
	{
		return true;
	}


	switch (pCfg->getCfgType())
	{
		case TYPE_REG:
			SetDlgItemText(IDC_EDIT_REGKEY, pCfg->getCfgValue());			
			::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, TRUE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, TRUE, 0L);

			::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, FALSE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, TRUE, 0L);


			::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_SETCURSEL, 1, 0);

			break;
		case TYPE_REG_VAL:
			SetDlgItemText(IDC_EDIT_REGVAL, pCfg->getCfgValue());
			
			::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, TRUE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, TRUE, 0L);

			::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, TRUE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, FALSE, 0L);

			::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_SETCURSEL, 1, 0);
			break;
		case TYPE_FILE:
			SetDlgItemText(IDC_EDIT_DIR, pCfg->getCfgValue());
			
			::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, FALSE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, FALSE, 0L);

			::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, TRUE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, TRUE, 0L);

			
			::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_SETCURSEL, 0, 0);
			break;
		case TYPE_DIR:
			SetDlgItemText(IDC_EDIT_DIR, pCfg->getCfgValue());
			
			::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, FALSE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, FALSE, 0L);

			::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, TRUE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, TRUE, 0L);

			
			::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_SETCURSEL, 0, 0);
			break;
		case TYPE_FILE_TYPE:
			SetDlgItemText(IDC_EDIT_FILETYPE, pCfg->getCfgValue());
			
			::SendMessage(GetDlgItem(IDC_EDIT_DIR), EM_SETREADONLY, FALSE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_FILETYPE), EM_SETREADONLY, FALSE, 0L);

			::SendMessage(GetDlgItem(IDC_EDIT_REGKEY), EM_SETREADONLY, TRUE, 0L);
			::SendMessage(GetDlgItem(IDC_EDIT_REGVAL), EM_SETREADONLY, TRUE, 0L);

			
			::SendMessage( GetDlgItem(IDC_COMBO_NODETYPE), CB_SETCURSEL, 0, 0);
			break;
		default:
			break;
	}
	
	TRACE(pCfg->getCfgValue());

	return true;
}

void CEntrySetDlg::OnOK()
{
	CDialog::OnOK();
}

void CEntrySetDlg::OnButton()
{
	//SetDlgItemText(IDC_STATIC3, _T("Button Pressed"));
	TRACE(_T("Button Pressed\n"));
}

void CEntrySetDlg::OnCheck1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 1"));
	TRACE(_T("Check Box 1\n"));
}

void CEntrySetDlg::OnCheck2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 2"));
	TRACE(_T("Check Box 2\n"));
}

void CEntrySetDlg::OnCheck3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 3"));
	TRACE(_T("Check Box 3\n"));
}

void CEntrySetDlg::OnRadio1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 1"));
	TRACE(_T("Radio 1\n"));
}

void CEntrySetDlg::OnRadio2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 2"));
	TRACE(_T("Radio 2\n"));
}

void CEntrySetDlg::OnRadio3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 3"));
	TRACE(_T("Radio 3\n"));
}

