/*
This also comprises of some handy dandy debugging tricks I've accumulated.
*/

#ifndef LOG_H
#define LOG_H

#include <windows.h>

//#ifdef _DEBUG
//#define _CALLBACK
//#endif

extern void log(const char *s,...);
#ifdef WIN32
extern void logdderr(HRESULT dderr);
#endif
extern void logp(char *s,...);
extern void logok(void);
extern void initlog(const char* s);

#ifdef _CALLBACK
#include <string>
extern void callback_log(const char* s,...);

class CDebuggingthing
{
    static std::string callback;
    int len;
public:
    CDebuggingthing(const char* s);
    ~CDebuggingthing();
};

#define CDEBUG(x) CDebuggingthing __debugthing(x)
#else
#define CDEBUG(x)
#endif

#endif