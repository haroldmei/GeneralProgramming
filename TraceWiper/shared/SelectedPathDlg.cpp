#include "stdafx.h"
#include "testtfileview.h"
#include "SelectedPathDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSelectedPathDlg::CSelectedPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectedPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectedPathDlg)
	m_sPath = _T("");
	//}}AFX_DATA_INIT
}

void CSelectedPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectedPathDlg)
	DDX_Text(pDX, IDC_PATH, m_sPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectedPathDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectedPathDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

