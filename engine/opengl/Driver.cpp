#include <cassert>
#include <vector>
#include <math.h>

#include "SDL/SDL_opengl.h"
//#include "GL/gl.h"

#include "Driver.h"
#include "Image.h"

#include "common/types.h"
#include "common/misc.h"
#include "common/Canvas.h"
#include "common/log.h"

#ifdef WIN32
static void __stdcall glBlendEquationStub(int){}
#else
static void glBlendEquationStub(int){}
#endif

namespace OpenGL
{
#ifndef GL_FUNC_REVERSE_SUBTRACT_EXT
    const uint GL_FUNC_REVERSE_SUBTRACT_EXT = 0x800B;
#endif
#ifndef GL_FUNC_ADD_EXT
    const uint GL_FUNC_ADD_EXT = 0x8006;
#endif

    Driver::Driver(int xres, int yres, int bpp, bool fullScreen, bool doubleSize)
        : _lasttex(0)
        , _xres(xres)
        , _yres(yres)
        , _bpp(bpp)
        , _fullScreen(fullScreen)
        , _doubleSize(doubleSize) // TEST CODE
    {
        if (_doubleSize)
        {
            xres += xres;
            yres += yres;
            glGenTextures(1, &_bufferTex);
        }

        // from this point, xres and yres are the physical resulotion,
        // _xres and _yres are the virtual resolution

        _screen = SDL_SetVideoMode(xres, yres, bpp, SDL_OPENGL | (fullScreen ? SDL_FULLSCREEN : 0));
        if (!_screen)
            throw Video::Exception();

        glShadeModel(GL_SMOOTH);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

#if 0
        gluOrtho2D(0, xres, yres, 0);
#else
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(2.0f / (float)xres, -2.0f / (float)yres, 1.0f);
        glTranslatef(-((float)xres / 2.0f), -((float)yres / 2.0f), 0.0f);
        glViewport(0, 0, xres, yres);
#endif

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);

        glScissor(0, 0, xres, yres);
        glEnable(GL_SCISSOR_TEST);

        glEnable(GL_TEXTURE_2D);

        SDL_ShowCursor(SDL_DISABLE);

#ifdef WIN32
        glBlendEquationEXT = (void (__stdcall *)(int))SDL_GL_GetProcAddress("glBlendEquationEXT");
#else
        glBlendEquationEXT = (void (*)(int))SDL_GL_GetProcAddress("glBlendEquationEXT");
#endif

        if (!glBlendEquationEXT) 
        {
            Log::Write("glBlendEquationEXT not found.  Colour subtraction disabled.");
            glBlendEquationEXT = &glBlendEquationStub;
        }
    }

    Driver::~Driver()
    {
    }

    // Trouble is, SDL nukes the GL context when it switches modes.
    // Which means that our textures go down the crapper. ;_;
    bool Driver::SwitchResolution(int x, int y)
    {
#if 1
        return false;
#else
        if (!SDL_VideoModeOK(x, y, _bpp, SDL_OPENGL | (_fullScreen ? SDL_FULLSCREEN : 0)))
            return false;

        _screen = SDL_SetVideoMode(x, y, _bpp, SDL_OPENGL | (_fullScreen ? SDL_FULLSCREEN : 0));
        if (!_screen)
            throw std::runtime_error("OpenGL Video panic.  No video display!!");

        _xres = x;
        _yres = y;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(2.0f / (float)_xres, -2.0f / (float)_yres, 1.0f);
        glTranslatef(-((float)_xres / 2.0f), -((float)_yres / 2.0f), 0.0f);
        glViewport(0, 0, _xres, _yres);

        return true;
#endif
    }

    // This is far, far too long.  Refactor.
    Image* Driver::CreateImage(Canvas& src)
    {
        /*
            First, if src is 16x16, we figure out if we have a texture that can fit it.
            If we do, we use glSubTexImage2D, update the used region, and fly away on wings
            of silver dust.  If not, we create a 256x256 texture, and do the aforementioned
            glSubTexImage2Ding. (we do NOT, however, fly away on wings of silver dust.  That's
            strictly reserved for when there is an existing texture for us to use.

            oh yeah.  I know this is messy.  Don't give a shit yet.  Prototyping is a bitch
            that way.

            TODO: Generalize this so that we hold onto a bunch of textures for every power-of-two
            size of image that is created. (8x8, 64x64, et cetera)
            Or maybe just group images of equal height togeather.  Then ika would be able to
            leverage texture-sharing for the font as well.
        */

        // disabled for now, due to glitches and things
#ifdef SHARE_TEXTURES
        Texture* blah[255];
        int q = 0;
        for (TextureSet::iterator iter = _textures.begin(); iter != _textures.end(); iter++)
            blah[q++] = *iter;

        if (src.Width() == 16 && src.Height() == 16)
        {
            Texture* tex = 0;
            for (TextureSet::iterator
                iter  = _textures.begin(); 
                iter != _textures.end(); 
                iter++)
            {
                Texture* t = *iter;
                const Point& p = (*iter)->unused;
                if (p.x <= 256 - 16 && p.y <= 256 - 16)
                {
                    tex = *iter;
                    break;
                }
            }

            if (!tex)
            {
                // no texture?  no problem.
                static u32 dummyShit[256 * 256] = {0}; // initialized to 0
                tex = new Texture(0, 256, 256);
                glGenTextures(1, &tex->handle);
                SwitchTexture(tex->handle);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, dummyShit);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                _textures.insert(tex);
            }

            int x = tex->unused.x;
            int y = tex->unused.y;
            tex->unused.x += 16;
            if (tex->unused.x >= 256)
                tex->unused = Point(0, tex->unused.y + 16);

            SwitchTexture(tex->handle);

            src.Flip();
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, src.GetPixels());
            src.Flip();

            const float texcoords[4] = {
                float(x) / 256.0f,      float(y) / 256.0f,
                float(x + 16) / 256.0f, float(y + 16) / 256.0f
            };

            tex->refCount++;
            return new Image(tex, texcoords, 16, 16);
        }
        else
