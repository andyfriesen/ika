// log.cpp
#ifdef WIN32
#include <windows.h>
#include <ddraw.h>
#endif
#include <stdio.h>
#include <stdarg.h>

#include "log.h"

// nuke this to disable all logging
#define LOG_ENABLE

int logging=0;

#ifdef _CALLBACK
std::string CDebuggingthing::callback;

CDebuggingthing::CDebuggingthing(const char* s)
{
    len=callback.length();
    callback=callback+"->"+s;
    
    FILE* f=fopen("callback.log","a");
    fprintf(f,"  %s\n",callback.c_str());
    fclose(f);
}

CDebuggingthing::~CDebuggingthing()
{
    callback.erase(len);

    FILE* f=fopen("callback.log","a");
    fprintf(f,"\t\t%s\n",callback.c_str());
    fclose(f);
}

#endif

char logname[255];

void log(const char *s,...)
{
#ifdef LOG_ENABLE
	if (!logging) return;
	if (!strlen(logname)) return;

	char tempbuf[1024];
	va_list lst;
	
	va_start(lst,s);
	vsprintf(tempbuf,s,lst);
	va_end(lst);
	
	FILE *logfile;
	
	logfile=fopen(logname,"a");
	fprintf(logfile,"%s\n",tempbuf);
	fflush(logfile);
	fclose(logfile);
#endif
}

#ifdef WIN32
void logdderr(HRESULT dderr)
{
#ifdef LOG_ENABLE
	if (!logging) return;
	switch (dderr)
	{
	case DD_OK: break;
	case DDERR_SURFACELOST:     log("DDerr: SURFACELOST");			break;
	case DDERR_INVALIDRECT:     log("DDerr: INVALIDRECT");			break;
	case DDERR_WASSTILLDRAWING: log("DDerr: WASSTILLDRAWING");		break;
	case DDERR_INVALIDCLIPLIST: log("DDerr: INVALIDCLIPLIST");		break;
	case DDERR_INVALIDOBJECT:   log("DDerr: INVALIDOBJECT");		break;
	case DDERR_INVALIDPARAMS:   log("DDerr: INVALIDPARAMS");		break;
	case DDERR_NOALPHAHW:       log("DDerr: NOALPHAHW");			break;
	case DDERR_NOBLTHW:         log("DDerr: NOBLTHW");				break;
	case DDERR_SURFACEBUSY:     log("DDerr: SURFACEBUSY");			break;
	case DDERR_UNSUPPORTED:     log("DDerr: UNSUPPORTED");			break;
	case DDERR_GENERIC:         log("DDerr: GENERIC");				break;
	default: log("DDerr: Unknown DirectDraw Error");				break;
	}
#endif
}
#endif

void logp(char *s,...)
{
#ifdef LOG_ENABLE
	if (!logging) return;

	char tempbuf[1024];
	va_list lst;
	
	va_start(lst,s);
	vsprintf(tempbuf,s,lst);
	va_end(lst);
	
	FILE *logfile;
		
	logfile=fopen(logname,"a");
	fprintf(logfile,"%s...",tempbuf);
	fflush(logfile);
	fclose(logfile);
#endif
}

void logok(void)
{
#ifdef LOG_ENABLE
	log("OK.");
#endif
}

void initlog(const char* s)
{
#ifdef LOG_ENABLE
	static int i=0;
	if (i)
	{
		log("Wtf?!?!");
		return;
	}
	
	strcpy(logname,s);
	
	i=1;
	logging=1;
	remove(logname);
	remove("callback.log");
#endif
}

