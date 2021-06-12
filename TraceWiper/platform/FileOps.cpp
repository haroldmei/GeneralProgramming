#include   <stdlib.h> 
#include   <fstream> 
#include "direct.h"
#include "afx.h"
#include "LMCONS.H"
#include "ffsco.h"
#include "SysGlobal.h"
#include "RegisterOps.h"
#include "FileOps.h"
#include "CSpreadSheet.h"
#include <time.h>
#include <iosfwd>

FileOps* FileOps::mInstancePtr =  NULL;

FileOps::FileOps(string &pName)
{
#if 0
    mFile = new CFile(LPCTSTR(pName.c_str()), CFile::modeReadWrite);
#endif
}

FileOps::FileOps()
{
}


FileOps::~FileOps()
{
#if 0
    if(mFile)
    {
//        mFile->Close();
        delete mFile;
    }
#endif
}

FileOps* FileOps::getInstance()
{
    if(NULL == mInstancePtr)
    {
        mInstancePtr = new FileOps();
    }
    return mInstancePtr;

}


bool FileOps::create(string & pName)
{
    CFile lFile(pName.c_str(), CFile::modeCreate|CFile::modeReadWrite);
    return true;
}

bool FileOps::remove(string & pName)
{
    if (FileOps::getInstance()->isFileExists((char*)pName.c_str()))  
    {
        CFile::Remove(LPCTSTR(pName.c_str()));
    }
    return true;
}
bool FileOps::rename(string & pOld, string & pNew)
{
    CFile::Rename(LPCTSTR(pOld.c_str()), LPCTSTR(pNew.c_str()));
    return true;
}

bool FileOps::read(char * pBuffer, int pLen)
{
#if 0
    if(mFile->Read(pBuffer, pLen) > 0)
    {
        return true;
    }
#endif
    return false;
}

//append to end
bool FileOps::write(char * pBuffer, unsigned int pLen)
{
#if 0
    mFile->SeekToEnd();
    mFile->Write(pBuffer, pLen);
#endif
    return true;
}

void FileOps::findFiles(char * findFolder, char * extName, ValueList& lKeyList, int lFlag)
{
    helper_coffs::ffsco o;

    string path = findFolder;
    string fext = extName;

    o.findFiles(path,fext, lKeyList, lFlag);
}

void FileOps::findFilesByKeywords(string path, string keywords)
{
    helper_coffs::ffsco o;
    o.findFilesByKeywords(path,keywords);
}
//获取磁盘剩余空间大小(主要用于U盘，交叉检测使用)
LONGLONG FileOps::getDiskFreeSpace(const char* path)
{
    ULARGE_INTEGER TotalNumberofBytes;
    if (NULL == path)
    {
        return 0;
    }
    
    GetDiskFreeSpaceEx(path,NULL,&TotalNumberofBytes,NULL);
    return TotalNumberofBytes.QuadPart;
}

//获取磁盘内所有文件及文件夹个数(主要用于U盘，交叉检测使用)
DWORD FileOps::getFilesDirsTotalNum(const char* path)
{
    /*path 的格式诸如H:\\*/
    helper_coffs::ffsco o;
    string spath = path;
    string fext = "*.*";
    ValueList lKeyList;
    o.findFiles(spath,fext, lKeyList);
    return o.count();
}


//计算HASH值
BOOL FileOps::GetHash(const BYTE *pbData, __int64 dwDataLen, ALG_ID algId, LPSTR pszHash)
{
    HCRYPTPROV hProv;
    if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return FALSE;

    HCRYPTHASH hHash;
    if(!CryptCreateHash(hProv, algId, 0, 0, &hHash)) //Alg Id:CALG_MD5,CALG_SHA
    {
        CryptReleaseContext(hProv, 0);
        return FALSE;
    }

    unsigned __int64 dwDataToRead = dwDataLen;
    while(dwDataToRead)
    {
        DWORD dwReadLen = 4096000;
        if(dwReadLen > dwDataToRead) dwReadLen = (DWORD)dwDataToRead;
        if(!CryptHashData(hHash, pbData, dwReadLen, 0))
        {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return FALSE;
        }
        dwDataToRead -= dwReadLen;
        pbData += dwReadLen;
    }

    DWORD dwSize;
    DWORD dwLen = sizeof(DWORD);
    CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)(&dwSize), &dwLen, 0);

    BYTE* pHash = new BYTE[dwSize];
    dwLen = dwSize;
    CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwLen, 0);

    pszHash[0] = 0;
    char szTemp[3];
    for(DWORD i=0; i<dwLen; i++)
    {
        wsprintfA(szTemp, "%02X", pHash[i]);
        lstrcatA(pszHash, szTemp);
    }
    delete[] pHash;

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return TRUE;
}

