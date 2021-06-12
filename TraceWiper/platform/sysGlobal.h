#ifndef _SYS_GLABLE_H_
#define _SYS_GLABLE_H_

#define DELIMITERC '\n'
#define DELIMITER "deadbeef"

/*��Ʒ��*/
//#define PRODUCT_U_BINDING 1

#define SQLITE_MSG_ID_001 0xfedc
#define SCAN_PRIVATE_MSG 0xfe00

//��Ϣ�ֶ���
#define ISCT_DETECT_BASEMSG 0X45000

#define ISCT_DETECT_0       (ISCT_DETECT_BASEMSG + 1)
#define ISCT_PERSONLIST_MSG001 (ISCT_DETECT_BASEMSG + 2)

//��Ա�б���Ϣ��
#define ISCT_PERSONLIST_MSG_CREATE          (ISCT_DETECT_BASEMSG + 3)
#define ISCT_PERSONLIST_MSG_MODIFY          (ISCT_DETECT_BASEMSG + 4)
#define ISCT_PERSONLIST_MSG_DELETE          (ISCT_DETECT_BASEMSG + 5)
#define ISCT_PERSONLIST_MSG_TONORMAL     (ISCT_DETECT_BASEMSG + 6)
#define ISCT_PERSONLIST_MSG_TOSUSPECT    (ISCT_DETECT_BASEMSG + 7)
#define ISCT_PERSONLIST_MSG_TOEXCEPTION (ISCT_DETECT_BASEMSG + 8)
#define ISCT_PERSONLIST_MSG_VIEWRESULT   (ISCT_DETECT_BASEMSG + 9)

/*DISPLAY RESULT*/
#define ISCT_DETECT_DISPLAYRESULT              (ISCT_DETECT_BASEMSG + 0x20)

/*DECTECT PROGESS*/
#define ISCT_DETECT_PROGRESS                       (ISCT_DETECT_BASEMSG + 0x21)

/*clear the displayed info for the CViewText class*/
#define ISCT_CLEAR_BOTTOM_RSULT                 (ISCT_DETECT_BASEMSG + 0x22)

/*SHOW DECTECT PROGESS*/
#define ISCT_DETECT_PROGRESS_SHOW            (ISCT_DETECT_BASEMSG + 0x23)



#define MD5KEY_FILE "md5Key.txt"
#define DETECT_CFG_FILE "detectCfg.xml"
#define MAX_KEYWORDS_SIZE 1024

/*for triee view node generation and the name of saving log */
#define PRLIST_DETECT_NAME       "������Ա"
#define PRLIST_SUSPECTED_NAME     "������Ա"
#define PRLIST_NORMAL_NAME      "������Ա"
#define PRLIST_EXCEPTION_NAME "�쳣��Ա"

/*define the file postfix of the log*/
#define FILE_POSTFIX_NAME ".log"

/*define the system param config file, the location is the default path which  current exe located.*/
#define SYSCFG_SET_FILE "set.ini"

#define ROOT_DRIVE "�ҵĵ���"

#define INVALID_WEIGTH (-255)


/***********************Sector Definition******************************/
#define MAX_SECTOR_NUM 128


/*****************Resource definition*********************************/
#define IDC_STATUS 0x888

/*****************score range definition******************************/
#define NORMAL_SCORE 10
#define SUSPECTED_SCORE 50
#define EXCEPTION_SCORE 100

/*****************registry defintion************************/
#define REG_SAM                "SAM"
#define REG_SOFTWARE     "SOFTWARE"
#define REG_SYSTEM          "SYSTEM"

#define REG_KEY_PREFIX   "ISCT_"
#define REG_KEY_CUR_NEWNAME  "ISCT_NEW_CURRENT"
#define REG_KEY_CUR_PREFIX "CURRENT_"
#include <vector>
#include <map>
using namespace std;

typedef vector<std::string> ValueList;
typedef vector<int> IntList;
typedef map<int, std::string>TableList;

//��Ա�б�����(����������ڵ�)
typedef enum
{
    PRLIST_INVALID,
    PRLIST_DETECTED,  //������Ա
    PRLIST_SUSPECTED,     //������Ա
    PRLIST_NORMAL,          //������Ա
    PRLIST_EXCEPTION,    //�쳣��Ա
}PRLIST_TYPE;

/*��Ա�б����״̬*/
typedef enum
{
    PRLIST_ACTION_INVALID,
    PRLIST_ACTION_ADD,                       /*���*/
    PRLIST_ACTION_MODIFY,                 /*�޸�*/
    PRLIST_ACTION_DELETE,                 /*ɾ��*/
    PRLIST_ACTION_SETNORMAL,          /*��������*/
    PRLIST_ACTION_SETSUSPECT,        /*�������*/
    PRLIST_ACTION_SETEXCEPTION,    /*�����쳣*/
}PRLIST_ACTION;


/*the Display Type of the Score*/
typedef enum
{
    EN_SCORE_PREVIEW,
    EN_SCORE_SETTING,
};

