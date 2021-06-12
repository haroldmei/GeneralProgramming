#include "stdafx.h"
#include "testtfileview.h"
#include "RootFolderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRootFolderDlg::CRootFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRootFolderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRootFolderDlg)
	m_sFolder = _T("");
	//}}AFX_DATA_INIT
}

void CRootFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRootFolderDlg)
	DDX_Text(pDX, IDC_FOLDER, m_sFolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRootFolderDlg, CDialog)
	//{{AFX_MSG_MAP(CRootFolderDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

