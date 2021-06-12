#ifndef __SYSSETTINGDLG__
#define __SYSSETTINGDLG__

#include "../../platform/CSector.h"

/////////////////////////////////////////////////////////////////////////////
// SystemSetting dialog

class SysSettingDlg : public CDialog
{
// Construction
public:
    SysSettingDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~SysSettingDlg();
    BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void BuildData();
protected:    
    virtual BOOL OnInitDialog();
private:

};

int   CALLBACK   BrowseCallbackProc(HWND   hwnd,   UINT   uMsg,   LPARAM   lParam,   LPARAM   lpData) ;


class PersonListSettingDlg : public CDialog
{
// Construction
public:
    PersonListSettingDlg(UINT id, HWND hwnd); 
    virtual ~PersonListSettingDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void setParams(std::string name = "", std::string depart = "", std::string ipaddr = "", std::string hostName = "",std::string startTime = "");    
    void getParams(std::string& name, std::string& lDepart, std::string& lIpAddr, std::string& lHostName, std::string& startTime);
    virtual void BuildData();    
    static PersonListSettingDlg* getInstance(UINT id, HWND hwnd);
public:    
    bool m_bCancelFlag;
protected:    
    virtual BOOL OnInitDialog();
private:
    static PersonListSettingDlg *m_pPersonListSettingDlgInstance;
    std::string m_Name;
    std::string m_Depart;
    std::string m_IpAddr;
    std::string m_HostName;
    std::string m_StartTime;

};


class SysReportDlg : public CDialog
{
// Construction
public:
    SysReportDlg(UINT id, HWND hwnd); 
    virtual ~SysReportDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:    
    virtual BOOL OnInitDialog();
    void BuildPRListData();
    int getSelectedRowNum();
    //void exportResultToDoc(PRLISTDATA stInfo, std::string lpath, std::string docName, ValueList& lList);
    void exportResultToTXT(PRLISTDATA stInfo, std::string lpath, std::string docName, ValueList& lList);

private:
    /*句柄定义*/
    HWND m_hPrList;
    HWND m_hDate;
    HWND m_hLocation;
    HWND m_hPath;
    HWND m_hStatusBar;
    /*当前选择的人员信息*/
    PRLISTDATA m_personInfo;
    /*检测日期, 检测地点,保存路径*/
    string m_date;
    string m_location;
    string m_path;
    
};


class DetectProgressDlg : public CDialog
{
// Construction
public:
    DetectProgressDlg(UINT id, HWND hwnd); 
    virtual ~DetectProgressDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND GetHwnd();
    //LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    static DetectProgressDlg* getInstance(UINT id, HWND hwnd);
    static DetectProgressDlg* getInstance();
    void get_personParams(string& lName, string& lDepart, string& lIpAddr, string& lHostName,string& lStartTime);    
    void set_personParams(string& lName, string& lDepart, string& lIpAddr, string& lHostName,string& lStartTime);
    
    void endThread();
    void release_Thread(){m_hDetectThread = NULL;}
protected:    
    virtual BOOL OnInitDialog();
private:
    
    static DetectProgressDlg *m_pDetectProgressDlgInstance;
    /*句柄定义*/
    HWND m_hProgressBar;
    HWND m_hCurItemText;    

    string m_name;
    string m_depart;
    string m_ipAddr;
    string m_hostName;
    string m_startTime;
    HANDLE m_hDetectThread;
};

class UserDefinedDlg : public CDialog
{
// Construction
public:
    UserDefinedDlg(UINT id, HWND hwnd); 
    virtual ~UserDefinedDlg();
    virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL AddItems();    
    void PreCreate(CREATESTRUCT &cs);
    void ComboBox_OnChange(int index);
    void save();
protected:    
    virtual BOOL OnInitDialog();
private:
    ValueList m_ItemList;
    
    /*句柄定义*/
    HWND m_hSelectCombo;    
    HWND m_hStaticText;
    HWND m_hContent;
    HWND m_hTip;
};

class RegLoadDlg : public CDialog
{
// Construction
public:
    RegLoadDlg(UINT id, HWND hwnd);   // standard constructor
    virtual ~RegLoadDlg();
    BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void BuildData();
    BOOL AddItems();
    void LoadUsers();

protected:    
    virtual BOOL OnInitDialog();
private:
    HWND m_hFilePath;
    HWND m_hSelectCombo;
    ValueList m_userList;
    int m_selIndex;
};


#endif

