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
    _chdir(s);
}

char* GetCurrentDirectory()
{
    static char s[255];
    _getcwd(s,255);
    return s;
}

#include <windows.h>
