
#include "windows.h"
#include "direct.h"
#include "SysGlobal.h"
#include "CSector.h"
#include "FileOps.h"

using namespace std;

extern LPWSTR string2WCHAR(const char* srcStr);

CValueList::CValueList()
{
}

CValueList::~CValueList()
{
    m_vList.clear();
}

int CValueList::get_valueindex(string& lVal)
{
	ValueList::iterator iter = m_vList.begin();
	ValueList::iterator endIter = m_vList.end();
	int index = 0;

    if (m_vList.size() == 0)
    {
        return -1;
    }
    
    iter = m_vList.begin();
    while(iter != m_vList.end())
    {
        if ( 0 == strcmp((*iter).c_str(), lVal.c_str()))
        {
            //iter = m_vList.erase(iter);
            break;
        }
		index++;
        iter++;
    }
	
	return index;
}

void CValueList::remove_value(string& lVal)
{
    ValueList::iterator iter = m_vList.begin();
	ValueList::iterator endIter = m_vList.end();

    if (m_vList.size() == 0)
    {
        return;
    }
    
    iter = m_vList.begin();
    while(iter != m_vList.end())
    {
        if ( 0 == strcmp((*iter).c_str(), lVal.c_str()))
        {
            iter = m_vList.erase(iter);
            break;
        }
        iter++;
    }    
}


CSector::CSector()
{
    m_hSectorDev = NULL;
    memset(m_driveName, 0, MAX_SIZE);
}

CSector::~CSector()
{
}

char* CSector::get_driveName()
{
    return m_driveName;
}

void CSector::set_driveName(char* pName)
{
    if (strlen(pName) > MAX_SIZE)
    {
        return;
    }
    else
    {
        sprintf(m_driveName, pName);
    }
}

HANDLE CSector::get_sectorDev()
{
    return m_hSectorDev;
}

void CSector::set_sectorDev(HANDLE hDev)
{
    if (NULL != hDev)
    {    
        m_hSectorDev = hDev;
    }
}

void CSector::set_ThreadHandle(HANDLE lThread)
{
    m_Thread = lThread;
}

HANDLE CSector::get_ThreadHandle()
{
    return m_Thread;
}

