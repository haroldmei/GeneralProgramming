///////////////////////////////////////
// MyDialog.h

#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

// Declaration of the CAuthSetDlg class
class CAuthSetDlg : public CDialog
{
public:
	CAuthSetDlg(UINT nResID, HWND hWndParent, CString Server, CString Secret, CString User, CString Password);
	virtual ~CAuthSetDlg();
	int AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode);
	void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);

	
	CString m_Server;
	CString m_Secret;
	CString m_User;
	CString m_Password;
	

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();

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

#endif //MYDIALOG_H
