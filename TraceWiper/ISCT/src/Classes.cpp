///////////////////////////////////////////////////
// Classes.cpp - Definitions for the CViewClasses, CContainClasses
//               and CDockClasses classes


#include "stdafx.h"
#include "Classes.h"
#include "resource.h"
#include "ContainerApp.h"

#include "resrc1.h"
#include "../../platform/CSector.h"
#include "CfgFile.h"
#include "../../platform/FileOps.h"
///////////////////////////////////////////////
// CViewClasses functions
HWND g_hWndClasses;
extern HWND g_hWndFiles;

DWORD g_totalSelectedNodes = 0;

CViewClasses::CViewClasses() : m_himlNormal(0)
{
}

CViewClasses::~CViewClasses()
{
    if (IsWindow()) DeleteAllItems();
    ImageList_Destroy(m_himlNormal);
}



extern bool   DetectorTreeBuild();

extern HINSTANCE g_hInstance;
void CViewClasses::OnInitialUpdate()
{
    //set the image lists
    m_himlNormal = ImageList_Create(16, 15, ILC_COLOR32 | ILC_MASK, 1, 0);
    HBITMAP hbm = LoadBitmap(MAKEINTRESOURCE(IDB_CLASSVIEW));
    ImageList_AddMasked(m_himlNormal, hbm, RGB(255, 0, 0));
    SetImageList(m_himlNormal, TVSIL_NORMAL);
    ::DeleteObject(hbm);

    // Adjust style to show lines and [+] button
    DWORD dwStyle = (DWORD)GetWindowLongPtr(GWL_STYLE);
    dwStyle |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_CHECKBOXES;
    SetWindowLongPtr(GWL_STYLE, dwStyle);

    DeleteAllItems();
    
    //LoadCfgFile();//Initialize the nodes.

    // Add some tree-view items
    //BuildTree((Node*)(pRoot->getNodeChilds().at(0)));
    if (!FileOps::getInstance()->isFileExists(DETECT_CFG_FILE))
    {
        MessageBox(_T("XML�ļ�����ȷ�򲻴���"), _T("�����˳�"), MB_ICONWARNING);
        exit(0);
    }
        
    DetectorTreeBuild();
    BuildTree((Node*)(pDetectorRoot));
//    BuildTree((Node*)(pDetectorRoot->getNodeChilds().at(0)));

    m_himlState       = ImageList_Create(13, 13, ILC_COLOR32 | ILC_MASK, 1, 0);
    HBITMAP hbmState = LoadBitmap(MAKEINTRESOURCE(IDB_THREESTATE));
    ImageList_AddMasked(m_himlState, hbmState, RGB(255, 255, 255));
    SetImageList(m_himlState, TVSIL_STATE);
    ::DeleteObject(hbmState);

    g_hWndClasses = m_hWnd;
    //((EraseNode*)(pRoot->getNodeChilds().at(0)))->erase();
    //BuildTree(pRoot);


    
    /*
    HTREEITEM htiRoot = AddItem(NULL, _T("�����ʹ�úۼ����"), 0);

    HTREEITEM htiCTreeViewApp = AddItem(htiRoot, _T("Windows���кۼ�"), 0);
    AddItem(htiCTreeViewApp, _T("��ʼ/���кۼ�"), 0);$
    AddItem(htiCTreeViewApp, _T("��ʼ/�����ۼ�"), 0);
    AddItem(htiCTreeViewApp, _T("��ʼ/����������ۼ�"), 0);
    AddItem(htiCTreeViewApp, _T("��ʼ/�ĵ��ۼ�"), 0);
    AddItem(htiCTreeViewApp, _T("windows���ʹ���ļ��б�"), 0);
    AddItem(htiCTreeViewApp, _T("������������ĺۼ�"), 0);
    AddItem(htiCTreeViewApp, _T("���г����������ʱ�ļ�"), 0);
    AddItem(htiCTreeViewApp, _T("windows�¼���Ӧ�ó�����־"), 0);
    AddItem(htiCTreeViewApp, _T("xpԤ���ļ��ۼ�"), 0);
    AddItem(htiCTreeViewApp, _T("regedit�����ۼ�"), 0);

    HTREEITEM htiMainFrame = AddItem(htiRoot, _T("������ۼ�"), 0);
    HTREEITEM htiMainFrameIE = AddItem(htiMainFrame, _T("IE������ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE��ַ���ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE���������ҳ�ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE�����cookies�ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE�������ʱ�ļ��ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE������Զ���ɱ��ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE������Զ��������ۼ�"), 0);
    AddItem(htiMainFrameIE, _T("IE���������������վ��ۼ�"), 0);
    HTREEITEM htiMainFrameFireFox = AddItem(htiMainFrame, _T("firefox�ۼ�"), 0);
    AddItem(htiMainFrameFireFox, _T("FireFox���������ҳ�ۼ�"), 0);
    AddItem(htiMainFrameFireFox, _T("FireFox�����cookies�ۼ�"), 0);
    AddItem(htiMainFrameFireFox, _T("FireFox�������ʱ�ļ��ۼ�"), 0);
    AddItem(htiMainFrameFireFox, _T("FireFox����������ļ��ۼ�"), 0);
    AddItem(htiMainFrameFireFox, _T("FireFox������Զ��������ۼ�"), 0);

    HTREEITEM htiView = AddItem(htiRoot, _T("�ִ�������ۼ�"), 0);
    AddItem(htiView, _T("Windowsд�ְ�ۼ�"), 0);
    AddItem(htiView, _T("officeʹ�úۼ�"), 0);
    AddItem(htiView, _T("Acrobat reader�ۼ�"), 0);
    AddItem(htiView, _T("ultraedit�ۼ�"), 0);

    HTREEITEM htiView1 = AddItem(htiRoot, _T("ͼ�δ�������ۼ�"), 0);
    AddItem(htiView1, _T("windows������ͼʹ�úۼ�"), 0);
    AddItem(htiView1, _T("ACDSee�ۼ�"), 0);
    AddItem(htiView1, _T("PhotoShop�ۼ�"), 0);


    HTREEITEM htiView2 = AddItem(htiRoot, _T("ý�岥������ۼ�"), 0);
    AddItem(htiView2, _T("Windows media player�ۼ�"), 0);
    AddItem(htiView2, _T("Quik Time�ۼ�"), 0);
    AddItem(htiView2, _T("ReadPlayer�ۼ�"), 0);
    AddItem(htiView2, _T("winamp�ۼ�"), 0);
    AddItem(htiView2, _T("gom�ۼ�"), 0);

    HTREEITEM htiView3 = AddItem(htiRoot, _T("ѹ����������ۼ�"), 0);
    AddItem(htiView3, _T("winrar�ۼ�"), 0);
    AddItem(htiView3, _T("winzip�ۼ�"), 0);
    AddItem(htiView3, _T("7zip�ۼ�"), 0);

    HTREEITEM htiView4 = AddItem(htiRoot, _T("ɱ���ͷ���ǽ����ۼ�"), 0);
    AddItem(htiView4, _T("ŵ�ٺۼ�"), 0);
    AddItem(htiView4, _T("antivir�ۼ�"), 0);
    AddItem(htiView4, _T("Zone alarm�ۼ�"), 0);

    HTREEITEM htiView5 = AddItem(htiRoot, _T("��������ۼ�"), 0);
    AddItem(htiView5, _T("Net Transport�ۼ�"), 0);
    AddItem(htiView5, _T("FlashGet�ۼ�"), 0);
    AddItem(htiView5, _T("NetAnts�ۼ�"), 0);
    AddItem(htiView5, _T("Internet Download Manager�ۼ�"), 0);

    HTREEITEM htiView6 = AddItem(htiRoot, _T("���̿�¼����ۼ�"), 0);
    AddItem(htiView6, _T("Nero�ۼ�"), 0);
    AddItem(htiView6, _T("Alcohol�ۼ�"), 0);
    AddItem(htiView6, _T("Easy CD Creator�ۼ�"), 0);
    AddItem(htiView6, _T("UltraISO�ۼ�"), 0);

    HTREEITEM htiView7 = AddItem(htiRoot, _T("����վ����"), 0);

    HTREEITEM htiView8 = AddItem(htiRoot, _T("Windowsϵͳ��ԭ�ۼ�"), 0);

    HTREEITEM htiView9 = AddItem(htiRoot, _T("Windows���ز��ͼ��ۼ�"), 0);

    HTREEITEM htiViewa = AddItem(htiRoot, _T("�����ļ�Ŀ¼���"), 0);

    HTREEITEM htiViewb = AddItem(htiRoot, _T("����ȫ�������"), 0);

    HTREEITEM htiViewc = AddItem(htiRoot, _T("���������������"), 0);

    HTREEITEM htiViewd = AddItem(htiRoot, _T("���������������"), 0);

    // Expand some tree-view items
    Expand(htiRoot, TVE_EXPAND);
    Expand(htiCTreeViewApp, TVE_EXPAND);
    Expand(htiMainFrame, TVE_EXPAND);
    Expand(htiView, TVE_EXPAND);
    Expand(htiView1, TVE_EXPAND);
    */

    /*Expand(htiView2, TVE_EXPAND);
    Expand(htiView3, TVE_EXPAND);
    Expand(htiView4, TVE_EXPAND);
    Expand(htiView5, TVE_EXPAND);
    Expand(htiView6, TVE_EXPAND);
    Expand(htiView7, TVE_EXPAND);
    Expand(htiView8, TVE_EXPAND);
    Expand(htiView9, TVE_EXPAND);
    Expand(htiViewa, TVE_EXPAND);
    Expand(htiViewb, TVE_EXPAND);
    Expand(htiViewc, TVE_EXPAND);
    Expand(htiViewd, TVE_EXPAND);*/

}


