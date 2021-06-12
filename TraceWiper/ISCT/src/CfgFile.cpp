//#include "stdafx.h"
#include "afx.h"
#include "afxwin.h"
#include "windef.h"
#include "atlbase.h"
#include "atlcom.h"
#include "atlcomtime.h"
#include <tlhelp32.h>
#include <WinInet.h>
#include "commctrl.h"
#include "../../platform/ffsco.h"
#include "../../platform/stringEx.h"
#include "../../platform/ImpersonateUser.h"

#include <objbase.h>

#include <UrlHist.h>  // IUrlHistoryStg2 
#include <shlobj.h>   // CLSID_CUrlHistory, SHAddToRecentDocs
#include <atlbase.h>  // USES_CONVERSION;
#include <fstream> 

//#include <msxml5.h>

//#include "registry.h"

#pragma comment(lib, "netapi32.lib")

#pragma comment(lib, "ws2_32.lib")

#import "pstorec.dll" no_namespace //IE上网自动填充账号

#import <msxml3.dll> //raw_interfaces_only 

using namespace MSXML2; 
using darka::ImpersonateUser;

#include "afx.h"
#include "afxwin.h"
#include "windef.h"
#include "atlbase.h"
#include "atlcom.h"
#include "atlcomtime.h"
#include "commctrl.h"

#include <objbase.h>

#include   <stdlib.h> 
#include   <fstream> 
#include <io.h>

#include "../../platform/libxml/XmlTree.h"
#include "../../platform/RegisterOps.h"
#include "../../platform/FileOps.h"

#include "../../platform/Cachereader.h"
#include "CfgFile.h"
#include "DataGridView.h"
#include "../../platform/SysGlobal.h"
#include "SysSettingDlg.h"

//---------------------------------------------------------------------------
//
// 可能的 TCP 端点状态.
//
static char TcpState[][32] = {
    TEXT("???"),
    TEXT("CLOSED"),
    TEXT("LISTENING"),
    TEXT("SYN_SENT"),
    TEXT("SYN_RCVD"),
    TEXT("ESTABLISHED"),
    TEXT("FIN_WAIT1"),
    TEXT("FIN_WAIT2"),
    TEXT("CLOSE_WAIT"),
    TEXT("CLOSING"),
    TEXT("LAST_ACK"),
    TEXT("TIME_WAIT"),
    TEXT("DELETE_TCB")
};


typedef struct sqlST
{
    char sqlCmd[1024];
    char sqlPath[1024];
} SQLQUERY;

DWORD g_firefoxTaskId;
HANDLE g_firefoxThreadHandle;
extern ValueList g_FireFoxValueList;

extern HWND g_hWndFiles;
extern HWND g_hWndClasses;

//extern int AddItem(HWND hList1,LPCTSTR szText, int nImage);
//extern void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);
//extern LPWSTR string2WCHAR(const char* srcStr);
extern void ctow(WCHAR* Dest, const CHAR* Source);

extern HINSTANCE g_hInstance;
extern "C" int do_meta_command(char *zLine, struct callback_data *p);
extern "C" void main_init(struct callback_data *data);
extern "C" char *Argv0;
extern "C" void open_db(struct callback_data *p);
extern "C" int process_sqliterc(
  struct callback_data *p,        /* Configuration data */
  const char *sqliterc_override   /* Name of config file. NULL to use default */
);
extern "C" DWORD db_task(SQLQUERY* pSqlStatement, HANDLE Thread);

extern HWND g_hWndText;
extern HWND g_hWndDetectProgress;

extern DWORD g_totalSelectedNodes;

DNode * pDetectorRoot;

#define KEY_SPACE "XXXXXXXXXX"
#define KEY_SPACE_LEN strlen(KEY_SPACE)

DNode::DNode(char * pName, int pId): Node()
    ,mName(pName), mId(pId)
{
    CString lTmp;
    
    mChildrenList.clear();
    mTitles.clear();

    lTmp = pName;
    setNodeName(lTmp);

    mCalcResule = 0;
}

DNode::~DNode()
{

}


int DNode::AddItem(HWND hList1,LPCTSTR szText, int nImage)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void DNode::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

void DNode::saveCfg()
{
    if(getCfg()->getCfgType() == TYPE_ACTION_NODE)
    {
        XmlTree::getInstance()->nodeSet(getCfg()->getData(), 
                (CHECKED == getItemCheck()),
                getCfg()->getEvaWeigth(),
                getCfg()->getCalcWeigth(),
                getCfg()->getRemark());
    }
    
    return;
}

void DNode::treeSave()
{
    unsigned int idx = 0;
    DNode * pDNode = NULL;

    saveCfg();
    for(idx = 0; idx < getNodeChilds().size(); idx++)
    {
        pDNode = (DNode*)(getNodeChilds()[idx]);
        pDNode->treeSave();
    }
    
    XmlTree::getInstance()->save();
    return;
}

void DNode::replaceXmlNode()
{
    TiXmlElement lNode = (VAL_NODE);
    char buffer[32] = {0};

    lNode.SetAttribute(NODE_NAME, getName().c_str());
    if(getItemCheck() == CHECKED)
    {
        sprintf(buffer, "%d", 1);
    }
    else
    {
        sprintf(buffer, "%d", 0);
    }
    lNode.SetAttribute(NODE_CHECKED, buffer);

    lNode.SetAttribute(NODE_EVA, getCfg()->getEvaWeigth());

    lNode.SetAttribute(NODE_CAL, getCfg()->getCalcWeigth());

    lNode.SetAttribute(NODE_NOTE, getCfg()->getRemark().c_str());
    
    for(int i = 0; i < mExcuteTitles.size(); i++)
    {
        TiXmlElement lTitle(VAL_TITLE);
        
        Title * title = mExcuteTitles[i];
        lTitle.SetAttribute(TITLE_NAME, title->mTitle.c_str());
        sprintf(buffer, "%d", title->mWidth);
        lTitle.SetAttribute(TITLE_WIDTH, buffer);
        
        //key
        for(int j = 0; j < title->mKeys.size(); j++)
        {
            Key * lkey = title->mKeys[j];
            TiXmlElement keyNode(VAL_KEY);
            keyNode.SetAttribute(NODE_ACCESS, XmlTree::getInstance()->accessTypeToStr(lkey->mAccess));
            keyNode.SetAttribute(NODE_KEY, lkey->mKey.c_str());

            for(int k = 0; k < title->mKeys[j]->mFields.size(); k++)
            {
                Field lField = title->mKeys[j]->mFields[k];

                TiXmlElement Field(VAL_FIELD);
                Field.SetAttribute(FIELD_TYPE, XmlTree::getInstance()->fieldTypeToStr(lField.mType));
                Field.SetAttribute(FIELD_ACTION, XmlTree::getInstance()->actionTypeToStr(lField.mAction));
                TiXmlText childText(lField.mField.c_str());
                Field.InsertEndChild(childText);

                keyNode.InsertEndChild(Field);
            }

            lTitle.InsertEndChild(keyNode);
        }
        lNode.InsertEndChild(lTitle);
    }

    void * userData = NULL;
    XmlTree::getInstance()->replaceNode(getCfg()->getData(), &lNode, &userData);
    getCfg()->setData(userData);
    
}

void DNode::setWeigth(int eva, int cal, string note)
{
    getCfg()->setEvaWeigth(eva);
    getCfg()->setCalcWeigth(eva);
    getCfg()->setRemark(note);

    CString lSubTreeNode = getNodeSuper()->getNodeName();
    CString lTreeNode = getNodeSuper()->getNodeSuper()->getNodeName();

    
}

DNode * DNode::getChildNode(char* pName)
{
    unsigned int idx = 0;
    CString strName = pName;
    for(idx = 0; idx < getNodeChilds().size(); idx++)
    {
        if((getNodeChilds()[idx]->getNodeName() == strName)
            &&(getNodeChilds()[idx]->getCfg()->getCfgType() == TYPE_ACTION_NODE))
        {
            return (DNode*)getNodeChilds()[idx];
        }
        else
        {
        DNode * pDNode = ((DNode*)(getNodeChilds()[idx]))->getChildNode(pName);
            if (pDNode)
            {
                return pDNode;
            }
        }
    }

    return NULL;
}

void DNode::resuleToFile()
{
    /*
    char buffer[32] = {0};
    sprintf(buffer, "%d", mId);
    std::string str(buffer);

    FileOps::remove(str);    
    FileOps::create(str);

    FileOps lFile(str);

    for(unsigned int i = 0; i < mResult.mResultList.size(); i++)
    {
        KeyList &lKeyList = mResult.mResultList[i];
        for(unsigned int j = 0; j < lKeyList.size(); j++)
        {
            lFile.write(const_cast<char*>(lKeyList[j].c_str()), lKeyList[j].length());
            lFile.write(KEY_SPACE, KEY_SPACE_LEN);
        }
    }
    */
}
void DNode::resultAddRow(KeyList *pRow)
{
    mResult.mResultList.push_back(*pRow);
    for(DWORD i = 0; i < pRow->size(); i++)
    {
        mResultLineList.push_back((*pRow)[i]);
    }
}

void DNode::getLineResult(ValueList& lLineList)
{
    lLineList = mResultLineList;
}

void DNode::setLineResult(ValueList& lLineList)
{
    mResultLineList.clear();
    mResultLineList = lLineList;
}

KeyList* DNode::resultGetRow(unsigned int pIndex)
{
    return &mResult.mResultList[pIndex];
}
int DNode::resultGetRowCount()
{
    return mResult.mResultList.size();
}

int DNode::getEvaScore()
{
    return getCfg()->getEvaWeigth();
}

int DNode::getCalcScore()
{
    return resultGetRowCount() * getCfg()->getCalcWeigth();
}
int DNode::getCalcResult()
{
      return mCalcResule;
}
void DNode::setCalcResult()
{
      mCalcResule = resultGetRowCount() * getCfg()->getCalcWeigth();;
}

void DNode::get_sumCalcScores(int& totalScore, PRLIST_TYPE& enType)
{
    unsigned int idx = 0;
    DNode * pDNode = NULL;
    
    if ((getLevel() == 3) && (UNCHECKED != getItemCheck()))
    {
        totalScore+= getCalcScore();
    }
    
    for(idx = 0; idx < getNodeChilds().size(); idx++)
    {
        pDNode = (DNode*)(getNodeChilds()[idx]);

        pDNode->get_sumCalcScores(totalScore, enType);
    }

    if (totalScore <= NORMAL_SCORE)
    {
        enType =  PRLIST_NORMAL;
    }
    else if (totalScore <= SUSPECTED_SCORE)
    {
        enType =  PRLIST_SUSPECTED;
    }
    else if (totalScore <= EXCEPTION_SCORE)
    {
        enType =  PRLIST_EXCEPTION;
    }
    else
    {
        enType =  PRLIST_EXCEPTION;
    }
}

