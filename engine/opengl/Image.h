#ifndef OPENGL_IMAGE_H
#define OPENGL_IMAGE_H

#include "Driver.h"
#include "video/Image.h"

namespace OpenGL
{
    class Driver;

    /// Encapsulates an image by putting it on a texture.
    /// Most of the work is actually done in the driver.  This is little more than a container.
    class Image : public Video::Image
    {
        friend class OpenGL::Driver;

    protected:
        uint _texture;
        int _texWidth, _texHeight;
        int _width, _height;
        bool _flip;                         // FUCKING RETARDED HACK BECAUSE OPENGL WAS DESIGNED BY APES.  (if true, we invert texture coords when blitting)

        Image(uint tex, int tw, int th, int w, int h);
        ~Image();       // protected for a reason.  Use Driver::FreeImage to nuke it.

    public:
        virtual int Width();
        virtual int Height();
    };
};

#endif