void CViewClasses::BuildTree(Node *pRoot)
{
    CString str = CString("");
    
    if (pRoot->getNodeSuper() && !(pRoot->getIsLeaf()))
    {
        pRoot->htiCTreeItem = AddItem(pRoot->getNodeSuper()->htiCTreeItem, pRoot->getNodeName(), 1, pRoot);
        if (pRoot->getLevel() != 3)
        {
            Expand(pRoot->getNodeSuper()->htiCTreeItem, TVE_EXPAND);  /*expand the tree to the 3rd level*/
        }
    }
    else if (!(pRoot->getIsLeaf()))
    {
        pRoot->htiCTreeItem = AddItem(NULL, pRoot->getNodeName(), 1, pRoot);
    }
    else
    {
        pRoot->htiCTreeItem = 0;
    }
    
    str.Format("%x", pRoot->htiCTreeItem);
    TRACE(str);

    //���������ӽ�㣬�ݹ���ñ�����ǰ�ӽ������Ӧ��������
    for(   long   i   =   0;   i   <   pRoot->getNodeChilds().size();   i++   )   
    {   
        BuildTree((Node*)(pRoot->getNodeChilds().at(i)));
    }   
}

HTREEITEM CViewClasses::AddItem(HTREEITEM hParent, LPCTSTR szText, int iImage, Node *pNode)
{
    TVITEM tvi = {0};
    tvi.mask = TVIF_STATE|TVIF_HANDLE | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tvi.iImage = iImage;
    tvi.iSelectedImage = iImage;
    tvi.pszText = (LPTSTR)szText;
    tvi.lParam = (LPARAM)pNode;
    tvi.stateMask = TVIS_STATEIMAGEMASK;
    tvi.state = INDEXTOSTATEIMAGEMASK(pNode->getItemCheck()+1);
    TVINSERTSTRUCT tvis = {0};
    tvis.hParent = hParent;
    tvis.item = tvi;

    return InsertItem(tvis);
}