void DNode::get_sumEvaScores(int& totalScore, PRLIST_TYPE& enType)
{
    unsigned int idx = 0;
    DNode * pDNode = NULL;
    
    if ((getLevel() == 3) && (UNCHECKED != getItemCheck()))
    {
        if (getEvaScore()  != 0)
        {
            totalScore+= getEvaScore();
        }
    }
    
    for(idx = 0; idx < getNodeChilds().size(); idx++)
    {
        pDNode = (DNode*)(getNodeChilds()[idx]);

        pDNode->get_sumEvaScores(totalScore, enType);
    }

    if (totalScore <= NORMAL_SCORE)
    {
        enType =  PRLIST_NORMAL;
    }
    else if (totalScore <= SUSPECTED_SCORE)
    {
        enType =  PRLIST_SUSPECTED;
    }
    else if (totalScore <= EXCEPTION_SCORE)
    {
        enType =  PRLIST_EXCEPTION;
    }
    else
    {
        enType =  PRLIST_EXCEPTION;
    }
}

/*get the column name, and push them in one titleList for display*/
void DNode::getTitleList(ValueList& lTitleList)
{
    lTitleList = mTitles;
}

char* DNode::resultGet(unsigned int pRowIndex, char*pTitleName)
{    
    KeyList & lKeys = mResult.mResultList[pRowIndex];

    for(unsigned int i = 0; i < mTitles.size(); i++)
    {
        if(strcmp(mTitles[i].c_str(), pTitleName)==0)
        {
            return (char*)(lKeys[i].c_str());
        }
    }
    return NULL;
}


void DNode::resultEmpty()
{
    mResult.mResultList.clear();
    mResultLineList.clear();
}
bool DNode::action()
{
    if(getCfg()->getCfgType() == TYPE_ACTION_NODE)
    {
        return excute(getName(), CMD_START);
    }

    return false;
}
bool DNode::excute(std::string& /*pNodeName*/, Command /*pCmd*/)
{
    int res = 0;
    Titles & lTitles = mExcuteTitles;
/*
    if(false == XmlTree::getInstance()->nodeParse(const_cast<char*>(pNodeName.c_str()), &lTitles))
    {
        cout<<"parse node "<<pNodeName<<" failed\r\n"<<endl;
        return false;
    }
*/
    //clear previous result
    resultEmpty();
    
    vector<KeyList> titleKeys;//how many titles
    int fields = 0;
    int titleNum = lTitles.size();

    for(int i = 0; i < titleNum; i++)
    {
        Title &lTitle = *lTitles[i];
//        mTitles.push_back(lTitle.mTitle);
        KeyList lKeyList;// = new KeyList;
        lKeyList.clear();
        
        for(unsigned int keyNo = 0; keyNo < lTitle.mKeys.size(); keyNo++)
        {
            Key & lKey = *lTitle.mKeys[keyNo];
            for(unsigned int j = 0; j < lKey.mFields.size(); j++)
            {
                   Field &lField = lKey.mFields[j];
                   ValueList lResult;
                switch(lKey.mAccess)
                {
                    case Register:
                    {
                        if(-1 == RegisterOps::getInstance()->access(lField.mAction, lTitle.mKeys[keyNo]->mKey, (char*)lField.mField.c_str(), lField.mType, lResult))
                        {
                            //do not break out;
                            res =1;//break out
                        }
                        break;
                    }
                    case File:
                    case Api:
                     {
                        res = DoAPIAction(lField, lResult, lTitle.mKeys[keyNo]->mKey);
                        break;
                     }
                    case Text:
                        lKeyList.push_back(lTitle.mKeys[keyNo]->mKey);
                        break;
                    default:
                        cout<<"******ERROR********* unsupported access "<<lTitle.mKeys[keyNo]->mAccess<<endl;
                        break;
                }

                //keep result
                if(lResult.size() == 0)
                {
                    //lKeyList.push_back("未知");
                }
                for(unsigned int idx = 0; idx < lResult.size(); idx++)
                {
                    FormatString(lResult);
                    lKeyList.push_back(lResult[idx]);
                }                        

            }
        }
        
        titleKeys.push_back(lKeyList);
        fields = (lKeyList.size()>fields)?lKeyList.size():fields;
        if(0 != res)
        {
            break;
        }
    }
    //release memort
//    for(i = 0; i < lTitles.size(); i++)
    {
//       delete lTitles[i];
    }

    //build from row to column
    if(0 == res)
    {
        for(  int field = 0; field < fields; field++)
        {
            KeyList lList;
            for(unsigned int j = 0; j < mTitles.size(); j++)
            {
                std::string lTmpStr;
                if(titleKeys[j].size() > field)
                {
                    lTmpStr = titleKeys[j][field];
                }
                else
                {
                    lTmpStr = " ";
                }
                cout<<lTmpStr<<" "<<endl;
                lList.push_back(lTmpStr);
            }
            cout<<"add row"<<endl;
            resultAddRow(&lList);
        }
    }
    else if(1 == res)
    {
        for(  int field = 0; field < fields; field++)
        {
            KeyList lList;
            CString row = titleKeys[0][field].c_str();
            int idx = 0;
            CString subStr;
            string row1 = titleKeys[0][field];

            size_t pos = 0, pos2 = 0;

            int j = 0;
            while((unsigned int)(-1) != (pos2 = row1.find(DELIMITER, pos)))
            {
                for(int i = pos; i < pos2; i++)
                {
                    CString str;
                    str.Format("%c", row1[i]);
                    subStr += str;
                }
                pos = pos2 + strlen(DELIMITER);
                
                if(subStr.GetLength() == 0)
                {
                    lList.push_back(" ");
                }
                else
                {
                    lList.push_back(subStr.GetBuffer(0));
                }
                subStr.Empty();
            }

            for(int i = pos; i < row1.length(); i++)
            {
                CString str;
                    str.Format("%c", row1[i]);
                subStr += str;
            }
            lList.push_back(subStr.GetBuffer(0));
/*
            while(row[idx] != 0)
            {
                if(row[idx] != DELIMITERC)
                {
                    CString str;
                    str.Format("%c", row[idx]);
                    subStr += str;
                }
                else
                {
                    if(subStr.GetLength() == 0)
                    {
                        lList.push_back(" ");
                    }
                    else
                    {
                        lList.push_back(subStr.GetBuffer(0));
                    }
                    subStr.Empty();
                }
                idx++;
            }
            lList.push_back(subStr.GetBuffer(0));
*/
            resultAddRow(&lList);
        }
    }
    return true;
}


void DNode::FormatString(ValueList & vResult)
{
    if (0 == strcmp(getName().c_str(), "windows防火墙"))
    {
        for(unsigned int idx = 0; idx < vResult.size(); idx++)
        {
           if (0 == strcmp(vResult[idx].c_str(), "0"))
           {
                vResult[idx] = "关闭";
           }
           else if (0 == strcmp(vResult[idx].c_str(), "1"))
           {
                vResult[idx] = "启用";
           }
       }    
    }

    if (RegisterOps::getInstance()->get_BootWay()) /*U盘启动时, 格式化显示内容,去掉前缀*/
    {
        StringEx oStr;
        
        if ((0 == strcmp(getName().c_str(), "BHO")) || (0 == strcmp(getName().c_str(), "SPI")))
        {
            for(unsigned int idx = 0; idx < vResult.size(); idx++)
            {
                oStr.string_replace(vResult[idx], REG_KEY_PREFIX, "");
            }
        }
    }
}

int DNode::DoAPIAction(Field lField, ValueList& lKeyList, std::string keyString)
{
   bool bUBootFlag = RegisterOps::getInstance()->get_BootWay();

   string lTmp = RegisterOps::getInstance()->get_RegFilePath();
   string lcurSysPath = lTmp.substr(0,2);
   string lcurUserName = "johnz";
   string indexFilepath = CSysConfig::getInstance()->get_detectedDrive() + "/Documents and Settings/" 
                                      + CSysConfig::getInstance()->get_detectedUser();
   
   int res = 0;
   Action lAction = lField.mAction;
    switch(lAction)
    {

      //IE WEB缓存
      case ACT_IE_WEBCACHE_URL:
      case ACT_IE_WEBCACHE_TIME:
      {
          if (!bUBootFlag)
          {
                API_getWEBCache(lAction, lKeyList);
          }
          else
          {
              indexFilepath += "/Local Settings/Temporary Internet Files/Content.IE5/index.dat";
              indexFileView(0, /*"c:/Documents and Settings/johnz/Local Settings/Temporary Internet Files/Content.IE5/index.dat "*/indexFilepath.c_str(), lKeyList);
              res = 1;
          }
          break;
      }
      //IE WEB历史记录
      case ACT_IE_HISTORY_TITLE:
      case ACT_IE_HISTORY_URL:
      case ACT_IE_HISTORY_TIME:
      {
           if (!bUBootFlag)
           {
                API_getWEBHistory(lAction, lKeyList);
           }
           else
           {
                indexFilepath += "/Local Settings/History/History.IE5/index.dat";
                indexFileView(1, /*"c:/Documents and Settings/johnz/Local Settings/History/History.IE5/index.dat"*/indexFilepath.c_str(), lKeyList);
                res = 1;
           }
           break;
      }
      //COOKIE记录
      case ACT_IE_COOKIE_URL:
      case ACT_IE_COOKIE_TIME:
      {
           indexFilepath += "/Cookies/index.dat";
           indexFileView(0, /*"c:/Documents and Settings/johnz/Cookies/index.dat"*/indexFilepath.c_str(), lKeyList);
           res =1;
           break;
      }
      //FIREFOX 缓存
      case ACT_FIREFOX_WEBCACHE_URL:
      case ACT_FIREFOX_WEBCACHE_TIME:
      {
          //db_fireFoxQurey(lAction, lKeyList, keyString);
         //break;
      }
      //FIREFOX 历史记录
      case ACT_FIREFOX_HISTORY_TITLE:
      case ACT_FIREFOX_HISTORY_URL:
      case ACT_FIREFOX_HISTORY_TIME:
      {
          //break;
      }
      //FIREFOX 下拉列表
      case ACT_FIREFOX_DOWNLIST_URLID:
      case ACT_FIREFOX_DOWNLIST_URL:
      {
          //break;
      }
      //FIREFOX COOKIE记录
      case ACT_FIREFOX_COOKIE_URL:
      case ACT_FIREFOX_COOKIE_TIME:
      {
          db_fireFoxQurey(lAction, lKeyList, keyString);
          break;
      }
      
       //内存信息
       case  ACT_MEM_SIZE:
       case ACT_MEM_AVAIL:
       case ACT_MEM_PAGE_SIZE:
       case ACT_MEM_ALLOC_STEP:
       case ACT_MEM_APP_START:
       case ACT_MEM_APP_END:
        {
            API_getMemory(lAction, lKeyList);
            break;
        }
       //系统进程
       case ACT_SYS_PID_NAME:
       case ACT_SYS_PID:
       {
           if (!bUBootFlag)
           {
                API_getSysPID(lAction, lKeyList);
           }
           break;
       }

       //系统服务
       case ACT_SYS_SRV_PIDNAME:
       case ACT_SYS_SRV_PIDDESC:
       case ACT_SYS_SRV_CURRENTSTATE:
       {
            if (!bUBootFlag)
            {
                API_getSysService(lAction, lKeyList);
            }
            break;
       }
       //服务端口信息
       case ACT_SYS_SRVPORT_PROTO:
       case ACT_SYS_SRVPORT_LOCALADDR:
       case ACT_SYS_SRVPORT_FOREIGNADDR:
       case ACT_SYS_SRVPORT_STATE:
       case ACT_SYS_SRVPORT_PROCESS:
       {
            if (!bUBootFlag)
            {
                API_getServicePortStatus(lAction, lKeyList);
            }
            break;
       }
       case ACT_PROG_TEMP_FILE:
       {
            char* fileNameList = (char*)lField.mField.c_str();
            API_findTempFiles(lAction, fileNameList, lKeyList, keyString);
            break;
       }
       case ACT_WIN_PRE_READ:
       {
           API_findTempFiles(lAction, "*.*", lKeyList, keyString);
           break;
       }
       //磁盘分区信息
       case ACT_HDISK_PARTITION:
       case ACT_HDISK_TYPE:
       case ACT_HDISK_AVAILSIZE:
       case ACT_HDISK_TOTALSIZE:
       {
           API_getHardDrive(lAction, lKeyList);
           break;
       }
       case ACT_VIRUS_IS_RUNNING: //杀毒软件一并在此处理
       case ACT_USER_DEFINED_PNAME:
       case ACT_USER_DEFINED_PID:
       case ACT_USER_DEFINED_PCURRENTSTATE:
       {
           if (!bUBootFlag)
           {
                char* pIDNameList = (char*)lField.mField.c_str();
                API_findPIDByName(lAction, pIDNameList, lKeyList);
           }
           break;
       }
       case ACT_IE_RSCNAME:
       case ACT_IE_RSCTYPE:
       case ACT_IE_USERID:
       case ACT_IE_USERPWD:
       {
            if (!bUBootFlag)
            {
                API_getIEAccounts(lAction, lKeyList);
            }
            break;
       }
       case ACT_WIN_SP_SOFTWARE:
       {
           res = 1;
           API_getSP(lKeyList, keyString.c_str());
           break;
       }
       case ACT_UDISK_CROSS_DETECT:
       {
            API_XCONDetect(lAction, lKeyList, keyString);
            break;
       }
       case ACT_SOFTWARE_INSTALL:
       {
            res = 1;
            API_getSoftwareInstall(lKeyList);
            break;
       }
       case ACT_NET_PASSWD:
       {
            res = 1;
            API_passwdCrack(lKeyList);
            break;
        } 
        default:
            res = -1;
            break;
    }

    return res;
}

