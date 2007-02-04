#include <windows.h>

#include "SDL/SDL_opengl.h"

#include "Driver.h"
#include "Image.h"

#include "common/types.h"
#include "common/utility.h"
#include "common/Canvas.h"
#include "common/log.h"

static void IKA_STDCALL glBlendEquationStub(int) {}

namespace OpenGL {

#ifndef GL_FUNC_REVERSE_SUBTRACT_EXT
    const uint GL_FUNC_REVERSE_SUBTRACT_EXT = 0x800B;
#endif
#ifndef GL_FUNC_ADD_EXT
    const uint GL_FUNC_ADD_EXT = 0x8006;
#endif

    Driver::Driver(int xres, int yres, int bpp, bool fullScreen, bool doubleSize)
        : _screen(0)
        , _xres(xres)
        , _yres(yres)
        , _bpp(bpp)
        , _tintColour(255, 255, 255)
        , _fullScreen(fullScreen)
        , _blendMode(Video::Normal)
        , _doubleSize(doubleSize) 
        , _lasttex(0)
    {
        if (_doubleSize) {
            xres *= 2;
            yres *= 2;
        }

        // from this point, xres and yres are the physical resolution,
        // _xres and _yres are the virtual resolution

        Log::Write("--Setting video mode");
        _screen = SDL_SetVideoMode(xres, yres, bpp, SDL_OPENGL | (fullScreen ? SDL_FULLSCREEN : 0));
        if (!_screen) {
            throw Video::Exception();
        }

        Log::Write("--Initializing OpenGL");
        glShadeModel(GL_SMOOTH);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(2.0f / float(xres), -2.0f / float(yres), 1.0f);
        glTranslatef(-(float(xres) / 2.0f), -(float(yres) / 2.0f), 0.0f);
        glViewport(0, 0, xres, yres);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);

        glScissor(0, 0, xres, yres);
        glEnable(GL_SCISSOR_TEST);

        glEnable(GL_TEXTURE_2D);

        Log::Write("--Disabling cursor");
        SDL_ShowCursor(SDL_DISABLE);

        glBlendEquationEXT = (void (IKA_STDCALL *)(int))SDL_GL_GetProcAddress("glBlendEquationEXT");

        if (!glBlendEquationEXT) {
            Log::Write("Warning! glBlendEquationEXT not found.  Colour subtraction disabled.");
            glBlendEquationEXT = &glBlendEquationStub;
        }

