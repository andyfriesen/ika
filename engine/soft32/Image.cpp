#if 0
#include "SDL/SDL.h"
#include "Image.h"

namespace Soft32
{
    Image::Image(SDL_Surface* s, int w, int h)
        : _surface(s)
        , _width(w)
        , _height(h)
    {
    }

    Image::~Image()
    {
        if (_surface != NULL) {
            SDL_FreeSurface(_surface);
            _surface = NULL;
        }
    }

    int Image::Width()
    {
        return _width;
    }

    int Image::Height()
    {
        return _height;
    }
}
#endif
