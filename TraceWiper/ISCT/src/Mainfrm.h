/////////////////////////////////////////////
// Mainfrm.h

#ifndef MAINFRM_H
#define MAINFRM_H


#include "Classes.h"
#include "Files.h"
#include "Simple.h"
#include "Text.h"
#include "Output.h"
#include "UserOutput.h"
#include "SysSettingDlg.h"
#include "DiskScanDlg.h"

// Declaration of the CMainFrame class
class CMainFrame : public CFrame
{
public:
	CMainFrame(void);
	virtual ~CMainFrame();
	void LoadDefaultDockers();
	inline CString& getServer(){return m_Server;};
	inline CString& getSecret(){return m_Secret;};
	inline CString& getUser(){return m_User;};
	inline CString& getPassword(){return m_Password;};
	
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnCreate();
	virtual void OnInitialUpdate();
	virtual void PreCreate(CREATESTRUCT &cs);
	virtual void SaveRegistrySettings();
	virtual void SetupToolbar();
	CDockSimple& getDockView(){return m_DockView;};
       void OnSysSetting();
private:
	CDockSimple m_DockView;
	DWORD m_dbTaskId;
       SysSettingDlg* m_pSysSettingDialog;
       HANDLE m_hDetectThread;
	   
	static CString m_Server;
	static CString m_Secret;
	static CString m_User;
	static CString m_Password;
	DiskTabDlg* m_pTabDialog;
};

#endif //MAINFRM_H

