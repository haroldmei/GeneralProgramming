///////////////////////////////////////
// MyDialog.cpp

#include "stdafx.h"
#include "NodeSetDialog.h"
#include "resrc1.h"
#include "resource.h"
#include "entrysetdialog.h"

#include <atlstr.h>
#include "cfgfile.h"


// Definitions for the CNodeSetDlg class
CNodeSetDlg::CNodeSetDlg(UINT nResID, HWND hWndParent, DWORD ptrNode)
	: CDialog(nResID, hWndParent)
{
	hBasicScanList = ::GetDlgItem(m_hWnd,IDC_LIST_ENTRIES);
	m_ptrNode = ptrNode;
	m_hInstRichEdit = ::LoadLibrary(_T("RICHED32.DLL"));
    if (!m_hInstRichEdit)
 		::MessageBox(NULL, _T("CNodeSetDlg::CRichView  Failed to load RICHED32.DLL"), _T(""), MB_ICONWARNING);
}

CNodeSetDlg::~CNodeSetDlg()
{
	::FreeLibrary(m_hInstRichEdit);
}

BOOL CNodeSetDlg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char aa[64] = {0};
	sprintf(aa, "0x%x-0x%x-0x%x\n", uMsg, wParam, lParam);
	
	switch (uMsg)
	{
		//Additional messages to be handled go here
	case WM_NOTIFY:
		if(IDC_LIST_ENTRIES == wParam)
		{
			LPNMHDR lpnm_hdr = LPNMHDR(lParam);
			
			switch(lpnm_hdr->code)
			{
			case NM_CLICK: 
				sprintf(aa, "LINE %d, 0x%x\n", __LINE__, lpnm_hdr->code);
				TRACE(aa); 
				break;
			case NM_DBLCLK: 
				{
					CPoint ptScreen;
					::GetCursorPos(&ptScreen);

					LRESULT lResult = 0;

					LVHITTESTINFO  tvhti;
					tvhti.pt = ptScreen;
					ScreenToClient(hBasicScanList, &tvhti.pt);
					tvhti.flags = LVHT_NOWHERE;
					
					ListView_HitTest(hBasicScanList, &tvhti);

					LVITEM tvItem = {0};
					tvItem.mask = LVIF_PARAM|LVIF_STATE;
					tvItem.iItem = tvhti.iItem;
					
					ListView_GetItem(hBasicScanList, &tvItem);
					if (!tvItem.lParam)
						return LRESULT(1); 
					Config *pCfg = ((Node*)(tvItem.lParam))->getCfg();
					CEntrySetDlg* m_pDialog;
					m_pDialog = new CEntrySetDlg(IDD_DIALOG_ENTRYCFG, m_hWnd, (DWORD)pCfg);
					if (m_pDialog->DoModal() == 1)
					{
						CString strType = CString("");
						switch(pCfg->getCfgType())
						{
							case TYPE_REG:
								strType = CString("注册表键");
								break;
							case TYPE_FILE:
								strType = CString("文件");
								break;
							case TYPE_DIR:
								strType = CString("目录");
								break;
							case TYPE_DISK:
								strType = CString("磁盘空间");
								break;
							case TYPE_FILE_TYPE:
								strType = CString("文件类型");
								break;
							case TYPE_REG_VAL:
								strType = CString("注册表值");
								break;
							default:
								strType = CString("未知类型");
								break;
						}
						//SetSubItem(hBasicScanList, tvhti.iItem, 1, strType);
						SetSubItem(hBasicScanList, tvhti.iItem, 1, pCfg->getCfgValue());
					}
					else
					{
					}

					// Clean up
					delete m_pDialog;
					m_pDialog = NULL;


					return LRESULT(0);
				}
			default:
				sprintf(aa, "LINE %d, 0x%x\n", __LINE__, lpnm_hdr->code);
				TRACE(aa);
				break;
			}
		}
		break;
		
	default:
		break;
	}

	// Pass unhandled messages on to parent DialogProc
	return DialogProcDefault(uMsg, wParam, lParam);
}