enum NODE_CHILD_ID
{
    NODE_SOFTWARE_NETANT,
    NODE_SOFTWARE_FLASHGET,
    NODE_SOFTWARE_EMULE,
    NODE_SOFTWARE_THUNDER,
    NODE_SOFTWARE_BT,
    NODE_SOFTWARE_QQ,
    NODE_SOFTWARE_TAOBAO,
    NODE_SOFTWARE_PAOPAO,
    NODE_SOFTWARE_SKYPE,
    NODE_COMPUTER_IE_WEBCACHE,
    NODE_COMPUTER_IE_WEBHISTORY,
    NODE_COMPUTER_IE_URLLIST,
    NODE_COMPUTER_IE_COOKIE,
    NODE_COMPUTER_FILEFOX_WEBCACHE,
    NODE_COMPUTER_FILEFOX_WEBHISTORY,
    NODE_COMPUTER_FILEFOX_URLLIST,
    NODE_COMPUTER_FILEFOX_COOKIE,
    NODE_COMPUTER_GRAPHIC,
    NODE_COMPUTER_MEDIA,
    NODE_COMPUTER_COMPRESS,
    NODE_COMPUTER_ANTIVIRUS,
    NODE_COMPUTER_DOWNLOAD,
    NODE_COMPUTER_CDROM,
    NODE_COMPUTER_WORDPROCESS,
    NODE_COMPUTER_STARTRUN,
    NODE_COMPUTER_SEARCHFILE,
    NODE_COMPUTER_DOC,
    NODE_COMPUTER_FILELIST,
    NODE_COMPUTER_TEMP,
    NODE_COMPUTER_CHINWORD,
    NODE_COMPUTER_WINEVENTAPP,
    NODE_COMPUTER_PREREAD,
    NODE_COMPUTER_REGSEARCH,
    NODE_COMPUTER_UDISKRECORD,
    NODE_COMPUTER_UDISKCROSS,
    NODE_SECURITY_BASIC,
    NODE_SECURITY_FULL,
    NODE_SECURITY_DEEP,
    NODE_HARDWARE_CPU,
    NODE_HARDWARE_HARDDISK,
    NODE_HARDWARE_MEMORY,
    NODE_HARDWARE_NP,
    NODE_HARDWARE_BLUETOOTH,
    NODE_HARDWARE_INFARE,
    NODE_HARDWARE_SYSVER,
    NODE_HARDWARE_NETCONF,
    NODE_HARDWARE_SFTINSTLIST,
    NODE_HARDWARE_USERINFO,
    NODE_HARDWARE_PWD,
    NODE_HARDWARE_IEUSER,
    NODE_HARDWARE_SHAREDIRECTORY,
    NODE_HARDWARE_MODEMBASIC,
    NODE_HARDWARE_MODEMLINK,
    NODE_HARDWARE_SYSPID,
    NODE_HARDWARE_BHO,
    NODE_HARDWARE_SPI,
    NODE_HARDWARE_STARTGROUP,
    NODE_HARDWARE_SYSSERVICE,
    NODE_HARDWARE_SRVPORT,
    NODE_SYSTEMPATCH_FIREWALL,
    NODE_SYSTEMPATCH_ANTIVIRUS,
    NODE_SYSTEMPATCH_INTRANET,
    NODE_SYSTEMPATCH_IETRUST,
    NODE_SYSTEMPATCH_IESECURITY,
    NODE_SYSTEMPATCH_IELIMITED,
    NODE_SYSTEMPATCH_PWDPOLICY,
    NODE_SYSTEMPATCH_USERLOCK,
    NODE_SYSTEMPATCH_AUDITPOLICY,
    NODE_SYSTEMPATCH_SERCURITY,
    NODE_SYSTEMPATCH_INSTALL,
    NODE_SYSTEMPATCH_TIP,
    NODE_SYSCONFIG_EXPORTLOG,
    NODE_SYSCONFIG_SETTING,
    NODE_SYSCONFIG_EXPORTCREATE,
    NODE_SYSCONFIG_PERSONLIST
};

typedef enum
{
    Register,
    File,
    Api,
    Text,
    NullAccess
}Access;

