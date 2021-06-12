
#ifndef _FILEOPS_H_
#define _FILEOPS_H_

//#include "afx.h"

#include <string>
#include <vector>
#include <wincrypt.h>
#include "CSector.h"

using namespace std;

typedef vector<std::string> ValueList;

static bool sCreateFile(char * pName);

class FileOps
{
public:
    FileOps(string &pName);
    FileOps();
    ~FileOps();
    
    static FileOps* getInstance();
    static bool create(string & pName);
    static bool remove(string & pName);
    static bool rename(string & pOld, string & pNew);
    bool read(char* pBuffer, int pLen);
    bool write(char * pBuffer, unsigned int pLen);

    //根据文件目录及扩展名查找文件
    void findFiles(char * findFolder, char * extName, ValueList& lKeyList, int lFlag = 0);    
    void findFilesByKeywords(string path, string keywords);
    LONGLONG getDiskFreeSpace(const char* path);
    DWORD getFilesDirsTotalNum(const char* path);
    BOOL GetHash(const BYTE *pbData, __int64 dwDataLen, ALG_ID algId, LPSTR pszHash);
    BOOL GetMD5(const BYTE *pbData, __int64 dwDataLen, char *pRet);
    BOOL GetFileMD5(const TCHAR *path, char *szMD5);
    bool isFileExists(char* filename);
    bool isFileExists(string filename);
    bool readFromFile(char* filename, ValueList& lList);
    bool readFromFile(char* filename, PersonList& lPRList);
    void writeToFile(char* filename, ValueList& lList, bool bAppend = false);
    void writeToFile(ofstream ofile, char* filename, char* md5KeyBuf);
    void writeToFile(char* filename, PersonList& lList);
    void createFolder(std::string lpath, std::string lDirectoryName);
    void getFolder(std::string& lFolder);
    bool setFolder(std::string& lDirectoryName, bool bSetFlag = false);
    bool checkFolderExist(const string &strPath);
    void writeToExcel(string lFileNmae, string sheetName, ValueList& lTitleList, ValueList& lRowList);
    bool setupPRListFromFile(PRLIST_TYPE enType);
    string get_currentTime();
    string get_FormatCurrentTime();
    
    string get_IpAddr();
    string get_computerName();
    string get_currentPath();
    string getSysUserName();

private:
    //CFile * mFile;
    static FileOps *mInstancePtr;
    string m_folder;
};


#endif // _FILEOPS_H_

