// OptionPages.h
//
// Eraser. Secure data removal. For Windows.
// Copyright © 1997-2001  Sami Tolvanen (sami@tolvanen.com).
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef __OPTIONPAGES_H__
#define __OPTIONPAGES_H__

#include "../shared/FlatListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsForFiles dialog

class COptionsForFreeSpace;

class COptionsForFiles : public CPropertyPage
{
    DECLARE_DYNCREATE(COptionsForFiles)

// Construction
public:
    void EnableButtons(BYTE);
    COptionsForFiles();
    ~COptionsForFiles();

    void UpdateList();

    LibrarySettings *m_plsSettings;

    COptionsForFreeSpace *m_ppgFreeSpace;
    BYTE m_nSelectedMethodID;
    int m_nSelectedMethod;

// Dialog Data
    //{{AFX_DATA(COptionsForFiles)
	enum { IDD = IDD_PAGE_FILES };
    CFlatListCtrl   m_lcMethod;
    CString m_strSelected;
	BOOL	m_bFileClusterTips;
	BOOL	m_bFileNames;
    BOOL    m_bFileAlternateDataStreams;
	//}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(COptionsForFiles)
    public:
    virtual void OnOK();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(COptionsForFiles)
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonDelete();
    afx_msg void OnButtonEdit();
    afx_msg void OnButtonNew();
    afx_msg void OnItemchangedListMethod(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListMethod(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// COptionsForFreeSpace dialog

class COptionsForFreeSpace : public CPropertyPage
{
    DECLARE_DYNCREATE(COptionsForFreeSpace)

// Construction
public:
    void EnableButtons(BYTE);
    COptionsForFreeSpace();
    ~COptionsForFreeSpace();

    void UpdateList();

    LibrarySettings *m_plsSettings;

    COptionsForFiles *m_ppgFiles;
    BYTE m_nSelectedMethodID;
    int m_nSelectedMethod;

// Dialog Data
    //{{AFX_DATA(COptionsForFreeSpace)
    enum { IDD = IDD_PAGE_FREESPACE };
    CFlatListCtrl   m_lcMethod;
    BOOL    m_bClusterTips;
    BOOL    m_bDirectoryEntries;
    BOOL    m_bFreeSpace;
    CString m_strSelected;
    //}}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(COptionsForFreeSpace)
    public:
    virtual void OnOK();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(COptionsForFreeSpace)
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonEdit();
    afx_msg void OnButtonNew();
    afx_msg void OnButtonDelete();
    afx_msg void OnItemchangedListMethod(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListMethod(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};



#endif // __OPTIONPAGES_H__
