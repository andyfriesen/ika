// This is pretty cool stuff.  Basically, I'm using C++ templates to inline the pixel blending functions.
// Not a lot of code doing a whole lot of work. :)

// I may run into some problems with namespaces later on; the blending modes,
// and the namespace 'Blitter' is general to the point of being vague.

#include "types.h"
#include "utility.h"

namespace Blitter {

    struct BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA) const {
            return src;
        }    
    };
    
    struct OpaqueBlend : BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA) const {
            return src;
        }
    };

    struct MatteBlend : BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA dest) const {
            if (src.a) {
                return src;
            } else {
                return dest;
            }
        }
    };

    struct AlphaBlend : BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA dest) const {
			RGBA result;
            int finalAlpha = src.a + ((255 - src.a) * dest.a) / 255;
            int sourceAlpha = (finalAlpha == 0) ? 0 : src.a * 255 / finalAlpha;

            result.r = (src.r * sourceAlpha + dest.r * (255 - sourceAlpha)) >> 8;
            result.g = (src.g * sourceAlpha + dest.g * (255 - sourceAlpha)) >> 8;
            result.b = (src.b * sourceAlpha + dest.b * (255 - sourceAlpha)) >> 8;
            result.a = finalAlpha;
            return result;
        }
    };

	struct PreserveBlend : BlendType {
		inline virtual RGBA operator()(RGBA src, RGBA dest) const {
            // Trivial cases: handle zero and full alpha.
            if (!src.a) return dest;
			if (!dest.a) return 0;

            u8  a = src.a;

            RGBA result;
            result.a = dest.a;

            // Classic alpha blend:
            // dest = (source * opacity) + (dest * (1 - opacity))
            result.r = ( (src.r * a) + (dest.r * (255 - a)) ) >> 8;
            result.g = ( (src.g * a) + (dest.g * (255 - a)) ) >> 8;
            result.b = ( (src.b * a) + (dest.b * (255 - a)) ) >> 8;
			return result;
		}
	};

    struct AddBlend : BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA dest) const {
            // add and clamp to 255
            dest.r = (u8)min<int>(dest.r + src.r, 255);
            dest.g = (u8)min<int>(dest.g + src.g, 255);
            dest.b = (u8)min<int>(dest.b + src.b, 255);
            dest.a = (u8)min<int>(dest.a + src.a, 255);
            return dest;
        }
    };

    struct SubtractBlend : BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA dest) const {
            // subtract and clamp at 0
            dest.r = (u8)max<int>(dest.r - src.r, 0);
            dest.g = (u8)max<int>(dest.g - src.g, 0);
            dest.b = (u8)max<int>(dest.b - src.b, 0);
            dest.a = (u8)max<int>(dest.a - src.a, 0);
            return dest;
        }
    };

    struct MultiplyBlend : BlendType {
        inline virtual RGBA operator()(RGBA src, RGBA dest) const {
            // multiply out of a total of 255
            dest.r = dest.r * src.r >> 8;
            dest.g = dest.g * src.g >> 8;
            dest.b = dest.b * src.b >> 8;
            dest.a = dest.a * src.a >> 8;
            return dest;
        }
    };

    BlendType* GetBlender(int blendId);
    
    /**
     * Helper function: Adjusts start and run length values on
     * x and y axes based on the clip rectangle given.
     */
    void DoClipping(int& x, int& y, int& xstart, int& xlen, int& ystart, int& ylen, const Rect& rClip);
    
    void AlphaMask(Canvas& src); 
    
    /// Renders an image on another image.
    template <typename Blender>
    void Blit(const Canvas& src, Canvas& dest, int x, int y, const Blender& blend) {
        const Rect& r = src.GetClipRect();

        int xstart = r.left;
        int ystart = r.top;
        int xlen = r.Width();
        int ylen = r.Height();

        DoClipping(x, y, xstart, xlen, ystart, ylen, dest.GetClipRect());
        if (xlen < 1 || ylen < 1) {
            // completely clipped
            return;
        }

        RGBA* sourcePixel  = src.GetPixels() + (ystart * src.Width()) + xstart;
        RGBA* destPixel = dest.GetPixels() + (y * dest.Width()) + x;
        int  srcinc= src.Width() - xlen;
        int  destinc = dest.Width() - xlen;

        while (ylen) {
            int x = xlen;
            while (x--) {
                *destPixel = blend(*sourcePixel, *destPixel);
                destPixel++;
                sourcePixel++;
            }

            sourcePixel += srcinc;
            destPixel += destinc;
            --ylen;
        }
    }

    /// Renders an image on another image, stretching as necessary.
    template <typename Blender>
    static void ScaleBlit(const Canvas& src, Canvas& dest, int cx, int cy, int w, int h, const Blender& blend) {
        int x, y;               // current pixel position
        int ix, iy;             // current image location (fixed point 16.16)
        int xinc, yinc;         // Increment to ix, iy per screen pixel (fixed point)

        int xstart, ystart;     // position of first pixel to copy (on the image)
        int xlen, ylen;         // number of screen pixels to copy along each axis

        if (w == 0 || h == 0)   return;

        ix = iy = 0;
        x = cx; y = cy;

        const Rect& srcclip = src.GetClipRect();

        xinc = ((srcclip.Width()) << 16) / w;
        yinc = ((srcclip.Height()) << 16) / h;

        xstart = srcclip.left;
        ystart = srcclip.top;
        xlen = w;
        ylen = h;

        DoClipping(x, y, xstart, xlen, ystart, ylen, dest.GetClipRect());
        if (xlen < 1 || ylen < 1)       return; // image is entirely offscreen

        int xs = xinc * xstart;
        int ys = yinc * ystart;

        ix = xs;
        iy = ys & 0xFFFF;

        RGBA* pSrc  = src.GetPixels() + ((ys >> 16) * src.Width());
        RGBA* pDest = dest.GetPixels() + (y * dest.Width()) + x;

        x = 0;
        y = h;

        while (ylen) {
            while (x < xlen) {
                pDest[x] = blend(pSrc[ix >> 16], pDest[x]);
                ix += xinc;
                ++x;
            }

            x = 0;
            ix = xs;

            pDest += dest.Width();

            iy += yinc;
            pSrc += (iy>>16) * src.Width();
            iy &= 0xFFFF;
            --ylen;
        }
    }

    template <typename Blender>
    static void TileBlit(const Canvas& src, Canvas& dest, int startX, int startY, int w, int h, int offsetX, int offsetY, const Blender& blend) {
        Rect oldClipRect = dest.GetClipRect();
        dest.SetClipRect(Rect(startX, startY, startX + w, startY + h));

        int curX = startX % src.Width() + offsetX;
        int curY = startY % src.Height() + offsetY;
        int lenX = w % src.Width();
        int lenY = h % src.Height();

        for (int y = 0; y < lenY; y++) {
            for (int x = 0; x < lenX; x++) {
                Blit(src, dest, curX, curY, blend);
                curX += src.Width();
            }
            curX -= src.Width() * lenX;
            curY += src.Height();
        }

        dest.SetClipRect(oldClipRect);
    }

    // ------------------------------------ Primitives ------------------------------------

    /// Draws a single dot on the image.
    template <typename Blender>
    static inline void SetPixel(Canvas& img, int x, int y, RGBA colour, Blender& blend) {
        RGBA* p = img.GetPixels()+(y * img.Width() + x);

        *p = blend(colour, *p);
    }

    /// Draws a horizontal line on the image.
    template <typename Blender>
    void HLine(Canvas& img, int x1, int x2, int y, RGBA colour, Blender& blend) {
        const Rect& r = img.GetClipRect();

        if (y < r.top || y >= r.bottom)
            return;

        if (x1 > x2)
            swap(x1, x2);

        //keepinrange(x1, r.left, r.right - 1);
        //keepinrange(x2, r.left, r.right - 1);
        x1 = clamp(x1, r.left, r.right - 1);
        x2 = clamp(x2, r.left, r.right - 1);

        RGBA* p = img.GetPixels()+(y * img.Width())+x1;

        int xlen = x2 - x1;

        while (xlen--) {
            *p = blend(colour, *p);
            p++;
        }

    }

    /// Draws a vertical line on the image.
    template <typename Blender>
    void VLine(Canvas& img, int x, int y1, int y2, RGBA colour, Blender& blend) {
        const Rect& r = img.GetClipRect();

        if (x < r.left || x >= r.right) return;

        if (y1 > y2)
            swap(y1, y2);

        //keepinrange(y1, r.top, r.bottom - 1);
        //keepinrange(y2, r.top, r.bottom - 1);
        y1 = clamp(y1, r.top, r.bottom - 1);
        y2 = clamp(y2, r.top, r.bottom - 1);

        RGBA* p = img.GetPixels()+(y1 * img.Width())+x;

        int yinc = img.Width();

        int ylen = y2 - y1;

        while (ylen--) {
            *p = blend(colour, *p);
            p += yinc;
        }
    }

    /// Draws a rectangle (outline or filled) on the image.
    template <typename Blender>
    void DrawRect(Canvas& img, int x1, int y1, int x2, int y2, RGBA colour, bool filled, Blender& blend) {
        if (filled) {
            int ydir = y1 < y2 ? 1 : -1;
            for (int y = y1; y != y2; y += ydir) {
                HLine(img, x1, x2, y, colour, blend);
            }
        } else {
            if (abs(y1 - y2) > 1) {
                VLine(img, x1, y1 + 1, y2 - 1, colour, blend);
                VLine(img, x2, y1 + 1, y2 - 1, colour, blend);
            } else {
                HLine(img, x1, x2, y1, colour, blend);
                return;
            }

            if (abs(x1 - x2) > 1) {
                HLine(img, x1, x2, y1, colour, blend);
                HLine(img, x1, x2, y2, colour, blend);
            } else {
                VLine(img, x1, y1, y2, colour, blend);
                return;
            }
        }
    }

    /**
    *  Draws an arbitrary line on the image.
    *  Kudos to zeromus for the algorithm.
    */
    template <typename Blender>
    static void DrawLine(Canvas& img, int x1, int y1, int x2, int y2, u32 colour, Blender& blend) {
        // check for the cases in which the line is vertical or horizontal or only one pixel big
        // we can do those faster through other means
        if(y1 == y2 && x1 == x2) {
            SetPixel(img, x1, y1, (RGBA) colour, blend);
            return;
        }
        if(y1 == y2) {
            HLine(img, x1, x2, y1, (RGBA) colour, blend);
            return;
        }
        if(x1 == x2) {
            VLine(img, x1, y1, y2, (RGBA) colour, blend);
            return;
        }

        const Rect& r = img.GetClipRect();

        // it's good to have these handy
        int cx1 = r.left;
        int cx2 = r.right - 1;
        int cy1 = r.top;
        int cy2 = r.bottom - 1;

        // variables for the clipping code
        int v1 = 0, v2 = 0;

        // here begins what is apparently the Cohen - Sutherland line clipping algorithm
        // there's a doc on it here: http://reality.sgi.com / tomcat_asd / algorithms.html#clipping
        if(y1 < cy1) v1 |= 8;
        if(y1 > cy2) v1 |= 4;
        if(x1 > cx2) v1 |= 2;
        if(x1 < cx1) v1 |= 1;
        if(y2 < cy1) v2 |= 8;
        if(y2 > cy2) v2 |= 4;
        if(x2 > cx2) v2 |= 2;
        if(x2 < cx1) v2 |= 1;

        while((v1 & v2) == 0 && (v1 | v2) != 0) {
            if(v1) {
                if(v1 & 8) {
                    // clip above
                    x1 -= ((x1 - x2) * (cy1 - y1))/(y2 - y1 + 1);
                    y1 = cy1;
                } else if(v1 & 4) {
                    // clip below
                    x1 -= ((x1 - x2) * (y1 - cy2))/(y1 - y2 + 1);
                    y1 = cy2;
                } else if(v1 & 2) {
                    // clip right
                    y1 -= ((y1 - y2) * (x1 - cx2))/(x1 - x2 + 1);
                    x1 = cx2;
                } else {
                    // clip left
                    y1 -= ((y1 - y2) * (cx1 - x1))/(x2 - x1 + 1);
                    x1 = cx1;
                }
                v1 = 0;
                if(y1 < cy1) v1 |= 8;
                if(y1 > cy2) v1 |= 4;
                if(x1 > cx2) v1 |= 2;
                if(x1 < cx1) v1 |= 1;
            } else {
                if(v2 & 8) {
                    // clip above
                    x2 -= ((x2 - x1) * (cy1 - y2))/(y1 - y2 + 1);
                    y2 = cy1;
                } else if(v2 & 4) {
                    // clip below
                    x2 -= ((x2 - x1) * (y2 - cy2))/(y2 - y1 + 1);
                    y2 = cy2;
                } else if(v2 & 2) {
                    // clip right
                    y2 -= ((y2 - y1) * (x2 - cx2))/(x2 - x1 + 1);
                    x2 = cx2;
                } else {
                    // clip left
                    y2 -= ((y2 - y1) * (cx1 - x2))/(x1 - x2 + 1);
                    x2 = cx1;
                }
                v2 = 0;
                if(y2 < cy1) v2 |= 8;
                if(y2 > cy2) v2 |= 4;
                if(x2 > cx2) v2 |= 2;
                if(x2 < cx1) v2 |= 1;
            }
        }

        // this tells us if the line was clipped in its entirety. yum!
        if(v1 & v2)
            return;

        //make these checks again
        if(y1 == y2 && x1 == x2) {
            SetPixel(img, x1, y1, (RGBA) colour, blend);
            return;
        }
        if(y1 == y2) {
            HLine(img, x1, x2, y1, (RGBA) colour, blend);
            return;
        }
        if(x1 == x2) {
            VLine(img, x1, y1, y2, (RGBA) colour, blend);
            return;
        }

        //for the line renderer
        int xi, yi, xyi;
        int d, dir, diu, dx, dy;

        //two pointers because we render the line from both ends
        RGBA* w1 = img.GetPixels() + (img.Width() * y1 + x1);
        RGBA* w2 = img.GetPixels() + (img.Width() * y2 + x2);

        //start algorithm presently.
        xi = 1;
        if((dx = x2 - x1)<0) {
            dx = -dx;
            xi = -1;
        }

        yi = img.Width();
        if((dy = y2 - y1)<0) {
            yi = -yi;
            dy = -dy;
        }

        xyi = xi + yi;

        if(dy < dx) {
            dir = dy * 2;
            d = -dx;
            diu = 2 * d;
            dy = dx / 2;

            for(;;) {
                *w1 = blend(colour, *w1);
                *w2 = blend(colour, *w2);
                if((d += dir) <= 0) {
                    w1 += xi;
                    w2 -= xi;
                    if((--dy) > 0) continue;
                    *w1 = blend(colour, *w1);
                    if((dx & 1) == 0) return;
                    *w2 = blend(colour, *w2);
                    return;
                } else {
                    w1 += xyi;
                    w2 -= xyi;
                    d += diu;
                    if((--dy) > 0) continue;
                    *w1 = blend(colour, *w1);
                    if((dx & 1) == 0) return;
                    *w2 = blend(colour, *w2);
                    return;
                }
            }
        } else {
            dir = dx * 2;
            d = -dy;
            diu = d * 2;
            dx = dy / 2;
            for(;;) {
                *w1 = blend(colour, *w1);
                *w2 = blend(colour, *w2);
                if((d += dir) <= 0) {
                    w1 += yi;
                    w2 -= yi;
                    if((--dx) > 0) continue;
                    *w1 = blend(colour, *w1);
                    if((dy & 1) == 0) return;
                    *w2 = blend(colour, *w2);
                    return;
                } else {
                    w1 += xyi;
                    w2 -= xyi;
                    d += diu;
                    if((--dx) > 0) continue;
                    *w1 = blend(colour, *w1);
                    if((dy & 1) == 0) return;
                    *w2 = blend(colour, *w2);
                    return;
                }
            }
        }
        return;
    }

}
