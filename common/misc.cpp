/*
look at misc.h :P
*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "misc.h"

int sgn(int x)
{
    if (x<0) return -1;
    if (x>0) return  1;
    return 0;
}

char* va(char* format, ...)
{
    va_list argptr;
    static char string[1024];
    
    va_start(argptr, format);
    vsprintf(string, format, argptr);
    va_end(argptr);
    
    return string;
}

int Random(int min,int max)
{
    int i;
    
    if (max<min)
        swap(max,min);
    
    i=(rand()%(max-min))+min;
    
    return i;
}

#include <direct.h>

void ChangeDirectory(const char* s)
{
//    _chdir(s);
}

char* GetCurrentDirectory()
{
/*    static char s[255];
    _getcwd(s,255);
    return s;*/return 0;
}

#include <string>
#include <strstream>

const string Trim(string s)
{
    for (unsigned int i=0; i<s.length(); i++)
    {
        if (s[i]!=' ')
        {
            s=s.substr(i);
            break;
        }
    }

    for (unsigned int j=s.length()-1; j; j--)
    {
        if (s[j]!=' ')
        {
            s=s.substr(0,j+1);
            break;
        }
    }

    return s;
}

string Upper(const string& s)
{
    string t=s;

    for (unsigned int i=0; i<s.length(); i++)
        if (t[i]>='a' && t[i]<='z')
            t[i]^=32;

    return t;
}

string ToString(int i)
{
    std::strstream s;
    s << i << '\0';
    return s.str();
}

//---------------------------------------
// put this in its own file? --andy

string Path::Directory(const string& s,const string& relativeto)
{
/*    struct Local
    {
        static inline string Next(const string& s,unsigned int& pos)
        {
            unsigned int startpos=pos;
            for (; pos<s.length(); pos++)
            {
                if (s[pos]==Path::cDelimiter)
                    return s.substr(startpos,++pos);
            }
            return s;
        }
    };
*/

    int p=s.rfind(Path::cDelimiter);
    if (p==string::npos) return s;

    string sPath=s.substr(0,p+1);

    // FIXME?  This assumes that relativeto and s are both absolute paths.
    // Or, at the least, that the two paths have the same reference point.
    unsigned int i=0;
    for (; i<sPath.length(); i++)
        if (i>=relativeto.length() || sPath[i]!=relativeto[i])
            break;        

    p=sPath.rfind(cDelimiter,i);    // go back to the last slash we found

    return p!=string::npos ? sPath.substr(p) : sPath;
}

string Path::Filename(const string& s)
{
    int p=s.rfind(Path::cDelimiter);

    if (p==string::npos) return s;
    return s.substr(p+1);
}
