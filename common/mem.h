
#pragma once

#ifdef _DEBUG


void* operator new(unsigned int nSize,const char* sFile,int nLine);
void operator delete(void* p);

#define DEBUG_NEW new(__FILE__,__LINE__)
#define new DEBUG_NEW
#endif

void DumpList();
