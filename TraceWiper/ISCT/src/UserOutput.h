///////////////////////////////////////////////////////
// Output.h - Declaration of the CViewUserOutput, CContainUserOutput, 
//          and CDockUserOutput classes

#ifndef USEROUTPUT_H
#define USEROUTPUT_H


#include <richedit.h>
#include "resource.h"

// Declaration of the CViewText class
class CViewUserOutput : public CViewClasses//CWnd
{
public:
	CViewUserOutput();
	virtual ~CViewUserOutput();

protected:
	//virtual void PreCreate(CREATESTRUCT &cs); 
	virtual void OnInitialUpdate(); 
	virtual void SetColumns();
	virtual void InsertItems();
    HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage, PRLIST_TYPE enType);
    LRESULT   OnLButtonDClick(TVHITTESTINFO  tvhti,   CPoint   point);
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HMODULE m_hRichEdit;
	HIMAGELIST m_himlSmall;

};

// Declaration of the CContainText class
class CContainUserOutput : public CContainer
{
public:
	CContainUserOutput() 
	{
		SetView(m_ViewOutput); 
		SetDockCaption (_T("被检人员 - 被检人员"));
		SetTabText(_T("人员列表"));
		SetTabIcon(IDI_TEXT);
	} 
	~CContainUserOutput() {}

private:
	CViewUserOutput m_ViewOutput;
};

// Declaration of the CDockText class
class CDockUserOutput : public CDocker
{
public:
	CDockUserOutput();
	virtual ~CDockUserOutput() {}
	virtual void OnInitialUpdate();

private:
	CContainUserOutput m_View;

};


#endif // USEROUTPUT_H

