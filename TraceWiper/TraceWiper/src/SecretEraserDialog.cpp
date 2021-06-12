///////////////////////////////////////
// MyDialog.cpp
#include "stdafx.h"
#include <atlstr.h>
#include "resrc1.h"
#include "resource.h"
#include "cfgfile.h"
#include <io.h>
#include <fstream>
#include "SecretEraserDialog.h"
#include "../../platform/stringEx.h"
#include "../../platform/Ffsco.h"
#include "../../platform/FileOps.h"

extern HWND g_hWndFiles;
extern ERASER_METHOD   g_eEraseMethodFiles;
extern void LockDirOp();
extern void UnLockDirOp();



ERASER_HANDLE	g_ehContext;
DWORD WINAPI  DeleteDirectories_TASK(LPVOID lParam);
DWORD DeleteDirectories(LPVOID lParam);
ValueList g_list_DriveList;
bool g_bStopDelDir = false;
CString strDrive;
CString strDriveDir;


DWORD WINAPI FileSearchTask(LPVOID lParam)
{
	CSecretEraserDlg* pBasicDlg = (CSecretEraserDlg*)lParam;
	helper_coffs::ffsco lFindFileObj;

	LockDirOp();
	
	ValueList lTitleList;
	CBasicDiskScan::getInstance()->get_titleList(lTitleList);
	ValueList lDriveList = pBasicDlg->getDriveList().get_list();
	string lpath;
	for(unsigned int i = 0; i < lDriveList.size(); i++)
	{
		lpath = lDriveList[i] + "\\";
		pBasicDlg->findFilesByKeywords(lpath, pBasicDlg->get_keywordsFileDir());
	}
	UnLockDirOp();
	return 0;
}

DWORD WINAPI sectorScanTask(LPVOID lParam)
{
	LONGLONG lPartBegin =0;
	LONGLONG lPartEnd = 0;

	char symbol[] = "\\\\.\\";
	char devName[20] = {0};

	CSecretEraserDlg* pFullDlg = (CSecretEraserDlg*)lParam;

	ValueList lDriveList = pFullDlg->getDriveList().get_list();
	for(unsigned int i = 0; i < lDriveList.size(); i++)
	{
		memset(devName, 0, 20);
		sprintf(devName, "%s%s",symbol, lDriveList[i].c_str());    

		pFullDlg->getSectorAddress(devName, lPartBegin, lPartEnd);
		pFullDlg->ReadSectors(devName, lPartBegin, lPartEnd);
	}

	return 0;
}


DWORD WINAPI SectorURLScanTask(LPVOID  lParam)
{
	CSecretEraserDlg* pDeepDlg = (CSecretEraserDlg*)lParam;
	ValueList lList;
	char szBuf[0x10000]; 
	DWORD tick1,tick2;
	LARGE_INTEGER Distance;
	ULARGE_INTEGER TotalNumberofBytes;
	LONGLONG TotalCnt=0;
	DWORD n=0;//循环计数
	DWORD dwReadCnt=0;
	LONGLONG percent=0,percent2=0;

	string lpath;

	ValueList lDriveList = pDeepDlg->getDriveList().get_list();
	for(unsigned int i = 0; i < lDriveList.size(); i++)
	{
		lpath = lDriveList[i] + "\\";
		GetDiskFreeSpaceEx(/*TEXT("C:\\")*/lpath.c_str(),NULL,&TotalNumberofBytes,NULL);
		TotalCnt=TotalNumberofBytes.QuadPart>>16;

		tick1=GetTickCount();
		lpath.clear();
		lpath = "\\\\.\\" + lDriveList[i];
		HANDLE hFile=CreateFile(/*TEXT("\\\\.\\C:")*/lpath.c_str(),
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			0,OPEN_EXISTING,
			FILE_FLAG_NO_BUFFERING,//表示不缓存
			0);
		Distance.QuadPart=0;
		Distance.LowPart=SetFilePointer(hFile, Distance.LowPart, &Distance.HighPart, FILE_BEGIN);
		for ( ; n< TotalCnt; ++n )
		{
			memset(szBuf, 0, 0x10000);
			ReadFile(hFile, szBuf, 0x10000, &dwReadCnt, 0);
			if (!pDeepDlg->CheckBuffer(szBuf))//有上网关键字
			{
				memset(szBuf, 0, 0x10000);
				WriteFile(hFile, szBuf, 0x10000, &dwReadCnt, 0);
			}

			percent2=n*100/TotalCnt;
			if(percent!=percent2)
			{
				PostMessage(pDeepDlg->m_hProgress, PBM_SETPOS, (WPARAM)percent2, 0L);
				percent=percent2;
			}
			//Sleep(500);
		}
		CloseHandle(hFile);
		tick2=GetTickCount();
	}
	return 0;

	//printf("共查到上网记录%d条，用时%d分%d秒！\n",m_URLCount,minute,second);
}