        if (_doubleSize) {
            Log::Write("--Generating doublesize buffer");
            glGenTextures(1, &_bufferTex);
        }
    }

    Driver::~Driver() {
        glDeleteTextures(1, &_bufferTex);
    }

    // Trouble is, SDL nukes the GL context when it switches modes.
    // Which means that our textures go down the crapper. ;_;
    bool Driver::SwitchResolution(int x, int y) {
#if 1
        return false;
#else
        if (!SDL_VideoModeOK(x, y, _bpp, SDL_OPENGL | (_fullScreen ? SDL_FULLSCREEN : 0))) {
            return false;
        }

        _screen = SDL_SetVideoMode(x, y, _bpp, SDL_OPENGL | (_fullScreen ? SDL_FULLSCREEN : 0));
        if (!_screen) {
            throw std::runtime_error("OpenGL Video panic.  No video display!!");
        }

        _xres = x;
        _yres = y;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glScalef(2.0f / float(_xres), -2.0f / float(_yres), 1.0f);
        glTranslatef(-(float(_xres) / 2.0f), -(float(_yres) / 2.0f), 0.0f);
        glViewport(0, 0, _xres, _yres);

        return true;
#endif
    }

    // This is far, far too long.  Refactor.
    Image* Driver::CreateImage(Canvas& src) {

#ifdef SHARE_TEXTURES

        /*
         * First, if src is 16x16, we figure out if we have a texture that can fit it.
         * If we do, we use glSubTexImage2D, update the used region, and fly away on wings
         * of silver dust.  If not, we create a 256x256 texture, and do the aforementioned
         * glSubTexImage2Ding. (we do NOT, however, fly away on wings of silver dust.  That's
         * strictly reserved for when there is an existing texture for us to use)
         *
         * oh yeah.  I know this is messy.  Don't give a shit yet.  Prototyping is a bitch
         * that way.
         *
         * TODO: Generalize this so that we hold onto a bunch of textures for every power-of-two
         * size of image that is created. (8x8, 64x64, et cetera)
         * Or maybe just group images of equal height togeather.  Then ika would be able to
         * leverage texture-sharing for the font as well.
         */

        if (src.Width() == 16 && src.Height() == 16) {
            Texture* tex = 0;
            for (TextureSet::iterator
                iter  = _textures.begin();
                iter != _textures.end();
                iter++)
            {
                const Point& p = (*iter)->unused;
                if (p.x <= 256 - 16 && p.y <= 256 - 16) {
                    tex = *iter;
                    break;
                }
            }

            if (!tex) {
                // no texture?  no problem.
                static u32 dummyShit[256 * 256] = {0}; // initialized to 0
                tex = new Texture(0, 256, 256);
                glGenTextures(1, &tex->handle);
                SwitchTexture(tex->handle);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, dummyShit);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                _textures.insert(tex);
            }

            int x = tex->unused.x;
            int y = tex->unused.y;
            tex->unused.x += 16;
            if (tex->unused.x >= 256) {
                tex->unused = Point(0, tex->unused.y + 16);
            }

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
        } else
