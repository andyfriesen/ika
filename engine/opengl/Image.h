#ifndef OPENGL_IMAGE_H
#define OPENGL_IMAGE_H

#include "video/Image.h"

namespace OpenGL
{
    struct Driver;
    struct Texture;

    /// Encapsulates an image by putting it on a texture.
    /// Most of the work is actually done in the driver.  This is little more than a container.
    struct Image : Video::Image {
        friend struct OpenGL::Driver;

        virtual int Width();
        virtual int Height();

    private:
        Texture* _texture;
        float _texCoords[4];               // Two x/y pairs.  Just like glTexCoord2dv is expecting
        int _width, _height;

        Image(Texture* texture, const float texCoords[4], int width, int height);
        ~Image();       // protected for a reason.  Use Driver::FreeImage to nuke it.
    };
};

#endif