CSecretEraserDlg::CSecretEraserDlg(UINT nResID, HWND hWndParent)
: CDialog(nResID, hWndParent)
{
	m_isRunning = false;
	m_SearchType = SRCH_DISK;
	m_SearchDiskType = SRCH_DISK_FULL;
	m_SearchDirNNameType = SRCH_DIRNFILE_FULL;
	m_SearchSectorType = SRCH_SECTOR_MINGAN;
	m_driveList.clear();
	
	memset(&m_hThreadId[0], 0, sizeof(HANDLE)*MAX_PROCESS_DRIVE);
	
	m_hInstRichEdit = ::LoadLibrary(_T("RICHED32.DLL"));
	if (!m_hInstRichEdit)
		::MessageBox(NULL, _T("CSecretEraserDlg::CRichView  Failed to load RICHED32.DLL"), _T(""), MB_ICONWARNING);
}

CSecretEraserDlg::~CSecretEraserDlg()
{
	::FreeLibrary(m_hInstRichEdit);
}


BOOL CSecretEraserDlg::CheckURL(char *szURL)
{
	szURL = szURL;
	return FALSE;
}

bool CSecretEraserDlg::CheckBuffer(char* pBuf)
{
	char myUrlBuf[256]={0};
	char szBuf[300];//代替CString
	FILETIME FileTime,LocalFileTime;
	SYSTEMTIME SystemTime;
	BOOL result;
	int i=0;
	int offset=0;
	do
	{
		//Sleep(1);
		if ( strncmp(pBuf+i, "LEAK", 4) && strncmp(pBuf+i, "URL ", 4) )
		{
			if ( !strncmp(pBuf+i, "REDR", 4))
			{
				if ( !strncmp(pBuf + 0x10 + i, "http://", 7))
				{
					if ( strlen(pBuf + 0x10 + i)<= 0xFF )                //这里是判断字符串的长度
					{
						strcpy(myUrlBuf, pBuf + 0x10 + i); //小于255就直接strcpy拷过来
					}
					else
					{
						memcpy(myUrlBuf, pBuf + 0x10 + i, 0xFC); // 大于的话就只拷前252个字符
					}
					result = CheckURL(myUrlBuf);            // 检查得到的URL串
					if ( !result )                          // 若符合要求，则记录！
					{
						//printf("%s\n",myUrlBuf);
						ValueList lList;

						m_urlList.push_back(myUrlBuf);

						FileTime.dwHighDateTime = *(DWORD *)(pBuf + 0x14 + i);
						FileTime.dwLowDateTime = *(DWORD *)(pBuf + 0x10 + i);
						FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
						FileTimeToSystemTime(&LocalFileTime, &SystemTime);
						sprintf(szBuf,"%d年%d月%d日 %d:%d:%d",
							SystemTime.wYear,
							SystemTime.wMonth,
							SystemTime.wDay,
							SystemTime.wHour,
							SystemTime.wMinute,
							SystemTime.wSecond);
						return true;
					}
				}
			}
		}
		else
		{
			if ( !strncmp(pBuf + 0x60 + i, "http://", 7) )
			{
				offset=0x60;

LABEL_29:	if ( strlen(pBuf + offset+ i)<= 0xFF )
				{
					strcpy(myUrlBuf, pBuf + offset + i);
				}
				else
				{
					memcpy(myUrlBuf, pBuf + offset + i, 0xFC);
				}
				result = CheckURL(myUrlBuf);
				if ( !result )
				{
					//printf("%s\n",myUrlBuf);
					FileTime.dwHighDateTime = *(DWORD *)(pBuf + 0x14 + i);
					FileTime.dwLowDateTime = *(DWORD *)(pBuf + 0x10 + i);
					FileTimeToLocalFileTime(&FileTime, &LocalFileTime);
					FileTimeToSystemTime(&LocalFileTime, &SystemTime);
					sprintf(szBuf,"%d年%d月%d日 %d:%d:%d",
						SystemTime.wYear,
						SystemTime.wMonth,
						SystemTime.wDay,
						SystemTime.wHour,
						SystemTime.wMinute,
						SystemTime.wSecond);

					return true;

				}
				goto LABEL_34;
			}
			else if( !strncmp(pBuf + 0x68 + i, "http://", 7) )
			{
				offset=0x68;
				goto LABEL_29;
			}
			else
			{
				if ( strncmp(pBuf + 0x60 + i, "Visited", 7) )
				{
					if ( !strncmp(pBuf + 0x68 + i, "Visited", 7))
					{
						if ( strstr(pBuf + 0x68 + i, "@http:")
							|| strstr(pBuf + 0x68 + i, "@ftp:")
							|| strstr(pBuf + 0x68 + i, "@java") )
						{
							offset = 0x71;
							goto LABEL_29;
						}
					}
				}
				else
				{
					if ( strstr(pBuf + 0x60 + i, "@http:")
						|| strstr(pBuf + 0x60 + i, "@ftp:")
						|| strstr(pBuf + 0x60 + i, "@java") )
					{
						offset = 0x69;
						goto LABEL_29;
					}
				}

			}
		}
LABEL_34:i += 0x80;
	}
	while ( i < 0xFF80u );

	return false;
}

