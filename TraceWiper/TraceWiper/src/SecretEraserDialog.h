///////////////////////////////////////
// MyDialog.h

#ifndef SECRETERASERDIALOG_H
#define SECRETERASERDIALOG_H

#include <string>

#include "../../platform/SysGlobal.h"
#include "../../platform/CSector.h"
#include "..\..\eraserlib\eraserdll.h"
#define MAX_PROCESS_DRIVE 16
typedef enum
{
    SRCH_START,
    SRCH_STOP,
    SRCH_PAUSE,
}SEARCH_STATE;

typedef enum
{
    SRCH_DISK,
    SRCH_DIRNFILE,
    SRCH_SECTOR,
    SRCH_END,
}SEARCH_TYPE;

typedef enum
{
    SRCH_DISK_FULL,
	SRCH_DISK_FREE,
	SRCH_DISK_END,
}SEARCH_TYPE_DISK;

typedef enum
{
    SRCH_DIRNFILE_FULL,
	SRCH_DIRNFILE_END,
}SEARCH_TYPE_DIRNFILE;

typedef enum
{
    SRCH_SECTOR_MINGAN,
	SRCH_SECTOR_SHANGWANG,
	SRCH_SECTOR_END,
}SEARCH_TYPE_SECTOR;

using namespace std;

// Declaration of the CSecretEraserDlg class
class CSecretEraserDlg : public CDialog
{
public:
	CSecretEraserDlg(UINT nResID, HWND hWndParent);
	virtual ~CSecretEraserDlg();
	int AddItem(HWND hList1, LPCTSTR szText, int nImage, void* pNode);
	HTREEITEM AddItem(HWND hwnd, HTREEITEM hParent, LPCTSTR szText, int iImage);
	HTREEITEM AddItem(HWND hwnd, HTREEITEM hParent, LPCTSTR szParam, int iImage, LPCTSTR szText);
	void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);
	bool CheckBuffer(char* pBuf);
	void getSectorAddress(char* devName, LONGLONG& lPartBegin, LONGLONG& lPartEnd);
	bool compareStrByMem(void* s1, void* s2, int len);
	int getSelectedSectorNum();
	void getSectorBuffer(HANDLE hDev, DWORD lSectorNum, char* lpSectBuff, bool bcloseFlag);
	void setSectorBuffer(HANDLE hDev, DWORD lSectorNum, char* lpSectBuff, bool bcloseFlag);
	bool ReadSectors(char* devName, LONGLONG lPartBegin, LONGLONG lPartEnd);
	bool hasKeywordsInString(string strSrc, ValueList keywordsList, string& keyword);
	void findFilesByKeywords(string path, string keywords);
	CValueList getDriveList(){return m_driveList;};
    string get_keywords() {return m_keywords;};
    string get_keywordsFileDir() {return m_keywordsFileDir;};
	BOOL CheckURL(char *szURL);
	void SetControls();
	LRESULT setupDrive(HWND hwnd, TVHITTESTINFO  tvhti,   CPoint   point);
	bool SetItemChecked(HWND hwnd, HTREEITEM hItem, ItemCheck check, bool isRoot);
	LRESULT OnEraserNotify(WPARAM wParam, LPARAM lParam);

    HWND m_hProgress;
	

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();

private:
	void OnButton();
	void OnRadio1();
	void OnRadio2();
	void OnRadio3();
	void OnCheck1();
	void OnCheck2();
	void OnCheck3();

	HMODULE m_hInstRichEdit;
	HWND m_hLeftTree; //
    HWND m_hCurSectorText;
    ValueList m_resultList;
    string m_keywords;
	string m_keywordsFileDir;
	ValueList m_urlList;
    DWORD m_URLCount;
    HWND m_hList;
    HANDLE m_hdSector;

	SEARCH_TYPE m_SearchType;
	SEARCH_TYPE_DISK m_SearchDiskType;
	SEARCH_TYPE_DIRNFILE m_SearchDirNNameType;
	SEARCH_TYPE_SECTOR m_SearchSectorType;

	CValueList m_driveList;

	bool	m_isRunning;
	HANDLE  m_hThreadId[MAX_PROCESS_DRIVE];
};

#endif //MYDIALOG_H