void DNode::API_getWEBCache(Action lAction,  ValueList& lKeyList)
{
    char lBuffer[2048] = {0};
    //cookier和临时文件
    HANDLE hEntry;
    LPINTERNET_CACHE_ENTRY_INFO lpCacheEntry=NULL; 
    DWORD dwEntrySize;

    dwEntrySize=0;
    hEntry=FindFirstUrlCacheEntry(NULL, NULL, &dwEntrySize);
    lpCacheEntry=(LPINTERNET_CACHE_ENTRY_INFO)new char[dwEntrySize*20];

    hEntry=FindFirstUrlCacheEntryEx(NULL,0,NORMAL_CACHE_ENTRY/*|URLHISTORY_CACHE_ENTRY*/,0,lpCacheEntry,&dwEntrySize,NULL,NULL,NULL);

     if (NULL == lpCacheEntry)
    {
            sprintf(lBuffer, "错误结果");
            return;
    }
    do
    {
        memset(lBuffer, 0, 2048);
        
        if (ACT_IE_WEBCACHE_URL == lAction)
        {
            int len = strlen(lpCacheEntry->lpszSourceUrlName);
            if (len > 1024)
            {
                strncpy(lBuffer, lpCacheEntry->lpszSourceUrlName,1023);
            }
            else
            {
                strcpy(lBuffer, lpCacheEntry->lpszSourceUrlName);
            }
        }
        else if (ACT_IE_WEBCACHE_TIME == lAction)
        {
            FILETIME   ftime;
            SYSTEMTIME   stime;
            FileTimeToLocalFileTime(&lpCacheEntry->LastAccessTime,&ftime);
            FileTimeToSystemTime(&ftime,&stime);
            sprintf(lBuffer,"%04d年%02d月%02d日 %02d:%02d:%02d",stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute,stime.wSecond);
        }
        else
        {
            sprintf(lBuffer, "未知错误");
        }
        lKeyList.push_back(lBuffer);
        dwEntrySize = 0;
        FindNextUrlCacheEntry(hEntry, NULL, &dwEntrySize);
        ZeroMemory(lpCacheEntry,dwEntrySize);
    }while(FindNextUrlCacheEntry(hEntry, lpCacheEntry, &dwEntrySize));

    delete lpCacheEntry;

}


void DNode::API_getWEBHistory(Action lAction,  ValueList& lKeyList)
{
    char lBuffer[2048] = {0};
    int     lBufferSize = 0;

    USES_CONVERSION;//Unicode转Ansi用
    CoInitialize(NULL); //初始化

    IUrlHistoryStg2* pUrlHistoryStg2 = NULL;
    HRESULT hr = CoCreateInstance(CLSID_CUrlHistory,
        NULL, CLSCTX_INPROC, IID_IUrlHistoryStg2,
        (void**)&pUrlHistoryStg2);

    IEnumSTATURL* pEnumURL;
    hr = pUrlHistoryStg2->EnumUrls(&pEnumURL);

    STATURL suURL;
    ULONG pceltFetched;
    suURL.cbSize = sizeof(suURL);
    hr = pEnumURL->Reset();

    while((hr = pEnumURL->Next(1, &suURL, &pceltFetched)) == S_OK)
    {
        switch(lAction)
        {
            case ACT_IE_HISTORY_TITLE:
            {
                if (NULL != suURL.pwcsTitle)
                {
                    int len = strlen(W2T(suURL.pwcsTitle));
                    if (len > 1024)
                    {
                        sprintf(lBuffer, W2T(suURL.pwcsTitle,1024));
                    }
                    else
                    {
                        strcpy(lBuffer, W2T(suURL.pwcsTitle));
                    }
                }
                else
                {
                    sprintf(lBuffer, "");
                }
                break;
            }
            case ACT_IE_HISTORY_URL:
            {
                int len = strlen((const char*)suURL.pwcsUrl);
                if (len > 1024)
                {
                    strncpy(lBuffer, W2T(suURL.pwcsUrl),1024);
                }
                else
                {
                    strcpy(lBuffer, W2T(suURL.pwcsUrl));
                }
                break;
            }
            case ACT_IE_HISTORY_TIME:
            {
                FILETIME   ftime;
                SYSTEMTIME   stime;
                FileTimeToLocalFileTime(&suURL.ftLastVisited,&ftime);
                FileTimeToSystemTime(&ftime,&stime);
                
                sprintf(lBuffer,"%04d年%02d月%02d日 %02d:%02d:%02d",stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute,stime.wSecond);
                break;
            }
            default:
            {
                sprintf(lBuffer, "未知错误");
                break;
            }
        }
        lKeyList.push_back(lBuffer);
    }

    pEnumURL->Release();
    pUrlHistoryStg2->Release();
    CoUninitialize();
}


void DNode::API_getMemory(Action lAction,  ValueList& lKeyList)
{
   SYSTEM_INFO mySysInfo;
   strSysInfo  sysInfo;
   memset(&sysInfo, 0, sizeof(sysInfo));
  
   GetSystemInfo(&mySysInfo);
  
   //内存特征
   sysInfo.dwPageSize = mySysInfo.dwPageSize;
   sysInfo.dwAlloc = mySysInfo.dwAllocationGranularity;
   sysInfo.pAppMinAddress = mySysInfo.lpMinimumApplicationAddress;
   sysInfo.pAppMaxAddress = mySysInfo.lpMaximumApplicationAddress;
  
   MEMORYSTATUS memStatus;
  
   memStatus.dwLength=sizeof(MEMORYSTATUS);
   
   GlobalMemoryStatus(&memStatus);
   
   DWORD tom=memStatus.dwTotalPhys;//总物理内存
   DWORD mem=memStatus.dwAvailPhys;//可用物理内存
   DWORD res=memStatus.dwAvailVirtual;//虚拟内存
   
   tom=(tom/1024)/1024;
   mem=(mem/1024)/1024;
   res=(res/1024)/1024;
   
   setlocale(LC_CTYPE, "");
  
   char lBuffer[24] = {0};

   switch(lAction)
    {
        case  ACT_MEM_SIZE:
        {    
            //lKeyList.push_back("1000");
            sprintf(lBuffer, "%d", tom); //总物理内存
            break;
        }
        case ACT_MEM_AVAIL:
        {
            //lKeyList.push_back("1000");
            sprintf(lBuffer, "%d", mem); //可用物理内存
            break;
        }
        case ACT_MEM_PAGE_SIZE:
        {
             sprintf(lBuffer, "%d", sysInfo.dwPageSize);
             break;
        }
        case ACT_MEM_ALLOC_STEP:
        {
              sprintf(lBuffer, "%d", sysInfo.dwAlloc);
              break;
        }
        case ACT_MEM_APP_START:
        {
               sprintf(lBuffer, "0x%x", sysInfo.pAppMinAddress);
               break;
        }
        case ACT_MEM_APP_END:
        {
               sprintf(lBuffer, "0x%x", sysInfo.pAppMaxAddress);
               break;
        }
        default:
        {
               sprintf(lBuffer, "未知错误");
               break;
        }

    }
    
    lKeyList.push_back(lBuffer);

    SALFDELETE(sysInfo.pArchitecture);
    SALFDELETE(sysInfo.pPrcessVersion);
    SALFDELETE(sysInfo.pProcessLevel);

}

void DNode::API_getSysPID(Action lAction,  ValueList& lKeyList)
{

   char lBuffer[128] = {0};

    PROCESSENTRY32 pe32;  

    pe32.dwSize = sizeof(pe32);  

    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  

    if(hProcessSnap == INVALID_HANDLE_VALUE)  
    {  
        TRACE("CreateToolhelp32Snapshot调用失败");  //需写错误日志
        return;  
    }
    BOOL bMore = ::Process32First(hProcessSnap,&pe32);  
    while(bMore)  
    {  
       switch(lAction)
       {
           case  ACT_SYS_PID_NAME:
           {    
               sprintf(lBuffer, "%s", pe32.szExeFile); 
               break;
           }
           case ACT_SYS_PID:
           {
               sprintf(lBuffer, "%d", pe32.th32ProcessID); 
               break;
           }
           default:
           {
                  sprintf(lBuffer, "未知错误");
                  break;
           }
        }

        lKeyList.push_back(lBuffer);

        bMore = ::Process32Next(hProcessSnap,&pe32);  
      }  
    ::CloseHandle(hProcessSnap);  

}