void CSecretEraserDlg::getSectorAddress(char* devName, LONGLONG& lPartBegin, LONGLONG& lPartEnd)
{
	DWORD dwOutByte;            // DeviceIoControl 读取的字节数
	DWORD dwSectorByte;
	DISK_GEOMETRY diskGeometry;

	HANDLE hDev = CreateFile(devName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDev == INVALID_HANDLE_VALUE) 
	{
		CWnd::MessageBox(_T("打开句柄失败"), _T("错误"), MB_OK);
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

	LONGLONG lPartitionSize = partInfo.PartitionLength.QuadPart;        // 分区的大小
	LONGLONG lOffset = partInfo.StartingOffset.QuadPart;                // 偏移

	// StartingOffset 除以每扇区字节数就是开始扇区；
	// PartitionLength 除以每扇区字节数就是扇区总数。
	lPartBegin = lOffset / dwSectorByte;
	lPartEnd = lPartitionSize / dwSectorByte;

	// 开始扇区加上扇区总数再减去1就是结束扇区。
	lPartEnd += lOffset;
	lPartEnd -= 1;
	CloseHandle(hDev);

}

bool CSecretEraserDlg::compareStrByMem(void* s1, void* s2, int len)
{
	int totalLen = len;

	int i = 0;
	if (s1 == NULL || s2 == NULL)
	{
		return false;
	}

	while(len-- && *(char *)s1 == *(char *)s2)
	{
		s1 = (char *)s1 + 1;
		s2 = (char *)s2 + 1;
		i++;
	}

	if (i < totalLen)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int CSecretEraserDlg::getSelectedSectorNum()
{
	DWORD i = 0;
	DWORD n = 0;
	n   =   ListView_GetItemCount(m_hList); 
	for   (i   =   0;   i   <   n;   i++) 
	{
		if (ListView_GetItemState( m_hList, i,   LVIS_FOCUSED)   ==   LVIS_FOCUSED) 
		{

			LVHITTESTINFO lvi = {0};
			char buf[32] = {0};
			int sectorNum = 0;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			ListView_GetItemText(m_hList, lvi.iItem, /*g_lpInst->hti.iSubItem*/0, buf, sizeof buf);
			sectorNum = atoi(buf);
			return sectorNum;
		}
	}

	return -1;
}

void CSecretEraserDlg::getSectorBuffer(HANDLE hDev, DWORD lSectorNum, char* lpSectBuff, bool bcloseFlag)
{
	char lErrorLogBuf[1024] = {0};
	DWORD dwCB = 0;

	HANDLE mutexSector;

	//访问扇区,进行互斥操作
	mutexSector = CreateMutex(NULL, FALSE, _T("sectorMutex"));

	WaitForSingleObject(mutexSector, INFINITE); //等待互斥量空置（没有线程拥有它）

	DWORD ret = SetFilePointer(hDev, 512 * lSectorNum, 0, FILE_BEGIN);
	if(ret == INVALID_SET_FILE_POINTER) 
	{
		sprintf(lErrorLogBuf,"%s, %s %d",  "访问扇区错误,错误位置:", __FUNCTION__, __LINE__);
		//CWnd::MessageBox(lErrorLogBuf, _T("内部错误"), MB_OK);
		TRACE(lErrorLogBuf);
		ReleaseMutex(mutexSector);  //释放信号量
		CloseHandle(hDev);
		return;
	}

	ret = ReadFile(hDev, lpSectBuff, 512 * 1, &dwCB, NULL);

	ReleaseMutex(mutexSector);  //释放信号量
	if (bcloseFlag) //扫描完成后关闭句柄，显示单个扇区完成后关闭，再次显示时重新打开
	{
		CloseHandle(hDev);
	}
}
void CSecretEraserDlg::setSectorBuffer(HANDLE hDev, DWORD lSectorNum, char* lpSectBuff, bool bcloseFlag)
{
	char lErrorLogBuf[1024] = {0};
	DWORD dwCB = 0;

	HANDLE mutexSector;

	//访问扇区,进行互斥操作
	mutexSector = CreateMutex(NULL, FALSE, _T("sectorMutex"));

	WaitForSingleObject(mutexSector, INFINITE); //等待互斥量空置（没有线程拥有它）

	DWORD ret = SetFilePointer(hDev, 512 * lSectorNum, 0, FILE_BEGIN);
	if(ret == INVALID_SET_FILE_POINTER) 
	{
		sprintf(lErrorLogBuf,"%s, %s %d",  "访问扇区错误,错误位置:", __FUNCTION__, __LINE__);
		CWnd::MessageBox(lErrorLogBuf, _T("内部错误"), MB_OK);

		ReleaseMutex(mutexSector);  //释放信号量
		CloseHandle(hDev);
		return;
	}

	ret = WriteFile(hDev, lpSectBuff, 512 * 1, &dwCB, NULL);

	ReleaseMutex(mutexSector);  //释放信号量
	if (bcloseFlag) //扫描完成后关闭句柄，显示单个扇区完成后关闭，再次显示时重新打开
	{
		CloseHandle(hDev);
	}
}

bool CSecretEraserDlg::ReadSectors(char* devName, LONGLONG lPartBegin, LONGLONG lPartEnd)
{
	char firstKey;
	char* pKeywords = (char*)m_keywords.c_str();
	firstKey = pKeywords[0];
	LPBYTE lpSectBuff[512] = {0};
	char lTmp[512] = {0};
	int len = 0;
	bool bRet = false;
	LONGLONG percent=0;
	LONGLONG percent2=0;

	string strTmp;

	m_hdSector = CreateFile(devName, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (m_hdSector == INVALID_HANDLE_VALUE) 
	{
		CWnd::MessageBox(_T("打开句柄失败"), _T("错误"), MB_OK);
		return false;
	}

	for (LONGLONG i = lPartBegin; i <= lPartEnd; i++)
	{
		//Sleep(1);

		sprintf(lTmp, "%d", i);
		string x = lTmp;
		strTmp = "正在扫描第 " + x + " 个扇区...";              
		CWnd::SendMessage(m_hCurSectorText,WM_SETTEXT, 128, (LPARAM)strTmp.c_str());

		getSectorBuffer(m_hdSector, i, (char*)lpSectBuff, false);
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
					memset(lpSectBuff, 0, 512);
					setSectorBuffer(m_hdSector, i, (char*)lpSectBuff, false);
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
			CWnd::PostMessage(m_hProgress, PBM_SETPOS, (WPARAM)percent2, 0L);
			percent=percent2;
		}
	}          
	CloseHandle(m_hdSector);
	//g_hFullSectorDev = NULL;
	return bRet;
}

bool CSecretEraserDlg::hasKeywordsInString(string strSrc, ValueList keywordsList, string& keyword)
{
	for(unsigned int i = 0; i < keywordsList.size(); i++)
	{
		if (strstr(strSrc.c_str(),keywordsList[i].c_str()))
		{
			keyword = keywordsList[i];
			return true;
		}
	}
	return false;
}
void CSecretEraserDlg::findFilesByKeywords(string path, string keywords)
{
	int startErase = 0;
	char tmpBuf[1024] = {0};
	helper_coffs::ffsco ffo;
	string matchkeyword;        
	ValueList keywordsList;
	ffo.splitStr(keywords, ";", keywordsList);

	struct _finddata_t filefind;
	if(path[path.size()-1]=='\\')
	{
		path.resize(path.size()-1);
	}
	string curr=path+"\\*.*";

	m_resultList.clear(); /*clear the previous result when scan files each time.*/

	int done=0;
	int handle = 0;
	if((handle=_findfirst(curr.c_str(),&filefind))==-1)
	{
		return;
	}
	
	do{
		
		if (_A_SUBDIR==filefind.attrib)
		{
			if(!strcmp(filefind.name,"..") || !strcmp(filefind.name,"."))
			{
				continue;
			}
			
			curr = path + "\\" + filefind.name;
			strDriveDir = CString(curr.c_str());
			if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
			{
				DeleteDirectories(&strDriveDir);
			}
			else
			{
				findFilesByKeywords(curr,keywords);
			}
		}
		else
		{
			curr=path+"\\"+filefind.name;
			strDriveDir = CString(curr.c_str());
			if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
			{
				DeleteFile(strDriveDir);
			}
			else
			{
				ifstream   ifile(filefind.name);   
				string   line;   
				if(ifile.is_open()) 
				{
					while (!ifile.eof())     
					{
						getline(ifile,   line);
						if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
						{
							DeleteFile(strDriveDir);
							break;
						}
					}   
				}
				ifile.close();
			}
		}
	}while(!(done=_findnext(handle,&filefind)));
	
	_findclose(handle);      
}

LRESULT   CSecretEraserDlg::setupDrive(HWND hwnd, TVHITTESTINFO  tvhti,   CPoint   point)
{
	(void)point;
	string lDrive;
	TVITEM tvItem = {0};
	ItemCheck check = UNCHECKED;

	tvItem.mask = TVIF_HANDLE/* | TVIF_PARAM*/ | TVIF_STATE;
	tvItem.hItem = tvhti.hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	//tvItem.pszText = hStr;
	//Node node;
	if (!TreeView_GetItem( hwnd, &tvItem ))
	{
		return LRESULT(-1);
	}

	lDrive = (char*)tvItem.lParam;
	if (((BOOL)(tvItem.state >> 12) -1)) /*UNCHECKED*/
	{
		if (lDrive == ROOT_DRIVE)
		{
			if (!memcmp(m_hThreadId, 0, sizeof(HANDLE)*(m_driveList.length())) && m_isRunning == false)
			{
				m_driveList.clear();
				check = UNCHECKED;
			}
			else
			{
				CWnd::MessageBox(_T("有任务正在运行，请先取消任务!"), _T("提示"), MB_OK);
				check = CHECKED;
			}
		}
		else
		{
			if (m_driveList.get_valueindex(lDrive) >= 0 
				&& (m_hThreadId[m_driveList.get_valueindex(lDrive)] != 0 || m_isRunning == true))
			{
				m_driveList.remove_value(lDrive);
				check = UNCHECKED;
			}
			else if (m_driveList.get_valueindex(lDrive) >= 0)
			{
				CWnd::MessageBox(_T("有任务正在运行，请先取消任务!"), _T("提示"), MB_OK);
				check = CHECKED;
			}
			else
			{
				TRACE("任务列表空!\n");
			}
		}
	}
	else /*CHECKED*/
	{
		if (lDrive == ROOT_DRIVE)
		{
			ValueList lList, lTmpList;

			CDiskScan::getInstance()->getLogicalDrive(lList, lTmpList);
			m_driveList.set_list(lList);
		}
		else
		{
			m_driveList.add_value(lDrive);
		}
		check = CHECKED;
	}
	SetItemChecked(hwnd, tvhti.hItem, check, 1);
	return LRESULT(0);
}
bool CSecretEraserDlg::SetItemChecked(HWND hwnd, HTREEITEM hItem, ItemCheck check, bool isRoot)
{
	//First Child
	HTREEITEM hChild = TreeView_GetChild(hwnd, hItem);   

	TVITEM tvItem = {0};
	tvItem.mask = TVIF_STATE|TVIF_HANDLE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	if(!TreeView_GetItem( hwnd, &tvItem ))
		return FALSE;
	//GetSelectedItem
	if (!isRoot)
	{
		tvItem.state = INDEXTOSTATEIMAGEMASK((check ? 2 : 1));
		TreeView_SetItem( hwnd, &tvItem);
	}

	while (hChild != NULL)
	{
		SetItemChecked(hwnd, hChild, check, 0);

		//Next Sibling
		hChild = TreeView_GetNextSibling(hwnd, hChild);
	}
	return TRUE;
}
LRESULT CSecretEraserDlg::OnEraserNotify(WPARAM wParam, LPARAM lParam)
{
	CString str1 = CString("");
	CString str2 = CString("");
	CString str3 = CString("");
	CString str0 = CString("");
	CString str4 = CString("");	
	DWORD percent=0;

	HWND hWndStatic = GetDlgItem(IDC_ERASE_STATUS);
	HWND hWndProgress = GetDlgItem(IDC_ERASE_PROGRESS);
	str0.Format("%x-", wParam);
	str1.Format("%x\n", lParam);
	str0 = str0 + str1;
    switch (wParam)
    {
    case ERASER_WIPE_BEGIN:
		{
			//str0.Format("%d", /*eraserRetrieveValueContextID(lParam)*/m_ItemCnt);
			str1.Format("%d", eraserRetrieveValueProgressTimeLeft(lParam));
			percent = (DWORD)eraserRetrieveValueProgressPercent(lParam);
			str2.Format("当前进度 %d\%", percent);
			str3.Format("%d", eraserRetrieveValueProgressTotalPercent(lParam));
			str4 = CString(eraserRetrieveValueStrData(lParam));

			SetDlgItemText(IDC_ERASE_STATUS, (LPCTSTR)(str2 + CString(" ") + str4));
			::PostMessage(hWndProgress, PBM_SETPOS, (WPARAM)percent, 0L);
			TRACE("ERASER_WIPE_BEGIN\n");
			break;
		}
    case ERASER_WIPE_UPDATE:
        {
			//str0.Format("%d", /*eraserRetrieveValueContextID(lParam)*/m_ItemCnt);
			str1.Format("%d", eraserRetrieveValueProgressTimeLeft(lParam));
			percent = (DWORD)eraserRetrieveValueProgressPercent(lParam);
			str2.Format("当前进度 %d\%", percent);
			str3.Format("%d", eraserRetrieveValueProgressTotalPercent(lParam));
			str4 = CString(eraserRetrieveValueStrData(lParam));

			
			SetDlgItemText(IDC_ERASE_STATUS, (LPCTSTR)(str2 + CString(" ") + str4));
			::PostMessage(hWndProgress, PBM_SETPOS, (WPARAM)percent, 0L);
			TRACE("ERASER_WIPE_UPDATE\n");

			
			break;
		}
    case ERASER_WIPE_DONE:
        SetDlgItemText(IDC_ERASE_STATUS, (LPCTSTR)"结束");
		TRACE("ERASER_WIPE_DONE\n");
		m_isRunning = false;
        break;

	case ERASER_TEST_PAUSED:
        SetDlgItemText(IDC_ERASE_STATUS, (LPCTSTR)"暂停");
		TRACE("ERASER_TEST_PAUSED\n");
		break;
		
	default:
		TRACE(str0.GetBuffer());
		break;
		
    }

    return TRUE;
}


BOOL CSecretEraserDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static DWORD dwThreadId = 0;
	CString strDrive1;
	int iSize = 0;
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case ID_SECRET_START:
			{
				switch (m_SearchType)
				{
				    case SRCH_DISK:
					break;
					case SRCH_DIRNFILE:
					{
						char KeyWords[128] = {0};
						::GetDlgItemText(m_hWnd, IDC_EDIT_1, KeyWords, 128);
						m_keywordsFileDir = KeyWords;
					}
					break;
					case SRCH_SECTOR:
					{
						if (m_SearchSectorType == SRCH_SECTOR_MINGAN)
						{
							char KeyWords[128] = {0};
							::GetDlgItemText(m_hWnd, IDC_EDIT_2, KeyWords, 128);
							m_keywords = KeyWords;
						}
					}
					break;
					default:
					break;
				}
							
				switch (m_SearchType)
				{
				case SRCH_DISK:
					if (m_driveList.length() > 0)
					{
						if (eraserError(eraserIsValidContext(g_ehContext)))
						{
							if (eraserError(eraserCreateContextEx(&g_ehContext, (const ERASER_METHOD)ERASER_METHOD_PSEUDORANDOM, 1, 0)))
							{
								TRACE("Initialize Context Failed!\n");
							}
						}
						eraserOK(eraserSetWindow(g_ehContext, m_hWnd));
						eraserOK(eraserSetWindowMessage(g_ehContext, WM_ERASERNOTIFY));

						// clear possible previous items
						eraserOK(eraserClearItems(g_ehContext));
						
						eraserOK(eraserSetDataType(g_ehContext, ERASER_DATA_DRIVES));

						iSize = m_driveList.length();
						for (int i = 0; i < iSize; i++)
						{
							strDrive.Format("%s", m_driveList.get_list().at(i).c_str());
							
							if (m_SearchDiskType == SRCH_DISK_FULL)
							{
								m_hThreadId[i] = ::CreateThread(NULL, 0, DeleteDirectories_TASK, (LPVOID)(&strDrive), 0, &dwThreadId);
							}
							
							strDrive1 = strDrive + "\\";
							eraserOK(eraserAddItem(g_ehContext,
								(LPVOID)(LPCTSTR)strDrive1, (E_UINT16)strDrive1.GetLength()));
						}
						m_isRunning = true;
						memset(&m_hThreadId[0], 0, sizeof(HANDLE)*MAX_PROCESS_DRIVE);
						//m_driveList.clear();

						if(eraserOK(eraserStart(g_ehContext)))
						{
							TRACE("Erase Disk Error!\n");
						}

					}
					else 
					{
						CWnd::MessageBox(_T("请选择磁盘"), _T("提示"), MB_OK);
					}
					break;
				case SRCH_DIRNFILE:
					if (m_driveList.length() > 0)
					{
						if (eraserError(eraserIsValidContext(g_ehContext)))
						{
							if (eraserError(eraserCreateContextEx(&g_ehContext, (const ERASER_METHOD)ERASER_METHOD_PSEUDORANDOM, 1, 0)))
							{
								TRACE("Initialize Context Failed!\n");
							}
						}
						eraserOK(eraserSetWindow(g_ehContext, m_hWnd));
						eraserOK(eraserSetWindowMessage(g_ehContext, WM_ERASERNOTIFY));

						// clear possible previous items
						eraserOK(eraserClearItems(g_ehContext));
						
						eraserOK(eraserSetDataType(g_ehContext, ERASER_DATA_DRIVES));

						iSize = m_driveList.length();
						for (int i = 0; i < iSize; i++)
						{
							strDrive.Format("%s", m_driveList.get_list().at(i).c_str());
							m_hThreadId[i] = ::CreateThread(NULL, 0, FileSearchTask, (LPVOID)(this), 0, &dwThreadId);
							strDrive1 = strDrive + "\\";
							eraserOK(eraserAddItem(g_ehContext,
								(LPVOID)(LPCTSTR)strDrive1, (E_UINT16)strDrive1.GetLength()));
						}
						m_isRunning = true;
						memset(&m_hThreadId[0], 0, sizeof(HANDLE)*MAX_PROCESS_DRIVE);
						//m_driveList.clear();

						if(eraserOK(eraserStart(g_ehContext)))
						{
							TRACE("Erase Disk Error!\n");
						}

					}
					else 
					{
						CWnd::MessageBox(_T("请选择磁盘"), _T("提示"), MB_OK);
					}
					break;
					
				case SRCH_SECTOR:
					if (m_SearchSectorType == SRCH_SECTOR_MINGAN)
						::CreateThread(NULL, 0, sectorScanTask, (LPVOID)(this), 0, &dwThreadId);
					else if (m_SearchSectorType == SRCH_SECTOR_SHANGWANG)
						::CreateThread(NULL, 0, SectorURLScanTask, (LPVOID)(this), 0, &dwThreadId);
					break;
					
				default:
					break; 
				}
			}
			break;
		case ID_SECRET_STOP:
			g_bStopDelDir = true;

			iSize = m_driveList.length();
			for (int i = 0; i < iSize; i++)
			{
				::CloseHandle(m_hThreadId[i]);
			}

			eraserOK(eraserStop(g_ehContext));
			g_ehContext = ERASER_INVALID_CONTEXT;
			m_isRunning = false;
			break;
		case ID_SECRET_CANCEL:
			g_bStopDelDir = true;

			
			iSize = m_driveList.length();
			for (int i = 0; i < iSize; i++)
			{
				::CloseHandle(m_hThreadId[i]);
			}

			
			if (m_isRunning == false)
			{
				eraserDestroyContext(g_ehContext);
				g_ehContext = ERASER_INVALID_CONTEXT;
				CDialog::OnCancel();
			}
			else
			{
				CWnd::MessageBox(_T("有任务正在运行"), _T("提示"), MB_OK);
			}
			break;
		default:
			break;
		}
		break;
	case WM_NOTIFY:
		{
			if(IDC_TREE_DISKINFO == wParam)
			{
				LPNMHDR lpnm_hdr = LPNMHDR(lParam);
				switch(lpnm_hdr->code)
				{
				case NM_CLICK:
					{
						CPoint ptScreen;
						::GetCursorPos(&ptScreen);

						LRESULT lResult = 0;

						TVHITTESTINFO  tvhti;
						tvhti.pt = ptScreen;
						ScreenToClient(m_hLeftTree, &tvhti.pt);
						tvhti.flags = LVHT_NOWHERE;
						TreeView_HitTest(m_hLeftTree, &tvhti);

						if(TVHT_ONITEM & tvhti.flags)
						{
							TVITEM tvItem = {0};
							tvItem.mask = TVIF_PARAM|TVIF_STATE;
							tvItem.hItem = tvhti.hItem;
							//Node node;
							TreeView_GetItem(m_hLeftTree, &tvItem);

							if(TVHT_ONITEMSTATEICON & tvhti.flags) /*点击的是节点前面的checkbox */
							{
								lResult = setupDrive(m_hLeftTree, tvhti , ptScreen);
							}
						}
						if (lResult == 3)
							return LRESULT(0);
						break;
					}
					break;
				default:
					return FALSE;
				}
			}
			else
			{
			}
		}
		break;
	case WM_ERASERNOTIFY:
		{
			TRACE("WM_ERASERNOTIFY3\n");
			OnEraserNotify(wParam,lParam);
			return TRUE;
		}
	default:
		break;
	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

void CSecretEraserDlg::SetControls()
{
	switch (m_SearchType)
	{
	case SRCH_DISK:
		::SendMessage(GetDlgItem(IDC_EDIT_1), EM_SETREADONLY, TRUE, 0L);

		::SendMessage(GetDlgItem(IDC_EDIT_2), EM_SETREADONLY, TRUE, 0L);
		::EnableWindow(GetDlgItem(IDC_CHECK3), FALSE);
		::EnableWindow(GetDlgItem(IDC_CHECK4), FALSE);

		::EnableWindow(GetDlgItem(IDC_CHECK1), TRUE);
		::EnableWindow(GetDlgItem(IDC_CHECK2), TRUE);
		break;
	case SRCH_DIRNFILE:
		::EnableWindow(GetDlgItem(IDC_CHECK1), FALSE);
		::EnableWindow(GetDlgItem(IDC_CHECK2), FALSE);

		::SendMessage(GetDlgItem(IDC_EDIT_2), EM_SETREADONLY, TRUE, 0L);
		::EnableWindow(GetDlgItem(IDC_CHECK3), FALSE);
		::EnableWindow(GetDlgItem(IDC_CHECK4), FALSE);

		::SendMessage(GetDlgItem(IDC_EDIT_1), EM_SETREADONLY, FALSE, 0L);
		break;
	case SRCH_SECTOR:
		::SendMessage(GetDlgItem(IDC_EDIT_1), EM_SETREADONLY, TRUE, 0L);

		::EnableWindow(GetDlgItem(IDC_CHECK1), FALSE);
		::EnableWindow(GetDlgItem(IDC_CHECK2), FALSE);

		::SendMessage(GetDlgItem(IDC_EDIT_2), EM_SETREADONLY, FALSE, 0L);
		::EnableWindow(GetDlgItem(IDC_CHECK3), TRUE);
		::EnableWindow(GetDlgItem(IDC_CHECK4), TRUE);
		break;
	default:
		break;
	}
}
BOOL CSecretEraserDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (LOWORD(wParam))
	{
	case IDC_RADIO1:
		m_SearchType = SRCH_DISK;
		SetControls();
		break;
	case IDC_RADIO2:
		m_SearchType = SRCH_DIRNFILE;
		SetControls();
		break;
	case IDC_RADIO3:
		m_SearchType = SRCH_SECTOR;
		SetControls();
		break;

	case IDC_CHECK1:
		m_SearchDiskType = SRCH_DISK_FULL;
		::CheckDlgButton(m_hWnd, IDC_CHECK1, 1);
		::CheckDlgButton(m_hWnd, IDC_CHECK2, 0);
		break;
	case IDC_CHECK2:
		m_SearchDiskType = SRCH_DISK_FREE;
		::CheckDlgButton(m_hWnd, IDC_CHECK1, 0);
		::CheckDlgButton(m_hWnd, IDC_CHECK2, 1);
		break;
	case IDC_CHECK3:
		m_SearchSectorType = SRCH_SECTOR_MINGAN;
		::CheckDlgButton(m_hWnd, IDC_CHECK3, 1);
		::CheckDlgButton(m_hWnd, IDC_CHECK4, 0);
		break;
	case IDC_CHECK4:
		m_SearchSectorType = SRCH_SECTOR_SHANGWANG;
		::CheckDlgButton(m_hWnd, IDC_CHECK3, 0);
		::CheckDlgButton(m_hWnd, IDC_CHECK4, 1);
		break;


	default:
		break;
	} //switch (LOWORD(wParam))

	return FALSE;
}

