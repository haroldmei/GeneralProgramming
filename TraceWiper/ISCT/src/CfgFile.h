
#ifndef _CFGFILE_H_
#define _CFGFILE_H_

//#include "afx.h"
//#include "afxwin.h"
//#include "windef.h"
//#include "atlbase.h"
//#include "atlcom.h"
//#include "atlcomtime.h"

#include <vector>

//#include "..\..\eraserlib\resource.h"
//#include "..\..\eraserlib\eraserdll.h"

//#include "DNode.h"

//#include <objbase.h>
//#include <string>

#include "../../platform/sysGlobal.h"

#include <vector>

//#include "XmlTree.h"

typedef vector<std::string> ValueList;


enum Command
{
    CMD_START,
    CMD_STOP
};


using namespace std;

enum ItemCheck
{
    UNCHECKED = 0,
    CHECKED,
    HALFCHECKED
};

#define TYPE_REG 0
#define TYPE_FILE 1
#define TYPE_DIR 2
#define TYPE_DISK 3
#define TYPE_FILE_TYPE 4
#define TYPE_ACTION_NODE 5

typedef ULONG       D_UINT32;
typedef D_UINT32 D_HANDLE;
#define D_INVALID_CONTEXT          ((D_HANDLE)-1)

typedef void (*buildTable)(int &pRow, int &pColum, std::string pContent, void* pDNode);


class Config
{
private:
    int cfgType;
    CString cfgValue;
    D_HANDLE   m_ehContext;
    int evaluateWeigth;
    int defaultEvaWeigth;
    int sumEva;
    int calculateWeigth;
    int defaultCalWeigth;
    int sumCalc;
    int iItem;
    int iSubItem;
    string note;
    void * mData;
public:
    virtual bool erase(){ return false;}

    void setData(void* pData){mData = pData;}
    void *getData(){return mData;}
    
    Config():m_ehContext(D_INVALID_CONTEXT){cfgType = -1; cfgValue = CString(""); sumEva = sumCalc = 0;};
    void setCfgType(int type){cfgType = type; return;};
    void setCfgValue(CString &value){cfgValue = value; return;};
    int getCfgType(){return cfgType;};
    CString& getCfgValue(){return cfgValue;};
    void setEvaWeigth(int val) {evaluateWeigth = val;}
    int getEvaWeigth() {return evaluateWeigth;}

    void setSumEva(int val){sumEva += val;}
    int getSumEva(){return sumEva;}
    void setDefaultEvaWeigth(int val) {defaultEvaWeigth = val;}
    int getDefaultEvaWeigth(){return defaultEvaWeigth;}
    
    void setCalcWeigth(int val) {calculateWeigth = val;}
    int getCalcWeigth() {return calculateWeigth;}
    void setSumCalc(int val){sumCalc += val;}
    int getSumCalc(){return sumCalc;}
    void setDefaultCalcWeigth(int val) {defaultCalWeigth = val;}
    int getDefaultCalcWeigth(){return defaultCalWeigth;}
    void setRemark(string mark){note = mark;}
    string getRemark(){return note;}

    void setRowCol(int row, int col){iItem = row; iSubItem = col;}
    void getRowCol(int& row, int& col){row = iItem; col = iSubItem;}
};

class Node
{
private:
    CString strName;
    vector<Node*> pChilds;
    Node *pSuper;
    Config *pCfg;
    ItemCheck checked;

    bool IsLeaf;
    int nLevel;
    
public:
    HTREEITEM htiCTreeItem;
    Node()
    {
        pSuper = 0;
        nLevel = 0;
        pCfg = 0;
        strName = CString("");
        checked = UNCHECKED;
        IsLeaf = 0;
        //pChilds = 0;
    };
    
    //bool getItemCheck(){return (UNCHECKED != checked);}
    ItemCheck getItemCheck(){return checked;}
    void setItemCheck(ItemCheck check){checked = check;};
    
    bool getIsLeaf(){return IsLeaf;};
    void setIsLeaf(bool isLeaf){IsLeaf = isLeaf;};