BOOL FileOps::GetMD5(const BYTE *pbData, __int64 dwDataLen, char *pRet)
{
    if(pRet == NULL) return FALSE;
    char szMD5[60] = {0};
    if(!GetHash(pbData, dwDataLen, CALG_MD5, szMD5)) return FALSE;
    memcpy(pRet, szMD5, 60);
    return TRUE;
}

//计算文件MDKey值，主要用于交叉检测功能
BOOL FileOps::GetFileMD5(const TCHAR *path, char *szMD5)
{
    //创建文件句柄
    HANDLE hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile)
    {
        DWORD dwHiSize;
        DWORD dwLoSize = GetFileSize(hFile, &dwHiSize);
        unsigned __int64 dwFileSize = dwHiSize;
        dwFileSize <<= 32;
        dwFileSize += dwLoSize;
        //创建文件映射对象
        HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, dwHiSize, dwLoSize, NULL);
        if(hFileMapping)
        {
            //获取共享的内存地址
            BYTE *pByte = (BYTE*)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
            if(pByte)
            {
                GetMD5(pByte, dwFileSize, szMD5);
                UnmapViewOfFile(pByte);
            }

            CloseHandle(hFileMapping);
        }
        else
        {
            GetMD5((BYTE*)path, strlen(path), szMD5);
        }
        CloseHandle(hFile);
        return TRUE;
    }
    return FALSE;
}

bool FileOps::isFileExists(char* filename)
{
    ifstream   infile(filename);
    return infile?true:false;
}
bool FileOps::isFileExists(string filename)
{
    ifstream   infile(filename.c_str());
	return infile ? true : false;
}

bool FileOps::readFromFile(char* filename, ValueList& lList)
{
#if 0
    ifstream   infile(filename);
    std::string word;
    
    if(infile ==NULL) 
    { 
        return false; 
    }

    while(   infile  >>  word   )                     //read   word   from   infile 
    { 
            lList.push_back(word);                 //cout   word 
    }
#endif
    ifstream   ifile(filename);   
    string   line;   
    if   (!ifile.is_open())   
    {
        return   false;   
    }
    while   (!ifile.eof())     
    {   
      getline(ifile,   line);
      lList.push_back(line);
    }   
    ifile.close();


    return true;
}

void FileOps::writeToFile(char* filename, ValueList& lList, bool bAppend)
{
    /*to avoid user delete the folder when running it.*/
    if (m_folder.length()!= 0 && !checkFolderExist(m_folder))
    {
        createFolder("", "");
    }
    
    if (bAppend) /*append to the end of the file*/
    {
        if (!isFileExists(filename))
        {
            bAppend = !bAppend;
        }
        
        ofstream ofile(filename, ios::app);
        for(unsigned int idx = 0; idx < lList.size(); idx++)
        {
            
            ofile << lList[idx].c_str() << endl;
        }
    }

    if (!bAppend)
    {
        /*first delete it*/
        if (isFileExists(filename))
        {
            string lName = filename;
            remove(lName);
        }

        ofstream ofile(filename);
        for(unsigned int idx = 0; idx < lList.size(); idx++)
        {
            
            ofile << lList[idx].c_str() << endl;
        }
    }

}

