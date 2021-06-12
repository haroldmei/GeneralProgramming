#ifndef _DISKSCAN_DLG_H_
#define _DISKSCAN_DLG_H_

#include <string>
#include "Classes.h"


typedef enum
{
    SRCH_START,
    SRCH_STOP,
    SRCH_PAUSE,
}SEARCH_STATE;

typedef struct
{
    int secIndex;
    string secDevName;
}ST_SECINFO;


using namespace std;

class DiskScanDlg : public CDialog/*, public CViewClasses*/
{
    // Construction
public:
    DiskScanDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~DiskScanDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void BuildLeftTree(HWND hWnd);
    virtual LRESULT setupDrive(HWND hwnd, TVHITTESTINFO  tvhti,   CPoint   point);
    virtual CValueList getDriveList(){return m_driveList;};
    virtual void clearResult(HWND hwnd);
    
    virtual void DisplayResult(HWND hwnd, ValueList& lTitleList, ValueList& lRowList);
    virtual HWND get_selfHandle() {return m_hwnd;};

    
    virtual HTREEITEM AddItem(HWND hwnd, HTREEITEM hParent, LPCTSTR szParam, int iImage, LPCTSTR szText = "");
    
    virtual int AddItem(HWND hList1,LPCTSTR szText, int nImage);
    virtual void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);

    /*end the thread*/
    virtual void endThread(HANDLE lThread);
    virtual void set_runState(SEARCH_STATE enState);
    virtual bool   SetItemChecked(HWND hwnd, HTREEITEM hItem, ItemCheck check, bool isRoot);
    virtual HWND GetHwnd() {return m_hWnd;}
    virtual void clearAll();
    
protected:    
    virtual BOOL OnInitDialog();
protected:
    HWND m_hwnd;
    HWND m_hLeftTree;

    //string m_drive;
    CValueList m_driveList;
    SEARCH_STATE m_enState;

    HANDLE m_hThreadId;

    /*store the result*/
    ValueList m_resultList;
    ValueList m_titleList;
    ValueList m_oneRowList;
    ValueList m_diskList;
    int m_TitleColNum;
};

class BasicDiskScanDlg : public DiskScanDlg
{
// Construction
public:
    BasicDiskScanDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~BasicDiskScanDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void BuildColumn();    
    virtual void BuildLeftTree(HWND hWnd);
    virtual void setupHandle();
    virtual HWND get_ListHandle(){return m_hList;};
    string get_keywords() {return m_keywords;};
    bool set_keywords();
    void set_runState(SEARCH_STATE enState);
    void DisplayResult(HWND hwnd, ValueList& lTitleList, ValueList lRowList);
    virtual bool hasKeywordsInString(string strSrc, ValueList keywordsList, string& keyword);
    virtual void findFilesByKeywords(string path, string keywords);
    virtual void clearAll();
    void fileFound(struct _finddata_t filefind, string path, string matchkeyword, ValueList & lTitleList);
private:
        /*句柄定义*/
        HWND m_hBasicKeywords;
        //HWND m_hLeftTree;
        HWND m_hSearch;
        HWND m_hList;
        HWND m_hSResultText;
        HWND m_hSaveResult;
        //string m_drive;
        string m_keywords;

        bool m_bSaveResult;
        string m_saveName;
};

class DeepDiskScanDlg : public DiskScanDlg
{
// Construction
public:
    DeepDiskScanDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~DeepDiskScanDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void BuildColumn();    
    //virtual void BuildLeftTree(HWND hWnd);
    virtual void setupHandle();
    virtual HWND get_ListHandle(){return m_hList;};
    void set_runState(SEARCH_STATE enState);
    void DisplayResult(HWND hwnd, ValueList& lTitleList, ValueList& lRowList);
    BOOL CheckURL(char *szURL);
    void CheckBuffer(char* pBuf);    
    void SectorURLScan(HANDLE hDev, string lpath);
    void addResult(string& lStr);
    void clearResult();
    void getResult(ValueList& lList);
    void setAction(bool bVal);
    void setPercent(DWORD lVal);    
    virtual void clearAll();
    void set_hdSector(HANDLE hd){m_hdSector = hd;}
    void closeHandleSector(){CloseHandle(m_hdSector); m_hdSector = NULL;}
private:
    
