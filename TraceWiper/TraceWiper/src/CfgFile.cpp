//#include "stdafx.h"
#include "afx.h"
#include "afxmt.h"
#include "WinBase.h"
#include "afxwin.h"
#include "afxcoll.h"
#include "windef.h"
#include "atlbase.h"
#include "atlcom.h"
#include "atlcomtime.h"
#include "commctrl.h"
#include "localauth.h"
#include <objbase.h>

//#include <msxml5.h>

#include "registry.h"

#import <msxml3.dll> //raw_interfaces_only 

using namespace MSXML2; 


#include "CfgFile.h"
//extern CString 	astrAuth[];
extern HWND 	g_hWndClasses;
extern HWND 	g_hWndFiles;
extern bool 	g_bStopDelDir;
ERASER_METHOD   g_eEraseMethodDisk = (ERASER_METHOD)1;
int 			g_iPassesDisk = 1;
ERASER_METHOD   g_eEraseMethodFiles = (ERASER_METHOD)1;
int 			g_iPassesFiles = 1;
EraseNode 		*pRoot;

const LPCTSTR DRIVE_ALL_LOCAL = TEXT(" :\\");
const LPCTSTR szLocalDrives   = TEXT("Local Hard Drives");
void GetLocalHardDrives(CStringArray& strDrives);
extern void findMatchingFiles(CString strSearch, CStringArray& saFiles, BOOL bSubFolders =FALSE );
extern BOOL
parseDirectory(LPCTSTR szDirectory, CStringArray& saFiles, CStringArray& saDirectories,
               BOOL bSubDirectories, LPDWORD pdwFiles =0, LPDWORD pdwDirectories = 0);
extern CString strDrive;

extern CCriticalSection critical_section_DIR;


DWORD DeleteDirectories(LPVOID lParam)
{
	CFileFind tempFind;
	CString tempFileFind;
	CString DirName = *((CString*)lParam);
	tempFileFind = DirName + CString("\\") + CString("*.*");
	//sprintf(tempFileFind,"%s\\*.*",DirName.GetBuffer());
	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind);
	

	g_bStopDelDir = false;
		
	while(IsFinded && !g_bStopDelDir)
	{
		IsFinded=(BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			CString foundFileName;
			foundFileName = tempFind.GetFileName();
			if(tempFind.IsDirectory())
			{
				CString strDir;
				strDir = DirName + CString("\\") + foundFileName;
				if (DeleteDirectories(&strDir))
				{
					continue;
				}
			}
			else
			{
				CString strDir;
				strDir = DirName + CString("\\") + foundFileName;
				if(DeleteFile(strDir))
					continue;
			}
		}
	}
	tempFind.Close();
	
	g_bStopDelDir = false;
	
	if(!RemoveDirectory(DirName))
	{
		TRACE(DirName);
	}
	return 0;
}

DWORD WINAPI  DeleteDirectories_TASK(LPVOID lParam)
{
	critical_section_DIR.Lock();
	DeleteDirectories(lParam);
	critical_section_DIR.Unlock();
	return 0;
}
bool   AddMenuContext()
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CLASSES_ROOT);
	
	reg.CreateKey("*\\shell\\痕迹清除\\command");//创建项file1
	reg.WriteString("", " C:\\Program Files\\Tencent\\QQ\\Bin\\QQ.exe");
    /*reg.SetValue(" 腾讯QQ","");      //修改数值数据为:腾讯QQ
    reg.Create(reg.m_hKey,"command");// 新建command命令项
    char ExeName[MAX_PATH]="C:\\Program Files\\Tencent\\QQ\\QQ.exe";
   
   
    reg.SetValue(ExeName,"");// 将QQ路径写入注册表
    reg.Close(); */
    return TRUE;
}

bool   DeleteMenuContext()
{
	return TRUE;
}

