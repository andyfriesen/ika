#ifndef SIMAGE_H
#define SIMAGE_H

#include "types.h"
#include "pixel_matrix.h"

struct SImage
{
    CPixelMatrix pixels;
    const int& nWidth;
    const int& nHeight;
    const Rect& cliprect;
    
    u32 hTex;
    int nTexwidth,nTexheight;
    
    bool bAltered;
    bool bIsscreen;
    
    SImage();
    
    ~SImage();
};

typedef SImage* IMAGE;

#endif