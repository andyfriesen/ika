#include "FPSCounter.h"
#include "timer.h"

void FPSCounter::Update() {
    int t = GetTime();
    count += t - lastTime;
    lastTime = t;

    ++frames;

    if (count > timeRate) {
        count -= timeRate;
        fps = frames;
        frames = 0;
    }
}