    ValueList m_urlList;
    DWORD m_URLCount;
    bool m_bSaveResult;
    string m_saveName;
    
    HWND m_hSaveResult;
    HANDLE m_hdSector;
    
public:
        /*句柄定义*/
        //HWND m_hLeftTree;
        HWND m_hList;
        HWND m_hProgress;
        HWND m_hStart;
};

class FullDiskScanDlg : public DiskScanDlg
{
// Construction
public:
    FullDiskScanDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~FullDiskScanDlg();
    
    BOOL OnInitDialog();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual int AddItem(HWND hwnd,LPCTSTR szText, int nImage);
    virtual void BuildColumn();    
    //virtual void BuildLeftTree(HWND hWnd);    
    //LRESULT   setupDrive(HWND hwnd, TVHITTESTINFO  tvhti,   CPoint   point);
    
    void AllocateSectorInfo();    
    void FreeSectorInfo();
    
    bool set_keywords();
    virtual void setupHandle();
    virtual HWND get_ListHandle(){return m_hList;};
    void set_runState(SEARCH_STATE enState);
    void DisplayResult(int lCurDriveIndex, HWND hwnd, ValueList& lTitleList, ValueList& lRowList);
    bool ReadSectors(int lCurDriveIndex, char* devName, __int64 lPartBegin, __int64 lPartEnd);
    void getSectorBuffer(HANDLE hDev, __int64 lSectorIndex, __int64 lSectorNum, char* lpSectBuff, bool bcloseFlag);
    bool getSectorAddress(char* devName, __int64& lPartBegin, __int64& lPartEnd);
    bool compareStrByMem(void* s1, void* s2, int len);
    void set_fmtDevName(char* val) {m_fmtSectorDevName = val;}
    int   getSelectedSectorNum(int& secIndex);    
    void DisplaySectorData(int sectorIndex, string& curSecDevName);
    __int64 FullDiskScanDlg::mySetFilePointer (HANDLE hf, __int64 distance, DWORD MoveMethod);
    void MatchKeywordsInSector(DWORD dwSectorIndex, char* lSectorBuf);
    string get_sectorDevNameByIndex(int secIndex);
    virtual HWND GetHwnd() {return m_hwnd;}
    void set_DeepPointer(DeepDiskScanDlg* p);
    virtual void clearAll();
    void convertSectorToString(string& lOutString, unsigned char* pSectorBuf, unsigned int lStartIndex, unsigned int lEndIndex);

private:
        /*句柄定义*/
        //HWND m_hLeftTree;
        HWND m_hKeywords;
        HWND m_hStart;
        HWND m_hList;
        HWND m_hEdit1;
        HWND m_hEdit2;
        HWND m_hProgress;
        HWND m_hCurSectorText;
        HWND m_hStatusBar;
        HWND m_hCheckURL;
        HWND m_hSaveResult;
        
        HANDLE m_hdSector;
        
        string m_keywords;
        string m_fmtSectorDevName;
        string m_currentSectorDevName;
        ST_SECINFO* m_pArrySecInfo;
        int m_secSize;
        char* m_pSecBuffer;
        DeepDiskScanDlg* m_pDeepDlg;
        bool m_bCheckURL;
        bool m_bSaveResult;
        string m_saveName;
};

class DiskTabDlg : public CDialog
{
// Construction
public:
    DiskTabDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~DiskTabDlg();
    BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void BuildData();
protected:    
    virtual BOOL OnInitDialog();
private:
    CTab m_Tab;
    BasicDiskScanDlg* m_pBasicDlg;
    FullDiskScanDlg*    m_pFullDlg;
    DeepDiskScanDlg* m_pDeepDlg;
};


DWORD WINAPI FileSearchTask(LPVOID lParam);
DWORD WINAPI sectorScanTask(LPVOID lParam);
DWORD WINAPI SectorURLScanTask(LPVOID  lParam);


#endif
