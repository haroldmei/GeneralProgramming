///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include "SvrParaSetDialog.h"
#include "resrc1.h"
#include "resource.h"
#include <atlstr.h>
#include "cfgfile.h"

// Definitions for the CSvrParaSetDlg class
CSvrParaSetDlg::CSvrParaSetDlg(UINT nResID, HWND hWndParent, DWORD ptrEntry)
	: CDialog(nResID, hWndParent)
{
	m_ptrEntry = ptrEntry;
	m_hInstRichEdit = ::LoadLibrary(_T("RICHED32.DLL"));
    if (!m_hInstRichEdit)
 		::MessageBox(NULL, _T("CSvrParaSetDlg::CRichView  Failed to load RICHED32.DLL"), _T(""), MB_ICONWARNING);
}

CSvrParaSetDlg::~CSvrParaSetDlg()
{
	::FreeLibrary(m_hInstRichEdit);
}

BOOL CSvrParaSetDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		//Additional messages to be handled go here
	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CSvrParaSetDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam))
    {

	default:
		break;
    } //switch (LOWORD(wParam))

	return FALSE;
}

int CSvrParaSetDlg::AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.lParam = (LPARAM)pNode;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void CSvrParaSetDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

BOOL CSvrParaSetDlg::OnInitDialog()
{
	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);


	return true;
}

void CSvrParaSetDlg::OnOK()
{
	CDialog::OnOK();
}

void CSvrParaSetDlg::OnButton()
{
	//SetDlgItemText(IDC_STATIC3, _T("Button Pressed"));
	TRACE(_T("Button Pressed\n"));
}

void CSvrParaSetDlg::OnCheck1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 1"));
	TRACE(_T("Check Box 1\n"));
}

void CSvrParaSetDlg::OnCheck2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 2"));
	TRACE(_T("Check Box 2\n"));
}

void CSvrParaSetDlg::OnCheck3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 3"));
	TRACE(_T("Check Box 3\n"));
}

void CSvrParaSetDlg::OnRadio1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 1"));
	TRACE(_T("Radio 1\n"));
}

void CSvrParaSetDlg::OnRadio2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 2"));
	TRACE(_T("Radio 2\n"));
}

void CSvrParaSetDlg::OnRadio3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 3"));
	TRACE(_T("Radio 3\n"));
}

