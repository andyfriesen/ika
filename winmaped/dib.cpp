
#define WINVER 0x0400
#include "dib.h"

namespace
{
    const int bpp=4;                                                                                            // BYTES PER PIXEL (not bits)
};

void CDIB::CreateDIB(int width,int height)
{
    
    if (hDC)		DeleteDC(hDC);
    if (hBitmap)	DeleteObject(hBitmap);
    
    hDC=CreateCompatibleDC(NULL);
    
    BITMAPINFO bmi;
    ZeroMemory(&bmi,sizeof bmi);
    BITMAPINFOHEADER& bmih=bmi.bmiHeader;
    
    bmih.biSize        = sizeof(bmih);
    bmih.biWidth       = width;
    bmih.biHeight      = -height;
    bmih.biPlanes      = 1;
    bmih.biBitCount    = bpp*8;											// bytes per pixel --> bits per pixel
    bmih.biCompression = BI_RGB;
    
    hBitmap=CreateDIBSection(hDC,&bmi,DIB_RGB_COLORS,(void**)&pPixels,NULL,0);
    
    SelectObject(hDC,hBitmap);
    
    nWidth=width;
    nHeight=height;
}

CDIB::CDIB(int width,int height)
{
    hDC=0;
    hBitmap=0;
    CreateDIB(width,height);
}

CDIB::CDIB(const CDIB& s)
{
    hDC=0;
    hBitmap=0;
    
    CreateDIB(s.nWidth,s.nHeight);
    
    memcpy(pPixels,s.pPixels,nWidth*nHeight*bpp);
}

CDIB::CDIB(const CPixelMatrix& s)
{
    hDC=0;
    hBitmap=0;
    
    CreateDIB(s.Width(),s.Height());
  
    CopyPixelData(s.GetPixelData(),s.Width(),s.Height());
}

CDIB::~CDIB()
{
    if (hDC)		DeleteDC(hDC);
    if (hBitmap)	DeleteObject(hBitmap);
}

CDIB& CDIB::operator = (const CPixelMatrix& s)
{
    CreateDIB(s.Width(),s.Height());

    RGBA* pSrc=s.GetPixelData();
    
    for (int i=0; i<nWidth*nHeight; i++)
        pPixels[i]=*pSrc++;

    return *this;
}

void CDIB::SetPixel(int x,int y,RGBA colour)
{
    if (x<0 || y<0) return;
    if (x>=nWidth || y>=nHeight) return;
    
    pPixels[y*nWidth+x]=colour;
}

void CDIB::CopyPixelData(RGBA* pixels,int width,int height)
{
    CreateDIB(width,height);
    
    if (bpp==1)
        memcpy(pPixels,pixels,width*height*bpp);
    else
    {
        for (int i=0; i<width*height; i++)
            pPixels[i]=pixels[i];
    }
}