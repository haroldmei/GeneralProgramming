///////////////////////////////////////
// MyDialog.h

#ifndef SOFTDIALOG_H
#define SOFTDIALOG_H

// Declaration of the CSoftSetDlg class
class CSoftSetDlg : public CDialog
{
public:
	CSoftSetDlg(UINT nResID, HWND hWndParent/*, CString Server, CString Secret, CString User, CString Password*/);
	virtual ~CSoftSetDlg();
	int AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode);
	void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);

	
	/*CString m_Server;
	CString m_Secret;
	CString m_User;
	CString m_Password;
	*/

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	void UnsetDlgBtn1();
	void UnsetDlgBtn2();
	void SetDlgBtn1();
	void SetDlgBtn2();

private:
	void OnButton();
	void OnRadio1();
	void OnRadio2();
	void OnRadio3();
	void OnCheck1();
	void OnCheck2();
	void OnCheck3();

	HMODULE m_hInstRichEdit;
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

#endif //MYDIALOG_H