bool   TraverseTreeLoad(   MSXML2::IXMLDOMNodePtr&   m_node  , Node *pNode   )   ;
bool   TraverseTreeSave(MSXML2::IXMLDOMDocumentPtr& pDoc, MSXML2::IXMLDOMElementPtr& xmlRoot, Node *pNode);
//深度优先搜索遍历算法函数 
bool   LoadCfgFile()  //输入参数为当前操作结点
{  

	//1、初始化COM   环境。采用MSXML进行XML文件的解析。
	CoInitialize(NULL);     
	HRESULT   hr;   
	CString   str;   
	MSXML2::IXMLDOMDocumentPtr   pDoc;   
	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
           IID_IXMLDOMDocument, (void**)&pDoc);
	//hr   = pDoc.CreateInstance(   __uuidof(   MSXML2::DOMDocument   )   );     
	if(   !SUCCEEDED(   hr   )   )   
	{   
		TRACE("无法创建DOMDocument对象，请检查是否安装了MS   XML   Parser   运行库!");   
		return   1;   
	}   
	pDoc->put_async(   false   );   
	BOOL   bResult;   

	//2、载入配置文件：eraseCfg.xml。配置文件和工具软件在同一个目录。
	VARIANT_BOOL success = 0;
	_variant_t varXml("curCfg.xml");

	bResult   =   pDoc->load(   varXml );//   
	if(   bResult   ==   FALSE   )   
	{   
		TRACE("加载xml文件失败!");   
		return   1;   
	}   
	pRoot = new EraseNode;
	TraverseTreeLoad((MSXML2::IXMLDOMNodePtr&)pDoc ,pRoot);//遍历  

	return 0;
}

bool SaveConfig()
{
	CString m_strId,m_strAuthor, m_strTitle;
	MSXML2::IXMLDOMDocumentPtr pDoc;
	
		//创建DOMDocument对象
	HRESULT hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
           IID_IXMLDOMDocument, (void**)&pDoc);
	if(!SUCCEEDED(hr))
	{  
	  TRACE("无法创建DOMDocument对象，请检查是否安装了MS   XML   Parser   运行库!");   
	  return false;
	}

	MSXML2::IXMLDOMElementPtr xmlNode;// = (MSXML2::IXMLDOMElementPtr)m_node;
	MSXML2::IXMLDOMElementPtr  xmlRoot ;
	//根节点的名称为Book
	//创建元素并添加到文档中
	//pDoc->appendChild(pDoc->createTextNode( "\n")); 
	Node *pNodeNow = pRoot->getNodeChilds().at(0);
	xmlNode=pDoc->createElement((_bstr_t)"Node");
	xmlNode->setAttribute("Name",(const char *)(pNodeNow->getNodeName().GetBuffer()));//设置属性
	CString strChecked;
	strChecked.Format("%d", pNodeNow->getItemCheck());
	xmlNode->setAttribute("Checked",(const char *)(strChecked.GetBuffer()));
	pDoc->appendChild(xmlNode);

	for (int i = 0; i < pNodeNow->getNodeChilds().size(); i++)
	{
		TraverseTreeSave(pDoc, xmlNode, pNodeNow->getNodeChilds().at(i));
	}
	pDoc->save("curCfg.xml"); 

	return true;
}
bool   TraverseTreeSave(MSXML2::IXMLDOMDocumentPtr& pDoc, MSXML2::IXMLDOMElementPtr& xmlRoot, Node *pNode)   
{
	bool isLeaf = pNode->getIsLeaf();
	int  iLevel = pNode->getLevel();
	ItemCheck itemChk = pNode->getItemCheck();
	vector<Node*> vecNodes = pNode->getNodeChilds();
	CString strNodeName = pNode->getNodeName();
	Config * pCfg = pNode->getCfg();

	CString strTabsStart = CString("\r\n    ");
	CString strTabsEnd = CString("\r\n");

	for (int i = 0; i < iLevel-2; i++)
	{
		strTabsStart = strTabsStart + CString("    ");
		strTabsEnd = strTabsEnd + CString("    ");
	}

	MSXML2::IXMLDOMElementPtr xmlNode;
	
	//xmlRoot->appendChild(pDoc->createTextNode( strTabsStart.GetBuffer())); 
	if (isLeaf && pCfg)
	{
		switch(pCfg->getCfgType())
		{
			case TYPE_REG:
				xmlNode=pDoc->createElement((_bstr_t)"DelReg");
				break;
			case TYPE_FILE:
				xmlNode=pDoc->createElement((_bstr_t)"DelFile");
				break;
			case TYPE_DIR:
				xmlNode=pDoc->createElement((_bstr_t)"DelDir");
				break;
			case TYPE_DISK:
				xmlNode=pDoc->createElement((_bstr_t)"DelDisk");
				break;
			case TYPE_FILE_TYPE:
				xmlNode=pDoc->createElement((_bstr_t)"DelFileType");
				break;
			case TYPE_REG_VAL:
				xmlNode=pDoc->createElement((_bstr_t)"DelRegVal");
				break;
			default:
				xmlNode=pDoc->createElement((_bstr_t)"Unknown");
				break;
		}
		xmlRoot->appendChild(pDoc->createTextNode( strTabsStart.GetBuffer())); 
		xmlNode->setAttribute("Name",(const char *)(pCfg->getCfgValue().GetBuffer()));
		xmlRoot->appendChild(xmlNode);
		if (pNode->getNodeSuper()->getNodeChilds().at(pNode->getNodeSuper()->getNodeChilds().size() - 1) == pNode)
			xmlRoot->appendChild(pDoc->createTextNode( strTabsEnd.GetBuffer())); 
	}
	else
	{
		xmlRoot->appendChild(pDoc->createTextNode( strTabsStart.GetBuffer())); 
		xmlNode=pDoc->createElement((_bstr_t)"Node");
		xmlNode->setAttribute("Name",(const char *)(strNodeName.GetBuffer()));
		CString strChecked;
		strChecked.Format("%d", itemChk);
		xmlNode->setAttribute("Checked",(const char *)(strChecked.GetBuffer()));
		xmlRoot->appendChild(xmlNode);
		xmlRoot->appendChild(pDoc->createTextNode( strTabsEnd.GetBuffer())); 
	}
	
	
	for (int i = 0; i < vecNodes.size(); i++)
	{
		TraverseTreeSave(pDoc, xmlNode, vecNodes.at(i));
	}
	return true;
}

