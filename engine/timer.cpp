#include "timer.h"
#include <SDL/SDL.h>

int GetTime()
{
    return SDL_GetTicks() * timeRate / 1000;
}
