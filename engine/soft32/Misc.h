#ifndef SOFT32_MISC_H
#define SOFT32_MISC_H

namespace Soft32
{

    struct scanspan {
        unsigned short s, e;
    };

    inline void CreateRect(SDL_Rect& r, int x, int y, int w, int h) {
        r.x = x;
        r.y = y;
        r.w = w;
        r.h = h;
        return;
    }

    inline bool ClipCoordinate(SDL_Surface* s, int x, int y) {
        return ((x >= s->clip_rect.x) && (y >= s->clip_rect.y) && (x < s->clip_rect.x + s->clip_rect.w) && (x < s->clip_rect.y + s->clip_rect.h));
    }

};

#endif