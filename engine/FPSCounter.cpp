#include "FPSCounter.h"
#include "timer.h"

FPSCounter::FPSCounter()
    : frames(0)
    , fps(0)
    , count(0)
    , lasttime(GetTime())
{
}

void FPSCounter::Update()
{
    int t = GetTime();
    count += t - lasttime;
    lasttime = t;

    frames++;

    if (count > timeRate)
    {
        count -= timeRate;
        fps = frames;
        frames = 0;
    }
}
