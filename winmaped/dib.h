
#ifndef CDIB_H
#define CDIB_H

/*
I get the feeling that I've got a million different abstractions for images. -_-

  Class wrapper for win32 Device Independant Bitmap Sections. (usually referred to as DIB sections)
*/

#include <windows.h>
#include "types.h"
#include "pixel_matrix.h"

class CDIB
{
private:
    HBITMAP hBitmap;														// Object handle, not used for much, but still important
    u32*	pPixels;														// pointer to actual pixel data
    HDC		hDC;															// Device context -- used when blitting to/from this DIB
    u8		pPal[768];														// palette, 24bpp RGB
    
    int nWidth,nHeight;														// Dimensions
    int		bpp;															// bytes per pixel (I find that storing bits per pixel is pointless)
    
    void CreateDIB(int width,int height,int bpp,u8* pal=0);
    
    inline static u32 SwapBR(u32 c)											// converts RGBA to BGRA and back
    {
        u32 r=c&0x00FF0000;
        u32 b=c&0x000000FF;
        c&=0xFF00FF00;
        r>>=16;  b<<=16;
        return c|r|b;
    }
    
public:
    CDIB(int width,int height,int bpp,u8* pal=0);
    CDIB(const CDIB& s);
    CDIB(const CPixelMatrix& s);
    ~CDIB();
    
    inline HDC GetDC() const { return hDC; }
    inline void* GetPixelData() const { return pPixels; }
    inline int	Width() const { return nWidth; }
    inline int	Height() const { return nHeight;	}
    
    operator = (const CPixelMatrix& s);										// more efficient than using the constructor implicitly
    
    const u8* GetPalette() const { return pPal; }
    void  SetPalette(u8* newpal);
    
    void SetPixel(int x,int y,u32 colour);
    
    void CopyPixelData(void* pixels,int width,int height,int bpp,u8* pal);
};

#endif