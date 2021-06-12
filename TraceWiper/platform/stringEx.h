#ifndef _STRING_EX_
#define _STRING_EX_

#include "wabdefs.h"
#include "winnt.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include<string>

using namespace std;

class StringEx
{
public:
    StringEx();
    virtual ~StringEx();
    LPWSTR string2LPWSTR(const char* src);
    void ctow(WCHAR* Dest, const CHAR* Source);
    void wtoc(CHAR* Dest, const WCHAR* Source);
    void string_replace(string& s1,const string& s2,const string& s3);
    void splitStr(std::string src, std::string token, ValueList& vect);
};

#endif
