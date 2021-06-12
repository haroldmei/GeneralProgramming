
#ifndef __REGISTRY_H__
#define __REGISTRY_H__

//#include "afx.h"
//#include "afxwin.h"
//#include "windef.h"
#include "atlbase.h"
#include "atlcom.h"
#include "atlcomtime.h"
#include <winreg.h>
#include <string>
#include <vector>
using std::vector;

typedef vector<std::string> ValueList;

class CReg
{
public:
    
	~CReg();
    CReg();
int m_nLastError;
private:
    	

// CReg properties	
protected:
	HKEY m_hRootKey;
	BOOL m_bLazyWrite;
	CString m_strCurrentPath;

public:
	inline BOOL PathIsValid() {
		return (m_strCurrentPath.GetLength() > 0); }
	inline CString GetCurrentPath() {
		return m_strCurrentPath; }
	inline HKEY GetRootKey() {
		return m_hRootKey; }


//CReg	methods
public:
	BOOL ClearKey();
    BOOL SetRootKey(std::string &pKey);
	BOOL SetRootKey(HKEY hRootKey);
	BOOL CreateKey(CString strKey);
	BOOL DeleteKey(CString strKey);
	BOOL DeleteValue(CString strName);
	int GetDataSize(CString strValueName);
	DWORD GetDataType(CString strValueName);
	int GetSubKeyCount();
	int GetValueCount();
	BOOL KeyExists(CString strKey, HKEY hRootKey = NULL);
	BOOL SetKey(CString strKey, BOOL bCanCreate=FALSE);
	BOOL ValueExists(CString strName);
	void RenameValue(CString strOldName, CString strNewName);

	// data reading functions
	//COleDateTime ReadDateTime(CString strName, COleDateTime dtDefault);
	BOOL UnloadKey(CString strKey);
    BOOL ReadSubKeys(ValueList &lValues);
    BOOL ReadDefaultValue( CString & lValue);
	BOOL ReadKeyValues(ValueList &lValues);
    BOOL ReadKeyValueNames(ValueList &lValues);
	double ReadFloat(CString strName, double fDefault);
	CString ReadString(CString strName, CString strDefault);
    CString ReadMutilString(CString strName, CString pSubStr, CString strDefault);
	int ReadInt(CString strName, int nDefault);
	void ReadBinary(CString strName, unsigned char ** pValue, int & pLen);
	BOOL ReadBool(CString strName, BOOL bDefault);
	COLORREF ReadColor(CString strName, COLORREF rgbDefault);
//	BOOL ReadFont(CString strName, CFont* pFont);
//	BOOL ReadPoint(CString strName, CPoint* pPoint);
//	BOOL ReadSize(CString strName, CSize* pSize);
//	BOOL ReadRect(CString strName, CRect* pRect);
	DWORD ReadDword(CString strName, DWORD dwDefault);

	// data writing functions
	BOOL WriteBool(CString strName, BOOL bValue);
//	BOOL WriteDateTime(CString strName, COleDateTime dtValue);
	BOOL WriteString(CString strName, CString strValue);
	BOOL WriteFloat(CString strName, double fValue);
	BOOL WriteInt(CString strName, int nValue);
	BOOL WriteColor(CString strName, COLORREF rgbValue);
//	BOOL WriteFont(CString strName, CFont* pFont);
//	BOOL WritePoint(CString strName, CPoint* pPoint);
//	BOOL WriteSize(CString strName, CSize* pSize);
//	BOOL WriteRect(CString strName, CRect* pRect);
	BOOL WriteDword(CString strName, DWORD dwValue);

};// end of CReg class definition


#endif
