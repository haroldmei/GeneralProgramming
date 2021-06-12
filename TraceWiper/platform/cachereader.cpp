//#include "stdafx.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS    // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>        // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>            // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "cachereader.h"
#ifdef TIXML_USE_STL    
#include <iostream>    
#include <sstream>    
using namespace std;
#else
#include <stdio.h>
#endif
#include "atlconv.h"
HANDLE OpenCacheFile(TCHAR* szCacheFilePath)
{
    //open the file in shared mode so IE can use it at the same time
    return CreateFile(szCacheFilePath,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);;
}
WORD GetCacheFolderNum(HANDLE hFile)
{
    WORD wFolders=0;
    //move the file pointer to the start of the folders area
    if (SetFilePointer(hFile,CACHE_FOLDERNUM,NULL,FILE_BEGIN) == CACHE_FOLDERNUM)
    {
        DWORD dwRead=0;
        //read the number of folders
        ReadFile(hFile,&wFolders,sizeof(WORD),&dwRead,NULL);
    }
    //return folder count
    return wFolders;
}
void GetCacheFolders(HANDLE hFile, WORD wFolders,LPCACHEFOLDERS& pFolders)
{
    //move file pointer to start of folder names
    if (SetFilePointer(hFile,CACHE_FOLDERNAMES,NULL,FILE_BEGIN) == CACHE_FOLDERNAMES)
    {
        DWORD dwRead=0;
        //allocate memory for the folders
        pFolders = (CacheFolders*)CoTaskMemAlloc(sizeof(CacheFolders));
        pFolders->folders = (CacheFolder*)CoTaskMemAlloc((sizeof(CacheFolder)+(sizeof(char)*9)+sizeof(DWORD))*wFolders);
        //store folder count
        pFolders->nNumOfFolders = wFolders;
        //loop through folders
        for (;wFolders > 0; wFolders--)
        {
            //get pointer to next folder structure
            CacheFolder *lpFolder = (&pFolders->folders[pFolders->nNumOfFolders-wFolders]);
            //read folder name
            ReadFile(hFile,&lpFolder->szFolderName,CACHEFOLDER_NAMESIZE,&dwRead,NULL);
            //append '\0' to make it a string
            lpFolder->szFolderName[8] = '\0';
            //return folder dword - unknown what this is for
            ReadFile(hFile,&lpFolder->dwData,sizeof(DWORD),&dwRead,NULL);
        }
    }
}
CString GetFolderName(LPCACHEFOLDERS pFolders, WORD wFolderNum)
{
    //takes a folder structure, and returns the folder name from within the structure.
    CString str;
    CacheFolder lpFolder = (pFolders->folders[wFolderNum-1]);
    str = lpFolder.szFolderName;
    return str;
}
DWORD GetFirstCacheEntry(HANDLE hFile,DWORD* lpdwOffset)
{
    //this function simply sets the file pointer to the correct place for a call to ReadCache*Entry
    //and returns the cache entry type
    DWORD dwType=0;
    //set file pointer to first cache entry
    if (SetFilePointer(hFile,CACHE_FOLDERNUM,NULL,FILE_BEGIN) == CACHE_FOLDERNUM)
    {
        DWORD dwRead=0;
        while (dwType != HASH_ID)//we do not want a hash entry, so keep looking until something else is found
        {
            ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);
        }
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of DataBlock
        if ((*lpdwOffset = SetFilePointer(hFile,(dwType*BLOCK_SIZE)-(sizeof(DWORD)*2),NULL,FILE_CURRENT)) > 0)//SetPointer to end of HASH_ID
        {
            ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read first type after the end of the HASH_ID
        }
        else
            *lpdwOffset = 0;//Invalid data
    }
    return dwType;
}
DWORD GetNextCacheEntry(HANDLE hFile,DWORD* lpdwOffset)//Takes a current offset, returns new offset
{
    //this function simply sets the file pointer to the correct place for a call to ReadCache*Entry
    //and returns the cache entry type
    DWORD dwType=0;
    //set file pointer to next data block
    if (SetFilePointer(hFile,*lpdwOffset,NULL,FILE_BEGIN) == *lpdwOffset)
    {
        DWORD dwRead=0;
        //read data type
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of ID
        //loop until we found something we recognized
        while ((dwType != URL_ID) && (dwType != LEAK_ID) && (dwType != REDR_ID) && (dwType != HASH_ID))
        {
            //Bad Block so move to next block and try to read its ID
            *lpdwOffset = SetFilePointer(hFile,BLOCK_SIZE-(sizeof(DWORD)),NULL,FILE_CURRENT);
            if (*lpdwOffset >= GetFileSize(hFile,&dwRead))
            {
                //pointer at end of file so return type 0
                dwType = 0;
                break;
            }
            if (ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL) != TRUE)//Read size of ID
            {
                //unable to read type so return 0
                dwType = 0;
                break;
            }
        }
        if ((*lpdwOffset = SetFilePointer(hFile,-(sizeof(DWORD)),NULL,FILE_CURRENT)) > 0)//SetPointer to begining of recognized block
        {
        }
        else
            *lpdwOffset = 0;//Invalid data
    }//tell debugger where we are looking
    TRACE("Offset=%d\n",*lpdwOffset);
    //return type found
    return dwType;
}
void ReadCacheEntry(HANDLE hFile, DWORD* lpdwOffset, LPURLENTRY& lpData)
{
    //reads a url entry
    //make sure file pointer is at correct place
    if (SetFilePointer(hFile,*lpdwOffset,NULL,FILE_BEGIN) == *lpdwOffset)
    {
        DWORD dwRead=0;
        DWORD dwType=0;
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of ID
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of DataBlock
        if ((*lpdwOffset = SetFilePointer(hFile,-(sizeof(DWORD)*2),NULL,FILE_CURRENT)) > 0)//SetPointer to beggining of structure
        {
            //allocate memory for url structure
            lpData = (UrlEntry*)CoTaskMemAlloc(dwType*BLOCK_SIZE);
            //read the entire structure at once
            ReadFile(hFile,lpData,(dwType*BLOCK_SIZE),&dwRead,NULL);//Read actual datablock
            //change offset to end of structure
            *lpdwOffset += (dwType*BLOCK_SIZE);
        }
        else
            *lpdwOffset = 0;//Invalid data
    }
}
void ReadCacheLeakEntry(HANDLE hFile, DWORD* lpdwOffset, LPLEAKENTRY& lpData)
{
    //the leak entry is the same as a url entry, so just recast it
    ReadCacheEntry(hFile,lpdwOffset,(LPURLENTRY&)lpData);
}
void ReadCacheRedrEntry(HANDLE hFile, DWORD* lpdwOffset, LPREDRENTRY& lpData)
{//these redr's are by far the most common entry in the cache that I have found
    //make sure file pointer is at correct place
    if (SetFilePointer(hFile,*lpdwOffset,NULL,FILE_BEGIN) == *lpdwOffset)
    {
        DWORD dwRead=0;
        DWORD dwType=0;
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of ID
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of DataBlock
        if ((*lpdwOffset = SetFilePointer(hFile,-(sizeof(DWORD)*2),NULL,FILE_CURRENT)) > 0)//SetPointer to beggining of structure
        {
            //allocate memory for structure
            lpData = (RedrEntry*)CoTaskMemAlloc(dwType*BLOCK_SIZE);
            ReadFile(hFile,lpData,(dwType*BLOCK_SIZE),&dwRead,NULL);//Read actual datablock
            //set file pointer to end of block
            *lpdwOffset += (dwType*BLOCK_SIZE);
        }
        else
            *lpdwOffset = 0;//Invalid data
    }
}
void ReadCacheHashEntry(HANDLE hFile, DWORD* lpdwOffset, LPHASHENTRY& lpData)
{
    //there are a lot of those, and no one seems to know what they actually contain
    //however if we want our url, leak,redr data we have to read them anyways and 
    //just discard the data

    //set file pointer to correct place
    if (SetFilePointer(hFile,*lpdwOffset,NULL,FILE_BEGIN) == *lpdwOffset)
    {
        DWORD dwRead=0;
        DWORD dwType=0;
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of ID
        ReadFile(hFile,&dwType,sizeof(DWORD),&dwRead,NULL);//Read size of DataBlock
        if ((*lpdwOffset = SetFilePointer(hFile,-(sizeof(DWORD)*2),NULL,FILE_CURRENT)) > 0)//SetPointer to end of HASH_ID
        {
            //allocate memory for structure
            lpData = (HashEntry*)CoTaskMemAlloc(dwType*BLOCK_SIZE);
            ReadFile(hFile,lpData,(dwType*BLOCK_SIZE),&dwRead,NULL);//Read actual datablock
            //set file pointer to end of block
            *lpdwOffset += (dwType*BLOCK_SIZE);
        }
        else
            *lpdwOffset = 0;//Invalid data
    }
}