bool CViewClasses::TravelChild(HTREEITEM hItem, ItemCheck check)
{
    //First Child
    HTREEITEM hChild = GetChild(hItem);

    

    TVITEM tvItem = {0};
    tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;
    if(!GetItem(tvItem))
        return FALSE;
    
    Node* pNode = (Node *)tvItem.lParam;
    if (NULL != pNode)
    {
        ItemCheck oldCheck = pNode->getItemCheck();
        if (oldCheck != check)
        {
            pNode->setItemCheck(check);
            if (pNode->getLevel() == 3)
            {
                if (CHECKED == check)
                {
                    g_totalSelectedNodes++;
                }
                else if (UNCHECKED == check)
                {
                    g_totalSelectedNodes--;
                }
            }
        }
    }
    
    tvItem.state = INDEXTOSTATEIMAGEMASK(check+1);
    SetItem(tvItem);

    //SelectItem(hItem);
    while (hChild != NULL)
    {
        TravelChild(hChild, check);

        //Next Sibling
        hChild = GetNextSibling(hChild);
    }
    return TRUE;
}

void CViewClasses::setItemState(HTREEITEM hItem, ItemCheck nState)
{
    TVITEM tvItem = {0};
    tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
    tvItem.hItem = hItem;
    tvItem.stateMask = TVIS_STATEIMAGEMASK;
    if(!GetItem(tvItem))
    {
        return;
    }
    tvItem.state = INDEXTOSTATEIMAGEMASK(nState + 1);
    SetItem(tvItem);
}
//check this item by children  
void CViewClasses::TraverseItemChild(HTREEITEM hItem)
{
    HTREEITEM hChild = GetChild(hItem);
    if(hChild == NULL)
    {
        return;
    }
    
    ItemCheck ChildState = GetItemState( hChild, TVIS_STATEIMAGEMASK);
    bool lChildStateNotSame = false;

    //check next child
    HTREEITEM hNextChild = GetNextSibling(hChild);
    while(hNextChild != NULL)
    {        
        ItemCheck NextChildState = GetItemState( hNextChild, TVIS_STATEIMAGEMASK);

        if(NextChildState != ChildState)
        {
            lChildStateNotSame = true;
            break;
        }
        hNextChild=GetNextSibling(hNextChild);
    }

    if(lChildStateNotSame)
    {
        setItemState(hItem, HALFCHECKED);//thrid state
    }
    else
    {
        if(ChildState == CHECKED)
        {
            setItemState(hItem, CHECKED);//all child in checked
        }
        else if(ChildState == UNCHECKED)
        {
            setItemState(hItem, UNCHECKED);//all child in unchecked
        }
        else
        {
            setItemState(hItem, HALFCHECKED);//all child in half-checked
        }
    }
}
//set all child to same state
void CViewClasses::ItemChildState(HTREEITEM hItem, ItemCheck nState)
{
    setItemState(hItem, nState);
    HTREEITEM hChild = GetChild(hItem);
    
    while(hChild != NULL)
    {
        setItemState(hChild, nState);
        ItemChildState(hChild, nState);
        
        hChild=GetNextSibling(hChild);
    }
}
void CViewClasses::TravelSiblingAndParent(HTREEITEM hItem, ItemCheck nState)
{
    HTREEITEM hNextSiblingItem,hPrevSiblingItem,hParentItem;
    
    TraverseItemChild(hItem);

    //���Ҹ��ڵ㣬û�оͽ���
    hParentItem=GetParentItem(hItem);
    if(hParentItem!=NULL)
    {
        TravelSiblingAndParent(hParentItem, nState);
#if 0
        ItemCheck nState1=nState;//���ʼֵ����ֹû���ֵܽڵ�ʱ����
        
        //���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
        hNextSiblingItem=GetNextSibling(hItem);
        while(hNextSiblingItem!=NULL)
        {
            nState1 = GetItemState( hNextSiblingItem, TVIS_STATEIMAGEMASK);
            if(nState1!=nState/* && nState1!=0*/)
            {
                break;
            }
            else
            {
                hNextSiblingItem=GetNextSibling(hNextSiblingItem);
            }
        }
        
        if(nState1==nState)
        {
            //���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
            hPrevSiblingItem=GetPrevSibling(hItem);
            while(hPrevSiblingItem!=NULL)
            {
                nState1 = GetItemState( hPrevSiblingItem, TVIS_STATEIMAGEMASK );
                if(nState1!=nState/* && nState1!=0*/)
                {
                    break;
                }
                else
                {
                    hPrevSiblingItem=GetPrevSibling(hPrevSiblingItem);
                }
            }
        }
        
        if(nState1==nState/* || nState1==UNCHECKED*/)
        {
            nState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK );
            if(nState1!=UNCHECKED)
            {
                //���״̬һ�£��򸸽ڵ��״̬�뵱ǰ�ڵ��״̬һ��
                //CTreeCtrl::SetItemState( hParentItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
                TVITEM tvItem = {0};
                tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
                tvItem.hItem = hItem;
                tvItem.stateMask = TVIS_STATEIMAGEMASK;
                if(!GetItem(tvItem))
                {
                    return;
                }
                tvItem.state = INDEXTOSTATEIMAGEMASK(nState + 1);
                SetItem(tvItem);

            }
            //�ٵݹ鴦���ڵ���ֵܽڵ���丸�ڵ�
            TravelSiblingAndParent(hParentItem,nState);
        }
        else
        {
            //״̬��һ�£���ǰ�ڵ�ĸ��ڵ㡢���ڵ�ĸ��ڵ㡭��״̬��Ϊ����̬
            hParentItem=GetParentItem(hItem);
            while(hParentItem!=NULL)
            {
                nState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK );
                if(nState1!=UNCHECKED)
                {
                    //CTreeCtrl::SetItemState( hParentItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
                    TVITEM tvItem = {0};
                    tvItem.mask = TVIF_STATE|TVIF_HANDLE|TVIF_PARAM;
                    tvItem.hItem = hItem;
                    tvItem.stateMask = TVIS_STATEIMAGEMASK;
                    if(!GetItem(tvItem))
                        return;

                    tvItem.state = INDEXTOSTATEIMAGEMASK(HALFCHECKED + 1);
                    SetItem(tvItem);

                }
                hParentItem=GetParentItem(hParentItem);
            }
        }
        #endif
    }    
}

