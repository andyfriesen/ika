/*
Things that just don't fit anywhere else, but are handy to have around.

  Some things are simple abstractions over common functions, to make everything
  all happy and portable. (kudos to aegis for the advice)
*/
#ifndef MISC_H
#define MISC_H

#include <string>

template <typename T>
void swap(T& a,T& b)
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

const std::string Trim(std::string s);

#endif