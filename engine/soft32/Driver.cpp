#include <cassert>
#include <math.h>

#include "SDL/SDL.h"

#include "Driver.h"
#include "Image.h"
#include "Misc.h"

#include "common/types.h"
#include "common/Canvas.h"

namespace Soft32
{
    Driver::Driver(int xres, int yres, int bpp, bool fullscreen)
        : _xres(xres)
        , _yres(yres)
        , _bpp(bpp)
        , _fullscreen(fullscreen)
    {
        _screen = SDL_SetVideoMode(xres, yres, bpp, SDL_SWSURFACE | SDL_ASYNCBLIT | (fullscreen ? SDL_FULLSCREEN | SDL_DOUBLEBUF: 0));
        if (!_screen)
            throw Video::Exception();

        SDL_SetClipRect(_screen, NULL);

        _rmask = RGBA(255, 0, 0, 0).i;
        _gmask = RGBA(0, 255, 0, 0).i;
        _bmask = RGBA(0, 0, 255, 0).i;
        _amask = RGBA(0, 0, 0, 255).i;
    }

    Driver::~Driver()
    {
        SDL_SetGamma(1.0, 1.0, 1.0);
        //SDL_FreeSurface(_screen);
    }

    void Driver::SwitchToFullScreen()
    {
        if (_screen != NULL)
            SDL_FreeSurface(_screen);
        _fullscreen = true;
        _screen = SDL_SetVideoMode(_xres, _yres, _bpp, SDL_HWSURFACE | SDL_ASYNCBLIT | (_fullscreen ? SDL_FULLSCREEN : 0));
        if (!_screen)
            throw Video::Exception();
        SDL_SetClipRect(_screen, NULL);
        return;
    }

    void Driver::SwitchToWindowed()
    {
        if (_screen != NULL)
            SDL_FreeSurface(_screen);
        _fullscreen = false;
        _screen = SDL_SetVideoMode(_xres, _yres, _bpp, SDL_HWSURFACE | SDL_ASYNCBLIT | (_fullscreen ? SDL_FULLSCREEN : 0));
        if (!_screen)
            throw Video::Exception();
        SDL_SetClipRect(_screen, NULL);
        return;
    }

    void Driver::SwitchResolution(int x, int y)
    {
        if (_screen != NULL)
            SDL_FreeSurface(_screen);
        _screen = SDL_SetVideoMode(x, y, _bpp, SDL_HWSURFACE | SDL_ASYNCBLIT | (_fullscreen ? SDL_FULLSCREEN : 0));
        if (!_screen)
            throw Video::Exception();
        SDL_SetClipRect(_screen, NULL);
        _xres = x;
        _yres = y;
        return;
    }