ItemCheck CViewClasses::GetItemState(HTREEITEM hItem, UINT nStateMask) const
{
#if 0
    TVITEM item;
    item.hItem = hItem;
    item.mask = TVIF_STATE;
    item.stateMask = nStateMask;
    item.state = 0;
    ::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
    return item.state;
#endif
        ItemCheck check = UNCHECKED;
        TVITEM tvItem;
        tvItem.mask = TVIF_HANDLE | TVIF_STATE;
        tvItem.hItem = hItem;
        tvItem.stateMask = TVIS_STATEIMAGEMASK;
        if(!GetItem(tvItem))
        {
            return UNCHECKED;
        }
        return (ItemCheck)((tvItem.state >> 12) -1);
        
        if (((BOOL)(tvItem.state >> 12) -1))
        {
            check = UNCHECKED;
        }
        else
        {
            check = CHECKED;
        }

        return check;
}

LRESULT   CViewClasses::OnRButtonDown(TVHITTESTINFO  tvhti,   CPoint   point)    
{  
    return LRESULT(0);
}   

LRESULT   CViewClasses::OnLButtonDown(TVHITTESTINFO  tvhti,   CPoint   point)    
{
    TVITEM tvItem = {0};
    tvItem.mask = TVIF_PARAM;
    tvItem.hItem = tvhti.hItem;
    //Node node;
    GetItem(tvItem);
    DNode* pDNodex = (DNode*)GetItemData(tvItem.hItem);
    if(TVHT_ONITEMLABEL & tvhti.flags)
    {
        //OnLButtonDown(tvhti.hItem , ptScreen);
    }
    if(TVHT_ONITEMBUTTON & tvhti.flags)
    {
        //OnLButtonDown(tvhti.hItem , ptScreen);
    }
    if(TVHT_ONITEMSTATEICON & tvhti.flags)
    {
        ItemCheck check = UNCHECKED;
        TVITEM tvItem;
        tvItem.mask = TVIF_HANDLE | TVIF_STATE;
        tvItem.hItem = tvhti.hItem;
        tvItem.stateMask = TVIS_STATEIMAGEMASK;
        if(!GetItem(tvItem))
            return LRESULT(-1);
#if 1
        if (((BOOL)(tvItem.state >> 12) -1))
        {
            check = UNCHECKED;
        }
        else
        {
            check = CHECKED;
        }
#else
        if ((tvItem.state >> 12) == 3)
        {
            check = HALFCHECKED;
        }
        else if ((tvItem.state >> 12) == 2)
        {
            check = UNCHECKED;
        }
        else if ((tvItem.state >> 12) == 1)
        {
            check = CHECKED;
        }
#endif

        TravelChild(tvhti.hItem, check);
//        setItemState(HTREEITEM hItem, ItemCheck nState)
//        ItemChildState(tvhti.hItem, check);
        
        TravelSiblingAndParent(GetParentItem(tvhti.hItem), check);
        return LRESULT(3);
        //OnLButtonDown(tvhti.hItem , ptScreen);
    }
    return LRESULT(0);
}   