void DNode::API_getSysService(Action lAction,  ValueList& lKeyList)
{
    LPENUM_SERVICE_STATUS st;
    st=(LPENUM_SERVICE_STATUS)LocalAlloc(LPTR,64*1024);
    DWORD ret=0;
    DWORD size=0;
    char lBuffer[128] = {0};

    SC_HANDLE sc=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

    EnumServicesStatus(sc,SERVICE_WIN32,SERVICE_STATE_ALL, (LPENUM_SERVICE_STATUS)st,1024*64,&size,&ret,NULL);
    

    for(DWORD i=0;i<ret;i++)
    {
        switch(lAction)
        {
             case  ACT_SYS_SRV_PIDNAME:
             {    
                 sprintf(lBuffer, "%s", st[i].lpServiceName); 
                 break;
             }
             case ACT_SYS_SRV_PIDDESC:
             {
                 sprintf(lBuffer, "%s", st[i].lpDisplayName); 
                 break;
             }
             case ACT_SYS_SRV_CURRENTSTATE:
             {
                 if (SERVICE_RUNNING == st[i].ServiceStatus.dwCurrentState)
                 {
                     sprintf(lBuffer, "服务正在运行"); 
                 }
                 else if (SERVICE_STOPPED == st[i].ServiceStatus.dwCurrentState)
                 {
                     sprintf(lBuffer, "服务未运行"); 
                 }
                 break;
             }
             default:
             {
                    sprintf(lBuffer, "未知错误");
                    break;
             }
        }

        lKeyList.push_back(lBuffer);
    }

    LocalFree(LocalHandle(st));

}

char* DNode::ProcessPidToName(DWORD ProcessId)
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processEntry = { 0 };
    processEntry.dwSize = sizeof(PROCESSENTRY32); 
    static char ProcessName[256];
    
    lstrcpy(ProcessName, "Idle");
    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return ProcessName;
    
    BOOL bRet=Process32First(hProcessSnap, &processEntry);
    
    while(bRet) 
    {
        if (processEntry.th32ProcessID == ProcessId)
        {
            MODULEENTRY32 me32 = {0}; 
            me32.dwSize = sizeof(MODULEENTRY32); 
            HANDLE hModuleSnap = CreateToolhelp32Snapshot
                (TH32CS_SNAPMODULE, processEntry.th32ProcessID); 

            Module32First(hModuleSnap, &me32); // 获得全程路径.
            lstrcpy(ProcessName, me32.szExePath);
            CloseHandle(hProcessSnap);
            return ProcessName;
        }

        bRet=Process32Next(hProcessSnap, &processEntry);
    }     
    
    CloseHandle(hProcessSnap);
    return ProcessName;
}

bool DNode::findProcess(LPCTSTR ProcessName, DWORD& pid)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    Process32First(hSnapShot, &pe);
    if(lstrcmp(ProcessName, pe.szExeFile) == 0)
    {
        pid = pe.th32ProcessID;
        return true;
    }
    while(Process32Next(hSnapShot, &pe))
    {
            if(lstrcmp(ProcessName, pe.szExeFile) == 0)
            {
                pid = pe.th32ProcessID;
                return true;
            }
    }
    
    return false;

}

void DNode::API_getServicePortStatus(Action lAction,  ValueList& lKeyList)
{
    char lBuffer[1024] = {0};

    PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK
          pAllocateAndGetTcpExTableFromStack = NULL;

    PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK
          pAllocateAndGetUdpExTableFromStack = NULL;
    WSADATA WSAData;
    if( WSAStartup(MAKEWORD(1, 1), &WSAData ))
    {
        printf("WSAStartup error!\n");
        return;
    }

    HMODULE hIpDLL = LoadLibrary( "iphlpapi.dll");
    if ( !hIpDLL)
        return;

    pAllocateAndGetTcpExTableFromStack =
        (PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK) 
        GetProcAddress( hIpDLL,    "AllocateAndGetTcpExTableFromStack");
    
    pAllocateAndGetUdpExTableFromStack =
       (PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK) 
        GetProcAddress(hIpDLL, "AllocateAndGetUdpExTableFromStack" );
   
    // 显示进程与端口关联
    DWORD i;
    PMIB_TCPEXTABLE TCPExTable;
    PMIB_UDPEXTABLE UDPExTable;
    char szLocalAddress[256];
    char szRemoteAddress[256];

    if(pAllocateAndGetTcpExTableFromStack(
        &TCPExTable, TRUE, GetProcessHeap(), 2, 2))
    {
        printf("AllocateAndGetTcpExTableFromStack Error!\n");
        return;
    }

    if(pAllocateAndGetUdpExTableFromStack
        (&UDPExTable, TRUE, GetProcessHeap(), 2, 2 ))
    {
        printf("AllocateAndGetUdpExTableFromStack Error!.\n");
        return;
    }

/*
    // 获得TCP列表.
    printf("%-6s%-22s%-22s%-11s%s\n",
        TEXT("Proto"),
        TEXT("Local Address"),
        TEXT("Foreign Address"),
        TEXT("State"),
        TEXT("Process"));
*/
    for( i = 0; i <TCPExTable->dwNumEntries; i++ )
    {
        sprintf( szLocalAddress, "%s:%d",
            GetIP(TCPExTable->table[i].dwLocalAddr),
            htons( (WORD) TCPExTable->table[i].dwLocalPort));

        sprintf( szRemoteAddress, "%s:%d",
            GetIP(TCPExTable->table[i].dwRemoteAddr),
            htons((WORD)TCPExTable->table[i].dwRemotePort));
/*
        printf("%-6s%-22s%-22s%-11s%s:%d\n", TEXT("TCP"),
            szLocalAddress, szRemoteAddress,
            TcpState[TCPExTable->table[i].dwState],
            ProcessPidToName(TCPExTable->table[i].dwProcessId),
            TCPExTable->table[i].dwProcessId);
*/
         switch(lAction)
         {
             case  ACT_SYS_SRVPORT_PROTO:
             {    
                 sprintf(lBuffer, "TCP"); 
                 break;
             }
             case ACT_SYS_SRVPORT_LOCALADDR:
             {
                 sprintf(lBuffer, "%s", szLocalAddress); 
                 break;
             }
             case ACT_SYS_SRVPORT_FOREIGNADDR:
             {
                 sprintf(lBuffer, "%s", szRemoteAddress); 
                 break;
             }
             case ACT_SYS_SRVPORT_STATE:
             {
                 sprintf(lBuffer, "%s", TcpState[TCPExTable->table[i].dwState]); //这里有问题导致无法正确输出，下同
                 break;
             }
             case ACT_SYS_SRVPORT_PROCESS:
             {
                 sprintf(lBuffer, "%s:%d", ProcessPidToName(TCPExTable->table[i].dwProcessId),TCPExTable->table[i].dwProcessId);  //这里有问题
                 break;
             }
             default:
             {
                    sprintf(lBuffer, "未知错误");
                    break;
             }
        }
        lKeyList.push_back(lBuffer);
    }

    // 获得UDP列表.
    for( i = 0; i < UDPExTable->dwNumEntries; i++ )
    {
        sprintf( szLocalAddress, "%s:%d",
            GetIP(UDPExTable->table[i].dwLocalAddr),
            htons((WORD)UDPExTable->table[i].dwLocalPort));

        sprintf( szRemoteAddress, "%s","*:*");
/*
        printf("%-6s%-22s%-33s%s:%d\n", TEXT("UDP"),
            szLocalAddress, szRemoteAddress,
            ProcessPidToName(UDPExTable->table[i].dwProcessId),
            UDPExTable->table[i].dwProcessId);
*/
         switch(lAction)
         {
             case  ACT_SYS_SRVPORT_PROTO:
             {    
                 sprintf(lBuffer, "UDP"); 
                 break;
             }
             case ACT_SYS_SRVPORT_LOCALADDR:
             {
                 sprintf(lBuffer, "%s", szLocalAddress); 
                 break;
             }
             case ACT_SYS_SRVPORT_FOREIGNADDR:
             {
                 sprintf(lBuffer, "%s", szRemoteAddress); 
                 break;
             }
             case ACT_SYS_SRVPORT_STATE:
             {
                 sprintf(lBuffer, ""); 
                 break;
             }
             case ACT_SYS_SRVPORT_PROCESS:
             {
                 sprintf(lBuffer, "%s:%d", ProcessPidToName(TCPExTable->table[i].dwProcessId),UDPExTable->table[i].dwProcessId); 
                 break;
             }
             default:
             {
                    sprintf(lBuffer, "未知错误");
                    break;
             }
        }
        lKeyList.push_back(lBuffer);
    }

    FreeLibrary(hIpDLL);
    WSACleanup();

}

void DNode::API_findTempFiles(Action lAction, char* fileNameList, ValueList& lKeyList, std::string keyString )
{
    char lBuffer[1024] = {0};

     switch(lAction)
     {
         case  ACT_PROG_TEMP_FILE:
         {
            keyString += "\\";
             FileOps::getInstance()->findFiles((char*)keyString.c_str(),fileNameList, lKeyList);
             break;
         }
         case  ACT_WIN_PRE_READ:
         {    
             FileOps::getInstance()->findFiles((char*)keyString.c_str(), fileNameList, lKeyList);
             break;
         }
         default:
         {
                sprintf(lBuffer, "未知错误");
                break;
         }
    }

    //lKeyList.push_back(lBuffer);

}