    int getLevel(){return nLevel;};
    void setLevel(int level){nLevel = level;};

    void setCfg(Config *cfg){pCfg = cfg; return;};
    Config * getCfg(){return pCfg;};

    bool setNodeName(CString name){strName = name; return 0;};
    CString& getNodeName(){return strName;};

    bool setNodeIsLeaf(bool leaf){IsLeaf = leaf; return 0;};

    bool setNodeSuper(Node *node){pSuper = node; return 0;};
    Node *getNodeSuper(){return pSuper;};

    vector<Node*>& getNodeChilds(){return pChilds;}

    virtual bool addConfig(Config* pCfg) = 0;
    virtual bool delConfig(Config* pCfg) = 0;
    virtual bool addNode(Node* pNode) = 0;
    virtual bool delNode(Node* pNode) = 0;
//    virtual bool erase()= 0;
};

using std::vector;
using namespace std;



struct  strSysInfo
{
 DWORD    dwCPUNumber;
 wchar_t    *pArchitecture;
 wchar_t    *pProcessLevel;
 wchar_t    *pPrcessVersion;

 DWORD    dwPageSize;
 DWORD    dwAlloc;
 void    *pAppMinAddress;
 void    *pAppMaxAddress;
};

#define SALFDELETE(pValue) \
 if(pValue) \
 delete[] pValue; \
 pValue = NULL;


//---------------------------------------------------------------------------
// 以下为与TCP相关的结构. 
typedef struct tagMIB_TCPEXROW{
    DWORD dwState;              // 连接状态.
    DWORD dwLocalAddr;             // 本地计算机地址.
    DWORD dwLocalPort;           // 本地计算机端口.
    DWORD dwRemoteAddr;            // 远程计算机地址.
    DWORD dwRemotePort;         // 远程计算机端口.
    DWORD dwProcessId;
} MIB_TCPEXROW, *PMIB_TCPEXROW;

