
//#define _UNICODE
//#define UNICODE
#pragma   warning(   disable   :   4786   )  

//#include   <afx.h>
#include <assert.h>
#define ASSERT assert
//#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers
//#include "stdafx.h"
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000


//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
//#include <afxdtctl.h>        // MFC support for Internet Explorer 4 Common Controls



//#include <winnt.h>

//#include <windows.h>

#include "afx.h"
#include "CReg.h"

#include <iostream>
using   namespace   std;

#define CLASS_NAME_LENGTH 255

/*
#define REG_NONE 0
#define REG_SZ 1
#define REG_EXPAND_SZ 2
#define REG_BINARY 3
#define REG_DWORD 4
#define REG_DWORD_LITTLE_ENDIAN 4
#define REG_DWORD_BIG_ENDIAN 5
#define REG_LINK 6
#define REG_MULTI_SZ 7
#define REG_RESOURCE_LIST 8
#define REG_FULL_RESOURCE_DESCRIPTOR 9
#define REG_RESOURCE_REQUIREMENTS_LIST 10
*/
//#define REG_QWORD 11
//#define REG_QWORD_LITTLE_ENDIAN 11 

/* IMPORTANT NOTES ABOUT CREGISTRY:
    
    CReg never keeps a key open past the end of a function call.
    This is incase the application crashes before the next call to close
    the registry 
    
    INCLUDE FILES
    "winreg.h" and "afxdisp.h" must be included in "stdafx.h"

    KEY NAMES:
    Key names must not begin with a \ and only absolute strings are accepted
    
*/
CReg::CReg()
{
    m_hRootKey = HKEY_CURRENT_USER;
    m_bLazyWrite = TRUE;
    m_nLastError = ERROR_SUCCESS;
}

CReg::~CReg()
{
    ClearKey();
}


BOOL CReg::ClearKey()
{
    /* Call CloseKey to write the current key to the registry and close the 
    key. An application should not keep keys open any longer than necessary. 
    Calling CloseKey when there is no current key has no effect.*/

    m_strCurrentPath.Empty();
    m_hRootKey = HKEY_CURRENT_USER;
    m_bLazyWrite = TRUE;
    return TRUE;
}


BOOL CReg::SetRootKey(string &pKey)
{
    size_t pos_of_first_slash = 0;
    if(pos_of_first_slash = pKey.find_first_of('\\', 0))
    {
        if(0 == pKey.compare(0, pos_of_first_slash, "HKEY_LOCAL_MACHINE", 18))
        {
            pKey.erase(0, 19);
            return SetRootKey(HKEY_LOCAL_MACHINE);
        }
        else if(0 == pKey.compare(0, pos_of_first_slash, "HKEY_CLASSES_ROOT", 17))
        {
            pKey.erase(0, 18);
            return SetRootKey(HKEY_CLASSES_ROOT);
        }
        else if(0 == pKey.compare(0, pos_of_first_slash, "HKEY_CURRENT_USER", 17))
        {
            pKey.erase(0, 18);
            return SetRootKey(HKEY_CURRENT_USER);
        }
        else if(0 == pKey.compare(0, pos_of_first_slash, "HKEY_USERS", 10))
        {
            pKey.erase(0, 11);
            return SetRootKey(HKEY_USERS);
        }
        else
        {
            cout<<"registry key invalid"<<endl;
            return FALSE;
        }
    }
    else
    {
        cout<<"registry key invalid"<<endl;
        return FALSE;
    }
}

BOOL CReg::SetRootKey(HKEY hRootKey)
{
    // sets the root key
    // make sure to set it to a valid key
    if (hRootKey != HKEY_CLASSES_ROOT &&
            hRootKey != HKEY_CURRENT_USER &&
            hRootKey != HKEY_LOCAL_MACHINE &&
            hRootKey != HKEY_USERS) return FALSE;

    m_hRootKey = hRootKey;
    return TRUE;
}


