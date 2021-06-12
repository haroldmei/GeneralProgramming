///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include <atlstr.h>
#include "AuthSetDialog.h"
#include "resrc1.h"
#include "resource.h"
#include "cfgfile.h"

// Definitions for the CAuthSetDlg class
CAuthSetDlg::CAuthSetDlg(UINT nResID, HWND hWndParent, CString Server, CString Secret, CString User, CString Password)
	: CDialog(nResID, hWndParent)
{
	//m_ptrEntry = ptrEntry;
	m_Server = Server;
	m_Secret = Secret;
	m_User = User;
	m_Password = Password;

	
	m_hInstRichEdit = ::LoadLibrary(_T("RICHED32.DLL"));
    if (!m_hInstRichEdit)
 		::MessageBox(NULL, _T("CAuthSetDlg::CRichView  Failed to load RICHED32.DLL"), _T(""), MB_ICONWARNING);
}

CAuthSetDlg::~CAuthSetDlg()
{
	::FreeLibrary(m_hInstRichEdit);
}

BOOL CAuthSetDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

BOOL CAuthSetDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	LPTSTR str = new TCHAR[10];
	switch (LOWORD(wParam))
    {
	case IDOK_AUTH:
		::GetDlgItemText(m_hWnd, IDC_EDIT_SERVER, str, 32);
		m_Server = CString(str);

		::GetDlgItemText(m_hWnd, IDC_EDIT_SECRET, str, 32);
		m_Secret = CString(str);
		
		::GetDlgItemText(m_hWnd, IDC_EDIT_USER, str, 32);
		m_User = CString(str);
		
		::GetDlgItemText(m_hWnd, IDC_EDIT_PASSWORD, str, 32);
		m_Password = CString(str);

		CDialog::OnOK();
		break;
	default:
		break;
    } //switch (LOWORD(wParam))

	return FALSE;
}

int CAuthSetDlg::AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.lParam = (LPARAM)pNode;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void CAuthSetDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

BOOL CAuthSetDlg::OnInitDialog()
{
	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);

	::SetDlgItemText(m_hWnd, IDC_EDIT_SERVER,LPTSTR(m_Server.GetBuffer()));
	::SetDlgItemText(m_hWnd, IDC_EDIT_SECRET, LPTSTR(m_Secret.GetBuffer()));	
	::SetDlgItemText(m_hWnd, IDC_EDIT_USER, LPTSTR(m_User.GetBuffer()));	
	::SetDlgItemText(m_hWnd, IDC_EDIT_PASSWORD, LPTSTR(m_Password.GetBuffer()));

	return true;
}

void CAuthSetDlg::OnOK()
{
	CDialog::OnOK();
}

void CAuthSetDlg::OnButton()
{
	//SetDlgItemText(IDC_STATIC3, _T("Button Pressed"));
	TRACE(_T("Button Pressed\n"));
}

void CAuthSetDlg::OnCheck1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 1"));
	TRACE(_T("Check Box 1\n"));
}

void CAuthSetDlg::OnCheck2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 2"));
	TRACE(_T("Check Box 2\n"));
}

void CAuthSetDlg::OnCheck3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 3"));
	TRACE(_T("Check Box 3\n"));
}

void CAuthSetDlg::OnRadio1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 1"));
	TRACE(_T("Radio 1\n"));
}

void CAuthSetDlg::OnRadio2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 2"));
	TRACE(_T("Radio 2\n"));
}

void CAuthSetDlg::OnRadio3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 3"));
	TRACE(_T("Radio 3\n"));
}