LRESULT   CViewClasses::OnLButtonDClick(TVHITTESTINFO  tvhti,   CPoint   point)    
{  
    TVITEM tvItem = {0};
    tvItem.mask = TVIF_PARAM;
    tvItem.hItem = tvhti.hItem;
    //Node node;
    GetItem(tvItem);
    DNode* pDNode = (DNode*)GetItemData(tvItem.hItem);
    if(TVHT_ONITEMLABEL & tvhti.flags)
    {
        PostMessage(g_hWndFiles, WM_COMMAND, /*ISCT_DETECT_0*/ISCT_DETECT_DISPLAYRESULT, (LPARAM)pDNode);
    }
    if(TVHT_ONITEMBUTTON & tvhti.flags)
    {
        //OnLButtonDown(tvhti.hItem , ptScreen);
    }
    CPersonListConfig::getInstance()->set_selectedType(PRLIST_INVALID);
    return LRESULT(0);
}   

LRESULT CViewClasses::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_CREATE:
        TRACE("Classes\n");
        break;


    case WM_DESTROY:
        SetImageList(NULL, LVSIL_SMALL);
        break;

    /*case WM_RBUTTONDOWN:
        {
            CString str = CString("");
            str.Format("%x\n", WM_RBUTTONDOWN);
            TRACE(str);
            break;
        }
    */
    case WM_RBUTTONDOWN:
        {
            CPoint ptScreen;
            ::GetCursorPos(&ptScreen);

            LRESULT lResult = 0;

            SetFocus();

            TVHITTESTINFO  tvhti;
            tvhti.pt = ptScreen;
            ScreenToClient(m_hWnd, &tvhti.pt);
            tvhti.flags = LVHT_NOWHERE;
            SelectItem(tvhti.hItem);
            
            return LRESULT(0);
            
            POINT stPos;
            stPos.x = LOWORD (lParam) ;
            stPos.y = HIWORD (lParam) ;

            
            CString str = CString("");
            str.Format("%x\n", WM_RBUTTONUP);
            TRACE(str);

            HMENU hMenu,hCMenu1,hCMenu2,hCMenu3;

            hMenu=CreatePopupMenu();
            hCMenu1=CreatePopupMenu();
            hCMenu2=CreatePopupMenu();
            hCMenu3=CreatePopupMenu();
            InsertMenu(hMenu,0,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu1,"File");
            InsertMenu(hMenu,1,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu2,"View");
            InsertMenu(hMenu,2,MF_BYPOSITION | MF_STRING | MF_POPUP,(UINT)hCMenu3,"Edit");
            AppendMenu(hMenu,MF_STRING,(UINT)43001,"About");
            AppendMenu(hCMenu1,MF_STRING,(UINT)43002,"�������");
            AppendMenu(hCMenu1,MF_STRING,(UINT)43003,"ɾ�����");
            AppendMenu(hCMenu1,MF_STRING,(UINT)43004,"�޸Ľ��");
            AppendMenu(hCMenu1,MF_STRING,(UINT)43005,"����");
            AppendMenu(hCMenu1,MF_STRING,(UINT)43006,"����");
            AppendMenu(hCMenu1,MF_STRING,(UINT)43007,"����");
            ClientToScreen(m_hWnd,&stPos); 
            TrackPopupMenu(hMenu, TPM_LEFTALIGN   |   TPM_RIGHTBUTTON,  stPos.x,
                stPos.y, NULL, m_hWnd, NULL);
            break;
        }

    case WM_LBUTTONDOWN:
        {
            CPoint ptScreen;
            ::GetCursorPos(&ptScreen);

            LRESULT lResult = 0;

            TVHITTESTINFO  tvhti;
            tvhti.pt = ptScreen;
            ScreenToClient(m_hWnd, &tvhti.pt);
            tvhti.flags = LVHT_NOWHERE;
            HitTest(tvhti);

            if(TVHT_ONITEM & tvhti.flags)
            {
                lResult = OnLButtonDown(tvhti , ptScreen);
            }
            if (lResult == 3)
                return LRESULT(0);
            break;
        }
    case WM_LBUTTONDBLCLK:
        {
            CPoint ptScreen;
            ::GetCursorPos(&ptScreen);

            LRESULT lResult = 0;

            TVHITTESTINFO  tvhti;
            tvhti.pt = ptScreen;
            ScreenToClient(m_hWnd, &tvhti.pt);
            tvhti.flags = LVHT_NOWHERE;
            HitTest(tvhti);

            if(TVHT_ONITEM & tvhti.flags)
            {
                //lResult = OnLButtonDown(tvhti , ptScreen);
                lResult = OnLButtonDClick(tvhti , ptScreen);
            }
            if (lResult == 3)
                return LRESULT(0);
            break;
        }

    /*case WM_LBUTTONUP:
        {
            CString str = CString("");
            str.Format("%x\n", WM_LBUTTONUP);
            TRACE(str);
            break;
        }
    */

    
    case WM_ERASERNOTIFY:
        TRACE("WM_ERASERNOTIFY\n");
        OnActionNotify(wParam,lParam);
        //TRACE(CString(itoa(wParam, 0, wParam)));
        return TRUE;
    }

    return WndProcDefault(uMsg, wParam, lParam);
}

