#pragma   warning(   disable   :   4786   )  
#include "XmlTree.h"

XmlTree * XmlTree::mInstancePtr = 0;

XmlTree * XmlTree::getInstance()
{
    if(0 == mInstancePtr)
    {
        mInstancePtr = new XmlTree(DETECT_CFG_FILE);
    }

    return mInstancePtr;
}

void XmlTree::save()
{
    static int i = 0;
    char str[16] = {0};
    //sprintf(str, "demo%d.xml", i);
    sprintf(str, DETECT_CFG_FILE, i);
    mDoc.SaveFile(str);
}
XmlTree::XmlTree(char * pDocName):mDoc(pDocName)
{
    bool lLoadOk = false;
    lLoadOk = mDoc.LoadFile();
    if(!lLoadOk)
    {
        cout<<"can not build xml tree from "<<pDocName<<endl;
    }
    else
    {
        cout<<"build xml tree from "<<pDocName<<" OK!"<<endl;    
    }            
    root = mDoc.RootElement();

    mNodeIdPoll = 0;

//    cout<<mDoc;
}

XmlTree::~XmlTree()
{
    
}
int XmlTree::getFreeNodeId()
{
    mNodeIdPoll++;
    return mNodeIdPoll;
}
void XmlTree::guiTree(s_TREENODE & pTree)
{
    NameList lTreeNodes;
    NameList lSubTreeNodes;

    treeNodes(&lTreeNodes);

//tree node
	unsigned int i, j, k;
    for( i = 0; i < lTreeNodes.size(); i++)
    {
        TREENODE lTreeNode;
        lTreeNode.dwNodeID = getFreeNodeId();
        lTreeNode.sNodeName = lTreeNodes[i];

        pTree.vChildNodes.push_back(lTreeNode);
    }
//sub tree node
    for(i = 0; i < lTreeNodes.size(); i++)
    {
        TREENODE & lSubNode = pTree.vChildNodes[i];
        subTreeNodes(lTreeNodes[i], &lSubTreeNodes);

        for(j = 0; j < lSubTreeNodes.size(); j++)
        {
            TREENODE lTreeNode;
            lTreeNode.dwNodeID = getFreeNodeId();
            lTreeNode.sNodeName = lSubTreeNodes[j];

            lSubNode.vChildNodes.push_back(lTreeNode);
        }
        //node
        for(j = 0; j < lSubTreeNodes.size(); j++)
        {
            TREENODE & lNode = lSubNode.vChildNodes[i];
            
            NameList lNodeName;
            IDList lNodeIds;

//            nodes(lTreeNodes[i], lSubTreeNodes[j], &lNodeName/*, &lNodeIds*/);

            for( k = 0; k < lSubTreeNodes.size(); k++)
            {
                TREENODE lTreeNode;
                lTreeNode.dwNodeID = getFreeNodeId();
                lTreeNode.sNodeName = lNodeName[k];

                lNode.vChildNodes.push_back(lTreeNode);
            }
        }

    }
}

