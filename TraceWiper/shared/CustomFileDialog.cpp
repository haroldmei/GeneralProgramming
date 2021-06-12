// CustomFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CustomFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog

IMPLEMENT_DYNAMIC(CCustomFileDialog, CFileDialog)

// Filter string
CString CCustomFileDialog::szCustomDefFilter(_T("All Files (*.*)|*.*||"));
CString CCustomFileDialog::szCustomDefExt(_T("*.*"));
CString CCustomFileDialog::szCustomDefFileName(_T("*.*"));
CString CCustomFileDialog::szCustomTitle(_T("Select File to be Erased"));

CCustomFileDialog::CCustomFileDialog(BOOL bOpenFileDialog, DWORD dwFlags, 
			LPCTSTR lpszFilter, // = szCustomDefFilter
			LPCTSTR lpszDefExt, //  = szCustomDefExt
			LPCTSTR lpszFileName, // = szCustomDefFileName
			CWnd* pParentWnd) : // = NULL
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	
	m_bSelectMode = TRUE;

	// Most of the "customization" of CCustomFileDialog is set by the dwFlags
	// passed in to the constructor. Attempts to enable these features after
	// constructing the CCustomFileDialog, such as accessing the m_ofn structure
	// directly, may cause CCustomFileDialog to not work correctly

	m_szBigBuffer[0]='\0';
	m_ofn.lpstrFile = m_szBigBuffer;

	m_ofn.nMaxFile = _MAX_PATH;

	if (m_ofn.Flags & OFN_EXPLORER)
		{
		// MFC added it, but we don't want it
			m_ofn.Flags &= ~(OFN_EXPLORER | OFN_SHOWHELP);
		}
}

void CCustomFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	
}

BOOL CCustomFileDialog::ReadListViewNames(CStringList& strlist)
{
		return FALSE;  
}

BOOL CCustomFileDialog::OnFileNameOK()
{
	if (!m_bSelectMode)
	{
		// only one set of selections
		ReadListViewNames(m_listDisplayNames);	
	}

	// CFileDialog's m_ofn.lpstrFile will contain last set of selections!
	return FALSE;
}

void CCustomFileDialog::OnSelectButton() 
{
	m_bSelectMode = TRUE;

	ReadListViewNames(m_listDisplayNames);

}

BOOL CCustomFileDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	if (pHelpInfo->iContextType == HELPINFO_WINDOW)
	{
		// Will only get WM_HELP from Open File Dialog for our controls
		AfxGetApp()->WinHelp(pHelpInfo->iCtrlId+0x50000,HELP_CONTEXTPOPUP);
	}

	return TRUE; 
}

void CCustomFileDialog::OnContextMenu(CWnd* pWnd, CPoint point) 
{
}

void CCustomFileDialog::OnHelp()
{
	// TODO: How do I bring up the main topic as a contextpopup?
	AfxGetApp()->WinHelp(1, HELP_CONTEXTPOPUP);
}

BOOL CCustomFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// In MFC 4.1, the GetNextPathName function incorrectly checked
// the version of Windows in order to determine which delimiter 
// separated filenames in the m_ofn.lpstrFile structure. Actually,
// it is whether the CFileDialog is old-style or Explorer that
// determines the delimiter.
// MFC 4.2 fixed this problem, and the code there is similar to
// this:
CString CCustomFileDialog::FixedGetNextPathName(POSITION& pos) const
{
#ifndef _MAC
	BOOL bExplorer = m_ofn.Flags & OFN_EXPLORER;
	TCHAR chDelimiter;
	if (bExplorer)
		chDelimiter = '\0';
	else
		chDelimiter = ' ';

	LPTSTR lpsz = (LPTSTR)pos;
	if (lpsz == m_ofn.lpstrFile) // first time
	{
		if ((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}

		// find char pos after first Delimiter
		while(*lpsz != chDelimiter && *lpsz != '\0')
			lpsz = _tcsinc(lpsz);
		lpsz = _tcsinc(lpsz);

		// if single selection then return only selection
		if ((lpsz - m_ofn.lpstrFile) > m_ofn.nFileOffset)
		{
			pos = NULL;
			return m_ofn.lpstrFile;
		}
	}

	CString strPath = m_ofn.lpstrFile;
	if (!bExplorer)
	{
		LPTSTR lpszPath = m_ofn.lpstrFile;
		while(*lpszPath != chDelimiter)
			lpszPath = _tcsinc(lpszPath);
		strPath = strPath.Left(lpszPath - m_ofn.lpstrFile);
	}

	LPTSTR lpszFileName = lpsz;
	CString strFileName = lpsz;

	// find char pos at next Delimiter
	while(*lpsz != chDelimiter && *lpsz != '\0')
		lpsz = _tcsinc(lpsz);

	if (!bExplorer && *lpsz == '\0')
		pos = NULL;
	else
	{
		if (!bExplorer)
			strFileName = strFileName.Left(lpsz - lpszFileName);

		lpsz = _tcsinc(lpsz);
		if (*lpsz == '\0') // if double terminated then done
			pos = NULL;
		else
			pos = (POSITION)lpsz;
	}

	// only add '\\' if it is needed
	if (!strPath.IsEmpty())
	{
		// check for last back-slash or forward slash (handles DBCS)
		LPCTSTR lpsz = _tcsrchr(strPath, '\\');
		if (lpsz == NULL)
			lpsz = _tcsrchr(strPath, '/');
		// if it is also the last character, then we don't need an extra
		if (lpsz != NULL && 
			(lpsz - (LPCTSTR)strPath) == strPath.GetLength()-1)
		{
			ASSERT(*lpsz == '\\' || *lpsz == '/');
			return strPath + strFileName;
		}
	}
	return strPath + '\\' + strFileName;
#else
	pos = NULL;
	return m_ofn.lpstrFile;
#endif
}