BOOL CReg::CreateKey(CString strKey)
{
    /* Use CreateKey to add a new key to the registry. 
        Key is the name of the key to create. Key must be 
        an absolute name. An absolute key 
        begins with a backslash (\) and is a subkey of 
        the root key. */

    ASSERT(strKey[0] != '\\');
    HKEY hKey;

    DWORD dwDisposition = 0;

    if (::RegCreateKeyEx(m_hRootKey, LPCTSTR(strKey), 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
            &dwDisposition)    != ERROR_SUCCESS) return FALSE;
    
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    m_strCurrentPath = strKey;
    return TRUE;
}
BOOL CReg::UnloadKey(CString strKey)
{
    /* Call DeleteKey to remove a specified key and its associated data, 
    if any, from the registry. Returns FALSE is there are subkeys
    Subkeys must be explicitly deleted by separate calls to DeleteKey.
    DeleteKey returns True if key deletion is successful. On error, 
    DeleteKey returns False. */
    
    // need to open the key first with RegOpenKeyEx

    ASSERT(strKey[0] != '\\');
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) return FALSE;

    if (!KeyExists(strKey)) return TRUE;
    if (::RegUnLoadKey(m_hRootKey, strKey) != ERROR_SUCCESS) return FALSE;
    ::RegCloseKey(hKey);
    
    return TRUE;
}


BOOL CReg::DeleteValue(CString strName)
{
    /* Call DeleteValue to remove a specific data value 
        associated with the current key. Name is string 
        containing the name of the value to delete. Keys can contain 
        multiple data values, and every value associated with a key 
        has a unique name. */

    ASSERT(m_strCurrentPath.GetLength() > 0);
    HKEY hKey;
    LONG lResult;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) return FALSE;

    lResult = ::RegDeleteValue(hKey, LPCTSTR(strName));
    ::RegCloseKey(hKey);

    if (lResult == ERROR_SUCCESS) return TRUE;
    return FALSE;
}


int CReg::GetDataSize(CString strValueName)
{
    /* Call GetDataSize to determine the size, in bytes, of 
    a data value associated with the current key. ValueName 
    is a string containing the name of the data value to query.
    On success, GetDataSize returns the size of the data value. 
    On failure, GetDataSize returns -1. */

    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);
    LONG lResult;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) return -1;

    DWORD dwSize = 1;
    lResult = ::RegQueryValueEx(hKey, LPCTSTR(strValueName),
        NULL, NULL, NULL, &dwSize);
    ::RegCloseKey(hKey);

    if (lResult != ERROR_SUCCESS) return -1;
    return (int)dwSize;
}

DWORD CReg::GetDataType(CString strValueName)
{
    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);

    m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_QUERY_VALUE, &hKey);

    if (m_nLastError != ERROR_SUCCESS) return 0;

    DWORD dwType = 1;
    m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strValueName),
        NULL, &dwType, NULL, NULL);
    ::RegCloseKey(hKey);        

    if (m_nLastError == ERROR_SUCCESS) return dwType;

    return 0;
}


int CReg::GetSubKeyCount()
{
    /* Call this function to determine the number of subkeys.
        the function returns -1 on error */
    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return -1;

    LONG lResult;
    DWORD dwSubKeyCount, dwValueCount, dwClassNameLength,
        dwMaxSubKeyName, dwMaxValueName, dwMaxValueLength;
    FILETIME ftLastWritten;

    _TCHAR szClassBuffer[CLASS_NAME_LENGTH];
        
    dwClassNameLength = CLASS_NAME_LENGTH;
    lResult = ::RegQueryInfoKey(hKey, szClassBuffer, &dwClassNameLength,
        NULL, &dwSubKeyCount, &dwMaxSubKeyName, NULL, &dwValueCount,
        &dwMaxValueName, &dwMaxValueLength, NULL, &ftLastWritten);
                
    ::RegCloseKey(hKey);
    if (lResult != ERROR_SUCCESS) return -1;

    return (int)dwSubKeyCount;
}


int CReg::GetValueCount()
{
    /* Call this function to determine the number of subkeys.
        the function returns -1 on error */
    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return -1;

    LONG lResult;
    DWORD dwSubKeyCount, dwValueCount, dwClassNameLength,
        dwMaxSubKeyName, dwMaxValueName, dwMaxValueLength;
    FILETIME ftLastWritten;

    _TCHAR szClassBuffer[CLASS_NAME_LENGTH];
        
    dwClassNameLength = CLASS_NAME_LENGTH;
    lResult = ::RegQueryInfoKey(hKey, szClassBuffer, &dwClassNameLength,
        NULL, &dwSubKeyCount, &dwMaxSubKeyName, NULL, &dwValueCount,
        &dwMaxValueName, &dwMaxValueLength, NULL, &ftLastWritten);
                
    ::RegCloseKey(hKey);
    if (lResult != ERROR_SUCCESS) return -1;

    return (int)dwValueCount;
}