HTREEITEM CSecretEraserDlg::AddItem(HWND hwnd, HTREEITEM hParent, LPCTSTR szText, int iImage)
{
	TVITEM tvi = {0};
	tvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
	tvi.iImage = iImage;
	tvi.iSelectedImage = iImage;
	tvi.pszText = (LPTSTR)szText;
	tvi.lParam = (LPARAM)szText;

	TVINSERTSTRUCT tvis = {0};
	tvis.hParent = hParent;
	tvis.item = tvi;
	return TreeView_InsertItem( hwnd, &tvis );
}

int CSecretEraserDlg::AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
	LVITEM lvi = {0};
	lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvi.iImage = nImage;
	lvi.pszText = (LPTSTR)szText;
	lvi.lParam = (LPARAM)pNode;
	lvi.iItem = ListView_GetItemCount(hList1);

	return ListView_InsertItem( hList1, &lvi );
}

void CSecretEraserDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
	LVITEM lvi1 = {0};
	lvi1.mask = LVIF_TEXT;
	lvi1.iItem = nItem;
	lvi1.iSubItem = nSubItem;
	lvi1.pszText = (LPTSTR)szText;
	ListView_SetItem(hList1,&lvi1);
}
HTREEITEM CSecretEraserDlg::AddItem(HWND hwnd, HTREEITEM hParent, LPCTSTR szParam, int iImage, LPCTSTR szText)
{
	TVITEM tvi = {0};
	tvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
	tvi.iImage = iImage;
	tvi.iSelectedImage = iImage;
	tvi.lParam = (LPARAM)szParam;
	tvi.pszText = (LPTSTR)szText;

	TVINSERTSTRUCT tvis = {0};
	tvis.hParent = hParent;
	tvis.item = tvi;
	return TreeView_InsertItem( hwnd, &tvis );
}

