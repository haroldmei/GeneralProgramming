/////////////////////////////////////////////////
// Files.h - Declaration of the CViewFiles, CContainFiles 
//           and CDockFiles classes

#ifndef FILES_H
#define FILES_H

#define WM_ERASERNOTIFY     (WM_USER + 10)
// wParam values
#define ERASER_WIPE_BEGIN   0
#define ERASER_WIPE_UPDATE  1
#define ERASER_WIPE_DONE    2
#define ERASER_TEST_PAUSED  3

//added by johnz

// Declaration of the CViewFiles class
class CViewFiles : public CListView
{
public:
    CViewFiles();
    virtual ~CViewFiles();
    virtual int  AddItem(LPCTSTR szText, int nImage);
    virtual void InsertItems();
    virtual void OnInitialUpdate();
    virtual void SetColumns();
    virtual BOOL SetSubItem(int nItem, int nSubItem, LPCTSTR szText);
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnActionNotify(WPARAM wParam, LPARAM lParam);
    void DisplayResult(ValueList& titleList,  ValueList& rowList, DNode* pDNode = NULL);
    int getSelectedRowNum();
    IntList getMultiSelectedRowNum();

private:
    HIMAGELIST m_himlSmall;
};

// Declaration of the CContainFiles class
class CContainFiles : public CContainer
{
public:
    CContainFiles(); 
    ~CContainFiles() {}

private:
    CViewFiles m_ViewFiles;
};

// Declaration of the CDockFiles class
class CDockFiles : public CDocker
{
public:
    CDockFiles(); 
    virtual ~CDockFiles() {}
    virtual void OnInitialUpdate();

private:
    CContainFiles m_Files;
};


#endif // FILES_H
