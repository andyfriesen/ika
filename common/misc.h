/*
Things that just don't fit anywhere else, but are handy to have around.

  Some things are simple abstractions over common functions, to make everything
  all happy and portable. (kudos to aegis for the advice)
*/
#ifndef MISC_H
#define MISC_H

//#include <windows.h>

template <typename T>
void swap(T& a,T& b)
{
    T c;
    c=a;
    a=b;
    b=c;
}

/*template <typename T>
T& min(T& a,T& b)
{
    return a<b?a:b;
}

template <typename T>
T& max(T& a,T& b)
{
    return a<b?b:a;
}*/

int sgn(int x);
int Random(int min,int max);
char* va(char* format, ...);
void ChangeDirectory(const char* s);
char* GetCurrentDirectory();

#endif