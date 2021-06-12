//////////////////////////////////////////////////////////
// Classes.h - Declaration of CViewClasses, CContainClasses,
//              and CDockClasses classes


#ifndef CLASSES_H
#define CLASSES_H



#include <atlstr.h>
#include "ComboBoxEx.h"
  
#include "cfgfile.h"

/*
#define ID_Menu               142
#define ID__40002				143
#define ID__40003				144

*/
#define ID_ADDNEWNODE		43002
#define ID_DELNODE			43003
#define ID_MODIFYNODE		43004
#define ID_COPENODE			43005
#define ID_EXCUTENODE		43006
#define ID_PASTENODE		43007





// Declaration of the CViewClasses class
class CViewClasses : public CTreeView
{
public:
	CViewClasses();
	virtual ~CViewClasses();
	virtual void OnInitialUpdate();
	virtual HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR sxText, int iImage, Node *pNode);
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnEraserNotify(WPARAM wParam, LPARAM lParam);
	void	BuildTree(Node *pRoot);
	LRESULT   OnRButtonDown(TVHITTESTINFO  tvhti,   CPoint   point);
	LRESULT   OnLButtonDown(TVHITTESTINFO  tvhti,   CPoint   point);
	bool   SetItemChecked(HTREEITEM hItem, ItemCheck check);

private:
	HIMAGELIST m_himlNormal;

    HIMAGELIST m_himlState;

	HMENU g_hMenu;
	HMENU g_hSubMenu;
	Node			*m_InProgess;
	
};

// Declaration of the CContainClasses class
class CContainClasses : public CContainer
{
public:
	CContainClasses();
	virtual ~CContainClasses() {}
	void AddCombo();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM /*lParam*/);
	virtual void SetupToolbar();
	CViewClasses& getView(){return m_ViewClasses;};

private:
	CViewClasses m_ViewClasses;
	CComboBoxEx_Win32xx m_ComboBoxEx;
};

// Declaration of the CDockClasses class
class CDockClasses : public CDocker
{
public:
	CDockClasses(); 
	virtual ~CDockClasses() {}
	virtual void OnInitialUpdate();
	CContainClasses& getContain(){return m_Classes;};
private:
	CContainClasses m_Classes;

};



#endif  //CLASSES_H
