#if 0
#ifndef SOFT32_IMAGE_H
#define SOFT32_IMAGE_H

#include "Driver.h"
#include "video/Image.h"

namespace Soft32
{
    /// Encapsulates an image by putting it on a SDL surface.
    /// Most of the work is actually done in the driver.  This is little more than a container.
    struct Image : Video::Image {
        friend struct Soft32::Driver;
        virtual int Width();
        virtual int Height();

    private:
        SDL_Surface* _surface;
        int _width, _height;

        Image(SDL_Surface* s, int w, int h);
        ~Image();
    };
};

#endif
#endif