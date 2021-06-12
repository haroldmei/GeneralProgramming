///////////////////////////////////////
// MyDialog.h

#ifndef SVRPARADIALOG_H
#define SVRPARADIALOG_H

// Declaration of the CSvrParaSetDlg class
class CSvrParaSetDlg : public CDialog
{
public:
	CSvrParaSetDlg(UINT nResID, HWND hWndParent = NULL, DWORD ptrEntry = NULL);
	virtual ~CSvrParaSetDlg();
	int AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode);
	void SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText);

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
	HWND hBasicScanList;//listctrl
	DWORD m_ptrEntry;
};

#endif //MYDIALOG_H
