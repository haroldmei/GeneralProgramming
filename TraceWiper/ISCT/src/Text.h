///////////////////////////////////////////////////////
// Text.h - Declaration of the CViewText, CContainText, 
//          and CDockText classes

#ifndef TEXT_H
#define TEXT_H


#include <richedit.h>


// Declaration of the CViewText class
class CViewText : public CViewFiles//CWnd
{
public:
    CViewText();
    virtual ~CViewText();
    LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    //virtual void PreCreate(CREATESTRUCT &cs); 
    virtual void OnInitialUpdate(); 
    virtual void SetColumns();
    virtual void InsertItems(DNode* pDNode);
    virtual void SetSubItem(int nItem, int nSubItem, int curVal, int TotalVal);

private:
    HMODULE m_hRichEdit;
    HIMAGELIST m_himlSmall;

};

// Declaration of the CContainText class
class CContainText : public CContainer
{
public:
    CContainText();
    ~CContainText() {}

private:
    CViewText m_ViewText;
};

// Declaration of the CDockText class
class CDockText : public CDocker
{
public:
    CDockText();
    virtual ~CDockText() {}
    virtual void OnInitialUpdate();

private:
    CContainText m_View;

};


#endif // TEXT_H

