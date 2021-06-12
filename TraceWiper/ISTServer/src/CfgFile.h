#include <vector>

using namespace std;



struct LegalItem
{
	CString name;
	CString dep;
	CString ipaddr;
	CString macaddr;
	CString harddrive;
	CString password;
	CString key;
};

extern vector<LegalItem*>    LegalItems;