//深度优先搜索遍历算法函数 
bool   TraverseTreeLoad(   MSXML2::IXMLDOMNodePtr&   m_node , Node *pNode  )   
{   
	Node *pChildNode = 0;
	CString   str;    
	CString   strstr;    
	_variant_t str2;
	HRESULT   hr;   
	MSXML2::IXMLDOMNodeListPtr   	m_nodelist;   
	MSXML2::IXMLDOMNodePtr 			namedItem;
	MSXML2::IXMLDOMNodePtr 			CheckedItem;
	MSXML2::DOMNodeType str1   =   (m_node->nodeType);   
	long   length   =   0;   
	MSXML2::IXMLDOMNamedNodeMap *attribute;
	//1、如果该结点有子结点，则便历所有的子结点；否则，将子结点配置信息读到内存中，作为工具清除的输入；
	VARIANT_BOOL success = 0;
	if(   m_node->hasChildNodes()   )   
	{   
		hr   =   m_node->get_childNodes(   &m_nodelist   );   
		if(   !SUCCEEDED(   hr   )   )     
			return 1;   
		hr   =   m_nodelist->get_length(   &length   );   

		str   =   (const wchar_t *)(m_node->nodeName);   
		//TRACE(str);
		//TRACE("\r");

		

		m_node->get_attributes(&attribute);

		if (attribute)
		{
			CString strCheck;

			namedItem = attribute->getNamedItem(_bstr_t("Name"));
			if (namedItem)
			{
				str2   =   (namedItem->nodeValue);
			}
			else
			{
				TRACE("Named Item parse error\n");
			}

			CheckedItem = attribute->getNamedItem(_bstr_t("Checked"));

			if (CheckedItem)
			{
				strCheck = CString(LPCTSTR(_bstr_t(CheckedItem->nodeValue)));
			}
			else
			{
				TRACE("Checked Item parse error\n");
			}

			((Node*)pNode)->setNodeName(CString(LPCTSTR(_bstr_t(str2))));
			pNode->setItemCheck((ItemCheck)atoi(strCheck));
			((Node*)pNode)->setNodeIsLeaf(0);
		}

		//便历所有子结点，递归调用便历当前子结点所对应的子树；
		for(   long   i   =   0;   i   <   length;   i++   )   
		{   
			//vector<Node> childs;
			pChildNode = new EraseNode;
			Config *pCfg = new Config();
			((Node*)pChildNode)->setNodeSuper(pNode);
			((Node*)pChildNode)->setLevel(((Node*)pNode)->getLevel() + 1);
			((Node*)pChildNode)->setCfg(pCfg);

			m_nodelist->get_item(   i,   &m_node   );   
			TraverseTreeLoad (m_node, pChildNode);  
			//childs = pNode->getNodeChilds();
			(((EraseNode*)pNode)->getNodeChilds()).push_back(pChildNode);
		}   
	}   
	else   
	{   
		m_node->get_attributes(&attribute);

		if (attribute)
		{
			Config *pCfg = ((EraseNode*)pNode)->getCfg();
			//如果该结点为叶子结点，则载入结点配置信息，作为软件清除的依据；
			//str   =   (const wchar_t *)(m_node->xml);   
			str   =   (const wchar_t *)(m_node->nodeName);   
			if (!str.CompareNoCase("DelDir"))
			{
				pCfg->setCfgType(TYPE_DIR);
			}
			else if (!str.CompareNoCase("DelReg"))
			{
				pCfg->setCfgType(TYPE_REG);
			}
			else if (!str.CompareNoCase("DelRegVal"))
			{
				pCfg->setCfgType(TYPE_REG_VAL);
			}
			else if (!str.CompareNoCase("DelFile"))
			{
				pCfg->setCfgType(TYPE_FILE);
			}
			else if (!str.CompareNoCase("DelFileType"))
			{
				pCfg->setCfgType(TYPE_FILE_TYPE);
			}
			else if (!str.CompareNoCase("DelDisk"))
			{
				pCfg->setCfgType(TYPE_DISK);
			}
			else
			{
				
			}
			
			namedItem = attribute->getNamedItem(_bstr_t("Name"));
			if (namedItem)
			{
				pCfg->setCfgValue(CString(LPCTSTR(_bstr_t(namedItem->nodeValue))));
			
				((Node*)pNode)->setNodeName(CString(LPCTSTR(_bstr_t(namedItem->nodeValue))));
				((Node*)pNode)->setNodeIsLeaf(1);
			}
			else
			{
				TRACE("Named Item parse error. 1\n");
			}
		}
	}   

	return 0;
}  

