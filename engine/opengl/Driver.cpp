#include <cassert>
#include <math.h>

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

        glShadeModel(GL_SMOOTH);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        gluOrtho2D(0, _xres, _yres, 0);
        glClearColor(0, 0, 0, 0);

        glDisable(GL_DEPTH_TEST);

        glScissor(0, 0, _xres, _yres);
        glEnable(GL_SCISSOR_TEST);

        glEnable(GL_TEXTURE_2D);
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
        fps.Update();
        SDL_GL_SwapBuffers();
    }

    Video::BlendMode Driver::SetBlendMode(Video::BlendMode bm)
    {
        Video::BlendMode m = _blendMode;
        
        switch (bm)
        {
            case Video::None:    glDisable(GL_BLEND);    break;
            case Video::Normal:  glEnable(GL_BLEND);     break;
            default:
                return _blendMode;
        }

        _blendMode = bm;
        return m;
    }

    void Driver::BlitImage(Video::Image* i, int x, int y)
    {
        Image* img = (Image*)i;
        int w = img->Width();
        int h = img->Height();

        float texX = 1.0f * img->_width / img->_texWidth;
        float texY = 1.0f * img->_height / img->_texHeight;

        SwitchTexture(img->_texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);         glVertex2i(x, y);
        glTexCoord2f(texX, 0);      glVertex2i(x + w, y);
        glTexCoord2f(texX, texY);   glVertex2i(x + w, y + h);
        glTexCoord2f(0, texY);      glVertex2i(x, y + h);
        glEnd();
    }

    void Driver::ScaleBlitImage(Video::Image* i, int x, int y, int w, int h)
    {
        Image* img = (Image*)i;

        float texX = 1.0f * img->_width / img->_texWidth;
        float texY = 1.0f * img->_height / img->_texHeight;

        SwitchTexture(img->_texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);         glVertex2i(x, y);
        glTexCoord2f(texX, 0);      glVertex2i(x + w, y);
        glTexCoord2f(texX, texY);   glVertex2i(x + w, y + h);
        glTexCoord2f(0, texY);      glVertex2i(x, y + h);
        glEnd();
    }

    void Driver::DistortBlitImage(Video::Image* i, int x[4], int y[4])
    {
        Image* img = (Image*)i;

        float endX = 1.0f * img->_width / img->_texWidth;
        float endY = 1.0f * img->_height / img->_texHeight;

        float texX[] = { 0, endX, endX, 0 };
        float texY[] = { 0, 0, endY, endY };

        SwitchTexture(img->_texture);
        glBegin(GL_QUADS);
        for (int i = 0; i < 4; i++)
        {
            glTexCoord2f(texX[i], texY[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
    }

    void Driver::DrawPixel(int x, int y, u32 colour)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);
        
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();

        glEnable(GL_TEXTURE_2D);
        glColor4ub(255, 255, 255, 255);
    }

    void Driver::DrawLine(int x1, int y1, int x2, int y2, u32 colour)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);
        glBegin(GL_LINES);
        glVertex2i(x1, y1);
        glVertex2i(x2, y2);
        glEnd();
        glEnable(GL_TEXTURE_2D);
        glColor4ub(255, 255, 255, 255);
    }
    
    void Driver::DrawRect(int x1, int y1, int x2, int y2, u32 colour, bool filled)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4ubv((u8*)&colour);
        if (filled)
            glBegin(GL_QUADS);
        else
            glBegin(GL_LINE_LOOP);

        glVertex2i(x1, y1);
        glVertex2i(x2, y1);
        glVertex2i(x2, y2);
        glVertex2i(x1, y2);
        glEnd();
        glColor4f(1, 1, 1, 1);
        glEnable(GL_TEXTURE_2D);
    }

    void Driver::DrawEllipse(int cx, int cy, int rx, int ry, u32 colour, bool filled)
    {
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
    }

    void Driver::DrawTriangle(int x[3], int y[3], u32 colour[3])
    {
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < 3; i++)
        {
            glColor3ubv((u8*)&colour[i]);
            glVertex2i(x[i], y[i]);
        }
        glEnd();
        glColor4ub(255, 255, 255, 255);
        glEnable(GL_TEXTURE_2D);
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

    int Driver::GetFrameRate() const
    {
        return fps.FPS();
    }
};