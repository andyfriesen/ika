#include "timer.h"

#include <windows.h>
#include <mmsystem.h>

int GetTime()
{
    return timeGetTime() / 10;
}