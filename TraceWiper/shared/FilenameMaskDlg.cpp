#include "stdafx.h"
#include "testtfileview.h"
#include "FilenameMaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFilenameMaskDlg::CFilenameMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFilenameMaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFilenameMaskDlg)
	m_sMask = _T("");
	//}}AFX_DATA_INIT
}

void CFilenameMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilenameMaskDlg)
	DDX_Text(pDX, IDC_MASK, m_sMask);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFilenameMaskDlg, CDialog)
	//{{AFX_MSG_MAP(CFilenameMaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