void FileOps::writeToFile(char* filename, PersonList& lList)
{
    /*first delete it*/
    if (isFileExists(filename))
    {
        string lName = filename;
        remove(lName);
    }
    
    ofstream   ofile(filename);
    for(unsigned int idx = 0; idx < lList.size(); idx++)
    {
        
        ofile << lList[idx].pr_type<< endl;
        ofile << lList[idx].pr_name.c_str()<< endl;
        ofile << lList[idx].pr_depart.c_str()<< endl;
        ofile << lList[idx].pr_ipaddr.c_str()<< endl;
        ofile << lList[idx].pr_hostname.c_str()<< endl;
        ofile << lList[idx].pr_starttime.c_str() << endl;
        ofile << lList[idx].pr_endtime.c_str() << endl;
    }
}

/*load the local file infomation to the PersonList such as m_personList, m_suspectList, m_normalList, m_exceptionList*/
bool FileOps::setupPRListFromFile(PRLIST_TYPE enType)
{
    PRLISTDATA stInfo;
    ValueList lList;
    PersonList xList;
    string filename;
    string lFolder;

    if (CSysConfig::getInstance()->get_FolderChange())
    {
        CSysConfig::getInstance()->get_filePath(lFolder);
        filename = lFolder + "\\";
    }
    else
    {
        FileOps::getInstance()->getFolder(lFolder);
    }
    //filename = lFolder + "\\";

    
    
    switch(enType)
    {
        case PRLIST_DETECTED:
            filename += PRLIST_DETECT_NAME;
            break;
        case PRLIST_SUSPECTED:
            filename += PRLIST_SUSPECTED_NAME;
            break;
        case PRLIST_NORMAL:
            filename += PRLIST_NORMAL_NAME;
            break;
        case PRLIST_EXCEPTION:
            filename += PRLIST_EXCEPTION_NAME;
            break;
    }
    filename += FILE_POSTFIX_NAME;
    if (!isFileExists((char*)filename.c_str()))
    {
        return false;
    }
    
    ifstream   ifile(filename.c_str());   
    string   line;   
    if   (!ifile.is_open())   
    {
        return   false;   
    }
    
    while(!ifile.eof())     
    {   
        getline(ifile,   line);
        lList.push_back(line);  
    }
    ifile.close();

    for(DWORD i = 6; i < lList.size(); i = i+ 7)
    {
        stInfo.pr_type = atoi(lList[i - 6].c_str());
        stInfo.pr_name = lList[i - 5];
        stInfo.pr_depart = lList[i - 4];
        stInfo.pr_ipaddr = lList[i - 3];
        stInfo.pr_hostname = lList[i - 2];
        stInfo.pr_starttime = lList[i - 1];
        stInfo.pr_endtime = lList[i];
        xList.push_back(stInfo);
    }
    CPersonListConfig::getInstance()->set_configList(enType, xList);
    return true;
}

void FileOps::writeToFile(ofstream ofile, char* filename, char* md5KeyBuf)
{
        ofile << md5KeyBuf << endl;
}

string FileOps::get_currentTime()
{
    string outStr;
    char lTimeBuf[1024] = {0};
    time_t curtime=time(0); 
    tm tim =*localtime(&curtime); 
    int day,mon,year,hours, min, sec; 
    day=tim.tm_mday; 
    mon=tim.tm_mon + 1; 
    year=tim.tm_year; 
    hours = tim.tm_hour;
    min = tim.tm_min;
    sec = tim.tm_sec;
    sprintf(lTimeBuf, "%04d%02d%02d-%02d%02d%02d", year+1900, mon, day, hours, min, sec);
    outStr = lTimeBuf;
    return outStr;
}

string FileOps::get_FormatCurrentTime()
{
    string outStr;
    char lTimeBuf[1024] = {0};
    time_t curtime=time(0); 
    tm tim =*localtime(&curtime); 
    int day,mon,year,hours, min, sec; 
    day=tim.tm_mday; 
    mon=tim.tm_mon + 1; 
    year=tim.tm_year; 
    hours = tim.tm_hour;
    min = tim.tm_min;
    sec = tim.tm_sec;
    sprintf(lTimeBuf, "%04d-%02d-%02d %02d:%02d:%2d", year+1900, mon, day, hours, min, sec);
    outStr = lTimeBuf;
    return outStr;
}

