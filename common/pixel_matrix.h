#ifndef PIXEL_MATRIX_H
#define PIXEL_MATRIX_H

#include "types.h"

// Pixel matrices are purely software representations of images.  Nothing more.
// Any time one is manipulating graphics for more than just user-feedback (editing image files, and such)
// these are used.

// gfx_opengl uses these as well, for offscreen drawing.

// TODO: Make all this stuff more robust and optimized, so that gfx_soft uses these directly.
//       Less code doing more work. ^_~
class CPixelMatrix
{
private:
    RGBA* pData;
    int nWidth,nHeight;
    
    Rect cliprect;
    
public:
    // con/destructors
    CPixelMatrix();
    CPixelMatrix(int width,int height);
    CPixelMatrix(u8* pData,int nWidth,int nHeight,u8* pal);
    CPixelMatrix(RGBA* pData,int nWidth,int nHeight);
    CPixelMatrix(const CPixelMatrix& src);
    virtual ~CPixelMatrix();
    
    // The basics
    void CopyPixelData(u8* data,int width,int height,u8* pal);
    void CopyPixelData(RGBA* data,int width,int height);
    CPixelMatrix& operator = (const CPixelMatrix& rhs);
    bool operator == (const CPixelMatrix& rhs);
    
    // Accessors
    inline const int& Width()   const { return nWidth;  }
    inline const int& Height()  const { return nHeight; }
    inline RGBA* GetPixelData() const { return pData;   }
    RGBA GetPixel(int x,int y);
    void SetPixel(int x,int y,RGBA c);

    // Misc junk
    void Clear(RGBA colour);
    void Rotate();
    void Flip();
    void Mirror();
    void Resize(int x,int y);
    
    const Rect& GetClipRect() { return cliprect; }
    void SetClipRect(Rect& r);
};

#include "pixel_matrix_blitter.h"

#endif