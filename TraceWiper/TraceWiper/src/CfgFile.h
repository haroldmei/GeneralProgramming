//#include "afx.h"
//#include "afxwin.h"
//#include "windef.h"
//#include "atlbase.h"
//#include "atlcom.h"
//#include "atlcomtime.h"

#include <vector>

#include "..\..\eraserlib\resource.h"
#include "..\..\eraserlib\eraserdll.h"


using namespace std;

typedef enum
{
	UNCHECKED = 0,
	CHECKED,
	HALFCHECKED
}ItemCheck;

#define TYPE_REG 0
#define TYPE_FILE 1
#define TYPE_DIR 2
#define TYPE_DISK 3
#define TYPE_FILE_TYPE 4
#define TYPE_REG_VAL 5

#define WM_REGNOTIFY (WM_USER + 20)

extern ERASER_METHOD   g_eEraseMethodDisk;
extern int 			g_iPassesDisk;

extern ERASER_METHOD   g_eEraseMethodFiles;
extern int 			g_iPassesFiles;

class Config
{
private:
	int cfgType;
	CString cfgValue;
	ERASER_HANDLE   m_ehContext;
	
	BOOL m_bShowResults;
	BOOL m_bResultsForFiles;
	BOOL m_bResultsForUnusedSpace;

    vector<CString>    m_saFiles;
    vector<CString>    m_saFolders;
    vector<CString>    m_saDrives;

	bool    m_bUseWildcards;
    bool    m_bWildCardsInSubfolders;
    bool    m_bRemoveFolder;
    bool    m_bSubfolders;
    bool    m_bRemoveOnlySub;
	
public:
	bool erase();
	bool eraseEnd();
	bool eraseDestroy();
	bool EraseAction();
	Config():m_ehContext(ERASER_INVALID_CONTEXT){cfgType = -1; cfgValue = CString("");};
	void setCfgType(int type){cfgType = type; return;};
	void setCfgValue(CString &value){cfgValue = value; return;};
	int getCfgType(){return cfgType;};
	CString& getCfgValue(){return cfgValue;};

	bool    UseWildcards()                     { return m_bUseWildcards; }
	bool    WildcardsInSubfolders()            { return m_bWildCardsInSubfolders; }
	bool    RemoveFolder() const               { return m_bRemoveFolder; }
    bool    Subfolders() const                 { return m_bSubfolders; }
    bool    OnlySubfolders() const             { return m_bRemoveOnlySub; }
	
};

class Node
{
private:
	CString strName;
	vector<Node*> pChilds;
	Node *pSuper;
	Config *pCfg;
	ItemCheck checked;

	bool IsLeaf;
	int nLevel;
	
public:
	HTREEITEM htiCTreeItem;
	Node()
	{
		pSuper = 0;
		nLevel = 0;
		pCfg = 0;
		strName = CString("");
		checked = UNCHECKED;
		IsLeaf = 0;
		//pChilds = 0;
	};
	~Node()
	{
		delete pCfg;
		pCfg = 0;
		//pChilds = 0;
	};
	
	ItemCheck getItemCheck(){return checked;};
	void setItemCheck(ItemCheck check){checked = check;};
	
	bool getIsLeaf(){return IsLeaf;};
	void setIsLeaf(bool isLeaf){IsLeaf = isLeaf;};

	int getLevel(){return nLevel;};
	void setLevel(int level){nLevel = level;};

	void setCfg(Config *cfg){pCfg = cfg; return;};
	Config * getCfg(){return pCfg;};

	bool setNodeName(CString name){strName = name; return 0;};
	CString& getNodeName(){return strName;};

	bool setNodeIsLeaf(bool leaf){IsLeaf = leaf; return 0;};

	bool setNodeSuper(Node *node){pSuper = node; return 0;};
	Node *getNodeSuper(){return pSuper;};

	vector<Node*>& getNodeChilds(){return pChilds;};


	virtual bool addConfig(Config* pCfg) = 0;
	virtual bool delConfig(Config* pCfg) = 0;
	virtual Node* addNode() = 0;
	virtual bool delNode() = 0;
	virtual void addNode(Node *pNode) = 0;
	virtual bool erase()= 0;
	virtual bool eraserEnd() = 0;
	virtual bool eraserDestroy() = 0;
};



class EraseNode: public Node
{
/*private:
	CString strName;
	vector<Node*> pChilds;
	Node *pSuper;
	Config *pCfg;
*/
	//vector<Config*> pConfig;

public:
	EraseNode();
	
	virtual bool addConfig(Config* pCfg);
	virtual bool delConfig(Config* pCfg);
	virtual Node* addNode();
	virtual void addNode(Node *pNode);
	virtual bool delNode();
	virtual bool erase();
	virtual bool eraserEnd();
	virtual bool eraserDestroy();
};


extern EraseNode *pRoot;
extern bool   LoadCfgFile();