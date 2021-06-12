//#include "stdafx.h"

#include "afx.h"
#include "atlstr.h"

#include "cfgfile.h"

#import <msxml3.dll> //raw_interfaces_only 

using namespace MSXML2; 

vector<LegalItem*>    LegalItems;
bool   TraverseTree(   MSXML2::IXMLDOMNodePtr&   m_node )   ;
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
	_variant_t varXml("ServerCfg.xml");

	bResult   =   pDoc->load(   varXml );//   
	if(   bResult   ==   FALSE   )   
	{   
		TRACE("加载xml文件失败!\n");   
		return   1;   
	}
	TraverseTree((MSXML2::IXMLDOMNodePtr&)pDoc);//遍历

	return 0;
}

//深度优先搜索遍历算法函数 
bool   TraverseTree(   MSXML2::IXMLDOMNodePtr&   m_node)   
{   
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

		m_nodelist->get_item(   0,   &m_node   );
		if(   m_node->hasChildNodes()   )   
		{   
			hr   =   m_node->get_childNodes(   &m_nodelist   );   
			if(   !SUCCEEDED(   hr   )   )     
				return 1;   

			hr   =   m_nodelist->get_length(   &length   );   
			for(   long   i   =   0;   i   <   length;   i++   )   
			{   
				LegalItem *item = new LegalItem;

				m_nodelist->get_item(   i,   &m_node   );   
				m_node->get_attributes(&attribute);
				if (attribute)
				{
					namedItem = attribute->getNamedItem(_bstr_t("Name"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));

						item->name = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}

					namedItem = attribute->getNamedItem(_bstr_t("Department"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));
						item->dep = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}

					namedItem = attribute->getNamedItem(_bstr_t("Ipaddr"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));
						item->ipaddr = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}

					namedItem = attribute->getNamedItem(_bstr_t("Macaddr"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));
						item->macaddr = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}

					namedItem = attribute->getNamedItem(_bstr_t("Hardserial"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));
						item->harddrive = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}

					namedItem = attribute->getNamedItem(_bstr_t("Key"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));
						item->key = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}

					namedItem = attribute->getNamedItem(_bstr_t("Password"));
					if (namedItem)
					{
						str2   =   (namedItem->nodeValue);
						CString strName = (CString(LPCTSTR(_bstr_t(str2))));
						item->password = strName;
					}
					else
					{
						TRACE("Named Item parse error\n");
					}
				}

				LegalItems.push_back(item);
			}
		}
	}
	return 0;
}