BOOL CReg::KeyExists(CString strKey, HKEY hRootKey)
{
    /* Call KeyExists to determine if a key of a specified name exists.
         Key is the name of the key for which to search. */

    ASSERT(strKey[0] != '\\');
    HKEY hKey;

    if (hRootKey == NULL) hRootKey = m_hRootKey;
    
    LONG lResult = ::RegOpenKeyEx(hRootKey, LPCTSTR(strKey), 0,
        KEY_ALL_ACCESS, &hKey);
    ::RegCloseKey(hKey);
    if (lResult == ERROR_SUCCESS) return TRUE;
    return FALSE;
}

BOOL CReg::SetKey(CString strKey, BOOL bCanCreate)
{
    /* Call SetKey to make a specified key the current key. Key is the 
        name of the key to open. If Key is null, the CurrentKey property 
        is set to the key specified by the RootKey property.

        CanCreate specifies whether to create the specified key if it does 
        not exist. If CanCreate is True, the key is created if necessary.

        Key is opened or created with the security access value KEY_ALL_ACCESS. 
        OpenKey only creates non-volatile keys, A non-volatile key is stored in 
        the registry and is preserved when the system is restarted. 

        OpenKey returns True if the key is successfully opened or created */

    ASSERT(strKey[0] != '\\');
    HKEY hKey;


    // close the current key if it is open
    if (strKey.GetLength() == 0)
    {
        m_strCurrentPath.Empty();
        return TRUE;
    }

    DWORD dwDisposition;
    if (bCanCreate) // open the key with RegCreateKeyEx
    {
        if (::RegCreateKeyEx(m_hRootKey, LPCTSTR(strKey), 0, NULL, 
            REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,
                &dwDisposition) != ERROR_SUCCESS) return FALSE;
        m_strCurrentPath = strKey;
        if (!m_bLazyWrite) ::RegFlushKey(hKey);
        ::RegCloseKey(hKey);    
        return TRUE;
    }

    // otherwise, open the key without creating
    // open key requires no initial slash
    m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(strKey), 0,
        KEY_ENUMERATE_SUB_KEYS|KEY_QUERY_VALUE|KEY_READ /*KEY_ALL_ACCESS*/, &hKey);
    if (m_nLastError != ERROR_SUCCESS) return FALSE;
    m_strCurrentPath = strKey;
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return TRUE;
}


BOOL CReg::ValueExists(CString strName)
{
    /* Call ValueExists to determine if a particular key exists in 
        the registry. Calling Value Exists is especially useful before 
        calling other TRegistry methods that operate only on existing keys.

        Name is the name of the data value for which to check.
    ValueExists returns True if a match if found, False otherwise. */

    HKEY hKey;
    LONG lResult;
    ASSERT(m_strCurrentPath.GetLength() > 0);

    
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return FALSE;

    lResult = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        NULL, NULL, NULL);
    ::RegCloseKey(hKey);

    if (lResult == ERROR_SUCCESS) return TRUE;
    return FALSE;
}


void CReg::RenameValue(CString strOldName, CString strNewName)
{
    /* Call RenameValue to change the name of a data value associated 
        with the current key. OldName is a string containing the current 
        name of the data value. NewName is a string containing the replacement 
        name for the data value.
        
        If OldName is the name of an existing data value for the current key, 
        and NewName is not the name of an existing data value for the current 
        key, RenameValue changes the data value name as specified. Otherwise 
        the current name remains unchanged.
    */
    ASSERT(FALSE); // functionality not yet implemented
}



