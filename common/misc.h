/*
Things that just don't fit anywhere else, but are handy to have around.

  Some things are simple abstractions over common functions, to make everything
  all happy and portable. (kudos to aegis for the advice)
*/
#ifndef MISC_H
#define MISC_H

#include "types.h"

#undef min  // GAAAAAAAAAY
#undef max

template <typename T>
inline void swap(T& a,T& b)
{
    T c;
    c=a;
    a=b;
    b=c;
}

template <typename T>
inline T min(T a, T b)
{
    return a < b ? a : b;
}

template <typename T>
inline T max(T a, T b)
{
    return a > b ? a : b;
}

template <typename T>
inline T clamp(T val, T lower, T upper)
{
    return min(max(lower, val), upper);
}

bool IsPowerOf2(int i);

int sgn(int x);
int Random(int min,int max);
char* va(char* format, ...);

const string Trim(string s);

string Upper(const string& s);

string ToString(int i);

namespace Path
{
    // ifdef and blah blah for platform independance
    static const char cDelimiter='\\';


    // strips the path from the filename, and returns it.
    // If relativeto is not specified or is empty, then the entire path is returned.
    // If it is specified, then the path returned is relative to the path contained therein.
    string Directory(const string& s,const string& relativeto=""); 

    string Filename(const string& s);           // same idea, but just returns the filename

    bool Compare(const string& s, const string& t); // returns true if the two paths are the same.  Disregards case in win32.
};

#endif
