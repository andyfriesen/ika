
#ifndef CDIB_H
#define CDIB_H

/*
I get the feeling that I've got a million different abstractions for images. -_-

  Class wrapper for win32 Device Independant Bitmap Sections. (usually referred to as DIB sections)
*/

#include <windows.h>
#include "types.h"
#include "pixel_matrix.h"

class CDIB : public CPixelMatrix
{
private:
    HBITMAP hBitmap;														// Object handle, not used for much, but still important
    BGRA*	pPixels;														// pointer to actual pixel data
    HDC		hDC;															// Device context -- used when blitting to/from this DIB
    
    int nWidth,nHeight;														// Dimensions
    
    void CreateDIB(int width,int height);
    
public:
    CDIB(int width,int height);
    CDIB(const CDIB& s);
    CDIB(const CPixelMatrix& s);
    ~CDIB();
    
    inline HDC GetDC() const { return hDC; }
    inline void* GetPixelData() const { return pPixels; }
    inline int	Width() const { return nWidth; }
    inline int	Height() const { return nHeight;	}
    
    operator = (const CPixelMatrix& s);										// more efficient than using the constructor implicitly
    
    void SetPixel(int x,int y,RGBA colour);
    
    void CopyPixelData(RGBA* pixels,int width,int height);
};

#endif