#include <wininet.h>
#pragma comment(lib,"wininet.lib")
//This is the structure of the cache folder
//we add one space to the foldername for a '\0' at the end to make it a string
typedef struct CacheFolder
{
    TCHAR szFolderName[9];//folder name
    DWORD dwData;
}CACHEFOLDER, *LPCACHEFOLDER;
//this is a cache folders holder
typedef struct CacheFolders
{
    WORD nNumOfFolders;//total number of folders
    CacheFolder *folders;//Placeholder
}CACHEFOLDERS, *LPCACHEFOLDERS;
//here we have a cookie entry
typedef struct CookieEntry
{
    PTCHAR    pszName;
    PTCHAR    pszValue;
    PTCHAR    pszUrlPath;
    DWORD    dwFlags;//Security Flags
    DWORD    dwLowWordExpiration;//LowWord of FILETIME
    DWORD    dwHighWordExpiration;//HighWord of FILETIME
    DWORD    dwLowWordCreation;//LowWord of FILETIME
    DWORD    dwHighWordCreation;//HighWord of FILETIME
    CHAR    chEnd;//'*'
}COOKIEENTRY, *LPCOOKIEENTRY;
//a few defines for our use
#define CACHEFOLDER_OFFSET(numFolder,maxFolders) ((maxFolders-numFolder)*sizeof(CacheFolder))
#define HASH_ID    0x48534148L//"HASH" 
#define REDR_ID    0x52444552L//"REDR" 
#define URL_ID    0x204C5255L//"URL " 
#define LEAK_ID    0x4C45414bL//"LEAK"
#define BLOCK_SIZE 0x80
#define CACHEFOLDER_NAMESIZE    8
#define CACHE_FOLDERNUM            0x48
#define CACHE_FOLDERNAMES        0x50
//this is the basic structure for the url entries
typedef struct UrlEntry
{
    TCHAR szRecordType[4];//=URL_ID
    DWORD dwRecordSize;//="02 00 00 00" : ActualSize =  dwRecordSize* (128 Bytes or 0x80)
    FILETIME modifieddate;

    FILETIME accessdate;
    DWORD dwUnsure1;
    DWORD dwUnsure2;

    DWORD wFileSizeLow;
    DWORD wFileSizeHigh;//???
    BYTE uBlank[8];//expire time?
#ifdef __IE40__
    DWORD dwExtra;//Extra one here if its IE4
#endif
    DWORD uSame; //= "60 00 00 00"
    DWORD dwCookieOffset;//="68 00 00 00"
    BYTE uFolderNumber;//="FE" FE=No Folder
    BYTE unknown[3];//="00 10 10"
    DWORD uFilenameOffset;

    DWORD dwCacheEntryType; //= "01 00 10 00"
    DWORD httpheadersoffset;//="00 00 00 00"  httpheaders offset
    DWORD dwHeaderSize;// 00 00 00 00
    DWORD dwUnknown;//"00 00 00 00" 

    DWORD dwUnsure3;//???
    DWORD wHitCount;
    DWORD dwUseCount;//00 00 00 00
    DWORD dwData2;//??

    BYTE uMiscExtraData[8];
    BYTE lpText[1];//this will contain the url, filename, http response, and user with 0x00, 0xF0, 0xAD, and 0x0B as separating characters.

    //Format:
    //WebUrl
    //LocalFileName
    // The order of the following changes and is optional
    //HTTP 1.1 / OK
    //Pragma: no cache
    //Content Type
    //ETag
    //Content Length
    //~U : username
}URLENTRY, *LPURLENTRY;

//Leak entries use the same format as a UrlEntry, however the lpText only has the WebUrl without extra info stored in it
typedef UrlEntry LeakEntry;
typedef URLENTRY LEAKENTRY, *LPLEAKENTRY;
//a redr entry
typedef struct RedrEntry
{
    TCHAR szRecordType[4];//=REDR_ID
    DWORD dwRecordSize;//="01 00 00 00" : ActualSize =  dwRecordSize* (128 Bytes (0x80 Bytes))
    FILETIME dwNotSur;
    BYTE lpWebUrl[1];//Url till end
}REDRENTRY, *LPREDRENTRY;
//a hash entry
typedef struct HashEntry
{
    TCHAR szRecordType[4];//=HASH_ID
    DWORD dwRecordSize;//="20 00 00 00" : ActualSize =  dwRecordSize* (128 Bytes (0x80 Bytes))
    BYTE lpHashText[1];
}HASHENTRY,*LPHASHENTRY;
//this opens the cache file
HANDLE OpenCacheFile(TCHAR* szCacheFilePath);
//deal with the cache folders
WORD GetCacheFolderNum(HANDLE hFile);
void GetCacheFolders(HANDLE hFile, WORD wFolders,LPCACHEFOLDERS& pFolders);
CString GetFolderName(LPCACHEFOLDERS pFolders, WORD wFolderNum);
//these get the cache entires
DWORD GetFirstCacheEntry(HANDLE hFile,DWORD* lpdwOffset);
DWORD GetNextCacheEntry(HANDLE hFile,DWORD* lpdwOffset);//Takes a current offset, returns new offset
//these read the various types of entries
void ReadCacheEntry(HANDLE hFile, DWORD* lpdwOffset, LPURLENTRY& lpData);
void ReadCacheLeakEntry(HANDLE hFile, DWORD* lpdwOffset, LPLEAKENTRY& lpData);
void ReadCacheRedrEntry(HANDLE hFile, DWORD* lpdwOffset, LPREDRENTRY& lpData);
void ReadCacheHashEntry(HANDLE hFile, DWORD* lpdwOffset, LPHASHENTRY& lpData);

//these two functions are use the winInet functions for dealing with caches
//the LPINTERNET_CACHE_ENTRY_INFO must be allocated space before these functions are called.
//Ideally you can allocate only one structure, and then re-use it for each call...
HANDLE GetFirstInetCacheEntry(LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry, DWORD &dwEntrySize/*=4096*/);
//It is important to note that the dwEntrySize is modified from within the function calls to represent 
//the size of the data actually returned. Therefore if you are using one variable as the size, you need to 
//reset the variable to the actual allocated entry size before the next call to the function.
BOOL GetNextInetCacheEntry(HANDLE &hCacheDir, LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry, DWORD &dwEntrySize/*=4096*/);

void indexFileView(CString pFile);