#if 0
COleDateTime CReg::ReadDateTime(CString strName, COleDateTime dtDefault)
{
    /* Call ReadDate to read a date value from a specified data value 
    associated with the current key. Name is the name of the data value to read.
    If successful, ReadDate returns a Delphi TDateTime value. The integral part 
    of a TDateTime value is the number of days that have passed since 12/30/1899. 
    The fractional part of a TDateTime value is the time of day.
    On error, an exception is raised, and the value returned by this function 
    should be discarded. */

    DWORD dwType = REG_BINARY;
    COleDateTime dt;
    DWORD dwSize = sizeof(dt);
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return dtDefault;
    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&dt, &dwSize) != ERROR_SUCCESS) dt = dtDefault;
    ::RegCloseKey(hKey);    
    return dt;
}
#endif
BOOL CReg::ReadDefaultValue(CString &lValue)
{
    DWORD lpType = REG_DWORD;
    PBYTE pbbinKeyData[4096];
    DWORD lDataLen = 4096;
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;

    if (::RegQueryValueEx(hKey, NULL, NULL,
        &lpType, (LPBYTE)pbbinKeyData, &lDataLen) != ERROR_SUCCESS)
    {
        ::RegCloseKey(hKey);    
        return FALSE;
    }
    if(lpType == REG_BINARY)
    {
        char lBuffer[32] = {0};
        for(DWORD i = 0; i < lDataLen; i++)
        {
            sprintf(lBuffer, "%x", pbbinKeyData[i]);
            CString lTmp(lBuffer);
            
            lValue = lValue+lTmp;
        }
    }
    else if((lpType == REG_SZ)
        ||(lpType == REG_EXPAND_SZ))
    {
        lValue = (char*)pbbinKeyData;
    }
    else if((lpType == REG_DWORD)
        ||(lpType == REG_DWORD_LITTLE_ENDIAN)
        ||(lpType == REG_DWORD_BIG_ENDIAN))
    {
        char lBuffer[32] = {0};
        sprintf(lBuffer, "%u", *(DWORD*)pbbinKeyData);
        
        lValue = (char*)lBuffer;
    }
    else if((lpType == REG_QWORD)
        ||(lpType == REG_QWORD_LITTLE_ENDIAN))
    {
        char lBuffer[32] = {0};
        sprintf(lBuffer, "%ul", *(long*)pbbinKeyData);
        
        lValue = (char*)lBuffer;
    }
    else
    {
        char lBuffer[32] = {0};
        std::string lStr;
        for(DWORD i = 0; i < lDataLen; i++)
        {
            sprintf(lBuffer, "%x", pbbinKeyData[i]);
            CString lTmp(lBuffer);
            
            lValue += lTmp;
        }
    }

    ::RegCloseKey(hKey);    
    return TRUE;
}



BOOL CReg::ReadKeyValueNames(ValueList &lValues)
{
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return false;


//    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
//    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  *achValue = NULL;  
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    if (cValues) 
    {
        LPBYTE   pbbinKeyData =   new   BYTE[cbMaxValueData+1];   
        achValue = new TCHAR[255];// [cchMaxValue+1];

        printf( "\nNumber of values: %d\n", cValues);


        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            DWORD lDataLen = cbMaxValueData+1;
            DWORD lNameLen = 255;//cchMaxValue+1;
            DWORD lpType = 0;
            achValue[0] = '\0'; 
            retCode = RegEnumValue(hKey, i, 
                achValue, 
                &lNameLen, 
                NULL, 
                &lpType,
                pbbinKeyData,
                &lDataLen);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
                _tprintf(TEXT("(%d) %s\n"), i+1, achValue); 
                if (0 == strcmp(achValue, "MRUList"))
                {
                    continue;
                }
                lValues.push_back(achValue);
            } 
        }

        delete pbbinKeyData;
        delete achValue;
    }
    else
    {
        return FALSE;
    }

    ::RegCloseKey(hKey);
    return true;
}

BOOL CReg::ReadKeyValues(ValueList &lValues)
{
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS) return false;


