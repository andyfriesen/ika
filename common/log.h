/*
	This also comprises of some handy dandy debugging tricks I've accumulated.
*/

#ifndef LOG_H
#define LOG_H

#include <windows.h>

#ifdef _DEBUG
//#define _CALLBACK
#endif

extern void log(const char *s,...);
#ifdef WIN32
extern void logdderr(HRESULT dderr);
#endif
extern void logp(char *s,...);
extern void logok(void);
extern void initlog(const char* s);

#ifdef _CALLBACK
extern void callback_log(const char* s,...);

class CDebuggingthing
{
	static char callback[1024];
public:
	int len;
	CDebuggingthing(const char* s)
	{
		len=strlen(s);
		strcat(callback,"->");
		strcat(callback,s);
		callback_log("  %s",callback);
	}
	~CDebuggingthing()
	{
		callback[strlen(callback)-len-2]=0;
		callback_log("\t\t%s",callback);
	}
};

#define CDEBUG(x) CDebuggingthing __debugthing(x)
#else
#define CDEBUG(x)
#endif

#endif