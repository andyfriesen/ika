#ifndef SOFT32_IMAGE_H
#define SOFT32_IMAGE_H

#include "Driver.h"
#include "video/Image.h"

namespace Soft32
{
    /// Encapsulates an image by putting it on a SDL surface.
    /// Most of the work is actually done in the driver.  This is little more than a container.
    class Image : public Video::Image
    {
        friend class Soft32::Driver;
    protected:
        SDL_Surface* _surface;
        int _width, _height;

        Image(SDL_Surface* s, int w, int h);


    public:
        ~Image();

        virtual int Width();
        virtual int Height();
    };
};

#endif