BOOL CNodeSetDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (LOWORD(wParam))
    {
	case IDOK_NEW:
		{
			CEntrySetDlg* m_pDialog;
			Config *pCfg = new Config;
			pCfg->setCfgType(TYPE_DIR);
			m_pDialog = new CEntrySetDlg(IDD_DIALOG_ENTRYCFG, m_hWnd, (DWORD)pCfg);
			TRACE(_T("ID_TEST_TEST5\n"));
			if (m_pDialog->DoModal() == 1)
			{
				EraseNode *pNode = (EraseNode*)m_ptrNode;
				EraseNode *pNewNode = new EraseNode;
				pNewNode->setCfg(pCfg);
				pNewNode->setNodeIsLeaf(1);
				pNewNode->setLevel(pNode->getLevel()+1);
				pNode->addNode(pNewNode);
				CString strType = CString("");
				switch(pCfg->getCfgType())
				{
					case TYPE_REG:
						strType = CString("注册表键");
						break;
					case TYPE_FILE:
						strType = CString("文件");
						break;
					case TYPE_DIR:
						strType = CString("目录");
						break;
					case TYPE_DISK:
						strType = CString("磁盘空间");
						break;
					case TYPE_FILE_TYPE:
						strType = CString("文件类型");
						break;
					case TYPE_REG_VAL:
						strType = CString("注册表值");
						break;
					default:
						strType = CString("未知类型");
						break;
				}
				int item = AddItem(hBasicScanList, strType, 3, pNewNode); //添加行
				SetSubItem(hBasicScanList, item, 1, pCfg->getCfgValue());
			}
			else
			{
			}

			// Clean up
			delete m_pDialog;
			m_pDialog = NULL;
			
			return TRUE; 
		}
		break;
	case ID_CONFIRM:
		{
			Node *pNode = (Node*)m_ptrNode;
			LPTSTR strNodeName = new TCHAR[100];
			::GetDlgItemText(m_hWnd, IDC_EDIT_NODENAME, strNodeName, 100);
			if (strlen(strNodeName))
				pNode->setNodeName(CString(strNodeName));
			else
				pNode->setNodeName(CString("新结点"));
			OnOK();
		}
		break;
	case ID_CANCEL:
		OnCancel();
		break;
	case IDOK_DELETE:
		{
			int   i,   n;
		    n   =   ListView_GetItemCount(hBasicScanList);
		    for   (i   =   0;   i   <   n;   i++)
		        if   (ListView_GetItemState(hBasicScanList,   i,   LVIS_FOCUSED)   ==   LVIS_FOCUSED)
		            break; 
				
			LVITEM tvItem = {0};
			tvItem.mask = LVIF_PARAM|LVIF_STATE;
			tvItem.iItem = i;
			
			ListView_GetItem(hBasicScanList, &tvItem);

			
			EraseNode* pNode = (EraseNode*)(tvItem.lParam);
			pNode->delNode();
			delete pNode;


			ListView_DeleteItem(hBasicScanList, i);
		}
		break;
	default:
		break;
    } //switch (LOWORD(wParam))

	return FALSE;
}

int CNodeSetDlg::AddItem(HWND hList1,LPCTSTR szText, int nImage, void* pNode)
{
    LVITEM lvi = {0};
    lvi.mask = LVIF_TEXT |LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvi.iImage = nImage;
    lvi.pszText = (LPTSTR)szText;
    lvi.lParam = (LPARAM)pNode;
    lvi.iItem = ListView_GetItemCount(hList1);

    return ListView_InsertItem( hList1, &lvi );
}

void CNodeSetDlg::SetSubItem(HWND hList1, int nItem, int nSubItem, LPCTSTR szText)
{
    LVITEM lvi1 = {0};
    lvi1.mask = LVIF_TEXT;
    lvi1.iItem = nItem;
    lvi1.iSubItem = nSubItem;
    lvi1.pszText = (LPTSTR)szText;
    ListView_SetItem(hList1,&lvi1);
}

