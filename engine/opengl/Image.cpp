#include "SDL/SDL_opengl.h"
#include "Image.h"
#include "common/log.h"

namespace OpenGL
{
    Image::Image(uint tex, int tw, int th, int w, int h)
        : _texture(tex)
        , _texWidth(tw)
        , _texHeight(th)
        , _width(w)
        , _height(h)
    {
    }

    Image::~Image()
    {
        glDeleteTextures(1, &_texture);
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