BOOL CSecretEraserDlg::OnInitDialog()
{
	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);

	ValueList list_Type;
	m_hLeftTree      = GetDlgItem(IDC_TREE_DISKINFO);
	g_list_DriveList.clear();
	CBasicDiskScan::getInstance()->getLogicalDrive(g_list_DriveList, list_Type);
	HTREEITEM htiRoot = AddItem(m_hLeftTree, NULL, _T("我的电脑"), 0);

	for(int i = 0; i < g_list_DriveList.size(); i++)
	{
		AddItem(m_hLeftTree, htiRoot, g_list_DriveList[i].c_str(), 0, g_list_DriveList[i].c_str());
	}
	TreeView_Expand(m_hLeftTree, htiRoot, TVE_EXPAND);

	//controls.
	::CheckDlgButton(m_hWnd, IDC_RADIO1, 1);
	::CheckDlgButton(m_hWnd, IDC_CHECK1, 1);
	::CheckDlgButton(m_hWnd, IDC_CHECK2, 0);
	::CheckDlgButton(m_hWnd, IDC_CHECK3, 1);
	::CheckDlgButton(m_hWnd, IDC_CHECK4, 0);
	SetControls();
	
	m_hWnd = ::GetActiveWindow();
	m_hProgress = GetDlgItem(IDC_ERASE_PROGRESS);


	
	/*if (eraserError(eraserIsValidContext(g_ehContext)))
	{
		if (eraserError(eraserCreateContextEx(&g_ehContext, (const ERASER_METHOD)ERASER_METHOD_PSEUDORANDOM, 1, 0)))
		{
			TRACE("Initialize Context Failed!\n");
		}
	}
	eraserOK(eraserSetWindow(g_ehContext, m_hWnd));
	eraserOK(eraserSetWindowMessage(g_ehContext, WM_ERASERNOTIFY));

	// clear possible previous items
	eraserOK(eraserClearItems(g_ehContext));*/

	return true;
}

