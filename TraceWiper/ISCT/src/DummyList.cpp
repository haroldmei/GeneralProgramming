// DummyList.cpp : implementation file
//
#ifdef _WINDOWS_
#undef _WINDOWS_

#include "stdafx.h"
//#include "tabwnd.h"
#include "DummyList.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDummyList

IMPLEMENT_DYNCREATE(CDummyList, CListView)

CDummyList::CDummyList()
{
}

CDummyList::~CDummyList()
{
}


BEGIN_MESSAGE_MAP(CDummyList, CListView)
	//{{AFX_MSG_MAP(CDummyList)
	ON_WM_CREATE()
	ON_MESSAGE(WM_ADDCONTACT, OnAddContact)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDummyList drawing

void CDummyList::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDummyList diagnostics

#ifdef _DEBUG
void CDummyList::AssertValid() const
{
	CListView::AssertValid();
}

void CDummyList::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDummyList message handlers

void CDummyList::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
}

int CDummyList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
  CListCtrl& lc = GetListCtrl();
  // insert columns
  lc.InsertColumn(0,_T("SEQ"),LVCFMT_LEFT,150);
  lc.InsertColumn(1,_T("IP"),LVCFMT_LEFT,150);
  lc.InsertColumn(2,_T("TCP"),LVCFMT_LEFT,100);
  lc.InsertColumn(3,_T("UDP"),LVCFMT_LEFT,170);
  // insert rows

	return 0;
}

BOOL CDummyList::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= (LVS_SHOWSELALWAYS | LVS_REPORT | 
               /*LVS_SINGLESEL | */LVS_NOSORTHEADER);
	
	return CListView::PreCreateWindow(cs);
}

void CDummyList::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CListView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

afx_msg LRESULT CDummyList::OnAddContact(WPARAM wParam, LPARAM lParam)
{
	CListCtrl& lc = GetListCtrl();
	CString **ppstrMst = (CString**)lParam;
	TCHAR szSequence[7] = {0};
	CString *strIP = ppstrMst[0];
	CString *strTCP = ppstrMst[1];
	CString *strUDP = ppstrMst[2];
	
	swprintf(szSequence, _T("%d"), lc.GetItemCount());

	lc.InsertItem(0,(LPCTSTR)szSequence);
    lc.SetItemText(0,1,(LPCTSTR)strIP->GetBuffer());
    lc.SetItemText(0,2,(LPCTSTR)strTCP->GetBuffer());
    lc.SetItemText(0,3,(LPCTSTR)strUDP->GetBuffer());

	//delete strIP;
	//delete strTCP;
	//delete strUDP;

	return afx_msg LRESULT();
}

#endif