string FileOps::get_IpAddr()
{
    return RegisterOps::getInstance()->getCheckingIpAddress();
}

string FileOps::get_computerName()
{
    return RegisterOps::getInstance()->getComputerName("");
}

void FileOps::createFolder(std::string lpath, std::string lDirectoryName)
{
    if (CSysConfig::getInstance()->get_FolderChange())
    {
        return;
    }
    
    if (lDirectoryName.length() == 0)
    {
        PersonList  lPersonList;
        CPersonListConfig::getInstance()->get_configList(PRLIST_DETECTED, lPersonList);
        
        lDirectoryName = RegisterOps::getInstance()->getCheckingIpAddress()
                        + "-" + RegisterOps::getInstance()->getComputerName("") + "-" + lPersonList[0].pr_name + "-" + /*get_currentTime()*/lPersonList[0].pr_starttime;
    }
    else
    {
        lDirectoryName = RegisterOps::getInstance()->getCheckingIpAddress()
                        + "-" + RegisterOps::getInstance()->getComputerName("") + "-" + lDirectoryName;
    }
    std::string lDir = lpath+lDirectoryName;
    
    if (!checkFolderExist(lDir))
    {
        m_folder = lDir;
        mkdir(lDir.c_str());
    }
    else
    {
        m_folder = lDir;
    }
}

void FileOps::getFolder(std::string& lFolder)
{
    lFolder = m_folder;
}

bool FileOps::setFolder(std::string& lDirectoryName, bool bSetFlag)
{
    string lFolder = RegisterOps::getInstance()->getCheckingIpAddress()
                                            + "-" + RegisterOps::getInstance()->getComputerName("") + "-" + lDirectoryName;
    if (!checkFolderExist(lFolder))
    {
        return false;
    }
    else
    {
        m_folder = lFolder;
        return true;
    }
}

bool FileOps::checkFolderExist(const string &strPath)
{
    WIN32_FIND_DATA  wfd;
    bool rValue = false;
    HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
    if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        rValue = true;   
    }
    FindClose(hFind);
    return rValue;
}

void FileOps::writeToExcel(string lFileName, string sheetName, ValueList& lTitleList, ValueList& lRowList)
{
    if (lRowList.size() == 0)
    {
        return;
    }

    /*列数*/
    int lColNum = lTitleList.size();
    int lRowNum = lRowList.size()/lColNum;
    string lName;
    
    // 新建Excel文件名及路径，TestSheet为内部表名
    //CSpreadSheet SS("c:\\Test.xls", "TestSheet");
    lName = m_folder + "\\" + lFileName + ".xls";
    CSpreadSheet SS(lName.c_str(), sheetName.c_str());
    CStringArray sampleArray;
    
    SS.BeginTransaction();
    
    // 加入标题
    sampleArray.RemoveAll();
    for (DWORD i = 0; i < lTitleList.size(); i++)
    {
        sampleArray.Add(lTitleList[i].c_str());
    }

    SS.AddHeaders(sampleArray);
#if 1
    // 加入数据
    for (int row = 0; row < lRowNum; row++)
    {
        for(int col = 0; col < lColNum; col++)
        {
            sampleArray.RemoveAll();
            sampleArray.Add(lRowList[row * lColNum + col].c_str());
        }
        SS.AddRow(sampleArray);
    }        
#endif
    SS.Commit();    
}

string FileOps::get_currentPath()
{
     char   path[MAX_PATH];   
     GetCurrentDirectory(MAX_PATH, path);
     return path;
}

string FileOps::getSysUserName()
{
       char szUser[UNLEN] = {0}; 
       DWORD cbUser = UNLEN; 
       if (GetUserName (szUser, &cbUser)) 
         return szUser; 
       else       
         return (""); 
}

bool sCresteFile(char * pName)
{
    CFile lFile(pName, CFile::modeCreate|CFile::modeReadWrite);
    return true;
//    lFile.Close();
}