//    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
//    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  *achValue = NULL;  
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    if (cValues) 
    {
        LPBYTE   pbbinKeyData =   new   BYTE[cbMaxValueData+1];   
        achValue = new TCHAR [cchMaxValue+1];

        printf( "\nNumber of values: %d\n", cValues);


        for (i=0, retCode=ERROR_SUCCESS; i<cValues; i++) 
        { 
            DWORD lDataLen = cbMaxValueData+1;
            DWORD lNameLen = cchMaxValue+1;
            DWORD lpType = 0;
            achValue[0] = '\0'; 
            retCode = RegEnumValue(hKey, i, 
                achValue, 
                &lNameLen, 
                NULL, 
                &lpType,
                pbbinKeyData,
                &lDataLen);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
                _tprintf(TEXT("(%d) %s\n"), i+1, achValue); 
                if (0 == strcmp(achValue, "MRUList"))
                {
                    continue;
                }
                if(lpType == REG_BINARY)
                {
                    char lBuffer[32] = {0};
                    std::string lStr;
                    for(DWORD i = 0; i < lDataLen; i++)
                    {
                        sprintf(lBuffer, "%x", pbbinKeyData[i]);
                        std::string lTmp(lBuffer);
                        
                        lStr += lTmp;
                    }
                    lValues.push_back(lStr);
                }
                else if((lpType == REG_SZ)
                    ||(lpType == REG_EXPAND_SZ))
                {
                    std::string lStr;
                    lStr.assign ((char*)pbbinKeyData);
                    lValues.push_back(lStr);
                }
                else if((lpType == REG_DWORD)
                    ||(lpType == REG_DWORD_LITTLE_ENDIAN)
                    ||(lpType == REG_DWORD_BIG_ENDIAN))
                {
                    char lBuffer[32] = {0};
                    sprintf(lBuffer, "%u", *(DWORD*)pbbinKeyData);
                    
                    std::string lStr(lBuffer);
                    lValues.push_back(lStr);
                }
                else if((lpType == REG_QWORD)
                    ||(lpType == REG_QWORD_LITTLE_ENDIAN))
                {
                    char lBuffer[32] = {0};
                    sprintf(lBuffer, "%ul", *(long*)pbbinKeyData);
                    
                    std::string lStr(lBuffer);
                    lValues.push_back(lStr);
                }
                else if(lpType == REG_MULTI_SZ)
                {
                    char *lBuffer = (char*)pbbinKeyData;
                    
                    std::string lStr(lBuffer);
                    lValues.push_back(lStr);
                    
                    while(lStr.length())
                    {
                        lStr = lBuffer + 1 + lStr.length();
                        if(lStr.length())
                        {
                            lValues.push_back(lStr);
                        }
                    }
                }
                else
                {
                    char lBuffer[32] = {0};
                    std::string lStr;
                    for(DWORD i = 0; i < lDataLen; i++)
                    {
                        sprintf(lBuffer, "%x", pbbinKeyData[i]);
                        std::string lTmp(lBuffer);
                        
                        lStr += lTmp;
                    }
                    lValues.push_back(lStr);
                }
            } 
        }

        delete pbbinKeyData;
        delete achValue;
    }
    else
    {
        return FALSE;
    }

    ::RegCloseKey(hKey);
    return true;
}

BOOL CReg::ReadSubKeys(ValueList &lValues)
{
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

    HKEY hKey;
    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS)
        return false;


//    TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
//    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
 
    DWORD i, retCode; 
 
    TCHAR  *achValue = NULL;  
    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    if (cSubKeys) 
    {
        LPBYTE   pbbinKeyData =   new   BYTE[cbMaxValueData+1];   
        achValue = new TCHAR [cbMaxSubKey+1];

//        printf( "\nNumber of subkeys: %d\n", cSubKeys);


        for (i=0, retCode=ERROR_SUCCESS; i<cSubKeys; i++) 
        { 
            DWORD lDataLen = cbMaxValueData+1;
            DWORD lNameLen = cbMaxSubKey+1;
            DWORD lpType = 0;
            achValue[0] = '\0'; 
            retCode = RegEnumKeyEx(hKey, i, 
                achValue, 
                &lNameLen, 
                NULL, 
                NULL,
                NULL,
                NULL);
 
            if (retCode == ERROR_SUCCESS ) 
            { 
//                _tprintf(TEXT("(%d) %s\n"), i+1, achValue); 
                string lStr = (char*)achValue;

                lValues.push_back(lStr);
            } 
        }

        delete pbbinKeyData;
        delete achValue;
    }
    else
    {
        return FALSE;
    }

    ::RegCloseKey(hKey);
    return true;
}

double CReg::ReadFloat(CString strName, double fDefault)
{
    /* Call ReadFloat to read a float value from a specified 
        data value associated with the current key. Name is the name 
        of the data value to read.
        
        If successful, ReadFloat returns a double value. 
        On error, an exception is raised, and the value returned by 
        this function should be discarded. */

    DWORD dwType = REG_BINARY;
    double d;
    DWORD dwSize = sizeof(d);
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return fDefault;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&d, &dwSize) != ERROR_SUCCESS) d = fDefault;
    ::RegCloseKey(hKey);    
    return d;
}

