/*
	Things that just don't fit anywhere else, but are handy to have around.

	Some things are simple abstractions over common functions, to make everything
	all happy and portable. (kudos to aegis for the advice)
*/
#ifndef MISC_H
#define MISC_H

#include <windows.h>

int sgn(int x);

template <typename T>
void swap(T& a,T& b)
{	T c;
	c=a;
	a=b;	b=c;
}

int Random(int min,int max);
char* va(char* format, ...);
void ChangeDirectory(const char* s);
char* GetCurrentDirectory();
RECT MakeRect(int x1,int y1,int x2,int y2);

#endif