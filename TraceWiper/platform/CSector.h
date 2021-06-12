#ifndef _CSECTOR_
#define _CSECTOR_

#include "commctrl.h"

const int MAX_SIZE = 128;

/*��Ա�б����ݽṹ*/
typedef struct
{
    int pr_type;                         /*��Ա�б�����,�ֱ�Ϊ:������Ա,������Ա, ������Ա,�쳣��Ա*/
    std::string pr_name;            /* ����*/
    std::string pr_depart;          /* ����*/
    std::string pr_ipaddr;          /* IP��ַ*/
    std::string pr_hostname;    /*������*/
    std::string pr_starttime;     /*��ʼ���ʱ��*/
    std::string pr_endtime;      /*�������ʱ��*/
}PRLISTDATA;

typedef vector<PRLISTDATA> PersonList;

class CValueList
{
public:
    CValueList();
    ~CValueList();
    void add_value(string& lVal){m_vList.push_back(lVal);};
    void remove_value(string& lVal);
    int get_valueindex(string& lVal);
    int length(){return m_vList.size();}
    void set_list(ValueList& lList){clear();m_vList = lList;}
    ValueList get_list(){return m_vList;}
    void clear(){m_vList.clear();}
private:
    ValueList m_vList;
};

class CSector
{
public:
    CSector();
    virtual ~CSector();
    
    char* get_driveName();
    void set_driveName(char* pName);
    HANDLE get_sectorDev();
    void set_sectorDev(HANDLE hDev);
    void getSectorAddress(LPWSTR tranDevName, DWORD& lPartBegin, DWORD& lPartEnd);
    void getSectorBuffer(HANDLE hDev, DWORD lSectorNum, char* lpSectBuff, bool bcloseFlag);
    bool ReadSectorsXXX(LPCWSTR devName, char* pKeywords, DWORD lPartBegin, DWORD lPartEnd);
    void set_ThreadHandle(HANDLE lThread);
    HANDLE get_ThreadHandle();
private:
    HANDLE m_hSectorDev;   //���������豸���
    HANDLE m_Thread;          //�߳̾��
    char m_driveName[MAX_SIZE];
};


/*ϵͳ������*/
class CSysConfig
{
    public:
        CSysConfig();
        virtual ~CSysConfig();
        
        static CSysConfig* getInstance();

        
        void set_filePath(std::string lpath, bool bChange = false);
        void get_filePath(std::string& lpath);

        void set_subfilePath(std::string lpath);
        void get_subfilePath(std::string& lpath);

        void set_keyWords(ValueList& lList);
        void get_keyWords(ValueList& lList);
        void get_keyWords(std::string& lKeywords);
        void saveToFile();
        void loadFromFile();

        //�ַ����ָ�
        void splitStr(char* pSrc, char* pSeps, ValueList& lList);
        void set_authType(bool bFlag){m_bNetworkFlag = bFlag;}
        bool get_authType(){return m_bNetworkFlag;}
        void set_detectedDrive(std::string val){m_detectedDrive = val;}
        string get_detectedDrive(){return m_detectedDrive;}
        void set_detectedUser(std::string val){m_detectedUser = val;}
        string get_detectedUser(){return m_detectedUser;}
        bool get_FolderChange(){return m_bChange;}
    private:
        
        static CSysConfig *m_pSysConfigInstance;
        
        std::string m_filePath;
        std::string m_subfilePath;
        ValueList   m_keywordsList;
        bool m_bNetworkFlag;
        std::string m_detectedDrive; /*���汻�������ϵͳ�̷�, U������*/
        std::string m_detectedUser; /*���汻������û�, U������*/
        bool m_bChange;                  /*�Ƿ�ı���־����·��*/
};

/*��Ա�б�*/
class CPersonListConfig
{
    public:
        CPersonListConfig();
        virtual ~CPersonListConfig();
        