void CSecretEraserDlg::OnOK()
{
	switch (m_SearchType)
	{
	    case SRCH_DISK:
		break;
		case SRCH_DIRNFILE:
		{
			char KeyWords[128] = {0};
			::GetDlgItemText(m_hWnd, IDC_EDIT_1, KeyWords, 128);
			m_keywordsFileDir = KeyWords;
		}
		break;
		case SRCH_SECTOR:
		{
			if (m_SearchSectorType == SRCH_SECTOR_MINGAN)
			{
				char KeyWords[128] = {0};
				::GetDlgItemText(m_hWnd, IDC_EDIT_2, KeyWords, 128);
				m_keywords = KeyWords;
			}
		}
		break;
		default:
		break;
	}
	
	CDialog::OnOK();
}

void CSecretEraserDlg::OnButton()
{
	//SetDlgItemText(IDC_STATIC3, _T("Button Pressed"));
	TRACE(_T("Button Pressed\n"));
}

void CSecretEraserDlg::OnCheck1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 1"));
	TRACE(_T("Check Box 1\n"));
}

void CSecretEraserDlg::OnCheck2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 2"));
	TRACE(_T("Check Box 2\n"));
}

void CSecretEraserDlg::OnCheck3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 3"));
	TRACE(_T("Check Box 3\n"));
}

void CSecretEraserDlg::OnRadio1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 1"));
	TRACE(_T("Radio 1\n"));
}

void CSecretEraserDlg::OnRadio2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 2"));
	TRACE(_T("Radio 2\n"));
}

void CSecretEraserDlg::OnRadio3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 3"));
	TRACE(_T("Radio 3\n"));
}

