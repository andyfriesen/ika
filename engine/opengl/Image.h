#ifndef OPENGL_IMAGE_H
#define OPENGL_IMAGE_H

#include "Driver.h"
#include "video/Image.h"

namespace OpenGL
{
    /// Encapsulates an image by putting it on a texture.
    /// Most of the work is actually done in the driver.  This is little more than a container.
    class Image : public Video::Image
    {
        friend class OpenGL::Driver;

    protected:
        uint _texture;
        int _texWidth, _texHeight;
        int _width, _height;

        Image(uint tex, int tw, int th, int w, int h);

    public:
        ~Image();

        virtual int Width();
        virtual int Height();
    };
};

#endif