void XmlTree::treeNodes(NameList * pName)
{
    TiXmlNode* node = 0;
    int i = 0;
    for(node =  root->FirstChild(); node; node = node->NextSibling())
    {
        TiXmlElement* todoElement = 0;
        todoElement = node->ToElement();
        if(todoElement)
        {
            pName->push_back(const_cast<char*>(todoElement->Attribute(TREENODE_NAME)));
            i++;
        }
    }
}
void XmlTree::subTreeNodes(char * pTreeNode, NameList * pSubTreeName)
{
    TiXmlNode* treenode = 0;
//    int i = 0;
    for(treenode =  root->FirstChild(); treenode; treenode = treenode->NextSibling())
    {
        TiXmlElement* todoElement = 0;
        todoElement = treenode->ToElement();
        if(todoElement)
        {
            if(strcmp(const_cast<char*>(todoElement->Attribute(TREENODE_NAME)), pTreeNode) == 0)
            {
                //reach the treenode
                TiXmlNode* subnode = 0;
                for(subnode = treenode->FirstChild(); subnode; subnode = subnode->NextSibling())
                {
                    TiXmlElement* subotodoElement = 0;
                    subotodoElement = subnode->ToElement();
                    if(subotodoElement)
                    {
                        pSubTreeName->push_back(const_cast<char*>(subotodoElement->Attribute(NODE_NAME)));
//                        i++;
                    }
                    else
                    {
                        cout<<"subTreeNodes to element failed! "<<endl;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void XmlTree::getNodeWeigth(string pTreeNode, string pSubTreeNode, string pName, int & eva, int &cal, string & note)
{
    TiXmlNode* treenode = 0;
//    int i = 0;
    for(treenode =  root->FirstChild(); treenode; treenode = treenode->NextSibling())
    {
        TiXmlElement* todoElement = 0;
        todoElement = treenode->ToElement();
        if(todoElement)
        {
            if(strcmp(const_cast<char*>(todoElement->Attribute(TREENODE_NAME)), pTreeNode.c_str()) == 0)
            {
                TiXmlNode* subnode = 0;
                for(subnode = treenode->FirstChild(); subnode; subnode = subnode->NextSibling())
                {
                    TiXmlElement* subotodoElement = 0;
                    subotodoElement = subnode->ToElement();
                    if(strcmp(const_cast<char*>(subotodoElement->Attribute(SUBTREENODE_NAME)), pSubTreeNode.c_str()) == 0)
                    {
                        //hit the node list
                        TiXmlNode* node = 0;
                        for(node =  subotodoElement->FirstChild(); node; node = node->NextSibling())
                        {
                            TiXmlElement* nodeElement = 0;
                            nodeElement = node->ToElement();
                            if(0 == strcmp(const_cast<char*>(nodeElement->Attribute(NODE_NAME)), pName.c_str()))
                            {
                                eva = atoi(const_cast<char*>(nodeElement->Attribute(NODE_EVA)));
                                cal = atoi(const_cast<char*>(nodeElement->Attribute(NODE_CAL)));
                                note = const_cast<char*>(nodeElement->Attribute(NODE_NOTE));
                                break;
                            }
                        }
                        break;
                    }
                }
                
                break;
            }
        }
    }
}
void XmlTree::replaceNode(void * replaceThis, void* withThis, void ** pNewNode)
{
    TiXmlElement* nodeElement = (TiXmlElement*)replaceThis;
    TiXmlNode * parent = nodeElement->Parent();

    TiXmlNode *tmp = (TiXmlNode*)withThis;

    TiXmlNode * newNode = parent->ReplaceChild(nodeElement, *tmp);
    
    *pNewNode = newNode;

    return;
}

void XmlTree::nodeSet(void * pXmlNode, bool checked, int eva, int cal, string note)
{
    TiXmlElement* nodeElement = (TiXmlElement*)pXmlNode;
    char str[32] = {0};
    
    sprintf(str, "%d", eva);
    nodeElement->SetAttribute(NODE_EVA, const_cast<char*>(str));

    str[0] = 0;
    sprintf(str, "%d", cal);
    nodeElement->SetAttribute(NODE_CAL, const_cast<char*>(str));

    nodeElement->SetAttribute(NODE_NOTE, note.c_str());

    str[0] = 0;
    if(checked)
    {
        strcpy(str, "1");
    }
    else
    {
        strcpy(str, "0");
    }
    nodeElement->SetAttribute(NODE_CHECKED, const_cast<char*>(str));

    return;
}
void XmlTree::nodeSetAttr(string pTreeNode, string pSubTreeNode, string pName,
    bool checked, int eva, int cal, string note)
{
    TiXmlNode* treenode = 0;
//    int i = 0;
    for(treenode =  root->FirstChild(); treenode; treenode = treenode->NextSibling())
    {
        TiXmlElement* todoElement = 0;
        todoElement = treenode->ToElement();
        if(todoElement)
        {
            if(strcmp(const_cast<char*>(todoElement->Attribute(TREENODE_NAME)), pTreeNode.c_str()) == 0)
            {
                TiXmlNode* subnode = 0;
                for(subnode = treenode->FirstChild(); subnode; subnode = subnode->NextSibling())
                {
                    TiXmlElement* subotodoElement = 0;
                    subotodoElement = subnode->ToElement();
                    if(strcmp(const_cast<char*>(subotodoElement->Attribute(SUBTREENODE_NAME)), pSubTreeNode.c_str()) == 0)
                    {
                        //hit the node list
                        TiXmlNode* node = 0;
                        for(node =  subotodoElement->FirstChild(); node; node = node->NextSibling())
                        {
                            TiXmlElement* nodeElement = 0;
                            nodeElement = node->ToElement();
                            if(0 == strcmp(const_cast<char*>(nodeElement->Attribute(NODE_NAME)), pName.c_str()))
                            {
                                char str[32] = {0};
                                
                                sprintf(str, "%d", eva);
                                nodeElement->SetAttribute(NODE_EVA, const_cast<char*>(str));

                                str[0] = 0;
                                sprintf(str, "%d", cal);
                                nodeElement->SetAttribute(NODE_CAL, const_cast<char*>(str));

                                nodeElement->SetAttribute(NODE_NOTE, note.c_str());

                                str[0] = 0;
                                if(checked)
                                {
                                    strcpy(str, "true");
                                }
                                else
                                {
                                    strcpy(str, "false");
                                }
                                nodeElement->SetAttribute(NODE_CHECKED, const_cast<char*>(str));

                                break;
                            }
                        }
                        break;
                    }
                }
                
                break;
            }
        }
    }
}


void XmlTree::nodes(char * pTreeNode, char*pSubTreeNode, NameList * pName, BoolList *pCheckedList)
{
    TiXmlNode* treenode = 0;
//    int i = 0;
    for(treenode =  root->FirstChild(); treenode; treenode = treenode->NextSibling())
    {
        TiXmlElement* todoElement = 0;
        todoElement = treenode->ToElement();
        if(todoElement)
        {
            if(strcmp(const_cast<char*>(todoElement->Attribute(TREENODE_NAME)), pTreeNode) == 0)
            {
                TiXmlNode* subnode = 0;
                for(subnode = treenode->FirstChild(); subnode; subnode = subnode->NextSibling())
                {
                    TiXmlElement* subotodoElement = 0;
                    subotodoElement = subnode->ToElement();
                    if(strcmp(const_cast<char*>(subotodoElement->Attribute(SUBTREENODE_NAME)), pSubTreeNode) == 0)
                    {
                        //hit the node list
                        TiXmlNode* node = 0;
                        for(node =  subotodoElement->FirstChild(); node; node = node->NextSibling())
                        {
                            TiXmlElement* nodeElement = 0;
                            nodeElement = node->ToElement();
                            pName->push_back(const_cast<char*>(nodeElement->Attribute(NODE_NAME)));
                            if(0 == strcmp(const_cast<char*>(nodeElement->Attribute(NODE_CHECKED)), "1"))
                            {
                                pCheckedList->push_back(true);
                            }
                            else
                            {
                                pCheckedList->push_back(false);
                            }
//                            pID->push_back(atoi(nodeElement->Attribute(NODE_ID)));

                        }
                        break;
                    }
                }
                
                break;
            }
        }
    }
}

void XmlTree::node(char * pTreeNode, NameList *pName, IDList *pID)
{
    TiXmlNode* node = 0;
    TiXmlElement* todoElement = 0;
    int i = 0;
    for(node =  root->FirstChild(); node; node = node->NextSibling())
    {
        todoElement = node->ToElement();
        if(todoElement)
        {
            if(0 == strcmp(pTreeNode, todoElement->Attribute(TREENODE_NAME)))
            {
                break;
            }
        }
    }

    if(todoElement)//this is tree-node
    {
        for(node =  todoElement->FirstChild(); node; node = node->NextSibling())
        {
            TiXmlElement* toElement = 0;
            toElement = node->ToElement();
            if(toElement)
            {
                pName->push_back(const_cast<char*>(toElement->Attribute(NODE_NAME)));
                pID->push_back(atoi(toElement->Attribute(NODE_ID)));
                i++;
            }
        }
    }
}

TiXmlElement * XmlTree::rootElement()
{
    return root;
}

bool XmlTree::nodeParse(char* pNodeName, Titles * pTitles, void ** pXmlNode)
{
    TiXmlNode* treeNode = 0;
    TiXmlElement* nodeElement = 0;
                    
    bool lFound = false;
    for(treeNode =  root->FirstChild(); treeNode; treeNode = treeNode->NextSibling())
    {
        TiXmlElement* todoElement = 0;
        todoElement = treeNode->ToElement();

        TiXmlNode* subnodes;
        for(subnodes =  todoElement->FirstChild(); subnodes; subnodes = subnodes->NextSibling())
        {
            TiXmlElement* subnodeElement = 0;
            subnodeElement = subnodes->ToElement();
            if(subnodeElement)
            {
                TiXmlNode* nodes;
                for(nodes =  subnodeElement->FirstChild(); nodes; nodes = nodes->NextSibling())
                {
                    nodeElement = nodes->ToElement();
                    if(0 == strcmp(pNodeName, const_cast<char*>(nodeElement->Attribute(NODE_NAME))))
                    {
                        //found it
                        lFound = true;
                        if(pXmlNode)
                        {
                            *pXmlNode = (void*)nodeElement;
                        }
                        break;
                    }
                    
                }
            }
            if(lFound)
            {
                break;
            }
        }
        if(lFound)
        {
            break;
        }
    }
    if(lFound)
    {
        TiXmlElement* titleElement = nodeElement->FirstChildElement();
        for(; titleElement; titleElement=titleElement->NextSiblingElement())
        {
            Title *lTitle = new Title;
            if(titleElement->Attribute(TITLE_NAME))
            {
                lTitle->setTitle(titleElement->Attribute(TITLE_NAME));
            }
            if(titleElement->Attribute(TITLE_WIDTH))
            {
                lTitle->setTitleWidth(atoi(titleElement->Attribute(TITLE_WIDTH)));
            }
            TiXmlElement* keyElement = titleElement->FirstChildElement();
            for(; keyElement != 0; keyElement = keyElement->NextSiblingElement())
            {
                Key *lKey = new Key;
                const char * lAccess = keyElement->Attribute(NODE_ACCESS);
                const char * lpKey    = keyElement->Attribute(NODE_KEY);
                lKey->mAccess = accessTypeFromStr(lAccess);
                if(lpKey)
                {
                    lKey->mKey = lpKey;
                }
                
                TiXmlElement * lField = keyElement->FirstChildElement();
                for(; lField != 0; lField = lField->NextSiblingElement())
                {
                    Field lFieldValue;
                    const char * lFieldType = lField->Attribute(FIELD_TYPE);
                    const char * lAction     = lField->Attribute(FIELD_ACTION);
                    const char * lFieldText = lField->GetText();

                    lFieldValue.mAction = actionTypeFromStr(lAction);
                    lFieldValue.mType = fieldTypeFromStr(lFieldType);
                    lFieldValue.mField = const_cast<char*>(lFieldText);

                    lKey->mFields.push_back(lFieldValue);
                }
                lTitle->mKeys.push_back(lKey);
            }         
            pTitles->push_back(lTitle);
        }

        return true;
        
    }
    else
    {
        return false;
    }
    
}


char* XmlTree::fieldTypeToStr(FiledType pType)
{
    switch(pType)
    {
        case Binary:
            return "BINARY";
        case DWord:
            return "DWORD";
        case QWord:
            return "QWORD";
        case DWordBe:
            return "DWORDBE";
        case DWordLe:
            return "DWORDLE";
        case KString:
            return "STRING";
        case MString:
            return "MSTRING";
        default:
            return "null";
    }
    
}


FiledType XmlTree::fieldTypeFromStr(const char * pStr)
{
    if(!pStr)
    {
        return NullType;
    }
    if(!strcmp(pStr, "BINARY"))
    {
        return Binary;
    }
    else if(!strcmp(pStr, "DWORD"))
    {
        return DWord;
    }
    else if(!strcmp(pStr, "QWORD"))
    {
        return QWord;
    }
    else if(!strcmp(pStr, "DWORDBE"))
    {
        return DWordBe;
    }
    else if(!strcmp(pStr, "DWORDLE"))
    {
        return DWordLe;
    }
    else if(!strcmp(pStr, "STRING"))
    {
        return KString;
    }
    else if(!strcmp(pStr, "MSTRING"))
    {
        return MString;
    }
    else
    {
        return NullType;
    }
}

char* XmlTree::actionTypeToStr(Action pAction)
{
    switch(pAction)
    {
        case ACT_READ:
            return "READ";
        case ACT_WRITE:
            return "WRITE";
        case ACT_SUBKEYS:
            return "READSUBKEY";
        case ACT_SUBKEYS_TOTALPATH:
            return "READSUBKEYTOTALPATH";
        case ACT_ALL_FILEDS:
            return "READALLFIELDS";
        case ACT_BHO:
            return "BHO";
        case ACT_WIN_SHARES:
            return "GetShares";
        case ACT_WIN_SYS_SHARES:
            return "SYS_SHARES";
        case ACT_IE_SECURITY0:
            return "IESECURITY0";
        case ACT_IE_SECURITY1:
            return "IESECURITY1";
        case ACT_IE_SECURITY2:
            return "IESECURITY2";
        case ACT_IE_SECURITY3:
            return "IESECURITY3";
        case ACT_IE_SECURITY4:
            return "IESECURITY4";
        case ACT_IE_WEBCACHE_URL:
            return "IE_WEBCACHE_URL";
        case ACT_IE_WEBCACHE_TIME:
            return "IE_WEBCACHE_TIME";
        case ACT_IE_HISTORY_TITLE:
            return "IE_HISTORY_TITLE";
        case ACT_IE_HISTORY_URL:
            return "IE_HISTORY_URL";
        case ACT_IE_HISTORY_TIME:
            return "IE_HISTORY_TIME";
        case ACT_IE_COOKIE_URL:
            return "IE_COOKIE_URL";
        case ACT_IE_COOKIE_TIME:
            return "IE_COOKIE_TIME";
        case ACT_FIREFOX_WEBCACHE_URL:
            return "FIREFOX_WEBCACHE_URL";
        case ACT_FIREFOX_WEBCACHE_TIME:
            return "FIREFOX_WEBCACHE_TIME";
        case ACT_FIREFOX_HISTORY_TITLE:
            return "FIREFOX_HISTORY_TITLE";
        case ACT_FIREFOX_HISTORY_TIME:
            return "FIREFOX_HISTORY_TIME";
        case ACT_FIREFOX_COOKIE_URL:
            return "FIREFOX_COOKIE_URL";
        case ACT_FIREFOX_COOKIE_TIME:
            return "FIREFOX_COOKIE_TIME";
        case ACT_MEM_SIZE 			:     return "MEM_SIZE";
        case ACT_MEM_AVAIL 			:     return "MEM_AVAIL";
        case ACT_MEM_PAGE_SIZE		:     return "MEM_PAGESIZE";
        case ACT_MEM_ALLOC_STEP		:     return "MEM_STEP";
        case ACT_MEM_APP_START		:     return "MEM_APPSTART";
        case ACT_MEM_APP_END			:     return "MEM_APPEND";
        case ACT_SYS_PID_NAME		:     return "SYS_PID_NAME";
        case ACT_SYS_PID			:     return "SYS_PID";
        case ACT_SYS_SRV_PIDNAME		:     return "SYS_SRV_PIDNAME";
        case ACT_SYS_SRV_PIDDESC		:     return "SYS_SRV_PIDDESC";
        case ACT_SYS_SRV_CURRENTSTATE	:     return "SYS_SRV_CURRENTSTATE";
        case ACT_SYS_SRVPORT_PROTO		:     return "SYS_SRVPORT_PROTO";
        case ACT_SYS_SRVPORT_LOCALADDR	:     return "SYS_SRVPORT_LOCALADDR";
        case ACT_SYS_SRVPORT_FOREIGNADDR	:     return "SYS_SRVPORT_FOREIGNADDR";
        case ACT_SYS_SRVPORT_STATE		:     return "SYS_SRVPORT_STATE";
        case ACT_SYS_SRVPORT_PROCESS		:     return "SYS_SRVPORT_PROCESS";
        case ACT_PROG_TEMP_FILE		:     return "PROG_TEMP_FILE";
        case ACT_WIN_PRE_READ		:     return "WIN_PRE_READ";
        case ACT_HDISK_PARTITION		:     return "HDISK_PARTITION";
        case ACT_HDISK_TYPE			:     return "HDISK_TYPE";
        case ACT_HDISK_AVAILSIZE		:     return "HDISK_AVAILSIZE";
        case ACT_HDISK_TOTALSIZE		:     return "HDISK_TOTALSIZE";
        case ACT_VIRUS_IS_RUNNING		:     return "VIRUS_IS_RUNNING";
        case ACT_USER_DEFINED_PNAME		:     return "USER_DEFINED_PNAME";
        case ACT_USER_DEFINED_PID		:     return "USER_DEFINED_PID";
        case ACT_USER_DEFINED_PCURRENTSTATE	:     return "USER_DEFINED_PCURRENTSTATE";
        case ACT_IE_RSCNAME			:     return "IE_RSCNAME";
        case ACT_IE_RSCTYPE			:     return "IE_RSCTYPE";
        case ACT_IE_USERID			:     return "IE_USERID";
        case ACT_IE_USERPWD			:     return "IE_USERPWD";
        case ACT_USB_CLASS			:     return "USB_CLASS";
        case ACT_USB_TYPE			:     return "USB_TYPE";
        case ACT_USB_HARDWAREID		:     return "USB_HARDWAREID";
        case ACT_WIN_SP_SOFTWARE		:     return "SP_SOFTWARE";
        case ACT_WIN_SP_PATCH		:     return "SP_PATCH";
        case ACT_WIN_SP_DES			:     return "SP_DES";
        case ACT_WIN_SP_DATE			:     return "SP_DATE";
        case ACT_UDISK_CROSS_DETECT		:  return "UDISK_CROSS_DETECT";
        case ACT_NETIF_NAME			:  return "NETWORK_CARD_NAME";
        case ACT_NETIF_MAC			:  return "NETWORK_CARD_MAC";
        case ACT_NETIF_IP			:  return "NETWORK_CARD_IP";
        case ACT_NETIF_GW			:  return "NETWORK_CARD_GATEWAY";
        case ACT_NETIF_MASK			:  return "NETWORK_CARD_MASK";
        case ACT_SOFTWARE_INSTALL		:  return "SOFTWARE_INSTALL";
        case ACT_NETIF_WIRELESS_CARD		:  return "NETIF_WIRELESS_CARD";
        case ACT_NETIF_BLUETOOTH		:  return "NETIF_BLUETOOTH";
        case ACT_NETIF_BLUETOOTH_ID		:  return "NETIF_BLUETOOTH_ID";
        case ACT_NETIF_WIRELESS_CARD_ID	:          return "NETIF_WIRELESS_CARD_ID";
        case ACY_NETIF_MODEM_ID		:          return "NETIF_MODEM_ID";
        case ACY_NETIF_MODEM_NAME		: return "NETIF_MODEM_NAME";
        case ACY_NETIF_MODEM_PORT		: return "NETIF_MODEM_PORT";
        case ACT_NET_PASSWD			: return "NET_PASSWD";    

        default : return "null";
    }
}

Action XmlTree::actionTypeFromStr(const char * pStr)
{
    if(!pStr)
    {
        return NullAction;
    }
    if(!strcmp(pStr, "READ"))
    {
        return ACT_READ;
    }
    else if(!strcmp(pStr, "WRITE"))
    {
        return ACT_WRITE;
    }
    else if(!strcmp(pStr, "READSUBKEY"))
    {
        return ACT_SUBKEYS;
    }
    else if(!strcmp(pStr, "READSUBKEYTOTALPATH"))
    {
        return ACT_SUBKEYS_TOTALPATH;
    }
    else if(!strcmp(pStr, "READALLFIELDS"))
    {
        return ACT_ALL_FILEDS;
    }
    else if(!strcmp(pStr, "BHO"))
    {
        return ACT_BHO;
    }
    else if(!strcmp(pStr, "GetShares"))
    {
        return ACT_WIN_SHARES;
    }
    else if(!strcmp(pStr, "SYS_SHARES"))
    {
        return ACT_WIN_SYS_SHARES;
    }
    else if(!strcmp(pStr, "IESECURITY0"))
    {
        return ACT_IE_SECURITY0;
    }
    else if(!strcmp(pStr, "IESECURITY1"))
    {
        return ACT_IE_SECURITY1;
    }
    else if(!strcmp(pStr, "IESECURITY2"))
    {
        return ACT_IE_SECURITY2;
    }
    else if(!strcmp(pStr, "IESECURITY3"))
    {
        return ACT_IE_SECURITY3;
    }
    else if(!strcmp(pStr, "IESECURITY4"))
    {
        return ACT_IE_SECURITY4;
    }
    else if(!strcmp(pStr, "IE_WEBCACHE_URL"))
    {
        return ACT_IE_WEBCACHE_URL;
    }
    else if(!strcmp(pStr, "IE_WEBCACHE_TIME"))
    {
        return ACT_IE_WEBCACHE_TIME;
    }
    else if(!strcmp(pStr, "IE_HISTORY_TITLE"))
    {
        return ACT_IE_HISTORY_TITLE;
    }
    else if(!strcmp(pStr, "IE_HISTORY_URL"))
    {
        return ACT_IE_HISTORY_URL;
    }
    else if(!strcmp(pStr, "IE_HISTORY_TIME"))
    {
        return ACT_IE_HISTORY_TIME;
    }
    else if(!strcmp(pStr, "IE_COOKIE_URL"))
    {
        return ACT_IE_COOKIE_URL;
    }
    else if(!strcmp(pStr, "IE_COOKIE_TIME"))
    {
        return ACT_IE_COOKIE_TIME;
    }
    else if(!strcmp(pStr, "FIREFOX_WEBCACHE_URL"))
    {
        return ACT_FIREFOX_WEBCACHE_URL;
    }
    else if(!strcmp(pStr, "FIREFOX_WEBCACHE_TIME"))
    {
        return ACT_FIREFOX_WEBCACHE_TIME;
    }
    else if(!strcmp(pStr, "FIREFOX_HISTORY_TITLE"))
    {
        return ACT_FIREFOX_HISTORY_TITLE;
    }
    else if(!strcmp(pStr, "FIREFOX_HISTORY_TIME"))
    {
        return ACT_FIREFOX_HISTORY_TIME;
    }
    else if(!strcmp(pStr, "FIREFOX_COOKIE_URL"))
    {
        return ACT_FIREFOX_COOKIE_URL;
    }
    else if(!strcmp(pStr, "FIREFOX_COOKIE_TIME"))
    {
        return ACT_FIREFOX_COOKIE_TIME;
    }
    else if(!strcmp(pStr, "MEM_SIZE"))
    {
        return ACT_MEM_SIZE;
    }
    else if(!strcmp(pStr, "MEM_AVAIL"))
    {
        return ACT_MEM_AVAIL;
    }
    else if(!strcmp(pStr, "MEM_PAGESIZE"))
    {
        return ACT_MEM_PAGE_SIZE;
    }
    else if(!strcmp(pStr, "MEM_STEP"))
    {
        return ACT_MEM_ALLOC_STEP;
    }
    else if(!strcmp(pStr, "MEM_APPSTART"))
    {
        return ACT_MEM_APP_START;
    }
    else if(!strcmp(pStr, "MEM_APPEND"))
    {
        return ACT_MEM_APP_END;
    }
    else if(!strcmp(pStr, "SYS_PID_NAME"))
    {
        return ACT_SYS_PID_NAME;
    }
    else if(!strcmp(pStr, "SYS_PID"))
    {
        return ACT_SYS_PID;
    }
    else if(!strcmp(pStr, "SYS_SRV_PIDNAME"))
    {
        return ACT_SYS_SRV_PIDNAME;
    }
    else if(!strcmp(pStr, "SYS_SRV_PIDDESC"))
    {
        return ACT_SYS_SRV_PIDDESC;
    }
    else if(!strcmp(pStr, "SYS_SRV_CURRENTSTATE"))
    {
        return ACT_SYS_SRV_CURRENTSTATE;
    }
    else if(!strcmp(pStr, "SYS_SRVPORT_PROTO"))
    {
        return ACT_SYS_SRVPORT_PROTO;
    }
    else if(!strcmp(pStr, "SYS_SRVPORT_LOCALADDR"))
    {
        return ACT_SYS_SRVPORT_LOCALADDR;
    }
    else if(!strcmp(pStr, "SYS_SRVPORT_FOREIGNADDR"))
    {
        return ACT_SYS_SRVPORT_FOREIGNADDR;
    }
    else if(!strcmp(pStr, "SYS_SRVPORT_STATE"))
    {
        return ACT_SYS_SRVPORT_STATE;
    }
    else if(!strcmp(pStr, "SYS_SRVPORT_PROCESS"))
    {
        return ACT_SYS_SRVPORT_PROCESS;
    }
    else if(!strcmp(pStr, "PROG_TEMP_FILE"))
    {
        return ACT_PROG_TEMP_FILE;
    }
    else if(!strcmp(pStr, "WIN_PRE_READ"))  //windows预读程序
    {
        return ACT_WIN_PRE_READ;
    }
    else if(!strcmp(pStr, "HDISK_PARTITION")) //磁盘分区信息
    {
        return ACT_HDISK_PARTITION;
    }
    else if(!strcmp(pStr, "HDISK_TYPE"))
    {
        return ACT_HDISK_TYPE;
    }
    else if(!strcmp(pStr, "HDISK_AVAILSIZE"))
    {
        return ACT_HDISK_AVAILSIZE;
    }
    else if(!strcmp(pStr, "HDISK_TOTALSIZE"))
    {
        return ACT_HDISK_TOTALSIZE;
    }
    else if(!strcmp(pStr, "VIRUS_IS_RUNNING")) 
    {
        return ACT_VIRUS_IS_RUNNING;
    }
    else if(!strcmp(pStr, "USER_DEFINED_PNAME")) //用户自定义进程
    {
        return ACT_USER_DEFINED_PNAME;
    }
    else if(!strcmp(pStr, "USER_DEFINED_PID"))
    {
        return ACT_USER_DEFINED_PID;
    }
    else if(!strcmp(pStr, "USER_DEFINED_PCURRENTSTATE"))
    {
        return ACT_USER_DEFINED_PCURRENTSTATE;
    }
    else if(!strcmp(pStr, "IE_RSCNAME"))  //IE账号
    {
        return ACT_IE_RSCNAME;
    }
    else if(!strcmp(pStr, "IE_RSCTYPE")) 
    {
        return ACT_IE_RSCTYPE;
    }
    else if(!strcmp(pStr, "IE_USERID"))
    {
        return ACT_IE_USERID;
    }
    else if(!strcmp(pStr, "IE_USERPWD"))
    {
        return ACT_IE_USERPWD;
    }
    else if(!strcmp(pStr, "USB_CLASS"))
    {
        return ACT_USB_CLASS;
    }
    else if(!strcmp(pStr, "USB_TYPE"))
    {
        return ACT_USB_TYPE;
    }
    else if(!strcmp(pStr, "USB_HARDWAREID"))
    {
        return ACT_USB_HARDWAREID;
    }
    else if(!strcmp(pStr, "SP_SOFTWARE"))
    {
        return ACT_WIN_SP_SOFTWARE;
    }
    else if(!strcmp(pStr, "SP_PATCH"))
    {
        return ACT_WIN_SP_PATCH;
    }
    else if(!strcmp(pStr, "SP_DES"))
    {
        return ACT_WIN_SP_DES;
    }
    else if(!strcmp(pStr, "SP_DATE"))
    {
        return ACT_WIN_SP_DATE;
    }
	else if (!strcmp(pStr, "UDISK_CROSS_DETECT"))
    {
        return ACT_UDISK_CROSS_DETECT;
    }
	else if (!strcmp(pStr, "NETWORK_CARD_NAME"))
    {
        return ACT_NETIF_NAME;
    }
	else if (!strcmp(pStr, "NETWORK_CARD_MAC"))
    {
        return ACT_NETIF_MAC;
    }
	else if (!strcmp(pStr, "NETWORK_CARD_IP"))
    {
        return ACT_NETIF_IP;
    }
	else if (!strcmp(pStr, "NETWORK_CARD_GATEWAY"))
    {
        return ACT_NETIF_GW;
    }
	else if (!strcmp(pStr, "NETWORK_CARD_MASK"))
    {
        return ACT_NETIF_MASK;
    }
	else if (!strcmp(pStr, "SOFTWARE_INSTALL"))
    {
        return ACT_SOFTWARE_INSTALL;
    }    
	else if (!strcmp(pStr, "NETIF_WIRELESS_CARD"))
    {
        return ACT_NETIF_WIRELESS_CARD;
    }    
	else if (!strcmp(pStr, "NETIF_WIRELESS_CARD"))
    {
        return ACT_NETIF_WIRELESS_CARD;
    }    
	else if (!strcmp(pStr, "NETIF_BLUETOOTH"))
    {
        return ACT_NETIF_BLUETOOTH;
    }    
	else if (!strcmp(pStr, "NETIF_BLUETOOTH_ID"))
    {
        return ACT_NETIF_BLUETOOTH_ID;
    }    
	else if (!strcmp(pStr, "NETIF_WIRELESS_CARD"))
    {
        return ACT_NETIF_WIRELESS_CARD;
    }    
	else if (!strcmp(pStr, "NETIF_WIRELESS_CARD_ID"))
    {
        return ACT_NETIF_WIRELESS_CARD_ID;
    }    
	else if (!strcmp(pStr, "NETIF_MODEM_ID"))
    {
        return ACY_NETIF_MODEM_ID;
    }    
	else if (!strcmp(pStr, "NETIF_MODEM_NAME"))
    {
        return ACY_NETIF_MODEM_NAME;
    }    
	else if (!strcmp(pStr, "NETIF_MODEM_PORT"))
    {
        return ACY_NETIF_MODEM_PORT;
    }    
	else if (!strcmp(pStr, "NET_PASSWD"))
    {
        return ACT_NET_PASSWD;
    }    
    else
    {
        return NullAction;
    }
}
char* XmlTree::accessTypeToStr(Access pType)
{

    switch(pType)
    {
        case Register: return"REGISTER" ;
        case File: return"FILE" ;
        case Api: return "API" ;
        case Text: return "TEXT";
        default: return "null";
    }
}


Access XmlTree::accessTypeFromStr(const char * pStr)
{
    if(!pStr)
    {
        return NullAccess;
    }
    if(!strcmp(pStr, "REGISTER"))
    {
        return Register;
    }
    else if(!strcmp(pStr, "FILE"))
    {
        return File;
    }
    else if(!strcmp(pStr, "API"))
    {
        return Api;
    }
    else if(!strcmp(pStr, "TEXT"))
    {
        return Text;
    }
    else
    {
        return NullAccess;
    }
}

void printAttribtes(TiXmlElement* pElement)
{
    TiXmlAttribute *IDAttribute = pElement->FirstAttribute();  
    for(; IDAttribute; IDAttribute = IDAttribute->Next())
    {
        cout<<IDAttribute->Name()<<"="<<IDAttribute->Value()<<" ";
    }
    cout<<endl;
}

void printElement(TiXmlElement* pElement)
{
    cout<<pElement->Value()<<" ";
    printAttribtes(pElement);
}

void printChild(TiXmlElement* pElement)
{
    TiXmlElement * lEle = pElement->FirstChildElement();
    for(; lEle; lEle = lEle->NextSiblingElement())
    {
        cout<<lEle->Value()<<" ";
        printAttribtes(lEle);
		if(lEle->GetText())
		{
           cout<<lEle->GetText()<<endl;
		}
        
        printChild(lEle);      
                
    }
}