CString CReg::ReadMutilString(CString strName, CString pSubStr, CString strDefault)
{
    DWORD dwType = REG_SZ;
    DWORD dwSize = 255;
    BOOL bSuccess = TRUE;
    _TCHAR sz[255];
    HKEY hKey;
    CString lRes;
    
                                 
    ASSERT(m_strCurrentPath.GetLength() > 0);

    // make sure it is the proper type
    dwType = GetDataType(strName);
    
    if (dwType != REG_MULTI_SZ)
    {
        return strDefault;
    }

    m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey);
    if (m_nLastError != ERROR_SUCCESS) return strDefault;

    m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)sz, &dwSize);
    if (m_nLastError != ERROR_SUCCESS) bSuccess = FALSE;
    ::RegCloseKey(hKey);    

    DWORD i = 0;
    char * subStr = sz;
    for(i = 0; i < dwSize; i++)
    {
        if(sz[i] != '\0')
        {
            subStr = sz + i;
            
            if(pSubStr.GetLength() == 0)
            {
                if(lRes.GetLength() != 0)
                {
                    lRes += "; "; //split multi-string by ;
                }
                lRes += subStr;
                
            }
            else if(0 == strncmp(subStr, pSubStr.GetBuffer(0), pSubStr.GetLength()))
            {
                return CString((LPCTSTR)subStr);
            }
            //goto next subStr
            i = i + strlen(subStr);
        }
        else
        {
            
        }
    }
    if(pSubStr.GetLength() == 0)
    {
        return lRes;
    }

    if (!bSuccess) return strDefault;

    return strDefault;
}

CString CReg::ReadString(CString strName, CString strDefault)
{
    DWORD dwType = REG_SZ;
    DWORD dwSize = 255;
    BOOL bSuccess = TRUE;
    _TCHAR sz[255]={0};
    HKEY hKey;
    
                                 
    ASSERT(m_strCurrentPath.GetLength() > 0);

    // make sure it is the proper type
    dwType = GetDataType(strName);
    
    if (dwType != REG_SZ && dwType != REG_EXPAND_SZ)
    {
        return strDefault;
    }

    m_nLastError = ::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey);
    if (m_nLastError != ERROR_SUCCESS) return strDefault;

    m_nLastError = ::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)sz, &dwSize);
    if (m_nLastError != ERROR_SUCCESS) bSuccess = FALSE;
    ::RegCloseKey(hKey);    
    if ((TRUE == bSuccess) && (0 == (strcmp(strName, "UninstallString"))))
    {
        //sprintf(sz, "C:\\test");
    }
    if (!bSuccess) return strDefault;
    return CString((LPCTSTR)sz);
}

DWORD CReg::ReadDword(CString strName, DWORD dwDefault)
{
    DWORD dwType = REG_DWORD;
    DWORD dw;
    DWORD dwSize = sizeof(dw);
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return dwDefault;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&dw, &dwSize) != ERROR_SUCCESS) dw = dwDefault;
    ::RegCloseKey(hKey);    
    return dw;
}



int CReg::ReadInt(CString strName, int nDefault)
{
    DWORD dwType = REG_BINARY;
    int n;
    DWORD dwSize = sizeof(n);
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return nDefault;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&n, &dwSize) != ERROR_SUCCESS) n = nDefault;
    ::RegCloseKey(hKey);    
    return n;
}

void CReg::ReadBinary(CString strName, unsigned char ** pValue, int &pLen)
{
    DWORD dwType = REG_BINARY;
    LPBYTE b;
    DWORD dwSize = sizeof(b);
    HKEY hKey;
    CString val ;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return ;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, NULL, &dwSize) == ERROR_SUCCESS)
    {
        b = new BYTE[dwSize];
        if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
            &dwType, b, &dwSize) == ERROR_SUCCESS)
        {
            *pValue = b;
            pLen = dwSize;
        }
    }
    ::RegCloseKey(hKey);    
    return ;
}
BOOL CReg::ReadBool(CString strName, BOOL bDefault)
{
    DWORD dwType = REG_BINARY;
    BOOL b;
    DWORD dwSize = sizeof(b);
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return bDefault;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&b, &dwSize) != ERROR_SUCCESS) b = bDefault;
    ::RegCloseKey(hKey);    
    return b;
}