LRESULT CViewClasses::OnActionNotify(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case ERASER_WIPE_BEGIN:
        //EraserWipeBegin();
        TRACE("ERASER_WIPE_BEGIN\n");
        break;
    case ERASER_WIPE_UPDATE:
        //EraserWipeUpdate();
        TRACE("ERASER_WIPE_UPDATE\n");
        break;
    case ERASER_WIPE_DONE:
        //EraserWipeDone();
        TRACE("ERASER_WIPE_DONE\n");
        break;
    }

    return TRUE;
}
///////////////////////////////////////////////
// CContainClasses functions
CContainClasses::CContainClasses() 
{
    SetTabText(_T("Υ����Ϣ����б�"));
    SetTabIcon(IDI_CLASSVIEW);
    SetDockCaption (_T("��������Ŀ"));
    SetView(m_ViewClasses);
}

void CContainClasses::AddCombo()
{
    int nComboWidth = 120; 
    CToolbar& TB = GetToolbar();
    if (TB.CommandToIndex(IDM_FILE_SAVE) < 0) return;

    // Adjust button width and convert to separator   
    TB.SetButtonStyle(IDM_FILE_SAVE, TBSTYLE_SEP);
    TB.SetButtonWidth(IDM_FILE_SAVE, nComboWidth);

    // Determine the size and position of the ComboBox 
    int nIndex = TB.CommandToIndex(IDM_FILE_SAVE); 
    CRect rect = TB.GetItemRect(nIndex); 

    // Create the ComboboxEx window 
    m_ComboBoxEx.Create(TB.GetHwnd());
    m_ComboBoxEx.SetWindowPos(NULL, rect, SWP_NOACTIVATE);

    // Set ComboBox Height
    m_ComboBoxEx.SendMessage(CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)(rect.Height()-6));

    m_ComboBoxEx.AddItems();
}

