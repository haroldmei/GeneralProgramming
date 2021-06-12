

#ifndef _XML_TREE_H_
#define _XML_TREE_H_

#include <iostream>
#include <sstream>

#include "string.h"
    
#include "tinyxml.h"

#include "Vector"

//#include "Afx.h"
//#include <atlstr.h>
#include "afx.h"
#include "afxwin.h"
#include "windef.h"
#include "atlbase.h"
#include "atlcom.h"
#include "atlcomtime.h"

#include "../sysGlobal.h"

using namespace std;

typedef vector<char*> NameList;
typedef vector<int> IDList;


#define TREENODE_NAME "name"
#define SUBTREENODE_NAME "name"
#define NODE_NAME "name"
#define NODE_CHECKED "checked"
#define TITLE_NAME "name"
#define TITLE_WIDTH "width"
#define NODE_ID "ID"
#define NODE_ACCESS "access"
#define NODE_KEY     "key"
#define FIELD_TITLE "title"
#define FIELD_TYPE  "type"
#define FIELD_ACTION "action"

#define NODE_EVA "evaluate"
#define NODE_CAL "calculate"
#define NODE_NOTE "note"

#define VAL_NODE "node"
#define VAL_KEY "Key"
#define VAL_TITLE "title"
#define VAL_FIELD "Field"

#if 1
// 记录的结构
typedef struct s_RESULT_ITEM
{
	std::vector<CString> vItemStr;
}RESULT_ITEM;

// 列表的结构
typedef struct s_RESULT_TABLE
{
	std::vector<CString> vHeaderStr;		// 字段属性
	std::vector<RESULT_ITEM> vTableStr;		// 记录集合

}RESULT_TABLE;

typedef struct s_TREENODE
{
	DWORD dwNodeID;
    CString sNodeName;
	std::vector<s_TREENODE> vChildNodes;
}TREENODE;
#endif

typedef std::vector<bool> BoolList;

class XmlTree
{
public :
   static XmlTree * getInstance();
   XmlTree(char * pDocName);
   ~XmlTree();

   TiXmlElement * rootElement();
   void save();
   void guiTree(s_TREENODE & pTree);
   void treeNodes(NameList *pName);
   void subTreeNodes(char * pTreeNode, NameList * pSubTreeName);
   void nodes(char * pTreeNode, char*pSubTreeNode, NameList * pName, BoolList *pCheckedList);
   void nodeSet(void * pXmlNode, bool checked, int eva, int cal, string note, string pTitle, int width);
//   TiXmlElement * nextTreeNode(TiXmlElement * pElement);
   void node(char * pTreeNode, NameList *pName, IDList *pID);
   void getNodeWeigth(string pTreeNode, string pSubTreeNode, string pName, int & eva, int &cal, string & note);
   void nodeSetAttr(string pTreeNode, string pSubTreeNode, string pName,
       bool checked, int eva, int cal, string note);
   void nodeSet(void * pXmlNode, bool checked, int eva, int cal, string note);
   FiledType fieldTypeFromStr(const char * pStr);
   Action actionTypeFromStr(const char * pStr);
   Access accessTypeFromStr(const char * pStr);
   bool nodeParse(char* pNodeName, Titles * pTitles, void**xmlNode=NULL);
   int getFreeNodeId();
   void replaceNode(void * replaceThis, void* withThis, void ** pNewNode);
   char* fieldTypeToStr(FiledType pType);
   char* actionTypeToStr(Action pAction);
   char* accessTypeToStr(Access pType);
private:
    TiXmlElement * root;
    static XmlTree * mInstancePtr;
    TiXmlDocument mDoc;
    int mNodeIdPoll;
};

void printAttribtes(TiXmlElement* pElement);
void printElement(TiXmlElement* pElement);
void printChild(TiXmlElement* pElement);

#endif //_XML_TREE_H_
