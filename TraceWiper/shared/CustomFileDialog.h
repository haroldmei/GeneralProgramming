// CustomFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog dialog

class CCustomFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CCustomFileDialog)
public:
	CCustomFileDialog(BOOL bOpenFileDialog = TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = CCustomFileDialog::szCustomDefFilter,
		LPCTSTR lpszDefExt = CCustomFileDialog::szCustomDefExt,
		LPCTSTR lpszFileName = CCustomFileDialog::szCustomDefFileName,
		CWnd* pParentWnd = NULL);

// Attributes
public:
	CStringList m_listDisplayNames;    // list of actual items selected in listview
	TCHAR m_szBigBuffer[10000];

static CString szCustomDefFilter;
static CString szCustomDefExt;
static CString szCustomDefFileName;
static CString szCustomTitle;

// Operations
public:
	CString FixedGetNextPathName(POSITION& pos) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomFileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual BOOL OnFileNameOK();
	
// Implementation
protected:
	BOOL m_bSelectMode; // TRUE when in "Selection" mode
	
	BOOL ReadListViewNames(CStringList& strlist);	// protected -> not callable without dialog up

	//{{AFX_MSG(CCustomFileDialog)
	afx_msg void OnSelectButton();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg void OnHelp();
	//DECLARE_MESSAGE_MAP()
};
