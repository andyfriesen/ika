#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

class FPSCounter
{
private:
    int frames;
    int count;
    int fps;
    int lasttime;

public:
    FPSCounter();

    inline int FPS() const { return fps; }

    void Update();
};

#endif
