class CFilenameMaskDlg : public CDialog
{
// Construction
public:
	CFilenameMaskDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CFilenameMaskDlg)
	enum { IDD = IDD_FILENAME_MASK };
	CString	m_sMask;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CFilenameMaskDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CFilenameMaskDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