COLORREF CReg::ReadColor(CString strName, COLORREF rgbDefault)
{
    DWORD dwType = REG_BINARY;
    COLORREF rgb;
    DWORD dwSize = sizeof(rgb);
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return rgbDefault;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&rgb, &dwSize) != ERROR_SUCCESS) rgb = rgbDefault;
    ::RegCloseKey(hKey);    
    return rgb;
}
#if 0
BOOL CReg::ReadFont(CString strName, CFont* pFont)
{
    DWORD dwType = REG_BINARY;
    DWORD dwSize = sizeof(LOGFONT);
    BOOL bSuccess = TRUE;
    HKEY hKey;
    LOGFONT lf;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)&lf, &dwSize) != ERROR_SUCCESS) bSuccess = FALSE;
    ::RegCloseKey(hKey);    
    if (bSuccess)
    {
        pFont->Detach();
        pFont->CreateFontIndirect(&lf);
    }
    return bSuccess;
}


BOOL CReg::ReadPoint(CString strName, CPoint* pPoint)
{
    DWORD dwType = REG_BINARY;
    DWORD dwSize = sizeof(CPoint);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)pPoint, &dwSize) != ERROR_SUCCESS) bSuccess = FALSE;
    ::RegCloseKey(hKey);    
    return bSuccess;
}

BOOL CReg::ReadSize(CString strName, CSize* pSize)
{
    DWORD dwType = REG_BINARY;
    DWORD dwSize = sizeof(CSize);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)pSize, &dwSize) != ERROR_SUCCESS) bSuccess = FALSE;
    ::RegCloseKey(hKey);    
    return bSuccess;
}

BOOL CReg::ReadRect(CString strName, CRect* pRect)
{
    DWORD dwType = REG_BINARY;
    DWORD dwSize = sizeof(CRect);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    ASSERT(m_strCurrentPath.GetLength() > 0);
    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;

    if (::RegQueryValueEx(hKey, LPCTSTR(strName), NULL,
        &dwType, (LPBYTE)pRect, &dwSize) != ERROR_SUCCESS) bSuccess = FALSE;
    ::RegCloseKey(hKey);    
    return bSuccess;
}

#endif


BOOL CReg::WriteBool(CString strName, BOOL bValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&bValue, sizeof(bValue))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}
#if 0
BOOL CReg::WriteDateTime(CString strName, COleDateTime dtValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&dtValue, sizeof(dtValue))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}
#endif

BOOL CReg::WriteString(CString strName, CString strValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;
    _TCHAR sz[255];
    
    if (strValue.GetLength() > 254) return FALSE;

#ifdef _UNICODE
    wstrcpy(sz, LPCTSTR(strValue));
#else
    strcpy(sz, LPCTSTR(strValue));
#endif

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
#ifdef _UNICODE
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_SZ, (LPBYTE)sz, wstrlen(sz) + 1)
         != ERROR_SUCCESS) bSuccess = FALSE;
#else
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_SZ, (LPBYTE)sz, strlen(sz) + 1)
         != ERROR_SUCCESS) bSuccess = FALSE;
#endif
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}


BOOL CReg::WriteFloat(CString strName, double fValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&fValue, sizeof(fValue))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}

BOOL CReg::WriteInt(CString strName, int nValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&nValue, sizeof(nValue))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}

BOOL CReg::WriteDword(CString strName, DWORD dwValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&dwValue, sizeof(dwValue))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}

BOOL CReg::WriteColor(CString strName, COLORREF rgbValue)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&rgbValue, sizeof(rgbValue))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}

#if 0
BOOL CReg::WriteFont(CString strName, CFont* pFont)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    LOGFONT lf;
    pFont->GetLogFont(&lf);

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)&lf, sizeof(lf))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}


BOOL CReg::WritePoint(CString strName, CPoint* pPoint)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)pPoint, sizeof(CPoint))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}


BOOL CReg::WriteSize(CString strName, CSize* pSize)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)pSize, sizeof(CSize))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}

BOOL CReg::WriteRect(CString strName, CRect* pRect)
{
    ASSERT(m_strCurrentPath.GetLength() > 0);
    BOOL bSuccess = TRUE;
    HKEY hKey;

    if (::RegOpenKeyEx(m_hRootKey, LPCTSTR(m_strCurrentPath), 0,
        KEY_WRITE, &hKey) != ERROR_SUCCESS) return FALSE;
    
    if (::RegSetValueEx(hKey, LPCTSTR(strName), 0,
        REG_BINARY, (LPBYTE)pRect, sizeof(CRect))
         != ERROR_SUCCESS) bSuccess = FALSE;
        
    if (!m_bLazyWrite) ::RegFlushKey(hKey);
    ::RegCloseKey(hKey);
    return bSuccess;
}

#endif