bool Config::EraseAction()
{
    BOOL bReturn = FALSE;

    if (m_saDrives.size() == 0 || m_saFiles.size() > 0)
    {
        // we either have files to erase, or we have nothing
        // to erase...

        if (m_bResultsForFiles)
            m_bShowResults = TRUE;

        VERIFY(eraserOK(eraserSetDataType(m_ehContext, ERASER_DATA_FILES)));

        int iSize = m_saFiles.size();
        for (int i = 0; i < iSize; i++)
        {
            VERIFY(eraserOK(eraserAddItem(m_ehContext,
                (LPVOID)(LPCTSTR)m_saFiles[i], (E_UINT16)m_saFiles[i].GetLength())));
        }
        m_saFiles.clear();

        bReturn = eraserOK(eraserStart(m_ehContext));
    }
    else if (m_saDrives.size() > 0)
    {
        if (m_bResultsForUnusedSpace)
            m_bShowResults = TRUE;

        VERIFY(eraserOK(eraserSetDataType(m_ehContext, ERASER_DATA_DRIVES)));

        int iSize = m_saDrives.size();
        for (int i = 0; i < iSize; i++)
        {
            VERIFY(eraserOK(eraserAddItem(m_ehContext,
                (LPVOID)(LPCTSTR)m_saDrives[i], (E_UINT16)m_saDrives[i].GetLength())));
        }
        m_saDrives.clear();

        bReturn = eraserOK(eraserStart(m_ehContext));
    }

    return bReturn;
}

