#include <cassert>

#include "SDL/SDL_opengl.h"

#include "Driver.h"
#include "image.h"

#include "common/types.h"
#include "common/Canvas.h"

namespace OpenGL
{
    Driver::Driver(int xres, int yres, int bpp, bool fullscreen)
        : _lasttex(0)
        , _xres(xres)
        , _yres(yres)
    {
        _screen = SDL_SetVideoMode(xres, yres, bpp, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0));
        if (!_screen)
            throw Video::Exception();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        gluOrtho2D(0, _xres, _yres, 0);
        glClearColor(0, 0, 0, 0);

        glDisable(GL_DEPTH_TEST);

        glScissor(0, 0, _xres, _yres);
        glEnable(GL_SCISSOR_TEST);
    }

    Driver::~Driver()
    {
    }

    Video::Image* Driver::CreateImage(Canvas& src)
    {
        bool dealloc;
        RGBA* pixels;
        int texwidth;
        int texheight;

        if (IsPowerOf2(src.Width()) && IsPowerOf2(src.Height()))
        {
            dealloc = false;    // perfect match
            pixels = src.GetPixels();
            texwidth = src.Width();
            texheight = src.Height();
        }
        else
        {
            dealloc = true;
            texwidth  = 1;  while (texwidth < src.Width())  texwidth <<= 1;
            texheight = 1;  while (texheight < src.Height()) texheight <<= 1;

            pixels = new RGBA[texwidth * texheight];
            for (int y = 0; y < src.Height(); y++)
            {
                memcpy(
                    pixels + (y * src.Width()), 
                    src.GetPixels() + (y * src.Width()),
                    src.Width() * sizeof(RGBA));
            }
        }

        uint texture;
        glGenTextures(1, &texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (dealloc)
            delete[] pixels;

        return new Image(texture, texwidth, texheight, src.Width(), src.Height());
    }

    void Driver::FreeImage(Video::Image* img)
    {
        delete img;
    }

    void Driver::ShowPage()
    {
        SDL_GL_SwapBuffers();
    }

    void Driver::BlitImage(Video::Image* img, int x, int y, bool transparent)
    {
        ScaleBlitImage(img, x, y, img->Width(), img->Height(), transparent);
    }

    void Driver::ScaleBlitImage(Video::Image* i, int x, int y, int w, int h, bool transparent)
    {
        Image* img = (Image*)i;

        float texX = 1.0f * img->_width / img->_texWidth;
        float texY = 1.0f * img->_height / img->_texHeight;

        if (transparent)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        SwitchTexture(img->_texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);          glVertex2i(x, y);
        glTexCoord2f(texX, 0);      glVertex2i(x + w, y);
        glTexCoord2f(texX, texY);   glVertex2i(x + w, y + h);
        glTexCoord2f(0, texY);      glVertex2i(x, y + h);
        glEnd();
    }

    Point Driver::GetResolution() const
    {
        return Point(_xres, _yres);
    }

    void Driver::SwitchTexture(uint tex)
    {
        if (tex == _lasttex)
            return;

        _lasttex = tex;
        glBindTexture(GL_TEXTURE_2D, tex);
    }
};