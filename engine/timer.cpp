#include "timer.h"
#include <SDL/SDL.h>

int GetTime()
{
    return SDL_GetTicks() / 10;
}