BOOL CNodeSetDlg::OnInitDialog()
{
	Node *pNode = (Node*)m_ptrNode;
	// Set the Icon
	SetIconLarge(IDW_MAIN);
	SetIconSmall(IDW_MAIN);

	hBasicScanList = ::GetDlgItem(m_hWnd,IDC_LIST_ENTRIES);

	ListView_DeleteAllItems( hBasicScanList );

    /*******************添加列标题********************/
    TCHAR titles[][32] = {/*TEXT("名称"), */TEXT("类型"), TEXT("项值")};

	ListView_SetExtendedListViewStyle(hBasicScanList, LVS_SHOWSELALWAYS | LVS_REPORT | LVS_OWNERDRAWFIXED|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);


	LV_COLUMN lvColumn = {0};
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_IMAGE;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;

    int colNum = sizeof(titles)/sizeof(titles[0]); //列数
    for(int i = 0; i < colNum; i++)
    {
        lvColumn.pszText = titles[i];
        ListView_InsertColumn(hBasicScanList, i, &lvColumn);
    }
	if (!pNode)
	{
		return true;
	}
	SetDlgItemText(IDC_EDIT_NODENAME, pNode->getNodeName());
	int rowNum = pNode->getNodeChilds().size();
	for(   long   i   =   0;   i   <  rowNum;   i++   )   
	{
		Node *pSubNode = ((Node*)(pNode->getNodeChilds().at(i)));
		if (!pSubNode->getIsLeaf())
		{
			break;
		}

		
		Config *pCfg = pSubNode->getCfg();
		//int item = AddItem(hBasicScanList, pSubNode->getNodeName(), 3, pCfg); //添加行

		CString strType = CString("");
		switch(pCfg->getCfgType())
		{
			case TYPE_REG:
				strType = CString("注册表键");
				break;
			case TYPE_FILE:
				strType = CString("文件");
				break;
			case TYPE_DIR:
				strType = CString("目录");
				break;
			case TYPE_DISK:
				strType = CString("磁盘空间");
				break;
			case TYPE_FILE_TYPE:
				strType = CString("文件类型");
				break;
			case TYPE_REG_VAL:
				strType = CString("注册表值");
				break;
			default:
				strType = CString("未知类型");
				break;
		}
		int item = AddItem(hBasicScanList, strType, 3, pSubNode); //添加行
		SetSubItem(hBasicScanList, item, 1, pCfg->getCfgValue());
		//SetSubItem(hBasicScanList, item, 2, pCfg->getCfgValue());
			
	}   
	return true;
}

void CNodeSetDlg::OnOK()
{
	//::MessageBox(NULL, _T("OK Button Pressed.  Program will exit now."), _T("Button"), MB_OK);
	CDialog::OnOK();
}

void CNodeSetDlg::OnButton()
{
	//SetDlgItemText(IDC_STATIC3, _T("Button Pressed"));
	TRACE(_T("Button Pressed\n"));
}

void CNodeSetDlg::OnCheck1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 1"));
	TRACE(_T("Check Box 1\n"));
}

void CNodeSetDlg::OnCheck2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 2"));
	TRACE(_T("Check Box 2\n"));
}

void CNodeSetDlg::OnCheck3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Check Box 3"));
	TRACE(_T("Check Box 3\n"));
}

void CNodeSetDlg::OnRadio1()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 1"));
	TRACE(_T("Radio 1\n"));
}

void CNodeSetDlg::OnRadio2()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 2"));
	TRACE(_T("Radio 2\n"));
}

void CNodeSetDlg::OnRadio3()
{
	//SetDlgItemText(IDC_STATIC3, _T("Radio 3"));
	TRACE(_T("Radio 3\n"));
}