        static CPersonListConfig* getInstance();

        
        void add_configList(PRLIST_TYPE enType, PRLISTDATA& stInfo);
        void remove_configList(PRLIST_TYPE enType, PRLISTDATA& stInfo);
        void remove_configList(PRLIST_TYPE enType, int index);
        void get_configList(PRLIST_TYPE enType, PersonList& lPersonMap);        
        void set_configList(PRLIST_TYPE enType, PersonList& xList);
        void update_configList(PRLIST_TYPE enType, PersonList& xList);

        void findByIndex(PRLIST_TYPE enType, DWORD index, PRLISTDATA& stInfo);
        
        /*�б���*/
        void get_titleList(ValueList& ltitleList);
        
        void convertListToLine(PRLIST_TYPE enType, ValueList& lLineResultList);

        
        void set_selectedType(PRLIST_TYPE enType);
        PRLIST_TYPE get_selectedType();
        void set_actionType(PRLIST_ACTION enAction);
        PRLIST_ACTION get_actionType();

        void set_selectIndex(int index);
        int get_selectIndex();
        
        bool isExists(PRLIST_TYPE enType, PRLISTDATA& stInfo);
        ValueList get_personData(){return m_personData;}
    private:
        void get_personList(PRLIST_TYPE enType, PersonList& xList);
        
        static CPersonListConfig *m_pPersonListInstance;
        PersonList  m_personList;     /*������Ա�б�*/
        PersonList  m_suspectList;    /*������Ա�б�*/
        PersonList  m_normalList;     /*������Ա�б�*/
        PersonList  m_exceptionList; /*�쳣��Ա�б�*/
        ValueList      m_titleList;           /*�б���*/
        ValueList      m_lineResultList; /*���ڽ����ʾ*/
        ValueList m_personData;
        PRLIST_TYPE m_selectedType;
        PRLIST_ACTION m_currentAction;
        int m_selectIndex;
};


class CScoreConfig
{
    public:
        CScoreConfig();
        virtual ~CScoreConfig();
        static CScoreConfig* getInstance();
        void set_DisplayType(int type = EN_SCORE_SETTING);
        int get_DisplayType();

    private:
        static CScoreConfig *m_pScoreConfigInstance;
        int m_displayType;
};


class CDiskScan
{
    public:
        CDiskScan();
        virtual ~CDiskScan();
        static CDiskScan* getInstance();
        virtual void getLogicalDrive(ValueList& lList, ValueList& lDTypeList);
        void set_Drive(string& lDrive);
    private:
        static CDiskScan *m_pDiskScanInstance;
        string m_drive;
};

class CBasicDiskScan : public CDiskScan
{
public:
    CBasicDiskScan();
    virtual ~CBasicDiskScan();
    static CBasicDiskScan* getInstance();
    virtual void getLogicalDrive(ValueList& lList, ValueList& lDTypeList);

    void get_titleList(ValueList& lList);

private:
    static CBasicDiskScan *m_pBasicScanInstance;
    ValueList m_titleList;
};

class CFullDiskScan : public CDiskScan
{
public:
    CFullDiskScan();
    virtual ~CFullDiskScan();
    static CFullDiskScan* getInstance();

    void get_titleList(ValueList& lList);

private:
    static CFullDiskScan *m_pFullScanInstance;
    ValueList m_titleList;
};


class CDeepDiskScan : public CDiskScan
{
public:
    CDeepDiskScan();
    virtual ~CDeepDiskScan();
    static CDeepDiskScan* getInstance();

    void get_titleList(ValueList& lList);

private:
    static CDeepDiskScan *m_pDeepScanInstance;
    ValueList m_titleList;
};

class CEditor
{
public:
    CEditor();
    virtual ~CEditor();
    static CEditor* getInstance();
    void set_data(string& val) {m_data = val;};
    string get_data(){return m_data;};
    void set_hti(LVHITTESTINFO& hti){m_hti = hti;};
    void set_previewFlag(bool bFlag);
    bool get_previewFlag();
    LVHITTESTINFO get_hti(){return m_hti;};
private:
    static CEditor *m_pEditorInstance;
    LVHITTESTINFO m_hti;
    string m_data;
    bool m_bPerview;
};

#endif
