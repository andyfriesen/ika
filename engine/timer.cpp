#include "timer.h"
#include "log.h"
#include "sound.h"	// Don't like it.

void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);

bool Timer::Init(int Hz)
{
    Shutdown();
    
    t=0;
    systime=0;
   
    curtimer=timeSetEvent(1000/Hz,0,(LPTIMECALLBACK)TimeProc,(DWORD)this,TIME_PERIODIC);
    
    if (!curtimer) return false;
    return true;
}

void Timer::Shutdown()
{
    if (curtimer)
        timeKillEvent(curtimer);
    curtimer=0;
}

Timer::~Timer()
{
    Shutdown();
}

void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
    Timer* This=(Timer*)dwUser;
    This->systime++;
    This->t++;
    sfxUpdate();
}