typedef struct tagMIB_TCPEXTABLE{
    DWORD dwNumEntries;
    MIB_TCPEXROW table[100];    // 任意大小数组变量.
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

//---------------------------------------------------------------------------
// 以下为与UDP相关的结构. 
typedef struct tagMIB_UDPEXROW{
    DWORD dwLocalAddr;             // 本地计算机地址.
    DWORD dwLocalPort;             // 本地计算机端口.
    DWORD dwProcessId;
} MIB_UDPEXROW, *PMIB_UDPEXROW;

typedef struct tagMIB_UDPEXTABLE{
    DWORD dwNumEntries;
    MIB_UDPEXROW table[100];    // 任意大小数组变量. 
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;

//---------------------------------------------------------------------------
// 所用的iphlpapi.dll中的函数原型定义.
typedef DWORD (WINAPI *PALLOCATE_AND_GET_TCPEXTABLE_FROM_STACK)(
    PMIB_TCPEXTABLE *pTcpTable, // 连接表缓冲区.
    BOOL bOrder,                
    HANDLE heap,
    DWORD zero,
    DWORD flags
    );

typedef DWORD (WINAPI *PALLOCATE_AND_GET_UDPEXTABLE_FROM_STACK)(
    PMIB_UDPEXTABLE *pUdpTable, // 连接表缓冲区.
    BOOL bOrder,                
    HANDLE heap,
    DWORD zero,
    DWORD flags
    );


PCHAR GetIP(unsigned int ipaddr);

class DNode;

typedef vector<DNode> ChildList;


class DNode : public Node
{
public:
    DNode(char * pName, int pId);
    virtual ~DNode();

    void saveCfg();
    void treeSave();
    
    DNode * getChildNode(char* pName);
    
    int AddItem(HWND hList1,LPCTSTR szText, int nImage);
    
    void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);

    virtual bool excute(std::string &pNodeName, Command pCmd);
    void resultAddRow(KeyList *pRow);
    KeyList* resultGetRow(unsigned int pIndex);
    char* resultGet(unsigned int pRowIndex, char*pTitleName);
    int resultGetRowCount();
    int getEvaScore();
    int getCalcScore();
    void get_sumEvaScores(int& totalScore, PRLIST_TYPE& enType);
    void get_sumCalcScores(int& totalScore, PRLIST_TYPE& enType);

    void getLineResult(ValueList& lLineList);
    void setLineResult(ValueList& lLineList);

    void setWeigth(int eva, int cal, string note);

    int resultGetTitleCount(){return mTitles.size();}
    char* resultGetTitle(unsigned int pTitleIdx){return (char*)mTitles[pTitleIdx].c_str();}
    Title* getNodeTitle(unsigned int idx){return mExcuteTitles[idx];}
    Titles getNodeTitles(){return mExcuteTitles;}
    int getNodeTitleNum(){return mExcuteTitles.size();}
    void resultEmpty();
    void resuleToFile();
    void getTitleList(ValueList& lTitleList);
    int getCalcResult();
    void setCalcResult();

    void pushTitle(string & pTitle){mTitles.push_back(pTitle);}
    void pushExcute(Title* pTitle){mExcuteTitles.push_back(pTitle);}
    
    int getId(){return mId;}
    std::string getName(){return mName;}
    //字符串格式化
    void FormatString(ValueList& vResult);

    int newUSBImport(char * pUsbVol, string & pMd5KeyFile, int buildBy, bool fileSave=true);
    bool buildMD5FileName(char* pUsbVol, string & pMd5KeyFile);
    bool isUSBExisted(char* pUsbVol, string & pMd5KeyFile);
    int getUsbMD5FromFile(string pFilename, ValueList & lDiskMd5List);

    void API_getWEBCache(Action lAction,  ValueList& lKeyList);
    void API_getWEBHistory(Action lAction,  ValueList& lKeyList);
    void API_passwdCrack(ValueList& lKeyList);
    //根据进程名称查找进程ID
    bool findProcess(LPCTSTR ProcessName, DWORD& pid);

    //API 操作
    int DoAPIAction(Field lFiled, ValueList& lKeyList, std::string keyString);
    //获取内存信息
    void API_getMemory(Action lAction, ValueList& lKeyList);
    //获取系统进程信息
    void API_getSysPID(Action lAction, ValueList& lKeyList);
    //获取系统服务信息
    void API_getSysService(Action lAction, ValueList& lKeyList);
    //服务端口状态
    void API_getServicePortStatus(Action lAction,  ValueList& lKeyList);
    //运行程序时产生的临时文件信息
    void API_findTempFiles(Action lAction, char* fileNameList, ValueList& lKeyList, std::string keyString);
    void API_getHardDrive(Action lAction, ValueList& lKeyList);
    void API_findPIDByName(Action lAction, char* pIDNameList, ValueList& lKeyList);
    void API_getIEAccounts(Action lAction,  ValueList& lKeyList);
    void API_getSP(ValueList &PResult, CString pKeyPrefix);
    void indexFileView(int type, CString pFile, ValueList& lKeyList);
    void db_fireFoxQurey(Action lAction, ValueList& lKeyList, std::string keyString);
    void API_XCONDetect(Action lAction, ValueList& lList, std::string keyString, int buildBy = 0);
    void RefreshMD5Key(std::string keyString);
    void API_getSoftwareInstall(ValueList &PResult);

    void allChildrenExecute();

    bool action();
    void tableListBuild(int &pRow, int &pColumn, buildTable pBuild);

    void replaceXmlNode();

    virtual bool addConfig(Config* pCfg){ pCfg=pCfg; return true;}
    virtual bool delConfig(Config* pCfg){pCfg=pCfg; return true;}
    virtual bool addNode(Node* pNode){pNode = pNode; return true;}
    virtual bool delNode(Node* pNode){pNode = pNode; return true;}
    
private:

    ChildList mChildrenList;
    Result mResult;
    ValueList mResultLineList;
    TitleNames mTitles;
    std::string mName;
    int mId;
    int mCalcResule;

    Titles mExcuteTitles;

    char* ProcessPidToName(DWORD ProcessId);
};

extern DNode * pDetectorRoot;


#endif
