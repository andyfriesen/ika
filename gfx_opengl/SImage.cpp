#include "SImage.h"

#include <windows.h>
#include <gl/gl.h>

SImage::SImage()
    : hTex(0)
    , nTexwidth(0)
    , nTexheight(0)
    , bAltered(false)
    , bIsscreen(false)
    , nWidth(pixels.Width())
    , nHeight(pixels.Height())
    , cliprect(pixels.GetClipRect())
    {}
    
SImage::~SImage()
{
    if (hTex)
        glDeleteTextures(1,&hTex);
}