void CSector::getSectorAddress(LPWSTR tranDevName, DWORD& lPartBegin, DWORD& lPartEnd)
{
    DWORD dwOutByte;            // DeviceIoControl 读取的字节数
       DWORD dwSectorByte;
       DISK_GEOMETRY diskGeometry;

       HANDLE hDev = CreateFile((LPCTSTR)tranDevName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
       if (hDev == INVALID_HANDLE_VALUE) 
       {
              ::MessageBox(NULL, "打开句柄失败", "错误", MB_OK);
           return;
       }
       // 读取每扇区字节数
       BOOL bDeviceRet = DeviceIoControl(hDev, IOCTL_DISK_GET_DRIVE_GEOMETRY,
           NULL, 0, &diskGeometry, sizeof(DISK_GEOMETRY), &dwOutByte, NULL);
   
       // 每扇区字节数
       dwSectorByte = diskGeometry.BytesPerSector;
   
       // 读取分区大小和偏移量
       PARTITION_INFORMATION partInfo;
       bDeviceRet = DeviceIoControl(hDev, IOCTL_DISK_GET_PARTITION_INFO,
           NULL, 0, &partInfo, sizeof(PARTITION_INFORMATION), &dwOutByte, NULL);
   
       DWORD lPartitionSize = (DWORD)partInfo.PartitionLength.QuadPart;        // 分区的大小
       DWORD lOffset = (DWORD)partInfo.StartingOffset.QuadPart;                // 偏移
   
       // StartingOffset 除以每扇区字节数就是开始扇区；
       // PartitionLength 除以每扇区字节数就是扇区总数。
       lPartBegin = lOffset / dwSectorByte;
       lPartEnd = lPartitionSize / dwSectorByte;
   
       // 开始扇区加上扇区总数再减去1就是结束扇区。
       lPartEnd += lOffset;
       lPartEnd -= 1;
       CloseHandle(hDev);

}

void CSector::getSectorBuffer(HANDLE hDev, DWORD lSectorNum, char* lpSectBuff, bool bcloseFlag)
{
#if 0
    char lErrorLogBuf[1024] = {0};
    DWORD dwCB = 0;

    HANDLE mutexSector;
    
    if (NULL == hDev) //若为空，表示扫描完成，重新打开句柄
    {
            LPWSTR tranDevName;
            tranDevName = string2WCHAR(gFullPreTransDev);
            hDev = CreateFile(tranDevName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
            if (hDev == INVALID_HANDLE_VALUE) 
            {
                sprintf(lErrorLogBuf,"%s, %s %d",  "磁盘句柄无效,错误位置:", __FILE__, __LINE__);
                ::MessageBox(NULL, string2WCHAR(lErrorLogBuf), _T("内部错误"), MB_OK);
                return;
            }
    }

    //访问扇区,进行互斥操作
    mutexSector = CreateMutex(NULL, FALSE, _T("sectorMutex"));

    WaitForSingleObject(mutexSector, INFINITE); //等待互斥量空置（没有线程拥有它）
    
    DWORD ret = SetFilePointer(hDev, 512 * lSectorNum, 0, FILE_BEGIN);
    if(ret == INVALID_SET_FILE_POINTER) 
    {
      sprintf(lErrorLogBuf,"%s, %s %d",  "访问扇区错误,错误位置:", __FILE__, __LINE__);
      ::MessageBox(NULL, string2WCHAR(lErrorLogBuf), _T("内部错误"), MB_OK);

      ReleaseMutex(mutexSector);  //释放信号量
      CloseHandle(hDev);
      return;
    }

    ret = ReadFile(hDev, lpSectBuff, 512 * 1, &dwCB, NULL);

    ReleaseMutex(mutexSector);  //释放信号量
    if (bcloseFlag && !g_bFullRun) //扫描完成后关闭句柄，显示单个扇区完成后关闭，再次显示时重新打开
    {
        CloseHandle(hDev);
    }
#endif
}


bool CSector::ReadSectorsXXX(LPCWSTR devName, char* pKeywords, DWORD lPartBegin, DWORD lPartEnd)
{
    bool bRet = false;
#if 0
    char firstKey;
    firstKey = pKeywords[0];
    LPBYTE lpSectBuff[512] = {0};
    char lTmp[512] = {0};
    int len = 0;
    DWORD dwCB = 0;

    DWORD percent=0;
    DWORD percent2=0;
    HANDLE mutexSector;
    
    HANDLE hDev = CreateFile((LPCTSTR)devName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (hDev == INVALID_HANDLE_VALUE) 
    {
        return false;
    }
    g_hFullSectorDev = hDev;
    
    for (int i = lPartBegin; i <= lPartEnd; i++)
    {
          Sleep(1);
          //mutexSector = CreateMutex(NULL, FALSE, _T("sectorMutex"));
          //WaitForSingleObject(mutexSector, INFINITE); //等待互斥量空置（没有线程拥有它）
          getSectorBuffer(hDev, i, (char*)lpSectBuff, false);
          #if 0
          DWORD ret = SetFilePointer(hDev, 512 * i, 0, FILE_BEGIN);
          if(ret == INVALID_SET_FILE_POINTER) 
          {
              ::MessageBox(NULL, _T("访问扇区错误"), _T("内部错误"), MB_OK);
              ReleaseMutex(mutexSector);  //释放对互斥量的占有
              return false;
          }
          
          if (!g_bFullRun) //线程结束
          {
              ReleaseMutex(mutexSector);  //释放对互斥量的占有
              break;
          }
          
          bRet = ReadFile(hDev, lpSectBuff, 512 * 1, &dwCB, NULL);
          ReleaseMutex(mutexSector);  //释放对互斥量的占有
          #endif
          memcpy(lTmp, lpSectBuff, 512);
          char* p = NULL;
          p = lTmp;
          for(int j = 0; j < 512; j++)
          {
            if (lTmp[j] == firstKey)
            {
                p = lTmp + j;
                len = strlen(pKeywords);
                
                if (len > 512 - j)
                {
                    break;
                }
                
                if (compareStrByMem(p, pKeywords,/*512-j*/len))
                {
                    DWORD dwStyle = (DWORD)GetWindowLongPtr(hFullScanList, GWL_STYLE);
                    SetWindowLongPtr(hFullScanList, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | LVS_REPORT);
                    ListView_SetExtendedListViewStyle(hFullScanList, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

                    //printf("find it\n");
                    //printInfo((unsigned char*)lTmp);
                    char lstrNum[128] = {0};
                    sprintf(lstrNum, "%d", i);
                    int item = AddItem(hFullScanList, string2WCHAR(lstrNum), 3); //添加行
                    for(int col = 1; col < 2; col++) //在行中设置列
                    {
                        
                        SetSubItem(hFullScanList, item, col, string2WCHAR(pKeywords));
                    }
                    break;
                }
                else
                {
                    continue;
                }
              }
            }
         
          percent2=i*100/lPartEnd;
          if(percent!=percent2)
          {
               HWND hwProgressBar = GetDlgItem(g_fullHwnd, IDC_FULLSCAN_PROGRESS1);
               SendMessage(hwProgressBar, PBM_SETPOS, (WPARAM)percent2, 0L);
               percent=percent2;
          }
      }          
      CloseHandle(hDev);
      g_hFullSectorDev = NULL;
#endif
      return bRet;
}


CSysConfig* CSysConfig::m_pSysConfigInstance =  NULL;
CPersonListConfig* CPersonListConfig::m_pPersonListInstance =  NULL;
CScoreConfig* CScoreConfig::m_pScoreConfigInstance = NULL;
CDiskScan* CDiskScan::m_pDiskScanInstance = NULL;
CBasicDiskScan* CBasicDiskScan::m_pBasicScanInstance = NULL;
CFullDiskScan* CFullDiskScan::m_pFullScanInstance = NULL;
CDeepDiskScan* CDeepDiskScan::m_pDeepScanInstance = NULL;
CEditor* CEditor::m_pEditorInstance = NULL;

/*系统配置类*/

CSysConfig::CSysConfig()
{
    /*the default directory is the system auto-generated*/
    string lFolder;
    FileOps::getInstance()->getFolder(lFolder);
    m_filePath = lFolder;
    m_bNetworkFlag = false; /*default is the non-network authorizing*/

    char szBuf[MAX_PATH] = {0};
    char lTmp[3] = {0};
    GetSystemDirectory(szBuf, sizeof(szBuf)/sizeof(char)); /*获取系统盘符*/
    lTmp[0] = szBuf[0];
    lTmp[1] = szBuf[1];
    m_detectedDrive = lTmp;

    DWORD dwSize = MAX_PATH;
    char pszName[MAX_PATH] = {0};
    if (GetUserName(pszName, &dwSize))
    {
        m_detectedUser = pszName;
    }

    m_bChange = false;
}

CSysConfig::~CSysConfig()
{
    m_filePath.empty();
    m_keywordsList.clear();
}

CSysConfig* CSysConfig::getInstance()
{
    if(NULL == m_pSysConfigInstance)
    {
        m_pSysConfigInstance = new CSysConfig();
    }
    return m_pSysConfigInstance;

}

void CSysConfig::set_filePath(std::string lpath, bool bChange)
{
    if (bChange)
    {
        m_filePath.clear();
        m_filePath = lpath;
    }
    m_bChange = bChange;
}

void CSysConfig::get_filePath(std::string& lpath)
{
    lpath = m_filePath;
}

void CSysConfig::set_subfilePath(std::string lsubpath)
{
    if (!FileOps::getInstance()->checkFolderExist(m_filePath))
    {
        return;
    }
    else
    {
        string lpath = m_filePath + "\\" + lsubpath;
        mkdir(lpath.c_str());
    }
    
    m_subfilePath.clear();
    m_subfilePath = lsubpath;
}

void CSysConfig::get_subfilePath(std::string& lpath)
{
    lpath = m_subfilePath;
}

void CSysConfig::set_keyWords(ValueList& lList)
{
    m_keywordsList.clear();
    m_keywordsList = lList;
}

void CSysConfig::get_keyWords(ValueList& lList)
{
    lList = m_keywordsList;
}

void CSysConfig::get_keyWords(std::string& lKeywords)
{
    for(DWORD i = 0; i < m_keywordsList.size(); i++)
    {
        lKeywords += m_keywordsList[i];
        lKeywords += ";";
    }
}

/*load the local file to display*/
void CSysConfig::loadFromFile()
{
    ValueList lList;
    if (FileOps::getInstance()->isFileExists(SYSCFG_SET_FILE))
    {
        FileOps::getInstance()->readFromFile(SYSCFG_SET_FILE, lList);
        m_filePath = lList[0];
        m_keywordsList.clear();
        for(DWORD i = 1; i < lList.size(); i++)
        {
            m_keywordsList.push_back(lList[i]);
        }
    }
}

void CSysConfig::saveToFile()
{
    ValueList lList;
    lList.push_back(m_filePath);
    for (DWORD i = 0; i < m_keywordsList.size(); i++)
    {
        lList.push_back(m_keywordsList[i]);
    }
    FileOps::getInstance()->writeToFile(SYSCFG_SET_FILE, lList);
}

void CSysConfig::splitStr(char* pSrc, char* pSeps, ValueList& lList)
{
    char   *token   =   NULL; 
    lList.empty();
    
    token   =   strtok(   pSrc,   pSeps   ); 
    while(   token   !=   NULL   ) 
    { 
        lList.push_back(token); 
        token   =   strtok(   NULL,   pSeps   ); 
    }
}


/*人员列表*/
CPersonListConfig::CPersonListConfig()
{
    m_titleList.push_back("姓名");
    m_titleList.push_back("部门");
    m_titleList.push_back("IP地址");
    m_titleList.push_back("主机名");
    m_titleList.push_back("开始检测时间");
    m_titleList.push_back("结束检测时间");
    
    m_personList.clear();
    m_suspectList.clear();
    m_normalList.clear();
    m_exceptionList.clear();
    
    m_selectedType = PRLIST_INVALID;
    m_currentAction = PRLIST_ACTION_INVALID;
    m_selectIndex = -1;
    m_personData.push_back("");
    m_personData.push_back("");
    m_personData.push_back(PRLIST_SUSPECTED_NAME);
    m_personData.push_back(PRLIST_NORMAL_NAME);
    m_personData.push_back(PRLIST_EXCEPTION_NAME);
}

CPersonListConfig::~CPersonListConfig()
{
}

CPersonListConfig* CPersonListConfig::getInstance()
{
    if(NULL == m_pPersonListInstance)
    {
        m_pPersonListInstance = new CPersonListConfig();
    }
    return m_pPersonListInstance;
}

void CPersonListConfig::add_configList(PRLIST_TYPE enType, PRLISTDATA& stInfo)
{
    if (isExists(enType, stInfo))
    {
        return;
    }
    
    stInfo.pr_type = (int)enType;
    
    PersonList xList;
    switch(enType)
    {
        case PRLIST_DETECTED:
            m_personList.push_back(stInfo);
            xList = m_personList;
            break;
        case PRLIST_SUSPECTED:
            m_suspectList.push_back(stInfo);
            xList = m_suspectList;
            break;
        case PRLIST_NORMAL:
            m_normalList.push_back(stInfo);
            xList = m_normalList;
            break;
        case PRLIST_EXCEPTION:
            m_exceptionList.push_back(stInfo);
            xList = m_exceptionList;
            break;
        default:
            return;
    }
    update_configList(enType, xList);
}

void CPersonListConfig::remove_configList(PRLIST_TYPE enType, PRLISTDATA& stInfo)
{
    //
    PersonList xList ;
    xList.empty();
    get_configList(enType, xList);

	PersonList::iterator iter = xList.begin();
	PersonList::iterator endIter = xList.end();
    iter = xList.begin();
    while(iter != xList.end())
    {
        if ( (0 == strcmp((*iter).pr_name.c_str(), stInfo.pr_name.c_str()))
            && (0 == strcmp((*iter).pr_depart.c_str(), stInfo.pr_depart.c_str()))
            && (0 == strcmp((*iter).pr_ipaddr.c_str(), stInfo.pr_ipaddr.c_str()))
            && (0 == strcmp((*iter).pr_hostname.c_str(), stInfo.pr_hostname.c_str()))
             && (0 == strcmp((*iter).pr_starttime.c_str(), stInfo.pr_starttime.c_str())))
        {
            iter = xList.erase(iter);
            break;
        }
        iter++;
    }

    update_configList(enType, xList);
    
}

void CPersonListConfig::remove_configList(PRLIST_TYPE enType, int index)
{
    PRLISTDATA stInfo;
    
    findByIndex(enType, index, stInfo);
    remove_configList(enType, stInfo);
}

void CPersonListConfig::get_personList(PRLIST_TYPE enType, PersonList& xList)
{
    switch(enType)
    {
        case PRLIST_DETECTED:
            xList = m_personList;
            break;
        case PRLIST_SUSPECTED:
            xList = m_suspectList;
            break;
        case PRLIST_NORMAL:
            xList = m_normalList;
            break;
        case PRLIST_EXCEPTION:
            xList = m_exceptionList;
            break;
        default:
            break;
    }
}

void CPersonListConfig::findByIndex(PRLIST_TYPE enType, DWORD index, PRLISTDATA& stInfo)
{
    int i = 0;

	PersonList xList;
    get_personList(enType, xList);

	PersonList::iterator iter = xList.begin();
    if (!xList.empty())
    {
        if (index > xList.size())
        {
            return;
        }
        
        iter = xList.begin();
        
        while(iter != xList.end())
        {
            if (i == index)
            {
                stInfo = *iter;
                break;
            }
            else
            {
                i++;
                iter++;
            }
        }
    }
}

bool CPersonListConfig::isExists(PRLIST_TYPE enType, PRLISTDATA& stInfo)
{
    PersonList xList;
    
	get_configList(enType, xList);
	PersonList::iterator iter = xList.begin();
	PersonList::iterator endIter = xList.end();

    if (xList.empty())
    {
        return false;
    }

    iter = xList.begin();
    while(iter != xList.end())
    {
        if ( (0 == strcmp((*iter).pr_name.c_str(), stInfo.pr_name.c_str()))
            && (0 == strcmp((*iter).pr_depart.c_str(), stInfo.pr_depart.c_str()))
            && (0 == strcmp((*iter).pr_ipaddr.c_str(), stInfo.pr_ipaddr.c_str()))
            && (0 == strcmp((*iter).pr_hostname.c_str(), stInfo.pr_hostname.c_str()))
            && (0 == strcmp((*iter).pr_starttime.c_str(), stInfo.pr_starttime.c_str()))) /*there just compare the starttime, no need for endtime*/
        {
            return true;
        }
        iter++;
    }

    return false;
}

void CPersonListConfig::get_configList(PRLIST_TYPE enType, PersonList& lPersonList)
{
    switch(enType)
    {
        case PRLIST_DETECTED:
            lPersonList = m_personList;
            break;
        case PRLIST_SUSPECTED:
            lPersonList = m_suspectList;
            break;
        case PRLIST_NORMAL:
            lPersonList = m_normalList;
            break;
        case PRLIST_EXCEPTION:
            lPersonList = m_exceptionList;
            break;            
    }
}

void CPersonListConfig::set_configList(PRLIST_TYPE enType, PersonList& xList)
{
    switch(enType)
    {
        case PRLIST_DETECTED:
            m_personList.clear();
            m_personList = xList;
            break;
        case PRLIST_SUSPECTED:
            m_suspectList.clear();
            m_suspectList = xList;
            break;
        case PRLIST_NORMAL:
            m_normalList.clear();
            m_normalList = xList;
            break;
        case PRLIST_EXCEPTION:
            m_exceptionList.clear();
            m_exceptionList = xList;
            break;            
    }
}

void CPersonListConfig::update_configList(PRLIST_TYPE enType, PersonList& xList)
{
    string lFolder;
    string saveName;

    if (CSysConfig::getInstance()->get_FolderChange())
    {
        CSysConfig::getInstance()->get_filePath(lFolder);
        saveName = lFolder + "\\";
    }
    else
    {
        FileOps::getInstance()->getFolder(lFolder);
    }
    
    switch(enType)
    {
        case PRLIST_DETECTED:
        {
            m_personList.clear();
            m_personList = xList;
            saveName += PRLIST_DETECT_NAME;
        }
        break;
        case PRLIST_SUSPECTED:
        {
            m_suspectList.clear();
            m_suspectList = xList;
            saveName += PRLIST_SUSPECTED_NAME;
        }
        break;
        case PRLIST_NORMAL:
        {
            m_normalList.clear();
            m_normalList = xList;
            saveName += PRLIST_NORMAL_NAME;
        }
        break;
        case PRLIST_EXCEPTION:
        {
            m_exceptionList.clear();
            m_exceptionList = xList;
            saveName += PRLIST_EXCEPTION_NAME;
        }
        break;
    }
    saveName += FILE_POSTFIX_NAME;

    FileOps::getInstance()->writeToFile((char*)saveName.c_str(), xList);
    
}

void CPersonListConfig::get_titleList(ValueList& ltitleList)
{
    ltitleList = m_titleList;
}

/*将人员列表信息转换为线性存储以便显示结果接口统一显示*/
void CPersonListConfig::convertListToLine(PRLIST_TYPE enType, ValueList& lLineResultList)
{
	PersonList::iterator iter = m_personList.begin();
	PersonList::iterator endIter = m_personList.end();
    
    m_lineResultList.clear();

    switch(enType)
    {
        case PRLIST_DETECTED:
        {
            iter = m_personList.begin();
            endIter = m_personList.end();
        }
        break;
        case PRLIST_SUSPECTED:
        {
            iter = m_suspectList.begin();
            endIter = m_suspectList.end();
        }
        break;
        case PRLIST_NORMAL:
        {
            iter = m_normalList.begin();
            endIter = m_normalList.end();
        }
        break;
        case PRLIST_EXCEPTION:
        {
            iter = m_exceptionList.begin();
            endIter = m_exceptionList.end();
        }
        break;            
    }
    
    while(iter != endIter)
    {
        m_lineResultList.push_back((*iter).pr_name);
        m_lineResultList.push_back((*iter).pr_depart);
        m_lineResultList.push_back((*iter).pr_ipaddr);
        m_lineResultList.push_back((*iter).pr_hostname);
        m_lineResultList.push_back((*iter).pr_starttime);
        m_lineResultList.push_back((*iter).pr_endtime);
        iter++;
    }

    lLineResultList = m_lineResultList;
}

void CPersonListConfig::set_selectedType(PRLIST_TYPE enType)
{
    m_selectedType = enType;
}

PRLIST_TYPE CPersonListConfig::get_selectedType()
{
    return m_selectedType;
}

void CPersonListConfig::set_actionType(PRLIST_ACTION enAction)
{
    m_currentAction = enAction;
}

PRLIST_ACTION CPersonListConfig::get_actionType()
{
    return m_currentAction;
}

void CPersonListConfig::set_selectIndex(int index)
{
    m_selectIndex = index;
}

int CPersonListConfig::get_selectIndex()
{
    return m_selectIndex;
}

CScoreConfig::CScoreConfig()
{
}

CScoreConfig::~CScoreConfig()
{
}

CScoreConfig* CScoreConfig::getInstance()
{
    if(NULL == m_pScoreConfigInstance)
    {
        m_pScoreConfigInstance = new CScoreConfig();
    }
    return m_pScoreConfigInstance;
}

void CScoreConfig::set_DisplayType(int type)
{
    m_displayType = type;
}

int CScoreConfig::get_DisplayType()
{
    return m_displayType;
}

CDiskScan::CDiskScan()
{
    
}

CDiskScan::~CDiskScan()
{
    
}

CDiskScan* CDiskScan::getInstance()
{
    if(NULL == m_pDiskScanInstance)
    {
        m_pDiskScanInstance = new CDiskScan();
    }
    return m_pDiskScanInstance;
}

void CDiskScan::getLogicalDrive(ValueList& lList, ValueList& lDTypeList)
{
    char szBuf[MAX_SIZE];
    char lBuffer[1024] = {0};
    int DType = 0;
    memset(szBuf,0,MAX_SIZE);

    DWORD len=GetLogicalDriveStringsA(sizeof(szBuf)/sizeof(char),szBuf);
    int i = 0;
    for (char * s= szBuf;*s;s+=strlen(s)+1)
    {
        DType = GetDriveTypeA(s+i);
        if (DRIVE_CDROM != DType)
        {
            char tmp[3] = {0};
            tmp[0] = s[0];
            tmp[1] = s[1];
            tmp[2] = '\0';
            string x(tmp);
            lList.push_back(x);
            
            if(DType == DRIVE_FIXED)
            {
               sprintf(lBuffer, "本地磁盘");
            }
            else if(DType == DRIVE_REMOVABLE)
            {
                sprintf(lBuffer, "可移动磁盘");
            }
            else if(DType == DRIVE_REMOTE)
            {
                sprintf(lBuffer, "网络磁盘");
            }
            else if(DType == DRIVE_RAMDISK)
            {
                sprintf(lBuffer, "虚拟RAM磁盘");
            }
            else if (DType == DRIVE_UNKNOWN)
            {
                sprintf(lBuffer, "未知设备");
            }
            else
            {
               sprintf(lBuffer, "错误信息");
            }
             lDTypeList.push_back(lBuffer);
        }
    }
}

void CDiskScan::set_Drive(string& lDrive)
{
    m_drive = lDrive;
}

/*CBasicDiskScan class*/
CBasicDiskScan::CBasicDiskScan()
{
    m_titleList.push_back("名称");
    m_titleList.push_back("文件夹");
    m_titleList.push_back("大小");
    m_titleList.push_back("创建时间");
    m_titleList.push_back("修改时间");
    m_titleList.push_back("访问时间");
    m_titleList.push_back("关键字");
}

CBasicDiskScan::~CBasicDiskScan()
{
    m_titleList.clear();
}

void CBasicDiskScan::get_titleList(ValueList& lList)
{
    lList = m_titleList;
}

CBasicDiskScan* CBasicDiskScan::getInstance()
{
    if(NULL == m_pBasicScanInstance)
    {
        m_pBasicScanInstance = new CBasicDiskScan();
    }
    return m_pBasicScanInstance;
}

void CBasicDiskScan::getLogicalDrive(ValueList& lList, ValueList& lDTypeList)
{
    char szBuf[MAX_SIZE];
    char lBuffer[1024] = {0};

    int DType = 0;
    memset(szBuf,0,MAX_SIZE);

    DWORD len=GetLogicalDriveStringsA(sizeof(szBuf)/sizeof(char),szBuf);
    int i = 0;
    for (char * s= szBuf;*s;s+=strlen(s)+1)
    {
        DType = GetDriveTypeA(s+i);
        //if (DRIVE_CDROM != DType)
        {
            char tmp[3] = {0};
            tmp[0] = s[0];
            tmp[1] = s[1];
            tmp[2] = '\0';
            string x(tmp);
            lList.push_back(x);
        }
        if(DType == DRIVE_FIXED)
         {
            sprintf(lBuffer, "本地磁盘");
         }
         else if(DType == DRIVE_CDROM)
         {
             sprintf(lBuffer, "光驱");
         }
         else if(DType == DRIVE_REMOVABLE)
         {
             sprintf(lBuffer, "可移动磁盘");
         }
         else if(DType == DRIVE_REMOTE)
         {
             sprintf(lBuffer, "网络磁盘");
         }
         else if(DType == DRIVE_RAMDISK)
         {
             sprintf(lBuffer, "虚拟RAM磁盘");
         }
         else if (DType == DRIVE_UNKNOWN)
         {
             sprintf(lBuffer, "未知设备");
         }
         else
         {
            sprintf(lBuffer, "错误信息");
         }
         lDTypeList.push_back(lBuffer);
    }
}


/*CFullDiskScan class*/
CFullDiskScan::CFullDiskScan()
{
    m_titleList.push_back("偏移地址");
    m_titleList.push_back("关键字");
}

CFullDiskScan::~CFullDiskScan()
{
    m_titleList.clear();
}

void CFullDiskScan::get_titleList(ValueList& lList)
{
    lList = m_titleList;
}

CFullDiskScan* CFullDiskScan::getInstance()
{
    if(NULL == m_pFullScanInstance)
    {
        m_pFullScanInstance = new CFullDiskScan();
    }
    return m_pFullScanInstance;
}


/*CDeepDiskScan class*/
CDeepDiskScan::CDeepDiskScan()
{
    m_titleList.push_back("URL地址");
    m_titleList.push_back("访问时间");
}

CDeepDiskScan::~CDeepDiskScan()
{
    m_titleList.clear();
}

void CDeepDiskScan::get_titleList(ValueList& lList)
{
    lList = m_titleList;
}

CDeepDiskScan* CDeepDiskScan::getInstance()
{
    if(NULL == m_pDeepScanInstance)
    {
        m_pDeepScanInstance = new CDeepDiskScan();
    }
    return m_pDeepScanInstance;
}

CEditor::CEditor()
{
    m_bPerview = false;
}

CEditor::~CEditor()
{
}

CEditor* CEditor::getInstance()
{
    if(NULL == m_pEditorInstance)
    {
        m_pEditorInstance = new CEditor();
    }
    return m_pEditorInstance;
}

void CEditor::set_previewFlag(bool bFlag)
{
    m_bPerview = bFlag;
}

bool CEditor::get_previewFlag()
{
    return m_bPerview;
}
