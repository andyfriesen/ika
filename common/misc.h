/*
Things that just don't fit anywhere else, but are handy to have around.

  Some things are simple abstractions over common functions, to make everything
  all happy and portable. (kudos to aegis for the advice)
*/
#ifndef MISC_H
#define MISC_H

#include <string>
using std::string;

template <typename T>
inline void swap(T& a,T& b)
{
    T c;
    c=a;
    a=b;
    b=c;
}

int sgn(int x);
int Random(int min,int max);
char* va(char* format, ...);
void ChangeDirectory(const char* s);
char* GetCurrentDirectory();

const string Trim(string s);

namespace Path
{
    // ifdef and blah blah for platform independance
    static const char cDelimiter='\\';


    // strips the path from the filename, and returns it.
    // If relativeto is not specified or is empty, then the entire path is returned.
    // If it is specified, then the path returned is relative to the path contained therein.
    string Directory(const string& s,const string& relativeto=""); 

    string Filename(const string& s);           // same idea, but just returns the filename
};

#endif