#endif
        {
            bool dealloc;
            RGBA* pixels;
            int texwidth;
            int texheight;

            src.Flip(); // GAY

            if (isPowerOf2(src.Width()) && isPowerOf2(src.Height())) {
                dealloc = false;    // perfect match
                pixels = src.GetPixels();
                texwidth = src.Width();
                texheight = src.Height();
            } else {
                dealloc = true;
                texwidth  = 1; while (texwidth < src.Width()) { texwidth <<= 1; }
                texheight = 1; while (texheight < src.Height()) { texheight <<= 1; }

                pixels = new RGBA[texwidth * texheight];
                for (int y = 0; y < src.Height(); y++) {
                    memcpy(
                        pixels + (y * texwidth), 
                        src.GetPixels() + (y * src.Width()),
                        src.Width() * sizeof(RGBA)
                    );
                }
            }

            uint texture;
            glGenTextures(1, &texture);
            SwitchTexture(texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            src.Flip();

            if (dealloc) {
                delete[] pixels;
            }

            const float texCoords[] = { 0, 0, float(src.Width()) / texwidth, float(src.Height()) / texheight };
            Texture* tex = new Texture(texture, texwidth, texheight);
            tex->refCount++;
            return new Image(tex, texCoords, src.Width(), src.Height());
        }
    }

    void Driver::FreeImage(Video::Image* img) {
        if (!img) {
            return;
        }

        SwitchTexture(0);
        
        // Refcount update/cleanup
        Texture* tex = static_cast<OpenGL::Image*>(img)->_texture;

#ifdef SHARE_TEXTURES
        if (tex->refCount == 1) {
            _textures.erase(tex);
            glDeleteTextures(1, &tex->handle);
            delete tex;
        } else {
            tex->refCount--;
        }
#else
        glDeleteTextures(1, &tex->handle);
        delete tex;
#endif

        delete (OpenGL::Image*)img;
    }

    void Driver::ClipScreen(int left, int top, int right, int bottom) {
        if (left > right) {
            swap(left, right);
        }
        if (top > bottom) {
            swap(top, bottom);
        }

        // gah.
        // Convert x1,y1-x2,y2 to left,bottom/width,height.

        int width = right - left;
        int height = bottom - top;

        if (_doubleSize) {
            top = min(_yres * 2, _yres * 2 - top) - height;
        } else {
            top = min(_yres, _yres - top) - height;
        }

        glScissor(
            left, top,
            width, height
        );
    }

    Rect* Driver::GetClipRect() {
        GLint* cliprect = new GLint[4];
        glGetIntegerv(GL_SCISSOR_BOX, cliprect);

        int height = cliprect[3];
        int y = _yres - cliprect[1] - height;

        return new Rect(cliprect[0], y, cliprect[0] + cliprect[2], y + height);
    }

    void Driver::ShowPage() {
        if (_doubleSize) {
            // Grab the whole screen into our buffer texture and draw it at double size.
            glDisable(GL_BLEND);
            uint texW = nextPowerOf2(_xres);
            uint texH = nextPowerOf2(_yres);
            SwitchTexture(_bufferTex);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, _yres, texW, texH, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glClear(GL_COLOR_BUFFER_BIT);

            float endX = float(_xres) / texW;
            float endY = float(_yres) / texH;

            int x = _xres * 2;
            int y = _yres * 2;
            glPushAttrib(GL_SCISSOR_BIT);
            glScissor(0, 0, x, y);

            glBegin(GL_QUADS);
            glTexCoord2f(0,    endY); glVertex2i(0, 0);
            glTexCoord2f(endX, endY); glVertex2i(x, 0);
            glTexCoord2f(endX,    0); glVertex2i(x, y);
            glTexCoord2f(0,       0); glVertex2i(0, y);
            glEnd();

            glPopAttrib();
            glEnable(GL_BLEND);
        }

        fps.Update();
        SDL_GL_SwapBuffers();
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Driver::ClearScreen() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    Video::BlendMode Driver::SetBlendMode(Video::BlendMode bm) {
        if (bm == _blendMode) {
            return bm;
        }

        // Unset the blend equation if it was previously changed. (it is always changed if the current mode is Video::Subtract)
        if (_blendMode == Video::Subtract) {
            glBlendEquationEXT(GL_FUNC_ADD_EXT);
            
            // hack for old buggy ATi drivers. (GAY)
            // (fixed it recent releases.  Remove?)
            glBegin(GL_POINTS); glVertex2i(-1, -1); glEnd();
        }

        switch (bm) {
            case Video::None:    {  glDisable(GL_BLEND);     break; }
            case Video::Matte:   // TODO: See if we can get GL to do matte?
            case Video::Normal:  {  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  glEnable(GL_BLEND); break;  }
            case Video::Add:     {  glBlendFunc(GL_ONE, GL_ONE);                        glEnable(GL_BLEND); break;  }

            case Video::Subtract: {
                glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);
                glBlendFunc(GL_ONE, GL_ONE);
                glEnable(GL_BLEND);
                break;
            }

            default: {
                return _blendMode;
            }
        }

        Video::BlendMode m = _blendMode;
        _blendMode = bm;
        return m;
    }

    // This is, without question *the* method to optimize.
    // At least 20% of the sum total CPU goes into this.
    // Even when the profiler brings the framerate below 1fps, with the AI loop is being executed at 
    // least ten times per render, a fifth of the CPU goes here.
    void Driver::BlitImage(Video::Image* i, int x, int y) {
        OpenGL::Image* img = (Image*)i;

        // VC7 won't inline these because they're virtual.
        int w = img->_width;//Width();
        int h = img->_height;//Height();

        const float* texCoords = img->_texCoords;

        SwitchTexture(img->_texture->handle);
        glBegin(GL_QUADS);
        glTexCoord2f(texCoords[0], texCoords[3]); glVertex2i(x, y);
        glTexCoord2f(texCoords[2], texCoords[3]); glVertex2i(x + w, y);
        glTexCoord2f(texCoords[2], texCoords[1]); glVertex2i(x + w, y + h);
        glTexCoord2f(texCoords[0], texCoords[1]); glVertex2i(x, y + h);
        glEnd();
    }
    
    void Driver::ClipBlitImage(Video::Image* i, int x, int y, int ix, int iy, int iw, int ih) {
        OpenGL::Image* img = static_cast<Image*>(i);

        // VC7 won't inline these because they're virtual.  Homo.
        int w = img->_width;//Width();
        int h = img->_height;//Height();

        float texCoords[4] = {0,0,0,0};
        memcpy(texCoords, img->_texCoords, sizeof(float) * 4);
        if (ix > w || ix < 0) {
            ix = 0;
        }
        if (iy > h || iy < 0) {
            iy = 0;
        }
        if (iw > w || iw < 0) {
            iw = w;
        }
        if (ih > h || ih < 0) {
            ih = h;
        }

        float cw = texCoords[2] - texCoords[0];
        float ch = texCoords[3] - texCoords[1];
        texCoords[0] += cw * ix / w;
        texCoords[1] += ch * iy / h;
        texCoords[2] = texCoords[0] + cw * iw / w;
        texCoords[3] = texCoords[1] + ch * ih / h;

        SwitchTexture(img->_texture->handle);
        glBegin(GL_QUADS);
        glTexCoord2f(texCoords[0], texCoords[3]); glVertex2i(x, y);
        glTexCoord2f(texCoords[2], texCoords[3]); glVertex2i(x + iw, y);
        glTexCoord2f(texCoords[2], texCoords[1]); glVertex2i(x + iw, y + ih);
        glTexCoord2f(texCoords[0], texCoords[1]); glVertex2i(x, y + ih);
        glEnd();
    }

    void Driver::ScaleBlitImage(Video::Image* i, int x, int y, int w, int h) {
        Image* img = (Image*)i;

        const float* texCoords = img->_texCoords;
        SwitchTexture(img->_texture->handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        glTexCoord2f(texCoords[0], texCoords[3]);   glVertex2i(x, y);
        glTexCoord2f(texCoords[2], texCoords[3]);   glVertex2i(x + w, y);
        glTexCoord2f(texCoords[2], texCoords[1]);   glVertex2i(x + w, y + h);
        glTexCoord2f(texCoords[0], texCoords[1]);   glVertex2i(x, y + h);
        glEnd();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void Driver::DistortBlitImage(Video::Image* i, int x[4], int y[4]) {
        Image* img = (Image*)i;

        const float* texCoords = img->_texCoords;
        const float texX[] = { texCoords[0], texCoords[2], texCoords[2], texCoords[0] };
        const float texY[] = { texCoords[3], texCoords[3], texCoords[1], texCoords[1] };

        SwitchTexture(img->_texture->handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++) {
            glTexCoord2f(texX[i], texY[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void Driver::TileBlitImage(Video::Image* i, int x, int y, int w, int h, float scalex, float scaley) {
        Image* img = static_cast<Image*>(i);
        Texture* tex = img->_texture;
        SwitchTexture(img->_texture->handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Invert Y texel because we're working with raster coordinates, not cartesian.
        // (Y increases as we go down, but GL likes to do the opposite)
        float texX = float(w) / img->Width()  * scalex;
        float texY = 1 - (float(h) / img->Height() * scaley);

        // simplest case.  We can draw one big textured quad for the whole thing.
        if (tex->width == img->_width && tex->height == img->_height) {
            glBegin(GL_QUADS);
            glTexCoord2f(0,    1);    glVertex2i(x, y);
            glTexCoord2f(texX, 1);    glVertex2i(x + w, y);
            glTexCoord2f(texX, texY); glVertex2i(x + w, y + h);
            glTexCoord2f(0,    texY); glVertex2i(x, y + h);
            glEnd();
        } else {
            // backup: Draw a grid of textured quads.
            // This isn't so bad, really, but we could do another optimization
            // and see if we could get away with doing some big long horizontal
            // or vertical strips.  Something for another day.

            // Calculate clipping rect based off current one.
            Rect* cliprect = GetClipRect();
            int cx = (cliprect->left > x) ? cliprect->left : x;
            int cy = (cliprect->top > y) ? cliprect->top : y;
            int cx2 = (cliprect->right < x + w) ? cliprect->right : x + w;
            int cy2 = (cliprect->bottom < y + h) ? cliprect->bottom : y + h;

            glPushAttrib(GL_SCISSOR_BIT);
            ClipScreen(cx, cy, cx2, cy2);

            float imgWidth = float(img->_width) * scalex;
            float imgHeight = float(img->_height) * scaley;

            const float* texCoords = img->_texCoords;
    
            glBegin(GL_QUADS);
            for (float curY = float(y); curY < y + h; curY += imgWidth) {
                for (float curX = float(x); curX < x + w; curX += imgHeight) {
                    glTexCoord2f(texCoords[0], texCoords[3]); glVertex2f(curX, curY);
                    glTexCoord2f(texCoords[2], texCoords[3]); glVertex2f(curX + imgWidth, curY);
                    glTexCoord2f(texCoords[2], texCoords[1]); glVertex2f(curX + imgWidth, curY + imgHeight);
                    glTexCoord2f(texCoords[0], texCoords[1]); glVertex2f(curX, curY + imgHeight);
                }
            }
            glEnd();

            glPopAttrib();
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    void Driver::TintBlitImage(Video::Image* img, int x, int y, u32 tint) {
        glColor4ubv((u8*)&tint);
        ::OpenGL::Driver::BlitImage(img, x, y);
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
    }

    void Driver::TintDistortBlitImage(Video::Image* i, int x[4], int y[4], u32 colour[4]) {
        Image* img = (Image*)i;

        const float* texCoords = img->_texCoords;
        const float texX[] = { texCoords[0], texCoords[2], texCoords[2], texCoords[0] };
        const float texY[] = { texCoords[3], texCoords[3], texCoords[1], texCoords[1] };

        SwitchTexture(img->_texture->handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++) {
            glColor4ubv((u8*)&colour[i]);
            glTexCoord2f(texX[i], texY[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
    }

    /// Combines TintBlit and TileBlit.  'nuff said.
    void Driver::TintTileBlitImage(Video::Image* img, int x, int y, int w, int h, float scalex, float scaley, u32 tint) {
        glColor4ubv(reinterpret_cast<u8*>(&tint));
        TileBlitImage(img, x, y, w, h, scalex, scaley);
        glColor3ub(255, 255, 255);
    }

    void Driver::DrawPixel(int x, int y, u32 colour) {
        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);

        glBegin(GL_POINTS);
        glVertex2f(float(x) + 0.375f, float(y) + 0.375f);
        glEnd();

        glEnable(GL_TEXTURE_2D);
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
    }

    void Driver::DrawLine(int x1, int y1, int x2, int y2, u32 colour) {
        glPushMatrix();
        glTranslatef(0.375f, 0.375f, 0);

        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);
        glBegin(GL_LINES);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
        glEnd();
        glEnable(GL_TEXTURE_2D);
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);

        glPopMatrix();
    }

    void Driver::DrawRect(int x1, int y1, int x2, int y2, u32 colour, bool filled) {
        glPushMatrix();
        glTranslatef(0.375f, 0.375f, 0);

        SwitchTexture(0);
        glColor4ubv((u8*)&colour);
        if (filled) {
            x2++;
            y2++;
            glBegin(GL_QUADS);
        }
        else {
            //y1++;
            glBegin(GL_LINE_LOOP);
        }

        glVertex2i(x1, y1);
        glVertex2i(x2, y1);
        glVertex2i(x2, y2);
        glVertex2i(x1, y2);
        glEnd();
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
        glPopMatrix();
    }

    // Ellipse algorithm courtesy of aen.
    // I had to spend like 5 minutes deobfuscating this.
    void Driver::DrawEllipse(int cx, int cy, int rx, int ry, u32 colour, bool filled) {
        int x1 = cx - rx;
        int y1 = cy - ry;
        int width = rx * 2;
        int height = ry * 2;

        if (!((width < -3 || width > 3) || (height < -3 || height > 3))) {
            DrawRect(x1,y1,width,height, colour, filled);
            return;
        }

        width--;
        height--;
        int a = rx;
        int b = ry;

        int a2 = a * a;
        int b2 = b * b;
        int fa2 = 4 * a2;

        glPushMatrix();
        glTranslatef(0.375f, 0.375f, 0);
        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);

        if (filled) {
            glBegin(GL_LINES);
        } else {
            glBegin(GL_POINTS);
        }

        // most bloated for statement ever.
        for (int 
            x = 0,
            y = b,
            sigma = 2 * b2 + a2 * (-2 * b);
        
            b2 * x <= a2 * y;
            x++)
        {
            glVertex2i(cx + x, cy + y);
            glVertex2i(cx - x, cy + y);
            glVertex2i(cx + x, cy - y);
            glVertex2i(cx - x, cy - y);
            if (sigma >= 0) {
                sigma += fa2 * (1 - y);
                y--;
            }
            sigma += b2 * (4 * x + 6);
        }

        int fb2 = 4 * b2;
        for (int 
            x = a,
            y = 0,
            sigma = 2 * a2 + b2 * (-2 * a);
        
            a2 * y <= b2 * x;
            y++)
        {
            glVertex2i(cx + x, cy + y);
            glVertex2i(cx - x, cy + y);
            glVertex2i(cx + x, cy - y);
            glVertex2i(cx - x, cy - y);
            if (sigma >= 0) {
                sigma += fb2 * (1 - x);
                x--;
            }
            sigma += a2 * (4 * y + 6);
        }
        glEnd();

        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
        glEnable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    void Driver::DrawTriangle(int x[3], int y[3], u32 colour[3]) {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; i++) {
            glColor4ubv((u8*)&colour[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
        glEnable(GL_TEXTURE_2D);
    }

    Image* Driver::GrabImage(int x1, int y1, int x2, int y2) {
        // Way fast, since there are no pixels going from the video card to system memory.
        // They just get copied from the screen to a texture (which also video memory)

        // clip
        if (x1 > x2) {
            swap(x1, x2);
        }
        if (y1 > y2) {
            swap(y1, y2);
        }

        int w = x2 - x1;
        int h = y2 - y1;
        if (w < 0 || h < 0) {
            return 0;
        }

        if (_doubleSize) {
            y2 -= _yres;
        }

        uint texwidth = nextPowerOf2(w);
        uint texheight = nextPowerOf2(h);
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

    Canvas* Driver::GrabCanvas(int x1, int y1, int x2, int y2) {
        /*
         * Have to convert from the raster-coordinates that ika uses to
         * Cartesian coords, which is what OpenGL favours.
         */
        y1 = _yres - y1;
        y2 = _yres - y2;

        if (_doubleSize) { 
            y1 += _yres; 
            y2 += _yres; 
        }

        // clip
        if (x1 > x2) {
            swap(x1, x2);
        }
        if (y1 > y2) {
            swap(y1, y2);
        }

        int w = x2 - x1;
        int h = y2 - y1;
        if (w < 0 || h < 0) {
            return 0;
        }

        Canvas* c = new Canvas(w, h);
        glReadPixels(x1, y1, w, h, GL_RGBA, GL_UNSIGNED_BYTE, c->GetPixels());
        c->Flip();
        return c;
    }

    u32 Driver::GetTint() {
        return _tintColour;
    }

    void Driver::SetTint(u32 tint) {
        _tintColour = tint;
        glColor4ub(_tintColour.r, _tintColour.g, _tintColour.b, _tintColour.a);
    }

    Point Driver::GetResolution() const {
        return Point(_xres, _yres);
    }

    int Driver::GetFrameRate() const {
        return fps.FPS();
    }

    inline void Driver::SwitchTexture(uint tex) {
        if (tex != _lasttex) {
            _lasttex = tex;
            glBindTexture(GL_TEXTURE_2D, tex);
        }
    }
};