HANDLE GetFirstInetCacheEntry(LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry, DWORD &dwEntrySize/*=4096*/)
{
    //uses winInet functions to enumerate the cache

    //handle to cache to be used in the GetNextInet* call...
    HANDLE hCacheDir = NULL;

again:
    //attempt to get the first entry
    if (!(hCacheDir = FindFirstUrlCacheEntry(NULL,lpCacheEntry,&dwEntrySize)))
    {
        switch(GetLastError())
        {
            //no more items so restore cursor and close file
        case ERROR_NO_MORE_ITEMS: 
            FindCloseUrlCache(hCacheDir);
            SetCursor(LoadCursor(NULL,IDC_ARROW));
            return NULL;
            break;
        case ERROR_INSUFFICIENT_BUFFER:
            //maybe the structure size was not big enough so delete it then reallocate it and try again
            delete[]lpCacheEntry;
            lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) 
                new char[dwEntrySize];
            lpCacheEntry->dwStructSize = dwEntrySize;
            goto again;
            break;
        default:
            //dont know what happened so close the file, and restore normal cursor
            FindCloseUrlCache(hCacheDir);
            SetCursor(LoadCursor(NULL,IDC_ARROW));
            return NULL;
        }
    }
    //return handle to cache
    return hCacheDir;
}
BOOL GetNextInetCacheEntry(HANDLE &hCacheDir, LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry, DWORD &dwEntrySize/*=4096*/)
{
    //this uses the handle returned by the GetFirstInet* call.
retry:
    //attempt to get next cache entry
    if (!FindNextUrlCacheEntry(hCacheDir,lpCacheEntry, &dwEntrySize))
    {
        switch(GetLastError())
        {
            //nothing to read so close file
        case ERROR_NO_MORE_ITEMS: 
            FindCloseUrlCache(hCacheDir);
            return TRUE;
            break;
            //buffer not correct so resize it and try again
        case ERROR_INSUFFICIENT_BUFFER:
            delete[]lpCacheEntry;
            lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFO) 
                new char[dwEntrySize];
            lpCacheEntry->dwStructSize = dwEntrySize;
            goto retry;
            break;
        default:
            //dont know what happened so close file
            FindCloseUrlCache(hCacheDir);
            return FALSE;
        }
    }
    //return success
    return TRUE;
}