BOOL CContainClasses::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    // OnCommand responds to menu and and toolbar input

    switch(LOWORD(wParam))
    {
    case IDM_FILE_NEW:
        TRACE(_T("File New\n"));
        break;
    case IDM_FILE_OPEN:
        TRACE(_T("File Open\n"));
        break;
    case IDM_FILE_SAVE:
        TRACE(_T("FILE Save\n"));
        break;
    case WM_ERASERNOTIFY:
        TRACE("WM_ERASERNOTIFY CContainClasses\n");
        //TRACE(CString(itoa(wParam, 0, wParam)));
        return TRUE;
    }

    return FALSE;
}

void CContainClasses::SetupToolbar()
{
    // Set the Bitmap resource for the toolbar
    /*GetToolbar().SetImages(RGB(192,192,192), IDW_MAIN, 0, 0);

    // Set the Resource IDs for the toolbar buttons
    AddToolbarButton( IDM_FILE_NEW         );
    AddToolbarButton( IDM_FILE_OPEN, FALSE );

    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_FILE_SAVE, FALSE );

    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_EDIT_CUT         );
    AddToolbarButton( IDM_EDIT_COPY        );
    AddToolbarButton( IDM_EDIT_PASTE       );

    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_FILE_PRINT, FALSE );

    AddToolbarButton( 0 );    // Separator
    AddToolbarButton( IDM_HELP_ABOUT       );

    // Add the ComboBarEx control to the toolbar
    AddCombo();

    // Match the Container's Toolbar theme to the MainFrame's toolbar theme
    ToolbarTheme tt = GetContainerApp().GetMainFrame().GetToolbar().GetToolbarTheme();
    GetToolbar().SetToolbarTheme(tt);*/
}

LRESULT CContainClasses::OnNotifyReflect(WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);

    switch (((LPNMHDR)lParam)->code)
    {
    case TCN_SELCHANGE:
        {
            PostMessage(g_hWndFiles, WM_COMMAND, /*ISCT_DETECT_0*/ISCT_DETECT_DISPLAYRESULT, (LPARAM)NULL);
            CContainer::OnNotifyReflect(wParam, lParam);
        }
        break;
    }

    return 0L;
}
/////////////////////////////////////////////////
//  Definitions for the CDockClasses class
CDockClasses::CDockClasses() 
{ 
    SetView(m_Classes); 
}

void CDockClasses::OnInitialUpdate()
{
    // Set the width of the splitter bar
    SetBarWidth(8);
}



