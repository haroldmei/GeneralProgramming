
#include "windows.h"
#include "ffsco.h"
#include <fstream> 
#include <iostream>
#include<io.h>
#include <time.h>
#include "SysGlobal.h"
using namespace std;

#ifndef _CONSOLE
//extern HWND g_basicTextHdl;
#endif
extern LPWSTR string2WCHAR(const char* srcStr);

bool g_bRun = true;

namespace helper_coffs {
    void getLocalTime(FILETIME Inftime, char* timeBuf);

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////
    
    ffsco::ffsco()
        : _limit(def_limit), _dirs(0), m_bRun(true)
    {
        
    }
    
    ffsco::~ffsco()
    {

    }
    
    //////////////////////////////////////////////////////////////////////
    string ffsco::_uppercase_(string s)
    {
        const char aazz = 'z' - 'Z';
        string rs;
        for (string::iterator it = s.begin();
        s.end() != it; it++)
        {
            if ('a' <= *it && *it <= 'z') rs.append(1, *it - aazz);
            else rs.append(1, *it);
        }
        return rs;
    }
    //////////////////////////////////////////////////////////////////////
    
    int ffsco::match(string fext_, string file_)
    {
        string fext = _uppercase_(fext_);
        string file = _uppercase_(file_);
        
        int pos = file.find_last_of('.');
        if (string::npos != pos) 
        {
            file = file.substr(pos);
        }
        if (string::npos != fext.find(file))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    
    void ffsco::limit(int limit_)
    {
        if (limit_ < 1) _limit = def_limit;
        if (limit_ < max_limit) _limit = limit_;
    }
    
    int ffsco::dir(string s)
    {
        return (FILE_ATTRIBUTE_DIRECTORY == GetFileAttributes(s.c_str()));
    }
    
    int ffsco::find(string path_, string fext_, int lFlag)
    {
        _co_file.clear();
        _co_dir.clear();
        _co.clear();
        
        string path = path_;
        if (path.size() > 0 && c_pathSeparator != path[path.size() - 1])
            path.append(s_pathSeparator);
        
        _co_dir.push_back( path );
        
        string fext = fext_;
        if (0 == fext.compare("*")
            || 0 == fext.compare("*.*")
            ) fext = "";

        string file = "*";
        
        string s = path + file;
        
        WIN32_FIND_DATA fileinfo = {0};
        HANDLE handle = FindFirstFile( s.c_str(), &fileinfo );
        
        if (NULL != handle && INVALID_HANDLE_VALUE != handle)
        {
            
            do {
                                
                if ('.' != fileinfo.cFileName[0])    //--skip./..
                    if ((FILE_ATTRIBUTE_DIRECTORY & fileinfo.dwFileAttributes)
                        == FILE_ATTRIBUTE_DIRECTORY)    //--目录
                    {
                        _co_dir.push_back( path + fileinfo.cFileName + s_pathSeparator );
                        
                        if (0 != dirs())
                        {
                            ffsco o;
                            o.dirs(dirs());
                            o.find( path + fileinfo.cFileName, fext);
                            //--dir
                            typeT o_dir = o.co_dir();
                            for (typeT::iterator it_dir = o_dir.begin(); o_dir.end() != it_dir; it_dir ++) 
                            {
                                _co_dir.push_back(*it_dir);
                            }

                            //--file
                            typeT o_file = o.co_file();
                            for (typeT::iterator it_file = o_file.begin(); o_file.end() != it_file; it_file ++) 
                            {
                                if (1 == lFlag)
                                {
                                    int size = GetFileSize(handle, NULL);   

                                    char tmpBuf[1024] = {0};
                                    _co_file.push_back( fext ); //filename from the user
                                    _co_file.push_back( *it_file );

                                    sprintf(tmpBuf, "%d", fileinfo.nFileSizeHigh); //size
                                    _co_file.push_back(tmpBuf);

                                    memset(tmpBuf, 0, 1024);
                                    getLocalTime(fileinfo.ftLastAccessTime, tmpBuf);
                                    _co_file.push_back(tmpBuf); //put the local time
                                }
                                else
                                {
                                    _co_file.push_back(*it_file);
                                }
#if 0
                                char tmp[1024] = {0};
                                if (strlen(it_file->c_str()) > 255)
                                {
                                    memcpy(tmp, it_file->c_str(), 255);
                                }
                                else
                                {
                                    sprintf(tmp, it_file->c_str());
                                }
                                #ifndef _CONSOLE
                                //SendMessage(g_basicTextHdl, WM_SETTEXT, 200, (LPARAM)tmp);
                                #endif
#endif
                            }
                        }
                    }
                    else
                    {
                        if ((0 == fext.size() || match(fext, fileinfo.cFileName))) 
                        {
                            if (1 == lFlag)
                            {
                                char tmpBuf[1024] = {0};
                                _co_file.push_back( fext ); //filename from the user
                                _co_file.push_back( path + fileinfo.cFileName );

                                sprintf(tmpBuf, "%d", fileinfo.nFileSizeLow); //size
                                _co_file.push_back(tmpBuf);
                                memset(tmpBuf, 0, 1024);
                                getLocalTime(fileinfo.ftLastAccessTime, tmpBuf);
                                _co_file.push_back(tmpBuf); //put the local time
                            }
                            else
                            {
                                _co_file.push_back( path + fileinfo.cFileName );
                            }
                        }
                    }

                    if(!g_bRun) 
                    {
                        break;
                    }
            } while (FindNextFile( handle, &fileinfo));
            
            FindClose(handle);
        }

#if 0
        //--dir
        for (typeT::iterator it_dir = _co_dir.begin();
        _co_dir.end() != it_dir;
        it_dir ++) _co.push_back(*it_dir);
        //--file
        for (typeT::iterator it_file = _co_file.begin();
        _co_file.end() != it_file;
        it_file ++) _co.push_back(*it_file);
#endif        
        return count();
    }


    void ffsco::findFilesByKeywords(string path, string keywords)
    {
        char tmpBuf[1024] = {0};

        string matchkeyword;        
        ValueList keywordsList;
        splitStr(keywords, ";", keywordsList);

        struct _finddata_t filefind;
        if(path[path.size()-1]=='\\')
        {
            path.resize(path.size()-1);
        }
        string curr=path+"\\*.*";

        int done=0;
        int handle = 0;
        if((handle=_findfirst(curr.c_str(),&filefind))==-1)
        {
            return;
        }
        
        while(!(done=_findnext(handle,&filefind)))
        {
            if(!strcmp(filefind.name,".."))
            {
                continue;
            }
            
            curr=path+"\\"+filefind.name;
            //if(strstr(filefind.name,keywords.c_str()))
            if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
            {
                //if(FILE_ATTRIBUTE_DIRECTORY != (filefind.attrib & FILE_ATTRIBUTE_DIRECTORY))
                {
                    //cout<<curr<<endl;
                    _co_file.push_back(filefind.name);
                    _co_file.push_back(curr);
                    sprintf(tmpBuf, "%d", filefind.size); //size
                    _co_file.push_back(tmpBuf);

                    memset(tmpBuf, 0, 1024);
                    getLocalTime(filefind.time_create, tmpBuf);
                    _co_file.push_back(tmpBuf); //create time
                    memset(tmpBuf, 0, 1024);
                    getLocalTime(filefind.time_write, tmpBuf);
                    _co_file.push_back(tmpBuf); //modified time
                    memset(tmpBuf, 0, 1024);
                    getLocalTime(filefind.time_access, tmpBuf);
                    _co_file.push_back(tmpBuf); //access time
                    
                    _co_file.push_back(matchkeyword);
                }
            }
            else
            {
                ifstream   ifile(filefind.name);   
                string   line;   
                if(!ifile.is_open())   
                {
                    if (_A_SUBDIR==filefind.attrib)
                    {
                        ifile.close();
                        findFilesByKeywords(curr,keywords);
                    }
                }
                else
                {
                    while (!ifile.eof())     
                    {
                        getline(ifile,   line);
                        //if(strstr(line.c_str(),keywords.c_str()) /*&& (FILE_ATTRIBUTE_DIRECTORY != filefind.attrib)*/)
                        if (hasKeywordsInString(filefind.name, keywordsList, matchkeyword))
                        {
                            //cout<<curr<<endl;
                            _co_file.push_back(filefind.name);
                            _co_file.push_back(curr);
                            sprintf(tmpBuf, "%d", filefind.size); //size
                            _co_file.push_back(tmpBuf);

                            memset(tmpBuf, 0, 1024);
                            getLocalTime(filefind.time_create, tmpBuf);
                            _co_file.push_back(tmpBuf); //create time
                            memset(tmpBuf, 0, 1024);
                            getLocalTime(filefind.time_write, tmpBuf);
                            _co_file.push_back(tmpBuf); //modified time
                            memset(tmpBuf, 0, 1024);
                            getLocalTime(filefind.time_access, tmpBuf);
                            _co_file.push_back(tmpBuf); //access time

                            _co_file.push_back(matchkeyword);
                            break;
                        }
                    }   
                }
                ifile.close();
            }

            if (_A_SUBDIR==filefind.attrib)
                findFilesByKeywords(curr,keywords);
        }    
        _findclose(handle);      
            
    }    
    //--example
    void ffsco::findFiles(string path, string fext, typeT& lKeyList, int lFlag)
    {
        //string fext;
        
        ffsco o;
        o.dirs(1);    //--查找子目录
        int count = o.find(path, fext, lFlag);
        
        ffsco::typeT coo;

        string s;
        stringstream ss;
                
        //--file
        coo = o.co_file();
        lKeyList = coo;
        s.erase();
        ss.str("");
        ss << lKeyList.size();
        s.append("list = " + ss.str() + "\r\n");

    }

    void ffsco::splitStr(std::string src, std::string token, typeT& vect)    
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
    
    void ffsco::getLocalTime(FILETIME Inftime, char* timeBuf)
    {
            FILETIME   ftime;
            SYSTEMTIME   stime;
            FileTimeToLocalFileTime(&Inftime,&ftime);
            FileTimeToSystemTime(&ftime,&stime);
            sprintf(timeBuf,"%04d年%02d月%02d日 %02d:%02d:%02d",stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute,stime.wSecond);

    }

    void ffsco::getLocalTime(time_t ltime, char* timeBuf)
    {
        tm tim =*localtime(&ltime); 
        int day,mon,year,hours, min, sec; 
        day=tim.tm_mday; 
        mon=tim.tm_mon + 1; 
        year=tim.tm_year; 
        hours = tim.tm_hour;
        min = tim.tm_min;
        sec = tim.tm_sec;
        sprintf(timeBuf,"%04d年%02d月%02d日 %02d:%02d:%02d",year+1900,mon,day,hours,min,sec);

    }

    bool ffsco::hasKeywordsInString(string strSrc, ValueList keywordsList, string& keyword)
    {
        for(int i = 0; i < keywordsList.size(); i++)
        {
            if (strstr(strSrc.c_str(),keywordsList[i].c_str()))
            {
                keyword = keywordsList[i];
                return true;
            }
        }
        return false;
    }

};    //--namespace helper_coffs




