#include "stdio.h"
#include "comutil.h"
#include "SysGlobal.h"
#include "stringEx.h"

StringEx::StringEx()
{
}

StringEx::~StringEx()
{
}

LPWSTR StringEx::string2LPWSTR(const char* src)
{
    wchar_t   *wsir;
    _bstr_t   str=src;
    wsir=str;
    return wsir;
}

void StringEx::ctow(WCHAR* Dest, const CHAR* Source)
{
    int i = 0;

    while(Source[i] != '\0')
    {
        Dest[i] = (WCHAR)Source[i];
        ++i;
    }
}
void StringEx::wtoc(CHAR* Dest, const WCHAR* Source)
{
    int i = 0;

    while(Source[i] != '\0')
    {
        Dest[i] = (CHAR)Source[i];
        ++i;
    }
}

void StringEx::string_replace(string&s1,const string&s2,const string&s3)
{
    string::size_type pos=0;
    string::size_type a=s2.size();
    string::size_type b=s3.size();
    while((pos=s1.find(s2,pos))!=string::npos)
    {
        s1.replace(pos,a,s3);
        pos+=b;
    }
}

void StringEx::splitStr(std::string src, std::string token, ValueList& vect)    
{
    int nend=0;    
    int nbegin=0;    
    while(nend != -1)    
    {    
        nend = src.find_first_of(token, nbegin);    
        if(nend == -1)    
            vect.push_back(src.substr(nbegin, src.length()-nbegin));    
        else   
            vect.push_back(src.substr(nbegin, nend-nbegin));    
        nbegin = nend + 1;    
        }    
}

