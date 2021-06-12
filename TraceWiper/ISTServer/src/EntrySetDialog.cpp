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

	switch (LOWORD(wParam))
    {

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