void DNode::API_getHardDrive(Action lAction, ValueList& lKeyList)
{
    char lBuffer[1024] = {0};

    int DiskCount = 0;
    DWORD DiskInfo = GetLogicalDrives();

    //利用GetLogicalDrives()函数可以获取系统中逻辑驱动器的数量，函数返回的是一个32位无符号整型数据。
    while(DiskInfo)//通过循环操作查看每一位数据是否为1，如果为1则磁盘为真,如果为0则磁盘不存在。
    {
        if(DiskInfo&1)//通过位运算的逻辑与操作，判断是否为1
        {
            ++DiskCount;
        }
        DiskInfo = DiskInfo >> 1;//通过位运算的右移操作保证每循环一次所检查的位置向右移动一位。
    }

    int DSLength = GetLogicalDriveStrings(0,NULL);

    //通过GetLogicalDriveStrings()函数获取所有驱动器字符串信息长度。

    char* pDStr = new char[DSLength];//用获取的长度在堆区创建一个c风格的字符串数组

    GetLogicalDriveStrings(DSLength,(LPTSTR)pDStr);

    //通过GetLogicalDriveStrings将字符串信息复制到堆区数组中,其中保存了所有驱动器的信息。
    int DType;

    int si=0;

    BOOL fResult;

    unsigned _int64 i64FreeBytesToCaller;

    unsigned _int64 i64TotalBytes;

    unsigned _int64 i64FreeBytes;
    char dir[4] = {0};
    //为了显示每个驱动器的状态，则通过循环输出实现，由于DStr内部保存的数据是A:\NULLB:\NULLC:\NULL，这样的信息，所以DSLength/4可以获得具体大循环范围
    for(int i=0;i<DSLength/4;++i)
    {
        dir[0] = pDStr[si];
        dir[1] = ':';
        dir[2] = '\\';
        dir[3] = '\0'; 
         switch(lAction)
         {
             case  ACT_HDISK_PARTITION:
             {
                 sprintf(lBuffer, "%s",dir);
                 break;
             }
             case ACT_HDISK_TYPE:
             {    
                 DType = GetDriveType(pDStr+i*4);

                 //GetDriveType函数，可以获取驱动器类型，参数为驱动器的根目录
                 if(DType == DRIVE_FIXED)
                 {
                    sprintf(lBuffer, "硬盘");
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
                 break;
             }
             case ACT_HDISK_AVAILSIZE:
             case ACT_HDISK_TOTALSIZE:
             {
                 fResult = GetDiskFreeSpaceEx (
                 dir,
                 (PULARGE_INTEGER)&i64FreeBytesToCaller,
                 (PULARGE_INTEGER)&i64TotalBytes,
                 (PULARGE_INTEGER)&i64FreeBytes);
                 //GetDiskFreeSpaceEx函数，可以获取驱动器磁盘的空间状态,函数返回的是个BOOL类型数据
                 if(fResult)//通过返回的BOOL数据判断驱动器是否在工作状态
                 {
                     float fMB = 0;
                     if (ACT_HDISK_AVAILSIZE == lAction)
                     {
                        fMB = (float)i64FreeBytesToCaller/1024/1024;
                        if (fMB > 1024)
                        {
                            sprintf(lBuffer, "%f G",fMB/1024);//磁盘剩余空间
                        }
                        else
                        {
                            sprintf(lBuffer, "%f MB",(float)i64FreeBytesToCaller/1024/1024);//磁盘剩余空间
                        }
                     }
                     else
                     {
                        fMB = (float)i64TotalBytes/1024/1024;
                        if (fMB > 1024)
                        {
                            sprintf(lBuffer, "%f G", fMB/1024);//磁盘空间总容量
                        }
                        else
                        {
                            sprintf(lBuffer, "%f MB",(float)i64TotalBytes/1024/1024);//磁盘空间总容量
                        }
                     }                     
                 }
                 else
                 {
                     sprintf(lBuffer, "设备未准备好");
                 }

                  break;
             }
             default:
             {
                    sprintf(lBuffer, "未知错误");
                    break;
             }
        }
        si+=4;
        lKeyList.push_back(lBuffer);
    }
    if (NULL != pDStr)
    {
        delete pDStr;
        pDStr = NULL;
    }
}

void DNode::API_findPIDByName(Action lAction, char* pIDNameList, ValueList& lKeyList)
{
    char lBuffer[1024] = {0};

    std::string strSrc(pIDNameList);
    std::string token = ";";
    ValueList strPIDList;
    helper_coffs::ffsco o;
    o.splitStr(strSrc, token, strPIDList);
    DWORD dwPID = 0;
    
    for (ValueList::iterator it = strPIDList.begin(); strPIDList.end() != it; it++)
    {
        bool bRet = findProcess(it->c_str(), dwPID);

        switch(lAction)
        {
            case  ACT_USER_DEFINED_PNAME:
            {
                sprintf(lBuffer, it->c_str());
                break;
            }
            case  ACT_USER_DEFINED_PID:
            { 
                if (true == bRet)
                {
                    sprintf(lBuffer, "%d", dwPID);
                }
                else
                {
                    sprintf(lBuffer,"%s", "无");
                }
                break;
            }
            case ACT_VIRUS_IS_RUNNING:
            case  ACT_USER_DEFINED_PCURRENTSTATE:
            {    
                sprintf(lBuffer, bRet?"运行中":"未运行");
                break;
            }
            default:
            {
                   sprintf(lBuffer, "未知错误");
                   break;
            }
          }
          lKeyList.push_back(lBuffer);
    }
}

//获取IE自动填充账号密码信息
void DNode::API_getIEAccounts(Action lAction, ValueList& lKeyList)
{
    typedef HRESULT (WINAPI *tPStoreCreateInstance)(IPStore **, DWORD, DWORD, DWORD);
    HMODULE hpsDLL; 
    hpsDLL = LoadLibrary("pstorec.dll");

    if (NULL == hpsDLL)
    {
        return;
    }
    
    tPStoreCreateInstance pPStoreCreateInstance;
    pPStoreCreateInstance = (tPStoreCreateInstance)GetProcAddress(hpsDLL, "PStoreCreateInstance");

    IPStorePtr PStore; 
    HRESULT hRes = pPStoreCreateInstance(&PStore, 0, 0, 0); 
	if (!PStore)
		return;

    IEnumPStoreTypesPtr EnumPStoreTypes;
    hRes = PStore->EnumTypes(0, 0, &EnumPStoreTypes);

    char lBuffer[1024] = {0};

    if (!FAILED(hRes))
    {
        GUID TypeGUID;
        char szItemName[512];       
        char szItemData[512];
        char szResName[1512];
        char szResData[512];
        char szItemGUID[50];

        while(EnumPStoreTypes->raw_Next(1,&TypeGUID,0) == S_OK)
        {      
            wsprintf(szItemGUID,"%x",TypeGUID);
   
            IEnumPStoreTypesPtr EnumSubTypes;
            hRes = PStore->EnumSubtypes(0, &TypeGUID, 0, &EnumSubTypes);
           
            GUID subTypeGUID;
            while(EnumSubTypes->raw_Next(1,&subTypeGUID,0) == S_OK)
            {
                IEnumPStoreItemsPtr spEnumItems;
                HRESULT hRes = PStore->EnumItems(0, &TypeGUID, &subTypeGUID, 0, &spEnumItems);
   
                LPWSTR itemName;
                while(spEnumItems->raw_Next(1,&itemName,0) == S_OK)
                {
                    wsprintf(szItemName,"%ws",itemName);             
                    char chekingdata[200];
                    unsigned long psDataLen = 0;
                    unsigned char *psData = NULL;
                    _PST_PROMPTINFO *pstiinfo = NULL;
                    hRes = PStore->ReadItem(0,&TypeGUID,&subTypeGUID,itemName,&psDataLen,&psData,pstiinfo,0);
                    if(lstrlen((char *)psData)<(psDataLen-1))
                    {
                        int i=0;
                        for(int m=0;m<psDataLen;m+=2)
                        {
                            if(psData[m]==0)
                            {
                                szItemData[i]=',';
                             }
                             else
                             {
                                 szItemData[i]=psData[m];
                             }
                             i++;
                       }
                     szItemData[i-1]=0;                              
                   }
                   else 
                   {                            
                       wsprintf(szItemData,"%s",psData);                  
                   }    
                   lstrcpy(szResName,"");
                   lstrcpy(szResData,"");                 
                   //5e7e8100 - IE:Password-Protected sites
                   if(lstrcmp(szItemGUID,"5e7e8100")==0)
                   {                  
                       lstrcpy(chekingdata,"");
                       if(strstr(szItemData,":")!=0)
                       {
                           lstrcpy(chekingdata,strstr(szItemData,":")+1);
                           *(strstr(szItemData,":"))=0;                  
                       }
                       switch(lAction)
                       {
                           case  ACT_IE_RSCNAME:
                           {    
                               sprintf(lBuffer, szItemName);
                               break;
                           }
                           case  ACT_IE_RSCTYPE:
                           {    
                               sprintf(lBuffer, "IE:Password-Protected sites");
                               break;
                           }
                           case  ACT_IE_USERID:
                           {    
                               sprintf(lBuffer, szItemData);
                               break;
                           }
                           case  ACT_IE_USERPWD:
                           {    
                               sprintf(lBuffer, chekingdata);
                               break;
                           }
                           default:
                           {
                                  sprintf(lBuffer, "未知错误");
                                  break;
                           }                           
                        }
                        lKeyList.push_back(lBuffer);
                   }
                   //e161255a IE 
                   if(lstrcmp(szItemGUID,"e161255a")==0)
                   {
                       if(strstr(szItemName,"StringIndex")==0)
                       {
                           if(strstr(szItemName,":String")!=0) 
                           {
                               *strstr(szItemName,":String")=0;              
                           }
                           lstrcpyn(chekingdata,szItemName,8);              
                   
                          if((strstr(chekingdata,"http:/")==0)&&(strstr(chekingdata,"https:/")==0))
                          {
                          /*
                              printf("资源名:%s\n",szItemName);
                              printf("资源类型:IE Auto Complete Fields\n");;
                              printf("用户名:%s\n",szItemData);
                              printf("密码:%s\n",chekingdata);
                              printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                              */
                              switch(lAction)
                              {
                                  case  ACT_IE_RSCNAME:
                                  {    
                                      sprintf(lBuffer, szItemName);
                                      break;
                                  }
                                  case  ACT_IE_RSCTYPE:
                                  {    
                                      sprintf(lBuffer, "IE Auto Complete Fields");
                                      break;
                                  }
                                  case  ACT_IE_USERID:
                                  {    
                                      sprintf(lBuffer, szItemData);
                                      break;
                                  }
                                  case  ACT_IE_USERPWD:
                                  {    
                                      sprintf(lBuffer, chekingdata);
                                      break;
                                  }
                                  default:
                                  {
                                         sprintf(lBuffer, "未知错误");
                                         break;
                                  }                           
                               }
                              lKeyList.push_back(lBuffer);
                          }
                          else
                          {
                              lstrcpy(chekingdata,"");
                              if(strstr(szItemData,",")!=0)
                              {
                                   lstrcpy(chekingdata,strstr(szItemData,",")+1);
                                  *(strstr(szItemData,","))=0;                  
                              }
                              /*
                              printf("资源名:%s\n",szItemName);
                              printf("资源类型:AutoComplete Passwords\n");;
                              printf("用户名:%s\n",szItemData);
                              printf("密码:%s\n",chekingdata);
                              printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                              */
                              switch(lAction)
                               {
                                   case  ACT_IE_RSCNAME:
                                   {    
                                       sprintf(lBuffer, szItemName);
                                       break;
                                   }
                                   case  ACT_IE_RSCTYPE:
                                   {    
                                       sprintf(lBuffer, "Auto Complete Passwords");
                                       break;
                                   }
                                   case  ACT_IE_USERID:
                                   {    
                                       sprintf(lBuffer, szItemData);
                                       break;
                                   }
                                   case  ACT_IE_USERPWD:
                                   {    
                                       sprintf(lBuffer, chekingdata);
                                       break;
                                   }
                                   default:
                                   {
                                          sprintf(lBuffer, "未知错误");
                                          break;
                                   }                           
                                }
                              lKeyList.push_back(lBuffer);
                           }
                       }
                   }
                   ZeroMemory(szItemName,sizeof(szItemName));
                   ZeroMemory(szItemData,sizeof(szItemData));              
               }          
           }            
        }      
    }
}

void DNode::API_getSP(ValueList &PResult, CString pKeyPrefix)
{
    CReg lReg;
    CString lUpdates = (pKeyPrefix.GetLength() != 0)?
            (pKeyPrefix + "SOFTWARE\\Microsoft\\Updates")
            :"SOFTWARE\\Microsoft\\Updates";
    lReg.SetRootKey(HKEY_LOCAL_MACHINE);

    ValueList lSoftware ;

    lReg.SetKey(lUpdates, FALSE);

    lReg.ReadSubKeys(lSoftware);

    for(unsigned int i = 0; i < lSoftware.size(); i++)
    {
        CString res = lSoftware[i].c_str();
//        PResult.push_back(res.GetBuffer(0));
        
        CString lCls = lUpdates;
        lCls += "\\";
        lCls += lSoftware[i].c_str();
        lReg.SetKey(lCls, FALSE);

        if(lReg.GetSubKeyCount() == 0)
        {
            res += DELIMITER" ";
            res += DELIMITER" ";
            res += DELIMITER" ";
            PResult.push_back(res.GetBuffer(0));
        }
        else
        {
            ValueList lSPs;
            lReg.ReadSubKeys(lSPs);

            for(unsigned int j = 0; j < lSPs.size(); j++)
            {
                CString key = lCls;
                CString lDes, lDate;
                if(strncmp(lSPs[j].c_str(), "SP", 2) == 0)
                {
                    ValueList lSPSPs;
                    //skip this
                    key += "\\";

                    key += lSPs[j].c_str();

                    lReg.SetKey(key, FALSE);

                    lReg.ReadSubKeys(lSPSPs);

                    for(unsigned int k = 0; k < lSPSPs.size(); k++)
                    {
                        res += DELIMITER;
                        res += lSPSPs[k].c_str();
                        
                        key += "\\";
                        key += lSPSPs[k].c_str();
                        lReg.SetKey(key, FALSE);
                        
                        lDes = lReg.ReadString("Description", " ");
                        lDate = lReg.ReadString("InstalledDate", " ");

                        res += DELIMITER;
                        res += lDes;

                        res += DELIMITER;
                        res += lDate;
                        PResult.push_back(res.GetBuffer(0));
                        res = lSoftware[i].c_str();

                    }
                }
                else
                {
                    res += DELIMITER;
                    res += lSPs[j].c_str();
                    
                    key += "\\";
                    key += lSPs[j].c_str();
                    
                    lReg.SetKey(key, FALSE);

                    lDes = lReg.ReadString("Description", " ");
                    lDate = lReg.ReadString("InstalledDate", " ");

                    res += DELIMITER;
                    res += lDes;

                    res += DELIMITER;
                    res += lDate;
                    
                    PResult.push_back(res.GetBuffer(0));
                    res = lSoftware[i].c_str();
                }
            }
        }
    }
    return ;
}

void DNode::API_getSoftwareInstall(ValueList &PResult)
{
    CReg lReg;
    CString lUninstall = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    lReg.SetRootKey(HKEY_LOCAL_MACHINE);

    ValueList lSoftwares ;

    lReg.SetKey(lUninstall, FALSE);

    lReg.ReadSubKeys(lSoftwares);

    for(unsigned int i = 0; i < lSoftwares.size(); i++)
    {
//        PResult.push_back(res.GetBuffer(0));
        CString res ;
        
        CString lSoftware = lUninstall;
        lSoftware += "\\";
        lSoftware += lSoftwares[i].c_str();
        
        lReg.SetKey(lSoftware, FALSE);

        if(lReg.GetSubKeyCount() == 0)
        {
            CString lName = lReg.ReadString("DisplayName", "");
            CString lDate = lReg.ReadString("InstallDate", "");


            if((lName.GetLength() == 0)&&(lDate.GetLength() == 0))
            {
                continue;
            }

            if(lName.GetLength() == 0)
            {
                res += lSoftwares[i].c_str();
            }
            else
            {
                res += lName;
                if(lDate.GetLength() == 0)
                {
                    
                    res += DELIMITER" ";
                }
                else
                {
                    
                    res += DELIMITER;
                    res += lDate;
                }
            }
            PResult.push_back(res.GetBuffer(0));
        }
    }
    
    return ;
}

void DNode::indexFileView(int type, CString pFile, ValueList& lKeyList)
{//Cache Path on my computer: 
//c:\documents and settings\Owner\Local Settings\Temporary Internet Files\Content.IE5\index.dat
    //set cursor to busy
//    HCURSOR hCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    CString str;
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
#if 0
				int n=0;

                //This changes the bytes so that we can see the file and http info as well.
                for (DWORD dw = sizeof(URLENTRY); dw < url->dwRecordSize*BLOCK_SIZE; dw++)
                {
                    if ((url->lpText[n] == 0x0) || (url->lpText[n] == 0xF0) || (url->lpText[n] == 0xAD) || (url->lpText[n] == 0x0B))  
                    {
                        url->lpText[n] = '\r';
                    }
                    n++;
                }
#endif
                #if 0
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
                str.Format("URL: %s\r\n\tModified Date High: 0x%X\r\n\tModified Date Low: 0x%X\r\n\tAccess Date High: 0x%X\r\n\tAccess Date Low: 0x%X\r\n",
                url->lpText,
                url->modifieddate.dwHighDateTime,url->modifieddate.dwLowDateTime,
                url->accessdate.dwHighDateTime,url->accessdate.dwLowDateTime);
                #endif

                string lres;

                if(type == 1)//WEB cache
                {
                    lres = "xxx";
                    lres += DELIMITER;
//                    lKeyList.push_back("xxxx");
                }

                FILETIME lTime;
                lTime.dwLowDateTime = url->accessdate.dwLowDateTime;
                lTime.dwHighDateTime = url->accessdate.dwHighDateTime;
                helper_coffs::ffsco f;
                char timeBuf[128] = {0};
                f.getLocalTime(lTime, timeBuf);

                lres += (char*)url->lpText;
                lres += DELIMITER;
                
//                lKeyList.push_back((char*)url->lpText);
                lres += timeBuf;
                lKeyList.push_back(lres);
#if 0
                int item = AddItem(g_hWndFiles, (LPCTSTR)url->lpText, 3); //添加行

//                tmp = string2WCHAR((char*)url->lpText);

//                int item = AddItem(g_hWndFiles, (LPCTSTR)url->lpText, 3); //添加行

                for(int col = 1; col < 2; col++) //在行中设置列
                {

 //                   SetSubItem(g_hWndFiles, item, col,  TEXT("2010-05-29"));

                    #if 0
                    char * title = (char*)url->lpText;
                    //title start-flag 0x10 0x1f
                    while(1)
                    {
                        if((title[i] == 0x10)&&(title[i+1] == 0x1f))
                        {
                            break;
                        }
                        i++;
                    }
                    title = title + i + 2;
                    //tmp = string2WCHAR(title);
                    char buf[1024] = {0};
                    extern void wtoc(CHAR* Dest, const WCHAR* Source);

                    wtoc(buf, (const WCHAR *) title);

                    AddItem(g_hWndFiles, (LPCTSTR)buf, 3);
                    #endif

                    SetSubItem(g_hWndFiles, item, col, (LPCTSTR)"bbb");

                }
#endif
                //cout<<str<<endl;
                
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
//            CString str;
#if 0
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

#if 0//            CString str;
            if (url != NULL)
                str.Format("REDR: %s\r\n\tRecord Size: 0x%X\r\n",url->lpWebUrl,
                url->dwRecordSize);
//            CString strText;
//            m_test.GetWindowText(strText);
//            strText += str;
//            m_test.SetWindowText(strText);
#endif
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
    while ((dwType != 0));//This is limited to 50 because otherwise the edit box and strings run out of memory
//    ::CoTaskMemFree(pFolders->folders);//free folders info
//    ::CoTaskMemFree((void*)pFolders);//free cache folders holder
    CloseHandle(hFile);//close cache file
//    SetCursor(hCur);//return cursor to normal
}

void DNode::db_fireFoxQurey(Action lAction, ValueList& lKeyList, std::string keyString)
{

    SQLQUERY* sqlStatement = new SQLQUERY;
    memset(sqlStatement->sqlCmd, 0, 1024);
    memset(sqlStatement->sqlPath, 0, 1024);
    char username[128] = {0};
    DWORD len = 128;
    bool bRet = GetUserName(username, &len);

    switch(lAction)
    {
        case ACT_FIREFOX_WEBCACHE_URL:
        case ACT_FIREFOX_WEBCACHE_TIME:
        {
            
            //sprintf(sqlStatement->sqlCmd, "%s", "select * from moz_bookmarks;");
            //sprintf(sqlStatement->sqlPath, "%s", "C:\\Documents and Settings\\johnz\\Application Data\\Mozilla\\Firefox\\Profiles\\21lo6zq7.default\\places.sqlite");
            //sprintf(sqlStatement->sqlCmd, "%s", "select domain, chunk_id from moz_classifier;");
            sprintf(sqlStatement->sqlCmd, "%s", "select domain from moz_classifier;");
            sprintf(sqlStatement->sqlPath, "%s%s%s", "C:\\Documents and Settings\\", username, "\\Local Settings\\Application Data\\Mozilla\\Firefox\\Profiles\\21lo6zq7.default\urlclassifier3.sqlite");
            break;
        }
        //FIREFOX 历史记录
        case ACT_FIREFOX_HISTORY_TITLE:
        case ACT_FIREFOX_HISTORY_URL:
        case ACT_FIREFOX_HISTORY_TIME:
        {
            /*value 字段----HISTORY TITLE
                此数据库中无URL字段
                lastUsed      -----HISTORY TIME
            */
            //sprintf(sqlStatement->sqlCmd, "%s", "select value, lastUsed  from moz_formhistory;");
            sprintf(sqlStatement->sqlCmd, "%s", "select value from moz_formhistory;");
            sprintf(sqlStatement->sqlPath, "%s%s%s", "C:\\Documents and Settings\\", username, "\\Application Data\\Mozilla\\Firefox\\Profiles\\21lo6zq7.default\\formhistory.sqlite");

            break;
        }
        //FIREFOX 下拉列表
        case ACT_FIREFOX_DOWNLIST_URLID:
        case ACT_FIREFOX_DOWNLIST_URL:
        {
         break;
        }
        //FIREFOX COOKIE记录
        case ACT_FIREFOX_COOKIE_URL:
        case ACT_FIREFOX_COOKIE_TIME:
        {
            //sprintf(sqlStatement->sqlCmd, "%s", "select host, lastAccessed  from moz_cookies;");
            sprintf(sqlStatement->sqlCmd, "%s", "select host from moz_cookies;");
            sprintf(sqlStatement->sqlPath, "%s%s%s", "C:\\Documents and Settings\\", username, "\\Application Data\\Mozilla\\Firefox\\Profiles\\21lo6zq7.default\\cookies.sqlite");
            break;
        }
    }
    g_firefoxTaskId = db_task(sqlStatement, g_firefoxThreadHandle);
    do{
        if(!PostThreadMessage((DWORD)g_firefoxTaskId, NULL, (WPARAM)g_hWndFiles, (LPARAM)sqlStatement))
        {
            int lasterr = 0;
            lasterr = GetLastError();
            Sleep(1000);

        }
        else
        {
            break;
        }
        /*firefox only has two columns, so just put two fileds*/
        for(int i = 0; i < g_FireFoxValueList.size();i++)
        {
            lKeyList.push_back(g_FireFoxValueList[i]);
        }
    }while(1);
    
    setLineResult(lKeyList);
}

void DNode::RefreshMD5Key(std::string keyString)
{
    char lpath[10] = {0};
    char lBuffer[1024] = {0};
    char lMd5KeyBuf[1024] = {0};
    ValueList lFileList; //MD5检测结果
    bool bMd5FileExists = false; // 标识本地MD5文件是否存在

    string lFolder;
    FileOps::getInstance()->getFolder(lFolder); 
    string filename = lFolder + "\\" + "交叉检测" + FILE_POSTFIX_NAME;
    if (FileOps::getInstance()->isFileExists((char*)filename.c_str()))
    {
        FileOps::getInstance()->remove(filename);
    }
    
    if (keyString.length() > 2)
    {
        sprintf(lBuffer, "%s%s", "磁盘配置错误:", keyString.c_str());
        ::MessageBox(NULL, (LPCSTR)lBuffer, _T("刷新失败"), MB_OK);
        return;
    }
    else
    {
        sprintf(lpath, keyString.c_str());
        strcat(lpath, "\\");
    }

    //判断MD5KEY文件是否存在,若存在,取出其值
    if (FileOps::getInstance()->isFileExists(MD5KEY_FILE))
    {
        bMd5FileExists = true;
    }
    
    FileOps::getInstance()->findFiles(lpath,"*.*", lFileList);
    std::string strFileName = MD5KEY_FILE;
    if (bMd5FileExists)
    {
        FileOps::getInstance()->remove(strFileName);
    }
    
    ofstream   ofile(MD5KEY_FILE);
    for(unsigned int idx = 0; idx < lFileList.size(); idx++)
    {
        FileOps::getInstance()->GetFileMD5(lFileList[idx].c_str(), lMd5KeyBuf);
        //写MD5KEY本地文件
        ofile << lMd5KeyBuf << endl;
        //无需关闭文件流，析构ofstream对象时，系统会自动关闭
    }
}

//交叉检测
void DNode::API_XCONDetect(Action lAction, ValueList& lList, std::string keyString, int buildBy)
{
    string fileName;
    ValueList lDiskMd5List;
    if (isUSBExisted((char*)keyString.c_str(), fileName))
    {
        string tmpFile = "tmp_key.txt";
        buildMD5FileName((char*)keyString.c_str(), fileName);
        getUsbMD5FromFile(fileName, lDiskMd5List);     

        ValueList lCurMd5KeyList;

        if(0 != newUSBImport((char*)keyString.c_str(), tmpFile, buildBy, false))
        {
            lList.push_back("错误");
            return;
        }
        if(0 != getUsbMD5FromFile(tmpFile, lCurMd5KeyList))
        {
            lList.push_back("错误1");
             return;
        }

            //开始比较2个MD5KEY是否相同
        if (lCurMd5KeyList.size() != lDiskMd5List.size())
        {
            lList.push_back("已交叉使用");
        }
        else
        {
            for(unsigned int i = 0; i < lCurMd5KeyList.size(); i++)
            {
                if (lCurMd5KeyList[i] != lDiskMd5List[i]) //不相等
                {
                    lList.push_back("已交叉使用");
                    return;
                }
            }
            lList.push_back("未交叉使用");
        }
//        FileOps::getInstance()->remove(tmpFile);

    }
    else
    {
          lList.push_back("新U盘插入");
    }
    
#if 0
    char lpath[10] = {0};
    char lBuffer[1024] = {0};
    char lMd5KeyBuf[1024] = {0};
    ValueList lFileList; //MD5检测结果
    ValueList lCurMd5KeyList;   //此次MD5检测结果
    ValueList lDiskMd5List; //磁盘文件中MD5检测结果
    bool bMd5FileExists = false; // 标识本地MD5文件是否存在
    bool bRet = false;
    bool bFind = false;
    ValueList lDriveList, lList2;
    
    if (keyString.length() > 2)
    {
        sprintf(lBuffer, "%s%s", "磁盘配置错误:", keyString.c_str());
    }
    else
    {
        sprintf(lpath, keyString.c_str());
        strcat(lpath, "\\");
    }

    CBasicDiskScan::getInstance()->getLogicalDrive(lDriveList, lList2);
    for(int i = 0; i < lDriveList.size(); i++)
    {
        if (lDriveList[i] == keyString)
        {
            bFind = true;
            break;
        }
    }

    if (!bFind)
    {
        lList.push_back("无被检U盘");
        return;
    }
    
    //判断MD5KEY文件是否存在,若存在,取出其值
    if (FileOps::getInstance()->isFileExists(MD5KEY_FILE))
    {
        bMd5FileExists = true;
        bRet = FileOps::getInstance()->readFromFile(MD5KEY_FILE, lDiskMd5List);
        if (!bRet)
        {
            ::MessageBox(NULL, _T("打开本地MD5KEY文件失败"), _T("错误"), MB_OK);
            lList.push_back("内部错误");
            return;
        }
    }
    
    FileOps::getInstance()->findFiles(lpath,"*.*", lFileList);
    std::string strFileName = MD5KEY_FILE;
    if (bMd5FileExists)
    {
        FileOps::getInstance()->remove(strFileName);
    }
    ofstream   ofile(MD5KEY_FILE);
    for(unsigned int idx = 0; idx < lFileList.size(); idx++)
    {
        FileOps::getInstance()->GetFileMD5(lFileList[idx].c_str(), lMd5KeyBuf);
        lCurMd5KeyList.push_back(lMd5KeyBuf);
        //写MD5KEY本地文件
        ofile << lMd5KeyBuf << endl;
        //无需关闭文件流，析构ofstream对象时，系统会自动关闭
    }

    if (!bMd5FileExists)
    {
        lList.push_back("首次使用");
        return;
    }
    
    //开始比较2个MD5KEY是否相同
    if (lCurMd5KeyList.size() != lDiskMd5List.size())
    {
        lList.push_back("已交叉使用");
    }
    else
    {
        for(unsigned int i = 0; i < lCurMd5KeyList.size(); i++)
        {
            if (lCurMd5KeyList[i] != lDiskMd5List[i]) //不相等
            {
                lList.push_back("已交叉使用");
                return;
            }
        }
        lList.push_back("未交叉使用");
    }
    //lList.push_back("否");
#endif    
}

void DNode::API_passwdCrack(ValueList &pResult)
{

#ifdef PRODUCT_U_BINDING /*U盘启动, 不支持*/
    return;
#endif

    string userName = FileOps::getInstance()->getSysUserName();
    string domain = "";

    ValueList passwordList;
    string filename = "Password.txt";
    string lFolderName;
    
    if (CSysConfig::getInstance()->get_FolderChange())
    {
        string subPath;
        CSysConfig::getInstance()->get_filePath(lFolderName);
        filename = lFolderName + "\\" + filename;
    }

    FileOps::getInstance()->readFromFile((char*)filename.c_str(), passwordList);

    for (int i = 0; i < passwordList.size(); i++)
    {
        ImpersonateUser obLogon;
        
        if (obLogon.Logon(userName, domain, passwordList[i]))
        {
            pResult.push_back(userName);
            pResult.push_back(passwordList[i]);
            return;
        }
    }

    pResult.push_back(userName);
    pResult.push_back("未匹配到相应密码");
        
    return;
}

extern HWND g_hWndProgress;

void DNode::allChildrenExecute()
{
    unsigned int idx = 0;
    DNode * pDNode = NULL;

    for(idx = 0; idx < getNodeChilds().size(); idx++)
    {
        pDNode = (DNode*)(getNodeChilds()[idx]);
        if(getNodeChilds()[idx]->getCfg()->getCfgType() == TYPE_ACTION_NODE)
        {
            if (UNCHECKED != pDNode->getItemCheck())
            {
                pDNode->excute(pDNode->getName(),CMD_START);

                /*save file*/
                string lFolderName;
                if (CSysConfig::getInstance()->get_FolderChange())
                {
                    string subPath;
                    CSysConfig::getInstance()->get_filePath(lFolderName);
                    CSysConfig::getInstance()->get_subfilePath(subPath);
                    lFolderName += "\\" + subPath;
                }
                else
                {
                    FileOps::getInstance()->getFolder(lFolderName); 
                }

                pDNode->setCalcResult();
                
                string lFileName =lFolderName + "\\" + pDNode->getName() + FILE_POSTFIX_NAME;
                ValueList rowList;
                pDNode->getLineResult(rowList);
                FileOps::getInstance()->writeToFile((char*)lFileName.c_str(), rowList);

                PostMessage(g_hWndText, WM_COMMAND, ISCT_DETECT_PROGRESS, (LPARAM)pDNode);
                PostMessage(g_hWndProgress, WM_COMMAND, ISCT_DETECT_PROGRESS_SHOW, (LPARAM)pDNode);
            }
        }
        else
        {
            pDNode->allChildrenExecute();
        }
    }
    
}


void DNode::tableListBuild(int &pRow, int &pColumn, buildTable pBuild)
{
    unsigned int idx = 0;
    if (this != pDetectorRoot)
    {
        pBuild(pRow, pColumn, getNodeName().GetBuffer(0), (void*)this);
    }
    int lCol = ++pColumn;

    for(idx = 0; idx < getNodeChilds().size(); idx++)
    {
        int lChildCol = lCol;
        ((DNode*)getNodeChilds()[idx])->tableListBuild(++pRow, lChildCol, pBuild);
    }
}
/*
buildBy    : 0 - file content
             1 - USB剩余空间大小
             2 - 文件大小访问时间,创建时间，修改时间

*/
//新U盘认证信息导入
int DNode::newUSBImport(char * pUsbVol, string & pMd5KeyFile, int buildBy, bool fileSave)
{
    if(fileSave)
    {
        if(buildMD5FileName(pUsbVol, pMd5KeyFile) != true)
        {
            //error 
            return -1;
        }
    }
    else
    {
        if(pMd5KeyFile.length() == 0)
        {
            return -1;
        }
    }
    
    ValueList lCurMd5KeyList;
    char lMd5KeyBuf[1024] = {0};

    if(FileOps::getInstance()->isFileExists(pMd5KeyFile))
    {
        FileOps::getInstance()->remove(pMd5KeyFile);
    }
    
    ofstream   ofile(pMd5KeyFile.c_str());
    
    if(0 == buildBy)
    {
        ValueList lFileList;
        string lpath = pUsbVol;
        lpath += "\\";
        
        FileOps::getInstance()->findFiles(const_cast<char*>(lpath.c_str()),"*.*", lFileList);

        for(unsigned int idx = 0; idx < lFileList.size(); idx++)
        {
            FileOps::getInstance()->GetFileMD5(lFileList[idx].c_str(), lMd5KeyBuf);
//            lCurMd5KeyList.push_back(lMd5KeyBuf);
            //写MD5KEY本地文件
            ofile << lMd5KeyBuf << endl;
            //无需关闭文件流，析构ofstream对象时，系统会自动关闭
        }

    }
    else if(1 == buildBy) // USB 剩余空间大小
    {

        ULARGE_INTEGER nFreeBytesAvailable;
        ULARGE_INTEGER nTotalNumberOfBytes;
        ULARGE_INTEGER nTotalNumberOfFreeBytes;
        //
        if (GetDiskFreeSpaceEx(_T("C:"),
                &nFreeBytesAvailable,
                &nTotalNumberOfBytes,
                &nTotalNumberOfFreeBytes))
        {
            BYTE chBuf[256] = {0};
            __int64 len = (__int64)sprintf((char*)chBuf,"Av=%I64d,Total=%I64d,Free=%I64d",
            nFreeBytesAvailable,
            nTotalNumberOfBytes,
            nTotalNumberOfFreeBytes);

            FileOps::getInstance()->GetMD5(chBuf, len, lMd5KeyBuf);
        }
        

        
        
    }
    else if(2 == buildBy)//文件大小，访问时间，创建时间，修改时间
    {
        struct _finddata_t filefind;
        string curr=pUsbVol;
        curr += "\\*.*";
        int handle = 0;
        
        if((handle=_findfirst(curr.c_str(),&filefind))==-1)
        {
            return -1;
        }

        do
        {
            if((0 == strcmp(filefind.name,"..")) ||(0 == strcmp(filefind.name,".")))
            {
                continue;
            }
            
            if (_A_SUBDIR==filefind.attrib)
            {
            }
            else
            {
                char tmpBuf[1024] = {0};
                string fileMaterial;
                helper_coffs::ffsco ffo;
                //filefind.name
                //filefind.size
                //filefind.time_create
                //filefind.time_write
                //filefind.time_access

                fileMaterial += filefind.name;

                sprintf(tmpBuf, "%U64d", filefind.size); /*size*/
                fileMaterial += tmpBuf;

                memset(tmpBuf, 0, 1024);
                ffo.getLocalTime(filefind.time_create, tmpBuf);
                fileMaterial += tmpBuf;

                memset(tmpBuf, 0, 1024);
                ffo.getLocalTime(filefind.time_write, tmpBuf);
                fileMaterial += tmpBuf;

                memset(tmpBuf, 0, 1024);
                ffo.getLocalTime(filefind.time_access, tmpBuf);
                fileMaterial += tmpBuf;

                FileOps::getInstance()->GetMD5((BYTE*)fileMaterial.c_str(), fileMaterial.length(), lMd5KeyBuf);
                ofile << lMd5KeyBuf << endl;
                
            }
        }while(!(_findnext(handle,&filefind)));
    }

    return 0;
}

bool DNode::buildMD5FileName(char* pUsbVol, string & pMd5KeyFile)
{
    string pid, vid, sn;


    pMd5KeyFile = "test_key_file.txt";
    
    return true;
}
bool DNode::isUSBExisted(char* pUsbVol, string & pMd5KeyFile)
{
//    bool bMd5FileExists = false;
    //判断MD5KEY文件是否存在

    if(buildMD5FileName(pUsbVol, pMd5KeyFile) != true)
    {
        return false;
    }
    
    if (FileOps::getInstance()->isFileExists(pMd5KeyFile))
    {
        return true;
    }
    else
    {
        return false;
    }

}

int DNode::getUsbMD5FromFile(string filename, ValueList & lDiskMd5List)
{
    bool bRet = true;
    string lFile;
    if (!FileOps::getInstance()->isFileExists(filename))
    {
        return -1;// MD5 file 不存在
    }
    
    bRet = FileOps::getInstance()->readFromFile((char*)filename.c_str(), lDiskMd5List);
    if (!bRet)
    {
        return 1;//打开本地文件失败
        //::MessageBox(NULL, _T("打开本地MD5KEY文件失败"), _T("错误"), MB_OK);
        //lList.push_back("内部错误");
    }

    return 0;
}

void API_getPasswordPolicy()
{
    //password parameters
    //USER_MODALS_INFO_0
//    NetUserModalsGet (0);
}

void API_getLockoutPolicy()
{
    //Return lockout information
    //USER_MODALS_INFO_3
//    NetUserModalsGet (0);
}


void API_getAuditPolicy()
{
//    AuditQueryPerUserPolicy();
}

    
#define INVALID_NODE_ID 255

Node* NewDetectorNode(Node * ParentNode, string pName, int pID)
{
    Node *pNode = 0;
    DNode *pDNode = 0;

    void * data;

    Titles lTitles;

    XmlTree::getInstance()->nodeParse((char*)pName.c_str(), &lTitles, &data);

    pNode = pDNode = new DNode((char*)pName.c_str(), pID);
    Config *pCfg = new Config();
    pCfg->setData(data);

    //all titles
    for(unsigned int i = 0; i < lTitles.size(); i++)
    {
        pDNode->pushTitle(lTitles[i]->mTitle);
        pDNode->pushExcute(lTitles[i]);
    }

    
    pNode->setNodeSuper(ParentNode);
    pNode->setLevel(ParentNode->getLevel()+1);
    pNode->setCfg(pCfg);
    pNode->setItemCheck(UNCHECKED);

//    pNode->setNodeName(pName.c_str());

    if(pID != INVALID_NODE_ID)
    {
//        pNode->setNodeIsLeaf(1);
//        DNode * lDNode = new DNode((char*)pName.c_str(), pID);
        pCfg->setCfgType(TYPE_ACTION_NODE);
    }
//    else
//    {
        pNode->setNodeIsLeaf(0);
//    }
    CString lTmp;
    lTmp = pName.c_str();
    pCfg->setCfgValue(lTmp);

    ParentNode->getNodeChilds().push_back(pNode);

    return pNode;
}
void allNodes(NameList &pAllNodes)
{
    NameList lTreeNodeName;
    XmlTree::getInstance()->treeNodes(&lTreeNodeName);
    for(unsigned int i = 0; i < lTreeNodeName.size(); i++)
    {

        NameList lSubTreeName;
        XmlTree::getInstance()->subTreeNodes(lTreeNodeName[i], &lSubTreeName);

        for(unsigned int j = 0; j < lSubTreeName.size(); j++)
        {
            NameList lNodeNames;
            BoolList lCheckedList;
            XmlTree::getInstance()->nodes(lTreeNodeName[i], lSubTreeName[j], &lNodeNames, &lCheckedList);
            for(unsigned int k = 0; k < lNodeNames.size(); k++)
            {
                pAllNodes.push_back(lNodeNames[k]);
            }
        }
    }
}


//深度优先搜索遍历算法函数 
bool   DetectorTreeBuild()   
{   
    Node *pChildNode = 0;
    pDetectorRoot = new DNode("详细列表", 0);
//    pDetectorRoot->setNodeName("痕迹检测工具");
    pDetectorRoot->setNodeIsLeaf(0);
    pDetectorRoot->setItemCheck(UNCHECKED);

    Config * cfg = new Config();
    pDetectorRoot->setCfg(cfg);
    pDetectorRoot->setLevel(0);

    NameList lTreeNodeName;
//allNodes(lTreeNodeName);

    XmlTree::getInstance()->treeNodes(&lTreeNodeName);
    int treeNodeChecked = 0, treeNodeUnChecked = 0;
	unsigned int i;
    for(i = 0; i < lTreeNodeName.size(); i++)
    {
        pChildNode = NewDetectorNode(pDetectorRoot, lTreeNodeName[i], INVALID_NODE_ID);
        pChildNode->setIsLeaf(false);
        pChildNode->setLevel(1);
        pChildNode->getCfg()->setEvaWeigth(INVALID_WEIGTH);
        pChildNode->getCfg()->setCalcWeigth(INVALID_WEIGTH);
        int subTreeChecked = 0, subTreeUnChecked = 0;

        NameList lSubTreeName;
        XmlTree::getInstance()->subTreeNodes(lTreeNodeName[i], &lSubTreeName);
		unsigned int j;
        for( j = 0; j < lSubTreeName.size(); j++)
        {
            Node * pSubTreeNode = NewDetectorNode(pChildNode, lSubTreeName[j], INVALID_NODE_ID);
            pSubTreeNode->setIsLeaf(false);
            pSubTreeNode->setLevel(2);
            pSubTreeNode->getCfg()->setEvaWeigth(INVALID_WEIGTH);
            pSubTreeNode->getCfg()->setCalcWeigth(INVALID_WEIGTH);
            int subTreeNodeChecked = 0;
            NameList lNodeNames;
            IDList lNodeIds;
            BoolList lCheckedList;
            XmlTree::getInstance()->nodes(lTreeNodeName[i], lSubTreeName[j], &lNodeNames, &lCheckedList);
			unsigned int k;
            for(k = 0; k < lNodeNames.size(); k++)
            {
                Node * pTreeNode = NewDetectorNode(pSubTreeNode, lNodeNames[k], 0);
                pTreeNode->setIsLeaf(false);
                pTreeNode->setLevel(3);
                if(lCheckedList[k] == true)
                {
                    pTreeNode->setItemCheck(CHECKED);
                    g_totalSelectedNodes++;
                    subTreeNodeChecked++;
                }
                else
                {
                    pTreeNode->setItemCheck(UNCHECKED);
                    //g_totalSelectedNodes--;
                }
                int eva, cal;
                string note;
                XmlTree::getInstance()->getNodeWeigth(lTreeNodeName[i], lSubTreeName[j], lNodeNames[k], 
                    eva, cal, note);
                pTreeNode->getCfg()->setEvaWeigth(eva);
                pTreeNode->getCfg()->setDefaultEvaWeigth(eva);
                //pTreeNode->getCfg()->setSumEva(eva);
                pSubTreeNode->getCfg()->setSumEva(eva);
                pChildNode->getCfg()->setSumEva(eva);
                pTreeNode->getCfg()->setCalcWeigth(cal);
                pTreeNode->getCfg()->setDefaultCalcWeigth(cal);
                pTreeNode->getCfg()->setRemark(note);
                pDetectorRoot->getCfg()->setSumEva(eva);
                pDetectorRoot->getCfg()->setSumCalc(cal);
                //pTreeNode->setItemCheck(CHECKED);
//                pSubTreeNode->getNodeChilds().push_back(pTreeNode);
            }
            //if all children is checked, the parent is checked
            if(subTreeNodeChecked == k)// all subnode is checked
            {
                pSubTreeNode->setItemCheck(CHECKED);
                subTreeChecked++;
            }
            else if(0 == subTreeNodeChecked) // no subnode checked
            {
                pSubTreeNode->setItemCheck(UNCHECKED);
                subTreeUnChecked++;
            }
            else//half checked
            {
                pSubTreeNode->setItemCheck(HALFCHECKED);
            }
        }
        if(subTreeChecked == j)//all subtree is checked
        {
            pChildNode->setItemCheck(CHECKED);
            treeNodeChecked++;
        }
        else if(subTreeUnChecked == j)// no subtree checked
        {
            pChildNode->setItemCheck(UNCHECKED);
            treeNodeUnChecked++;
        }
        else//half check
        {
            pChildNode->setItemCheck(HALFCHECKED);
        }
    }
    if(treeNodeChecked == lTreeNodeName.size())
    {
        pDetectorRoot->setItemCheck(CHECKED);
    }
    else if(treeNodeUnChecked == lTreeNodeName.size())
    {
        pDetectorRoot->setItemCheck(UNCHECKED);
    }
    else 
    {
        pDetectorRoot->setItemCheck(HALFCHECKED);
    }
    
    return 0;
}  


//---------------------------------------------------------------------------
//
// 生成IP地址字符串.
//
PCHAR GetIP(unsigned int ipaddr)
{
    static char pIP[20];
    unsigned int nipaddr = htonl(ipaddr);
    sprintf(pIP, "%d.%d.%d.%d",
        (nipaddr >>24) &0xFF,
        (nipaddr>>16) &0xFF,
        (nipaddr>>8) &0xFF,
        (nipaddr)&0xFF);
    return pIP;
}

