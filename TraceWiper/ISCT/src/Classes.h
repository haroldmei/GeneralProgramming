//////////////////////////////////////////////////////////
// Classes.h - Declaration of CViewClasses, CContainClasses,
//              and CDockClasses classes


#ifndef CLASSES_H
#define CLASSES_H



#include <atlstr.h>
#include "ComboBoxEx.h"

#include "cfgfile.h"


// Declaration of the CViewClasses class
class CViewClasses : public CTreeView
{
public:
    CViewClasses();
    virtual ~CViewClasses();
    virtual void OnInitialUpdate();
    virtual HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR sxText, int iImage, Node *pNode);
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT OnActionNotify(WPARAM wParam, LPARAM lParam);
    void    BuildTree(Node *pRoot);
    LRESULT   OnRButtonDown(TVHITTESTINFO  tvhti,   CPoint   point);
    LRESULT   OnLButtonDown(TVHITTESTINFO  tvhti,   CPoint   point);
    LRESULT   OnLButtonDClick(TVHITTESTINFO  tvhti,   CPoint   point);
    virtual bool TravelChild(HTREEITEM hItem, ItemCheck check);
    virtual void TravelSiblingAndParent(HTREEITEM hItem, ItemCheck nState);
    ItemCheck GetItemState(HTREEITEM hItem, UINT nStateMask) const;
    void setItemState(HTREEITEM hItem, ItemCheck nState);
    void TraverseItemChild(HTREEITEM hItem);
    void ItemChildState(HTREEITEM hItem, ItemCheck nState);
private:
    HIMAGELIST m_himlNormal;
    HIMAGELIST m_himlState;
    HMENU g_hMenu;
    HMENU g_hSubMenu;
    
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
    virtual LRESULT OnNotifyReflect(WPARAM wParam, LPARAM lParam);

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