void indexFileView(CString pFile)
{//Cache Path on my computer: 
//c:\documents and settings\Owner\Local Settings\Temporary Internet Files\Content.IE5\index.dat
    //set cursor to busy
//    HCURSOR hCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    CString str;
int num = 0;
    //get the path to the cache to open
//    m_path.GetWindowText(str);
    //open the cache specified
    HANDLE hFile = OpenCacheFile(pFile.GetBuffer(0));
//    str = _T("This is a custom View reading bytes from the DAT files.\r\n");
//    m_test.SetWindowText(str);
#if 0
    //get the number of cache folders
    WORD wNum = GetCacheFolderNum(hFile);
    //if the cache folders=0 then we did not read the file correctly so exit
    if (wNum == 0)
    {
        return;
    }
    CacheFolders* pFolders,*p;
    //get the list of cache folders
    GetCacheFolders(hFile,wNum,pFolders);
    //loop the list and write out the folder names
    for (int n = 1; n <= wNum; n++)
    {
        CacheFolder lpFolder = (pFolders->folders[n-1]);
        str.Format("Folder: %s\r\n",lpFolder.szFolderName);
        CString strText;
        try{
            m_test.GetWindowText(strText);
        }catch (char* str) {
        }
        strText += str;
        m_test.SetWindowText(strText);
    }
#endif
    //we do not delete the list here because we will reference these later
    DWORD dwOffset = 0;
    int nEntries = 50;//retrieve only first 50 entries because they will be very large text on the screen
    DWORD dwType = GetFirstCacheEntry(hFile,&dwOffset);//get first entry
    do
    {
        if ((dwOffset >= 0xB5C00))
            dwType = dwType;
        if (dwType == URL_ID)//if its a url do this
        {
            URLENTRY *url;
            ReadCacheEntry(hFile,&dwOffset,url);
//            CString str;
            if (url != NULL)
            {
                int n=0;
                #if 0
                //This changes the bytes so that we can see the file and http info as well.
                for (DWORD dw = sizeof(URLENTRY); dw < url->dwRecordSize*BLOCK_SIZE; dw++)
                {
                    if ((url->lpText[n] == 0x0) || (url->lpText[n] == 0xF0) || (url->lpText[n] == 0xAD) || (url->lpText[n] == 0x0B))  
                    {
                        url->lpText[n] = '\r';
                    }
                    n++;
                }
                str.Format("URL: %s\r\n\tRecord Size: 0x%X\r\n\tModified Date High: 0x%X\r\n\tModified Date Low: 0x%X\r\n\tAccess Date High: 0x%X\r\n\tAccess Date Low: 0x%X\r\n\tFileSize Low: 0x%X\r\n\tFileSize High: 0x%X\r\n\tCookie Offset: 0x%X\r\n\tFolder Number: 0x%X\r\n\tFolder Name: %s\r\n\tFilename Offset: 0x%X\r\n\tCache Entry Type: 0x%X\r\n\tHeader Size: 0x%X\r\n\tHitCount: 0x%X\r\n\tUse Count: 0x%X\r\n",
                    url->lpText,
                    url->dwRecordSize,
                    url->modifieddate.dwHighDateTime,url->modifieddate.dwLowDateTime,
                    url->accessdate.dwHighDateTime,url->accessdate.dwLowDateTime,
                    url->wFileSizeLow,
                    url->wFileSizeHigh,
                    url->dwCookieOffset,
                    url->uFolderNumber,
                    "folder",//GetFolderName(pFolders,url->uFolderNumber),
                    url->uFilenameOffset,
                    url->dwCacheEntryType,
                    url->dwHeaderSize,
                    url->wHitCount,
                    url->dwUseCount);
                #endif
                str.Format("URL: %s\r\n",
                #if 0
                "\tModified Date High: 0x%X\r\n\tModified Date Low: 0x%X\r\n\tAccess Date High: 0x%X\r\n\tAccess Date Low: 0x%X\r\n"
                #endif
                url->lpText
                #if 0
                url->modifieddate.dwHighDateTime,url->modifieddate.dwLowDateTime,
                url->accessdate.dwHighDateTime,url->accessdate.dwLowDateTime
                #endif
                );
//                cout<<str<<endl;
                int i = 0;
#if 0
                //filename start-flag 0x15 0x1e
                while(1)
                {
                    if((url->lpText[i] == 0x15) &&(url->lpText[i+1] == 0x1e))
                    {
                        break;
                    }
                    i++;
                }
#endif
//http header offset
               char * httpheader = (char*)url->lpText + url->httpheadersoffset - url->dwCookieOffset;
//file name in disk
                char * fn = (char*)url->lpText;
                i = 0;
                //title start-flag 0x10 0x1f
                while(1)
                {
                    if((fn[i] == 0x10)&&(fn[i+1] == 0x1f))
                    {
                        break;
                    }
                    i++;
                }
                extern void wtoc(CHAR* Dest, const WCHAR* Source);
                char ss[256] = {0};
                
                WCHAR * title = (WCHAR*)(fn+i+2);
                i = 0;
                //wchar to char
                while(title[i] != 0)
                {
                    ss[i] = (CHAR)title[i];
                    ++i;
                }
                
//                cout <<ss <<endl;
                num++;
//                cout<<"number : "<<num<<endl;
                
            }
//            CString strText;
//            m_test.GetWindowText(strText);
//            strText += str;
//            m_test.SetWindowText(strText);
            CoTaskMemFree(url);//free url info
        }
        else if (dwType == LEAK_ID)//if its a leak do this
        {
            LEAKENTRY *url;
            ReadCacheLeakEntry(hFile,&dwOffset,url);
            #if 0
//            CString str;
            if (url != NULL)
                str.Format("LEAK: %s\r\n\tRecord Size: 0x%X\r\n\tModified Date High: 0x%X\r\n\tModified Date Low: 0x%X\r\n\tAccess Date High: 0x%X\r\n\tAccess Date Low: 0x%X\r\n\tFileSize Low: 0x%X\r\n\tFileSize High: 0x%X\r\n\tCookie Offset: 0x%X\r\n\tFolder Number: 0x%X\r\n\tFilename Offset: 0x%X\r\n\tCache Entry Type: 0x%X\r\n\tHeader Size: 0x%X\r\n\tHitCount: 0x%X\r\n\tUse Count: 0x%X\r\n",url->lpText,
                url->dwRecordSize,
                url->modifieddate.dwHighDateTime,url->modifieddate.dwLowDateTime,
                url->accessdate.dwHighDateTime,url->accessdate.dwLowDateTime,
                url->wFileSizeLow,
                url->wFileSizeHigh,
                url->dwCookieOffset,
                url->uFolderNumber,
                url->uFilenameOffset,
                url->dwCacheEntryType,
                url->dwHeaderSize,
                url->wHitCount,
                url->dwUseCount);
            #endif
//            CString strText;
//            m_test.GetWindowText(strText);
//            strText += str;
//            m_test.SetWindowText(strText);
            CoTaskMemFree(url);//free leak info
        }
        else if (dwType == REDR_ID)//if its a redr do this
        {
            REDRENTRY *url;
            ReadCacheRedrEntry(hFile,&dwOffset,url);
//            CString str;
            if (url != NULL)
                str.Format("REDR: %s\r\n\tRecord Size: 0x%X\r\n",url->lpWebUrl,
                url->dwRecordSize);
//            CString strText;
//            m_test.GetWindowText(strText);
//            strText += str;
//            m_test.SetWindowText(strText);
            CoTaskMemFree(url);//free redr info
        }
        else if (dwType == HASH_ID)//if its a hash do this
        {
            HASHENTRY *url;
            ReadCacheHashEntry(hFile,&dwOffset,url);
            //we dont know what the hash stuff is so we just read it and do not display it.
            CoTaskMemFree(url);//free hash info
        }
        dwType = GetNextCacheEntry(hFile,&dwOffset);
    }
    //while (dwType != 0); //This would read the entire file, takes about 20-25 minutes for a 8.5MB File 
	while (/*(nEntries-- >= 0) && */(dwType != 0));//This is limited to 50 because otherwise the edit box and strings run out of memory
//    ::CoTaskMemFree(pFolders->folders);//free folders info
//    ::CoTaskMemFree((void*)pFolders);//free cache folders holder
    CloseHandle(hFile);//close cache file
//    SetCursor(hCur);//return cursor to normal
}