bool Config::eraseEnd()
{
	switch (cfgType)
	{
	case TYPE_FILE:
	case TYPE_DIR:
	case TYPE_DISK:
	case TYPE_FILE_TYPE:
		eraserOK(eraserStop(m_ehContext));
	    m_ehContext = ERASER_INVALID_CONTEXT;
		break;
	default:
		break;
	}
	return 0;
}

bool Config::eraseDestroy()
{
	switch (cfgType)
	{
	case TYPE_FILE:
	case TYPE_DIR:
	case TYPE_DISK:
	case TYPE_FILE_TYPE:
		eraserDestroyContext(m_ehContext);
	    m_ehContext = ERASER_INVALID_CONTEXT;
		break;
	default:
		break;
	}
	return 0;
}


bool Config::erase()
{
	CRegistry reg;
	
    CString strData;

	TRACE(cfgValue);
	
	if (cfgValue.GetLength() <= 0)
		return false;

	strData = cfgValue;


	switch (cfgType){
	case TYPE_REG:
	case TYPE_REG_VAL:
		break;
	case TYPE_FILE:		
	case TYPE_DIR:
	case TYPE_FILE_TYPE:
		{
			if (eraserError(eraserIsValidContext(m_ehContext)))
		    {
		        //if (eraserError(eraserCreateContext(&m_ehContext)))
				if (eraserError(eraserCreateContextEx(&m_ehContext, (const ERASER_METHOD)g_eEraseMethodFiles, g_iPassesFiles, 0)))
		            return FALSE;
		    }

		    // set notification window & message
			VERIFY(eraserOK(eraserSetWindow(m_ehContext, g_hWndFiles)));
		    VERIFY(eraserOK(eraserSetWindowMessage(m_ehContext, WM_ERASERNOTIFY)));

		    // clear possible previous items
		    VERIFY(eraserOK(eraserClearItems(m_ehContext)));
			break;
		}
	case TYPE_DISK:		
		{
			if (eraserError(eraserIsValidContext(m_ehContext)))
		    {
		        //if (eraserError(eraserCreateContext(&m_ehContext)))
				if (eraserError(eraserCreateContextEx(&m_ehContext, (const ERASER_METHOD)g_eEraseMethodDisk, g_iPassesDisk, 0)))
		            return FALSE;
		    }

		    // set notification window & message
			VERIFY(eraserOK(eraserSetWindow(m_ehContext, g_hWndFiles)));
		    VERIFY(eraserOK(eraserSetWindowMessage(m_ehContext, WM_ERASERNOTIFY)));

		    // clear possible previous items
		    VERIFY(eraserOK(eraserClearItems(m_ehContext)));
		}
		break;
		
	default:
		TRACE("No such type of entry...");
		break;
	}

	
	switch (cfgType){
	case TYPE_REG:
		reg.DeleteKey(cfgValue);
		PostMessage(g_hWndFiles, WM_REGNOTIFY, 0, (LPARAM)(&cfgValue));
		break;

	case TYPE_REG_VAL:
		{
			reg.DeleteValue(cfgValue);
			PostMessage(g_hWndFiles, WM_REGNOTIFY, 1, (LPARAM)(&cfgValue));
			break;
		}
		
	case TYPE_FILE:
		if (UseWildcards())
        {
        	CStringArray saFiles;
			for (int aa = 0; aa < m_saFiles.size(); aa++)
				saFiles.InsertAt(aa, m_saFiles.at(aa));
            findMatchingFiles(strData, saFiles,
                              WildcardsInSubfolders());
        }
        else
        {
            m_saFiles.push_back(strData);
        }
        return EraseAction();
		
	case TYPE_DIR:
		{
            CStringArray saFolders;
			CStringArray saFiles;
			
            parseDirectory((LPCTSTR)strData,
                           saFiles,
                           saFolders,
                           Subfolders());

			
			for (int aa = 0; aa < saFiles.GetCount(); aa++)
				m_saFiles.push_back(saFiles.GetAt(aa));
			
            if (RemoveFolder())
            {
                if (OnlySubfolders())
                {
                    // remove the last folder from the list
                    // since the user does not want to remove
                    // it

                    if (saFolders.GetSize() > 0)
                        saFolders.SetSize(saFolders.GetSize() - 1);
                }
				for (int aa = 0; aa < saFolders.GetCount(); aa++)
	                m_saFolders.push_back(saFolders[aa]);
            }
        }
		return EraseAction();
		
	case TYPE_DISK:
		if (strData == DRIVE_ALL_LOCAL)
		{
			CStringArray saFiles;
			for (int aa = 0; aa < m_saDrives.size(); aa++)
				saFiles.InsertAt(aa, m_saDrives.at(aa));
            GetLocalHardDrives(saFiles);
		}
        else
            m_saDrives.push_back(strData);

		return EraseAction();
		
	case TYPE_FILE_TYPE:
		return EraseAction();
		
	default:
		TRACE("No such type of entry...");
		return EraseAction();
	}

	return 0;
}

