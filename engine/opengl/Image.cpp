//#include "SDL/SDL_opengl.h"
#include "Image.h"
#include "common/log.h"

#include <algorithm>

namespace OpenGL
{
    Image::Image(Texture* texture, const float texCoords[4], int width, int height)
        : _texture(texture)
        , _width(width)
        , _height(height)
    {
        for (uint i = 0; i < 4; i++)
            _texCoords[i] = texCoords[i];
    }

    Image::~Image()
    {
        //glDeleteTextures(1, &_texture);
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