    Video::Image* Driver::CreateImage(Canvas& src)
    {
        const SDL_VideoInfo *vinfo = NULL;
        SDL_Surface *newsurface = NULL;
        SDL_Surface *storagesurface = NULL;
        Image *newimage = NULL;
        vinfo = SDL_GetVideoInfo();
        storagesurface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, src.Width(), src.Height(), 32, _rmask, _gmask, _bmask, _amask);
        SDL_LockSurface(storagesurface);
        memcpy((void*)storagesurface->pixels, (const void*)src.GetPixels(), src.Width() * src.Height() * 4);
        SDL_UnlockSurface(storagesurface);
        newsurface = SDL_DisplayFormatAlpha(storagesurface);
        SDL_FreeSurface(storagesurface);
        newimage = new Image(newsurface, src.Width(), src.Height());
        return newimage;
    }

    void Driver::FreeImage(Video::Image* img)
    {
        delete (Image*)img;
    }

    void Driver::ShowPage()
    {
        fps.Update();
        SDL_Flip(_screen);
    }

    Video::BlendMode Driver::SetBlendMode(Video::BlendMode bm)
    {
        _blendMode = bm;
        return bm;
    }

    void Driver::BlitImage(Video::Image* i, int x, int y)
    {
        if (i == NULL) throw Video::Exception();
        SDL_Rect rSource, rDest;       
        Image* iImage = (Image*)i;
        if (iImage->_surface == NULL) throw Video::Exception();
        if ((iImage->_surface->w <= 0) || (iImage->_surface->h <= 0)) throw Video::Exception();
        CreateRect(rSource, 0, 0, iImage->_surface->w, iImage->_surface->h);
        CreateRect(rDest, x, y, iImage->_surface->w, iImage->_surface->h);
        SDL_BlitSurface(iImage->_surface, NULL, _screen, &rDest);
    }

    void Driver::ScaleBlitImage(Video::Image* i, int x, int y, int w, int h)
    {
        if (i == NULL) throw Video::Exception();
        Image* iImage = (Image*)i;
        if (iImage->_surface == NULL) throw Video::Exception();
        if ((iImage->_surface->w <= 0) || (iImage->_surface->h <= 0)) throw Video::Exception();
        if (ClipCoordinate(_screen, x, y) && ClipCoordinate(_screen, x+w, y+h))
        {
            float xi = (float)(iImage->_surface->w) / (w), yi = (float)(iImage->_surface->h) / (h);
            int cx = w, cy = h;
            float sx = 0, sy = 0;
            int iy = y;
            RGBA *pDest;
            SDL_LockSurface(_screen);
            SDL_LockSurface(iImage->_surface);
            pDest = ((RGBA*)_screen->pixels) + (y * _screen->w) + (x);
            switch (_blendMode)
            {
            case Video::None:
            default:
                while (cy--)
                {
                    while (cx--)
                    {
                        *pDest = *((RGBA*)iImage->_surface->pixels + ((int)sy * iImage->_surface->w) + (int)sx);
                        pDest++;
                        sx += xi;
                    }
                    sx = 0;
                    sy += yi;
                    iy++;
                    cx = w;
                    pDest = ((RGBA*)_screen->pixels) + (iy * _screen->w) + (x);
                }
                break;
            case Video::Normal:
                while (cy--)
                {
                    while (cx--)
                    {
                        *pDest = Alpha::Blend(*((RGBA*)iImage->_surface->pixels + ((int)sy * iImage->_surface->w) + (int)sx), *pDest);
                        pDest++;
                        sx += xi;
                    }
                    sx = 0;
                    sy += yi;
                    iy++;
                    cx = w;
                    pDest = ((RGBA*)_screen->pixels) + (iy * _screen->w) + (x);
                }
                break;
            }
            SDL_UnlockSurface(iImage->_surface);
            SDL_UnlockSurface(_screen);
        }
    }

    void Driver::DrawPixel(int x, int y, u32 colour)
    {
        switch (_blendMode)
        {
        case Video::None:
        default:
            SDL_Rect rPixel;
            CreateRect(rPixel, x, y, 1, 1);
            SDL_FillRect(_screen, &rPixel, colour);
            break;
        case Video::Normal:
            RGBA pSrc;
            RGBA *pDst;
            if (ClipCoordinate(_screen, x, y))
            {
                SDL_LockSurface(_screen);
                pSrc.i = colour;
                pDst = ((RGBA*)_screen->pixels + (y * _screen->w) + (x));
                *pDst = Alpha::Blend(pSrc, *pDst);
                SDL_UnlockSurface(_screen);
            }
            break;
        }
        return;
    }

    void Driver::DrawLine(int x1, int y1, int x2, int y2, u32 colour)
    {
        //check for the cases in which the line is vertical or horizontal or only one pixel big
        //we can do those faster through other means
        if(y1==y2&&x1==x2)
        {
            this->DrawPixel(x1, y1, colour);
            return;
        }
        if(y1==y2)
        {
            this->HLine(x1, x2, y1, colour);
            return;
        }
        if(x1==x2)
        {
            this->VLine(x1, y1, y2, colour);
            return;
        }

        //its good to have these handy
        int cx1=_screen->clip_rect.x;
        int cx2=(_screen->clip_rect.x + _screen->w) - 1;
        int cy1=_screen->clip_rect.y;
        int cy2=(_screen->clip_rect.y + _screen->h) - 1;

        //variables for the clipping code
        int v1=0, v2=0;
        //	int diff;
        //	int j;

        //here begins what is apparently the Cohen-Sutherland line clipping algorithm
        //there's a doc on it here: http://reality.sgi.com/tomcat_asd/algorithms.html#clipping
        if(y1<cy1) v1|=8;
        if(y1>cy2) v1|=4;
        if(x1>cx2) v1|=2;
        if(x1<cx1) v1|=1;
        if(y2<cy1) v2|=8;
        if(y2>cy2) v2|=4;
        if(x2>cx2) v2|=2;
        if(x2<cx1) v2|=1;

        while((v1&v2)==0&&(v1|v2)!=0)
        {
            if(v1)
            {
                if(v1&8) //clip above
                {
                    x1-=((x1-x2)*(cy1-y1))/(y2-y1+1);
                    y1=cy1;
                }
                else if(v1&4) //clip below
                {
                    x1-=((x1-x2)*(y1-cy2))/(y1-y2+1);
                    y1=cy2;
                }
                else if(v1&2) //clip right
                {
                    y1-=((y1-y2)*(x1-cx2))/(x1-x2+1);
                    x1=cx2;
                }
                else //clip left
                {
                    y1-=((y1-y2)*(cx1-x1))/(x2-x1+1);
                    x1=cx1;
                }
                v1=0;
                if(y1<cy1) v1|=8;
                if(y1>cy2) v1|=4;
                if(x1>cx2) v1|=2;
                if(x1<cx1) v1|=1;
            }
            else
            {
                if(v2&8) //clip above
                {
                    x2-=((x2-x1)*(cy1-y2))/(y1-y2+1);
                    y2=cy1;
                }
                else if(v2&4) //clip below
                {
                    x2-=((x2-x1)*(y2-cy2))/(y2-y1+1);
                    y2=cy2;
                }
                else if(v2&2) //clip right
                {
                    y2-=((y2-y1)*(x2-cx2))/(x2-x1+1);
                    x2=cx2;
                }
                else //clip left
                {
                    y2-=((y2-y1)*(cx1-x2))/(x1-x2+1);
                    x2=cx1;
                }
                v2=0;
                if(y2<cy1) v2|=8;
                if(y2>cy2) v2|=4;
                if(x2>cx2) v2|=2;
                if(x2<cx1) v2|=1;
            }
        }

        //this tells us if the line was clipped in its entirety. yum!
        if(v1&v2)
            return;

        //make these checks again
        if(y1==y2&&x1==x2)
        {
            this->DrawPixel(x1, y1, colour);
            return;
        }
        if(y1==y2)
        {
            this->HLine(x1, x2, y1, colour);
            return;
        }
        if(x1==x2)
        {
            this->VLine(x1, y1, y2, colour);
            return;
        }

        //for the line renderer
        int xi, yi, xyi;
        int d, dir, diu, dx, dy;

        //two pointers because we render the line from both ends
        u32* w1=&((u32*)_screen->pixels)[_screen->w*y1+x1];
        u32* w2=&((u32*)_screen->pixels)[_screen->w*y2+x2];

        SDL_LockSurface(_screen);

        //start algorithm presently.
        xi=1;
        if((dx=x2-x1)<0)
        {
            dx=-dx;
            xi=-1;
        }

        yi=_screen->w;
        if((dy=y2-y1)<0)
        {
            yi=-yi;
            dy=-dy;
        }

        xyi=xi+yi;

        switch(_blendMode)
        {
        case Video::None:
        default:
            if(dy<dx)
            {
                dir=dy*2;
                d=-dx;
                diu=2*d;
                dy=dx/2;

                while(true)
                {
                    *w1=colour;
                    *w2=colour;
                    if((d+=dir)<=0)
                    {
                        w1+=xi;
                        w2-=xi;
                        if((--dy)>0) continue;
                        *w1=colour;
                        if((dx&1)==0) break;
                        *w2=colour;
                        break;
                    }
                    else
                    {
                        w1+=xyi;
                        w2-=xyi;
                        d+=diu;
                        if((--dy)>0) continue;
                        *w1=colour;
                        if((dx&1)==0) break;
                        *w2=colour;
                        break;
                    }
                }
            }
            else
            {
                dir=dx*2;
                d=-dy;
                diu=d*2;
                dx=dy/2;
                while(true)
                {
                    *w1=colour;
                    *w2=colour;
                    if((d+=dir)<=0)
                    {
                        w1+=yi;
                        w2-=yi;
                        if((--dx)>0) continue;
                        *w1=colour;
                        if((dy&1)==0) break;
                        *w2=colour;
                        break;
                    }
                    else
                    {
                        w1+=xyi;
                        w2-=xyi;
                        d+=diu;
                        if((--dx)>0) continue;
                        *w1=colour;
                        if((dy&1)==0) break;
                        *w2=colour;
                        break;
                    }
                }
            }
            break;
        case Video::Normal:
            if(dy<dx)
            {
                dir=dy*2;
                d=-dx;
                diu=2*d;
                dy=dx/2;

                while(true)
                {
                    *w1=Alpha::Blend(colour, *w1);
                    *w2=Alpha::Blend(colour, *w2);
                    if((d+=dir)<=0)
                    {
                        w1+=xi;
                        w2-=xi;
                        if((--dy)>0) continue;
                        *w1=Alpha::Blend(colour, *w1);
                        if((dx&1)==0) break;
                        *w2=Alpha::Blend(colour, *w2);
                        break;
                    }
                    else
                    {
                        w1+=xyi;
                        w2-=xyi;
                        d+=diu;
                        if((--dy)>0) continue;
                        *w1=Alpha::Blend(colour, *w1);
                        if((dx&1)==0) break;
                        *w2=Alpha::Blend(colour, *w2);
                        break;
                    }
                }
            }
            else
            {
                dir=dx*2;
                d=-dy;
                diu=d*2;
                dx=dy/2;
                while(true)
                {
                    *w1=Alpha::Blend(colour, *w1);
                    *w2=Alpha::Blend(colour, *w2);
                    if((d+=dir)<=0)
                    {
                        w1+=yi;
                        w2-=yi;
                        if((--dx)>0) continue;
                        *w1=Alpha::Blend(colour, *w1);
                        if((dy&1)==0) break;
                        *w2=Alpha::Blend(colour, *w2);
                        break;
                    }
                    else
                    {
                        w1+=xyi;
                        w2-=xyi;
                        d+=diu;
                        if((--dx)>0) continue;
                        *w1=Alpha::Blend(colour, *w1);
                        if((dy&1)==0) break;
                        *w2=Alpha::Blend(colour, *w2);
                        break;
                    }
                }
            }
            break;
        }

        SDL_UnlockSurface(_screen);
        return;
    }

    void Driver::DrawRect(int x1, int y1, int x2, int y2, u32 colour, bool filled)
    {
        if (filled)
        {
            switch (_blendMode)
            {
            case Video::None:
            default:
                SDL_Rect rFill;
                CreateRect(rFill, x1, y1, x2-x1, y2-y1);
                SDL_FillRect(_screen, &rFill, colour);
                break;
            case Video::Normal:
                int ydir=y1<y2?1:-1;
                for (int y=y1; y!=y2; y+=ydir)
                    this->HLine(x1, x2, y, colour);
                break;
            }
        } else {
            if (abs(y1-y2)>1)
            {
                this->VLine(x1, y1+1, y2-1, colour);
                this->VLine(x2, y1+1, y2-1, colour);
            }
            else
            {
                this->HLine(x1, x2, y1, colour);
                return;
            }

            if (abs(x1-x2)>1)
            {
                this->HLine(x1, x2, y1, colour);
                this->HLine(x1, x2, y2, colour);
            }
            else
            {
                this->VLine(x1, y1, y2, colour);
                return;
            }
        }

        return;
    }

    void Driver::HLine(int x1, int x2, int y, u32 colour)
    {
        if (x1 == x2)
        {
            this->DrawPixel(x1, y, colour);
            return;
        }

        if (x1>x2)
            swap(x1, x2);	

        if (ClipCoordinate(_screen, x1, y) && ClipCoordinate(_screen, x2, y))
        {

            switch (_blendMode)
            {
            case Video::None:
            default:
                SDL_Rect rFill;
                CreateRect(rFill, x1, y, x2-x1, 1);
                SDL_FillRect(_screen, &rFill, colour);
                break;
            case Video::Normal:
                SDL_LockSurface(_screen);
                RGBA* p = ((RGBA*)_screen->pixels) + (y * _screen->w) + x1;
                int xlen=x2-x1;
                do
                {
                    *p=Alpha::Blend(colour, *p);
                    p++;
                }
                while (xlen--);
                SDL_UnlockSurface(_screen);
                break;
            }

        }
    }

    void Driver::VLine(int x, int y1, int y2, u32 colour)
    {

        if (y1 == y2)
        {
            this->DrawPixel(x, y1, colour);
            return;
        }

        if (y1>y2)
            swap(y1, y2);

        if (ClipCoordinate(_screen, x, y1) && ClipCoordinate(_screen, x, y2))
        {

            switch (_blendMode)
            {
            case Video::None:
            default:
                SDL_Rect rFill;
                CreateRect(rFill, x, y1, 1, y2-y1);
                SDL_FillRect(_screen, &rFill, colour);
                break;
            case Video::Normal:
                SDL_LockSurface(_screen);
                RGBA* p = ((RGBA*)_screen->pixels) + (y1 * _screen->w) + x;

                int yinc=_screen->w;

                int ylen=y2-y1;

                do
                {
                    *p=Alpha::Blend(colour, *p);
                    p+=yinc;
                }
                while (ylen--);
                SDL_UnlockSurface(_screen);
                break;
            }
        }
    }


    void Driver::DrawEllipse(int cx, int cy, int radx, int rady, u32 colour, bool fill)
    {
        // fear this algorithm, for 'twas spawned in the very depths of hell itself
        int mx1, my1, mx2, my2;
        int aq, bq;
        int dx, dy;
        int r, rx, ry;
        int x;
        scanspan *rc;
        int scans, maxscans, endscan, scanoffset, prevscan, startscan;

        // convert negative radii to positive ones
        radx = abs(radx);
        rady = abs(rady);

        // storage array for scanline start/end values
        rc = new scanspan[(rady*2)+1];
        // clear the scanline array
        memset((void*)rc, 0, (rady*2*sizeof(scanspan))+1);

        // offset value for quick access to scanline array
        scanoffset = -cy+(rady);

        // your somewhat typical ellipse algorithm...
        mx1=cx-radx; my1=cy;
        mx2=cx+radx; my2=cy;

        aq=radx*radx;
        bq=rady*rady;
        dx=aq<<1;
        dy=bq<<1;
        r=radx*bq;
        rx=r<<1;
        ry=0;
        x=radx;

        while (x>0)
        {
            if (r>0)
            {
                my1++;
                my2--;
                ry+=dx;
                r-=ry;
            }
            if (r<=0)
            {
                x--;
                mx1++;
                mx2--;
                rx-=dy;
                r+=rx;
            }
            // but we don't draw any pixels
            scans = my1+scanoffset;
            // update this scanline's start/end positions
            if ((rc[scans].s < mx1) || (rc[scans].s == 0)) rc[scans].s = mx1;
            if ((rc[scans].e > mx2) || (rc[scans].e == 0)) rc[scans].e = mx2;
            scans = my2+scanoffset;
            // update this scanline's start/end positions
            if ((rc[scans].s < mx1) || (rc[scans].s == 0)) rc[scans].s = mx1;
            if ((rc[scans].e > mx2) || (rc[scans].e == 0)) rc[scans].e = mx2;
        }

        // set the middle scanline since the ellipse algorithm sucks and misses it
        rc[rady].s = cx-radx;
        rc[rady].e = cx+radx;

        // some looping vars
        maxscans = (rady*2);
        endscan = -1;
        scanoffset = -(rady) + cy; 
        startscan = 0;
        for (scans = 0; scans < maxscans; scans++)
        {
            // if the scanline was hit by the algorithm:
            if (rc[scans].s < rc[scans].e)
            {
                // if filling:
                if (fill)
                {
                    // if this is not the first valid scanline:
                    if (endscan != -1) 
                    {
                        // fill
                        this->HLine(rc[endscan].s, rc[endscan].e, endscan + scanoffset, colour);
                    }
                    else 
                    {
                        // save the indice of the first scanline
                        startscan = scans;
                    }
                    // update the previous scanline variable
                    endscan = scans;
                }
                else
                {
                    // if this is not the first valid scanline:
                    if (endscan != -1)
                    {
                        // if we're going down:
                        if (scans >= rady)
                        {
                            // then fill in the proper pieces of each scanline downward
                            prevscan = endscan + 1;
                            if (prevscan >= maxscans) prevscan = maxscans-1;
                            // left,
                            this->HLine(rc[endscan].s, rc[prevscan].s, endscan + scanoffset, colour);
                            // and right
                            this->HLine(rc[endscan].e, rc[prevscan].e, endscan + scanoffset, colour);
                        }
                        else if (scans < rady)
                        {
                            // figure out if we're at the top scanline
                            prevscan = endscan - 1;
                            if (prevscan < startscan)
                            {
                                // top scanline
                                this->HLine(rc[endscan].s, rc[endscan].e, endscan + scanoffset, colour);
                            }
                            else
                            {
                                // left,
                                this->HLine(rc[endscan].s, rc[prevscan].s, endscan + scanoffset, colour);
                                // and right
                                this->HLine(rc[endscan].e, rc[prevscan].e, endscan + scanoffset, colour);
                            }
                        }
                    }
                    else
                    {
                        // save the indice of the first scanline
                        startscan = scans;
                    }
                    // update the previous scanline variable
                    endscan = scans;
                }
            }
        }
        // finish off our ellipse with a nice flat bottom
        this->HLine(rc[endscan].s, rc[endscan].e, endscan +scanoffset, colour);    

        // delete the lookup table, we don't need it anymore
        delete rc;

        return;
    }

    u32 Driver::GetPixel(int x, int y)
    {
        return 0;
    }

    Point Driver::GetResolution() const
    {
        return Point(_xres, _yres);
    }

    int Driver::GetFrameRate() const
    {
        return fps.FPS();
    }
};
