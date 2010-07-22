#pragma once

#include "timer.h"

struct FPSCounter {
    FPSCounter()
        : frames(0)
        , fps(0)
        , count(0)
        , lastTime(GetTime())
    {}

    inline int FPS() const {
        return fps; 
    }

    void Update();

private:
    int frames;
    int fps;
	int count;    
    int lastTime;
};
