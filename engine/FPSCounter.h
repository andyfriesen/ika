#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

struct FPSCounter {
    FPSCounter();

    inline int FPS() const { return fps; }

    void Update();

private:
    int frames;
    int count;
    int fps;
    int lasttime;
};

#endif