#endif
        {
            bool dealloc;
            RGBA* pixels;
            int texwidth;
            int texheight;

            src.Flip(); // GAY

            if (IsPowerOf2(src.Width()) && IsPowerOf2(src.Height()))
            {
                dealloc = false;    // perfect match
                pixels = src.GetPixels();
                texwidth = src.Width();
                texheight = src.Height();
            }
            else
            // The old way.  One texture for every image.  boooring.  Easy, but boooring.  And possibly slow.
            {
                dealloc = true;
                texwidth  = 1;  while (texwidth < src.Width())  texwidth <<= 1;
                texheight = 1;  while (texheight < src.Height()) texheight <<= 1;

                pixels = new RGBA[texwidth * texheight];
                for (int y = 0; y < src.Height(); y++)
                {
                    memcpy(
                        pixels + (y * texwidth), 
                        src.GetPixels() + (y * src.Width()),
                        src.Width() * sizeof(RGBA));
                }
            }

            uint texture;
            glGenTextures(1, &texture);
            SwitchTexture(texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            src.Flip();

            if (dealloc)
                delete[] pixels;

            const float texCoords[] = { 0, 0, float(src.Width()) / texwidth, float(src.Height()) / texheight };
            Texture* tex = new Texture(texture, texwidth, texheight);
            tex->refCount++;
            return new Image(tex, texCoords, src.Width(), src.Height());
        }
    }

    void Driver::FreeImage(Video::Image* img)
    {
        if (!img)   return;

        SwitchTexture(0);
        
        // Refcount update/cleanup
        Texture* tex = ((OpenGL::Image*)img)->_texture;
#ifdef SHARE_TEXTURES
        if (tex->refCount == 1)
        {
            _textures.erase(tex);
            glDeleteTextures(1, &tex->handle);
            delete tex;
        }
        else
            tex->refCount--;
#else
        glDeleteTextures(1, &tex->handle);
        delete tex;
#endif

        delete (OpenGL::Image*)img;
    }

    void Driver::ClipScreen(int left, int top, int right, int bottom)
    {
        if (left > right)
            swap(left, right);
        if (top > bottom)
            swap(top, bottom);

        // gah.
        // Convert x1,y1-x2,y2 to left,bottom/width,height.

        int width = right - left;
        int height = bottom - top;

        top = min(_yres, _yres - top) - height;

        glScissor(
            left, top,
            width, height
            );
    }

    void Driver::ShowPage()
    {
        if (_doubleSize)
        {
#if 0
            // The easy way
            _xres <<= 1;    _yres <<= 1;                            // Fool the driver for a moment.
            Image* tempImage = GrabImage(0, 0, _xres, _yres);       // Grab a hunk o screen
            ScaleBlitImage(tempImage, 0, 0, _xres * 2, _yres * 2);  // Draw it
            _xres >>= 1;    _yres >>= 1;                            // Restore xres and yres

            FreeImage(tempImage);                                   // Clean up
#else
            // The fast way
            // It's much faster because it bypasses all my gay abstraction crap.
            // Most notably, the allocation of a temporary image, and a lot of
            // pixel/screen space arithmatic that isn't needed.

            uint texW = NextPowerOf2(_xres);
            uint texH = NextPowerOf2(_yres);
            SwitchTexture(_bufferTex);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, _yres, texW, texH, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            float endX = float(_xres) / texW;
            float endY = float(_yres) / texH;

            _xres <<= 1;    _yres <<= 1;

            glBegin(GL_QUADS);
            glTexCoord2f(0, endY); glVertex2i(0, 0);
            glTexCoord2f(endX, endY); glVertex2i(_xres, 0);
            glTexCoord2f(endX, 0); glVertex2i(_xres, _yres);
            glTexCoord2f(0, 0); glVertex2i(0, _yres);
            glEnd();

            _xres >>= 1;    _yres >>= 1;
#endif
        }

        fps.Update();
        SDL_GL_SwapBuffers();
    }

    Video::BlendMode Driver::SetBlendMode(Video::BlendMode bm)
    {
        if (bm == _blendMode)
            return bm;

        // Unset the blend equation if it was previously changed. (it is always changed if the current mode is Video::Subtract)
        if (_blendMode == Video::Subtract)
        {
            glBlendEquationEXT(GL_FUNC_ADD_EXT);
            
            // hack for old buggy ATi drivers. (GAY)
            glBegin(GL_POINTS); glVertex2i(-1, -1); glEnd();
        }

        switch (bm)
        {
        case Video::None:    glDisable(GL_BLEND);     break;
        case Video::Matte:   // TODO: see if we can get GL to do matte?  A shader would do it, but I think that's overkill. :)
        case Video::Normal:  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  glEnable(GL_BLEND); break;
        case Video::Add:     glBlendFunc(GL_ONE, GL_ONE);                        glEnable(GL_BLEND); break;
        case Video::Subtract:
            glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_BLEND);
            break;
        default:
            return _blendMode;
        }

        Video::BlendMode m = _blendMode;
        _blendMode = bm;
        return m;
    }

    // This is, without question *the* method to optimize.
    // At least 20% of the sum total CPU goes into this.
    // Even when the profiler brings the framerate below 1fps, with the AI loop is being executed at 
    // least ten times per render, a fifth of the CPU goes here.
    void Driver::BlitImage(Video::Image* i, int x, int y)
    {
        OpenGL::Image* img = (Image*)i;
        // VC7 won't inline these, because they're virtual.
        int w = img->_width;//Width();
        int h = img->_height;//Height();

        const float* texCoords = img->_texCoords;

        SwitchTexture(img->_texture->handle);
        glBegin(GL_QUADS);
        glTexCoord2f(texCoords[0], texCoords[3]);   glVertex2i(x, y);
        glTexCoord2f(texCoords[2], texCoords[3]);   glVertex2i(x + w, y);
        glTexCoord2f(texCoords[2], texCoords[1]);   glVertex2i(x + w, y + h);
        glTexCoord2f(texCoords[0], texCoords[1]);   glVertex2i(x, y + h);
        glEnd();
    }

    void Driver::ScaleBlitImage(Video::Image* i, int x, int y, int w, int h)
    {
        Image* img = (Image*)i;

        const float* texCoords = img->_texCoords;
        SwitchTexture(img->_texture->handle);
        glBegin(GL_QUADS);
        glTexCoord2f(texCoords[0], texCoords[3]);   glVertex2i(x, y);
        glTexCoord2f(texCoords[2], texCoords[3]);   glVertex2i(x + w, y);
        glTexCoord2f(texCoords[2], texCoords[1]);   glVertex2i(x + w, y + h);
        glTexCoord2f(texCoords[0], texCoords[1]);   glVertex2i(x, y + h);
        glEnd();
    }

    void Driver::DistortBlitImage(Video::Image* i, int x[4], int y[4])
    {
        Image* img = (Image*)i;

        const float* texCoords = img->_texCoords;
        const float texX[] = { texCoords[0], texCoords[2], texCoords[2], texCoords[0] };
        const float texY[] = { texCoords[3], texCoords[3], texCoords[1], texCoords[1] };

        SwitchTexture(img->_texture->handle);
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++)
        {
            glTexCoord2f(texX[i], texY[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
    }

    void Driver::TileBlitImage(Video::Image* i, int x, int y, int w, int h, float scalex, float scaley)
    {
        Image* img = static_cast<Image*>(i);
        Texture* tex = img->_texture;

        float texX = float(w) / img->Width()  * scalex;
        float texY = float(h) / img->Height() * scaley;

        // simplest case.  We can draw one big textured quad for the whole thing.
        if (tex->width == img->_width && tex->height == img->_height)
        {

            SwitchTexture(img->_texture->handle);
            glBegin(GL_QUADS);
            glTexCoord2f(0,    texY);   glVertex2i(x, y);
            glTexCoord2f(texX, texY);   glVertex2i(x + w, y);
            glTexCoord2f(texX, 0);      glVertex2i(x + w, y + h);
            glTexCoord2f(0,    0);      glVertex2i(x, y + h);
            glEnd();
        }
        else
        // backup: Draw a grid of textured quads.
        // This isn't so bad, really, but we could do another optimization
        // and see if we could get away with doing some big long horizontal
        // or vertical strips.  Something for another day.
        {
            glPushAttrib(GL_SCISSOR_BIT);
            ClipScreen(x, y, x + w, y + h);

            float imgWidth = float(img->_width) * scalex;
            float imgHeight = float(img->_height) * scaley;
        
            SwitchTexture(img->_texture->handle);

            const float* texCoords = img->_texCoords;
    
            glBegin(GL_QUADS);
            for (float curY = float(y); curY < y + h; curY += imgWidth)
            {
                for (float curX = float(x); curX < x + w; curX += imgHeight)
                {
                    glTexCoord2f(texCoords[0], texCoords[3]);   glVertex2f(curX, curY);
                    glTexCoord2f(texCoords[2], texCoords[3]);   glVertex2f(curX + imgWidth, curY);
                    glTexCoord2f(texCoords[2], texCoords[1]);   glVertex2f(curX + imgWidth, curY + imgHeight);
                    glTexCoord2f(texCoords[0], texCoords[1]);   glVertex2f(curX, curY + imgHeight);
                }
            }
            glEnd();

            glPopAttrib();
        }
    }

    void Driver::TintBlitImage(Video::Image* img, int x, int y, u32 tint)
    {
        glColor4ubv((u8*)&tint);
        ::OpenGL::Driver::BlitImage(img, x, y); // inline this?  Pretty please? :(
        glColor4ub(255, 255, 255, 255);
    }

    void Driver::TintDistortBlitImage(Video::Image* i, int x[4], int y[4], u32 colour[4])
    {
        Image* img = (Image*)i;

        const float* texCoords = img->_texCoords;
        const float texX[] = { texCoords[0], texCoords[2], texCoords[2], texCoords[0] };
        const float texY[] = { texCoords[3], texCoords[3], texCoords[1], texCoords[1] };

        SwitchTexture(img->_texture->handle);
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++)
        {
            glColor4ubv((u8*)&colour[i]);
            glTexCoord2f(texX[i], texY[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
        glColor4ub(255, 255, 255, 255);
    }

    void Driver::DrawPixel(int x, int y, u32 colour)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);

        glBegin(GL_POINTS);
        glVertex2f(float(x) + 0.375f, float(y) + 0.375f);
        glEnd();

        glEnable(GL_TEXTURE_2D);
        glColor4ub(255, 255, 255, 255);
    }

    void Driver::DrawLine(int x1, int y1, int x2, int y2, u32 colour)
    {
        glPushMatrix();
        glTranslatef(0.375f, 0.375f, 0);

        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);
        glBegin(GL_LINES);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
        glEnd();
        glEnable(GL_TEXTURE_2D);
        glColor4ub(255, 255, 255, 255);

        glPopMatrix();
    }

    void Driver::DrawRect(int x1, int y1, int x2, int y2, u32 colour, bool filled)
    {
        glPushMatrix();
        glTranslatef(0.375f, 0.375f, 0);

        //glDisable(GL_TEXTURE_2D);
		SwitchTexture(0);
        glColor4ubv((u8*)&colour);
        if (filled)
        {
            x2++;
            y2++;
            glBegin(GL_QUADS);
        }
        else
        {
            //y1++;
            glBegin(GL_LINE_LOOP);
        }
        
        //y2++;

        glVertex2i(x1, y1);
        glVertex2i(x2, y1);
        glVertex2i(x2, y2);
        glVertex2i(x1, y2);
        glEnd();
        glColor4f(1, 1, 1, 1);
        //glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    void Driver::DrawEllipse(int cx, int cy, int rx, int ry, u32 colour, bool filled)
    {
        glPushMatrix();
        glTranslatef(0.375f, 0.375f, 0);

        // Kudos to Dante for coding this.
        if(rx==0 || ry==0) return;

        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);
        rx=abs(rx);	ry=abs(ry);
        if(filled)
        {
            if(rx>ry)
            {
                float rx2=rx*rx*1.0f, ry2=ry*ry*1.0f;
                float ry2rx2=ry2/rx2*1.0f, rxry=rx/ry*1.0f;
                float sfac=rxry*0.2f*1.0f;
                float i=0.0f, curr=0.0f;
                glBegin(GL_TRIANGLE_STRIP); // left part
                glVertex2i(cx-rx, cy);
                for(i=-rx+1*1.0f; i<=-1; i+=(rx-fabsf(i))*sfac+1)
                {
                    curr=sqrtf(ry2-i*i*ry2rx2);
                    glVertex2f(cx+i, cy-curr); glVertex2f(cx+i, cy+curr);
                }
                glVertex2i(cx, cy-ry); glVertex2i(cx, cy+ry);
                glEnd();
                glBegin(GL_TRIANGLE_STRIP); // right part
                glVertex2i(cx+rx, cy);
                for(i=rx-1*1.0f; i>=1; i-=(rx-fabsf(i))*sfac+1)
                {
                    curr=sqrtf(ry2-i*i*ry2rx2);
                    glVertex2f(cx+i, cy-curr); glVertex2f(cx+i, cy+curr);
                }
                glVertex2i(cx, cy-ry); glVertex2i(cx, cy+ry);
                glEnd();
            }
            else // ry>rx
            {
                float rx2=rx*rx*1.0f, ry2=ry*ry*1.0f;
                float rx2ry2=rx2/ry2*1.0f, ryrx=ry/rx*1.0f;
                float sfac=ryrx*0.2f*1.0f;
                float i=0.0f, curr=0.0f;
                glBegin(GL_TRIANGLE_STRIP); // upper part
                glVertex2i(cx, cy-ry);
                for(i=-ry+1*1.0f; i<=-1; i+=(ry-fabsf(i))*sfac+1)
                {
                    curr=sqrtf(rx2-i*i*rx2ry2);
                    glVertex2f(cx-curr, cy+i); glVertex2f(cx+curr, cy+i);
                }
                glVertex2i(cx-rx, cy); glVertex2i(cx+rx, cy);
                glEnd();
                glBegin(GL_TRIANGLE_STRIP);  //lower part
                glVertex2i(cx, cy+ry);
                for(i=ry-1*1.0f; i>=1; i-=(ry-fabsf(i))*sfac+1)
                {
                    curr=sqrtf(rx2-i*i*rx2ry2);
                    glVertex2f(cx-curr, cy+i); glVertex2f(cx+curr, cy+i);
                }
                glVertex2i(cx-rx, cy); glVertex2i(cx+rx, cy);
                glEnd();
            }
        }
        else //outlined
        {
            if(rx>ry)
            {
                double rx2=rx*rx*1.0, ry2=ry*ry*1.0;
                double ry2rx2=ry2/rx2*1.0, rxry=rx/(ry*1.0);
                double sfac=rxry*0.2;
                double i=rx*1.0, curr=0.0;
                double lastcurr=curr, lasti=i;
                glBegin(GL_LINES);
                for(; i>=0; i-=(rx-i)*sfac+1)
                {
                    curr=sqrt(ry2-i*i*ry2rx2);
                    glVertex2d(cx+lasti, cy+lastcurr); glVertex2d(cx+i, cy+curr);
                    glVertex2d(cx+lasti, cy-lastcurr); glVertex2d(cx+i, cy-curr);
                    glVertex2d(cx-lasti, cy+lastcurr); glVertex2d(cx-i, cy+curr);
                    glVertex2d(cx-lasti, cy-lastcurr); glVertex2d(cx-i, cy-curr);
                    lastcurr=curr;
                    lasti=i;
                }
                glVertex2d(cx+lasti, cy+lastcurr); glVertex2i(cx, cy+ry);
                glVertex2d(cx+lasti, cy-lastcurr); glVertex2i(cx, cy-ry);
                glVertex2d(cx-lasti, cy+lastcurr); glVertex2i(cx, cy+ry);
                glVertex2d(cx-lasti, cy-lastcurr); glVertex2i(cx, cy-ry);
                glEnd();
            }
            else // ry>rx
            {
                double rx2=rx*rx*1.0, ry2=ry*ry*1.0;
                double rx2ry2=rx2/ry2*1.0, ryrx=ry/(rx*1.0);
                double sfac=ryrx*0.2;
                double i=ry*1.0, curr=0.0;
                double lastcurr=curr, lasti=i;
                glBegin(GL_LINES);
                for(; i>=0; i-=(ry-i)*sfac+1)
                {
                    curr=sqrt(rx2-i*i*rx2ry2);
                    glVertex2d(cx+lastcurr, cy+lasti); glVertex2d(cx+curr, cy+i);
                    glVertex2d(cx-lastcurr, cy+lasti); glVertex2d(cx-curr, cy+i);
                    glVertex2d(cx+lastcurr, cy-lasti); glVertex2d(cx+curr, cy-i);
                    glVertex2d(cx-lastcurr, cy-lasti); glVertex2d(cx-curr, cy-i);
                    lastcurr=curr;
                    lasti=i;
                }
                glVertex2d(cx+lastcurr, cy+lasti); glVertex2i(cx+rx, cy);
                glVertex2d(cx-lastcurr, cy+lasti); glVertex2i(cx-rx, cy);
                glVertex2d(cx+lastcurr, cy-lasti); glVertex2i(cx+rx, cy);
                glVertex2d(cx-lastcurr, cy-lasti); glVertex2i(cx-rx, cy);
                glEnd();
            }
        }
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    void Driver::DrawTriangle(int x[3], int y[3], u32 colour[3])
    {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; i++)
        {
            glColor4ubv((u8*)&colour[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
    }

    Image* Driver::GrabImage(int x1, int y1, int x2, int y2)
    {
        // Way fast, since there are no pixels going from the video card to system memory.
        // They just get copied from the screen to a texture (which also video memory)

        //y1 = _yres - y1;
        //y2 = _yres - y2;

        // clip
        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);
        //x1 = clamp(x1, 0, _xres);        x2 = clamp(x2, 0, _xres);
        //y1 = clamp(y1, 0, _yres);        y2 = clamp(y2, 0, _yres);
        int w = x2 - x1;
        int h = y2 - y1;
        if (w < 0 || h < 0) return 0;

        uint texwidth = NextPowerOf2(w);
        uint texheight = NextPowerOf2(h);
        uint handle;
        glGenTextures(1, &handle);
        SwitchTexture(handle);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x1, _yres - y2, texwidth, texheight, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const float texCoords[] = { 0, 0, float(w) / texwidth, float(h) / texheight };
        Texture* tex = new Texture(handle, texwidth, texheight);
        tex->refCount++;
        return new Image(tex, texCoords, w, h);
    }

    Canvas* Driver::GrabCanvas(int x1, int y1, int x2, int y2)
    {
        y1 = _yres - y1;
        y2 = _yres - y2;

        // clip
        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);
        x1 = clamp(x1, 0, _xres);        x2 = clamp(x2, 0, _xres);
        y1 = clamp(y1, 0, _yres);        y2 = clamp(y2, 0, _yres);
        int w = x2 - x1;
        int h = y2 - y1;
        if (w < 0 || h < 0) return 0;

        Canvas* c = new Canvas(w, h);
        glReadPixels(x1, y1, w, h, GL_RGBA, GL_UNSIGNED_BYTE, c->GetPixels());
        c->Flip();
        return c;
    }

    Point Driver::GetResolution() const
    {
        return Point(_xres, _yres);
    }

    int Driver::GetFrameRate() const
    {
        return fps.FPS();
    }

    inline void Driver::SwitchTexture(uint tex)
    {
        if (tex == _lasttex)
            return;

        _lasttex = tex;
        glBindTexture(GL_TEXTURE_2D, tex);
    }
};
