/*
timer.h

  This is based loosely on VERGE v2.6's timer code, which is loosely based on vecna's winverge 1 code.
*/

#ifndef TIMER_H
#define TIMER_H

#include <windows.h>
#include <mmsystem.h>

class Timer
{
    UINT curtimer;
public:
    int t;		// omni-purpose timer variable
    int systime;	// ditto, but don't write to this!
    
    bool Init(int Hz);
    void Shutdown();
    ~Timer();
};

#endif