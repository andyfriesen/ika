/*
look at misc.h :P
*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "misc.h"

const char* IKA_VERSION = "0.54";

bool IsPowerOf2(uint i)
{
    return (i & (i - 1)) == 0;
}

// http://bob.allegronetwork.com/prog/tricks.html#nextpowerof2
uint NextPowerOf2(uint i)
{
    i--;
    i |= (i >> 1);
    i |= (i >> 2);
    i |= (i >> 4);
    i |= (i >> 8);
    i |= (i >> 16);
    i++;
}

int sgn(int x)
{
    if (x < 0) return -1;
    if (x > 0) return  1;
    return 0;
}

char* va(char* format, ...)
{
    va_list argptr;
    static char str[1024];
    
    va_start(argptr, format);
    vsprintf(str, format, argptr);
    va_end(argptr);
    
    return str;
}

int Random(int min, int max)
{
    int i;
    
    if (max < min)
        swap(max, min);
    
    i = (rand() % (max - min)) + min;
    
    return i;
}

#include <string>
#include <sstream>

const std::string Trim(const std::string& s)
{
    uint start = 0, end = s.length();

    while (s[start] == ' ' && start < s.length()) start++;
    while (s[end - 1] == ' ' && end > 0) end--;
    if (start >= end) 
        return "";
    else
        return s.substr(start, end - start);
}

std::string Lower(const std::string& s)
{
    std::string t(s);

    for (uint i = 0; i < t.length(); i++)
        if (t[i] >= 'A' && t[i] <= 'Z')
            t[i] ^= 32;
    
    return t;
}

std::string Upper(const std::string& s)
{
    std::string t(s);

    for (uint i = 0; i < t.length(); i++)
        if (t[i] >= 'a' && t[i] <= 'z')
            t[i] ^= 32;

    return t;
}

std::string ToString(int i)
{
    std::stringstream s;
    s << i << '\0';
    return s.str();
}

//---------------------------------------
// put this in its own file? --andy

std::string Path::Directory(const std::string& s, const std::string& relativeto)
{
    int p = s.rfind(Path::cDelimiter);
    if (p==std::string::npos) return "";

    std::string sPath = s.substr(0, p + 1);

    // FIXME?  This assumes that relativeto and s are both absolute paths.
    // Or, at the least, that the two paths have the same reference point.
    unsigned int i = 0;
    for (; i < sPath.length(); i++)
        if (i >= relativeto.length() || sPath[i]!=relativeto[i])
            break;        

    p = sPath.rfind(cDelimiter, i);    // go back to the last slash we found

    return p!=std::string::npos ? sPath.substr(p) : sPath;
}

std::string Path::Filename(const std::string& s)
{
    int p = s.rfind(Path::cDelimiter);

    if (p==std::string::npos) return s;
    return s.substr(p + 1);
}

std::string Path::Extension(const std::string& s)
{
    int pos = s.rfind('.');
    if (pos == std::string::npos)
        return "";

    return s.substr(pos + 1);
}

std::string Path::ReplaceExtension(const std::string& s, const std::string& extension)
{
    int pos = s.rfind('.');
    if (pos == std::string::npos)
        return s + "." + extension;

    return s.substr(0, pos + 1) + extension;
}

bool Path::Compare(const std::string& s, const std::string& t)
{
#ifdef WIN32
    return Upper(s) == Upper(t);
#else
    return s == t;
#endif
}