EraseNode::EraseNode() : Node()
{
}


bool EraseNode::addConfig(Config* pCfg)
{
	
	return 0;
}

bool EraseNode::delConfig(Config* pCfg)
{
	return 0;
}
void EraseNode::addNode(Node *pNode)
{
	//EraseNode *pNode = new EraseNode;
	pNode->setNodeSuper(this);
	getNodeChilds().push_back(pNode);
	return;
}

Node* EraseNode::addNode()
{
	EraseNode *pNode = new EraseNode;
	(Node*)pNode->setNodeSuper(this);
	pNode->setLevel(getLevel()+1);
	pNode->setItemCheck(getItemCheck());
	pNode->setIsLeaf(getIsLeaf());
	setIsLeaf(0);
	getNodeChilds().push_back(pNode);
	return pNode;
}

bool EraseNode::delNode()
{
	vector<Node*>::iterator itr;

	for (int i = 0; i < getNodeChilds().size(); i++)
	{
		getNodeChilds().at(i)->delNode();
	}
	
	itr = (getNodeSuper()->getNodeChilds()).begin();
	while (itr != (getNodeSuper()->getNodeChilds()).end())
	{
		if (*itr == this)
		{
			TRACE(getNodeName());
			(getNodeSuper()->getNodeChilds()).erase(itr);//删除值为3的元素
			break;
		}
		++itr;
	}

	if (getNodeSuper()->getNodeChilds().size() == 0)
	{
		getNodeSuper()->setIsLeaf(1);
	}
	return 0;
}

bool EraseNode::erase()
{
	Node* pNodeTmp = 0;
	int i = 0;
	
	if (getIsLeaf())
	{
		getCfg()->erase();
	}
	else
	{
		if (getCfg())//非叶子节点也可能有配置
		{
			getCfg()->erase();
		}
		
		for (i = 0; i < getNodeChilds().size(); i++)
		{
			pNodeTmp = getNodeChilds().at(i);
			if (pNodeTmp)
				((Node*)pNodeTmp)->erase();
		}
	}
	
	return 0;
}
bool EraseNode::eraserEnd()
{
	Node* pNodeTmp = 0;
	int i = 0;
	
	if (getIsLeaf())
	{
		getCfg()->eraseEnd();
	}
	else
	{
		for (i = 0; i < getNodeChilds().size(); i++)
		{
			pNodeTmp = getNodeChilds().at(i);
			((Node*)pNodeTmp)->eraserEnd();
		}
	}
	
	return 0;
}
bool EraseNode::eraserDestroy()
{
	Node* pNodeTmp = 0;
	int i = 0;
	
	if (getIsLeaf())
	{
		getCfg()->eraseDestroy();
	}
	else
	{
		for (i = 0; i < getNodeChilds().size(); i++)
		{
			pNodeTmp = getNodeChilds().at(i);
			((Node*)pNodeTmp)->eraserEnd();
		}
	}
	
	return 0;
}