typedef enum
{
    ACT_READ,
    ACT_WRITE,
    ACT_SUBKEYS,
    ACT_SUBKEYS_TOTALPATH,
    ACT_ALL_FILEDS,
    ReadValues,
    ACT_BHO,
    ACT_WIN_SHARES,
    ACT_WIN_SYS_SHARES,
    ACT_USB_CLASS,
    ACT_USB_TYPE,
    ACT_USB_HARDWAREID,

    //IE������ۼ�
    ACT_IE_WEBCACHE_URL,
    ACT_IE_WEBCACHE_TIME,

    //IE��ʷ��¼
    ACT_IE_HISTORY_TITLE,
    ACT_IE_HISTORY_URL,
    ACT_IE_HISTORY_TIME,

    //COOKIE
    ACT_IE_COOKIE_URL,
    ACT_IE_COOKIE_TIME,

    //FIREFOX ������ۼ�
    //����
    ACT_FIREFOX_WEBCACHE_URL,
    ACT_FIREFOX_WEBCACHE_TIME,

    //��ʷ��¼
    ACT_FIREFOX_HISTORY_TITLE,
    ACT_FIREFOX_HISTORY_URL,
    ACT_FIREFOX_HISTORY_TIME,

    //�����б�
    ACT_FIREFOX_DOWNLIST_URLID,
    ACT_FIREFOX_DOWNLIST_URL,

    //COOKIE
    ACT_FIREFOX_COOKIE_URL,
    ACT_FIREFOX_COOKIE_TIME,
    
    //�ڴ�
    ACT_IE_SECURITY0,
    ACT_IE_SECURITY1,
    ACT_IE_SECURITY2,
    ACT_IE_SECURITY3,
    ACT_IE_SECURITY4,
    ACT_MEM_SIZE,
    ACT_MEM_AVAIL,
    ACT_MEM_PAGE_SIZE,
    ACT_MEM_ALLOC_STEP,
    ACT_MEM_APP_START,
    ACT_MEM_APP_END,

    //ϵͳ����
    ACT_SYS_PID_NAME,
    ACT_SYS_PID,
    ACT_SYS_SRV_PIDNAME,
    ACT_SYS_SRV_PIDDESC,
    ACT_SYS_SRV_CURRENTSTATE,

    //����˿���Ϣ
    ACT_SYS_SRVPORT_PROTO,
    ACT_SYS_SRVPORT_LOCALADDR,
    ACT_SYS_SRVPORT_FOREIGNADDR,
    ACT_SYS_SRVPORT_STATE,
    ACT_SYS_SRVPORT_PROCESS,

    //���г���ʱ��������ʱ�ļ���Ϣ
    ACT_PROG_TEMP_FILE,
    //windowsԤ���ļ�
    ACT_WIN_PRE_READ,

    //���̷�����Ϣ
    ACT_HDISK_PARTITION,
    ACT_HDISK_TYPE,
    ACT_HDISK_AVAILSIZE,
    ACT_HDISK_TOTALSIZE,

    //ɱ�����
    ACT_VIRUS_IS_RUNNING,
    //�û��Զ������
    ACT_USER_DEFINED_PNAME,
    ACT_USER_DEFINED_PID,
    ACT_USER_DEFINED_PCURRENTSTATE,

    //IE�˺�
    ACT_IE_RSCNAME,
    ACT_IE_RSCTYPE,
    ACT_IE_USERID,
    ACT_IE_USERPWD,

    //windows system patch
    ACT_WIN_SP_SOFTWARE,
    ACT_WIN_SP_PATCH,
    ACT_WIN_SP_DES,
    ACT_WIN_SP_DATE,
    //������
    ACT_UDISK_CROSS_DETECT,
    // network car info
    ACT_NETIF_GW,
    ACT_NETIF_MASK,
    ACT_NETIF_IP,
    ACT_NETIF_NAME,
    ACT_NETIF_MAC,
    //software installed
    ACT_SOFTWARE_INSTALL,
    //wireless network cards
    ACT_NETIF_WIRELESS_CARD,
    ACT_NETIF_WIRELESS_CARD_ID,
    //blue tooth
    ACT_NETIF_BLUETOOTH,
    ACT_NETIF_BLUETOOTH_ID,
    //infrad
    ACT_NETIF_INFRAD,
    //modem
    ACY_NETIF_MODEM_NAME,
    ACY_NETIF_MODEM_ID,
    ACY_NETIF_MODEM_PORT,
    //net password
    ACT_NET_PASSWD,
    NullAction
}Action;

#define KEY_WILDCARD "*"

typedef enum
{
    Binary,
    DWord,
    DWordBe,
    DWordLe,
    None,
    QWord,
    QWordBe,
    QWodLe,
    KString,
    MString,
    NullType 
}FiledType;

class Field
{
public:
    Field(){ mType = NullType; mAction = NullAction;}
    virtual ~Field(){}
    
    FiledType mType;
    Action mAction;
    std::string mField;
};

typedef vector<Field> Fileds;


typedef vector<std::string> KeyList;

typedef vector<KeyList> ResultList;

typedef struct
{
    ResultList mResultList;
} Result;

typedef vector<std::string> TitleNames;

class Key {

public:
    Key(){mAccess = NullAccess; mKey = ""; mFields.clear();}
    virtual ~Key(){}
    
    Access mAccess;
    std::string mKey;
    Fileds mFields;
};

typedef vector<Key*> Keys;


class Title
{
public:
    Title(){mKeys.clear(); }
    std::string getKey(unsigned int idx){return mKeys[idx]->mKey;}
    string getTitle(){return mTitle;}
    void setTitle(string pTitle){ mTitle = pTitle;}
    void setTitleWidth(int pWidth) {mWidth = pWidth;}
    int getTitleWidth(){return mWidth;}
    virtual ~Title(){for(unsigned int i = 0; i < mKeys.size(); i++) if(mKeys[i]) delete mKeys[i];}
    std::string mTitle;
    Keys mKeys;
    int mWidth;
};

typedef vector<Title*> Titles;



#endif//_SYS_GLABLE_H_

