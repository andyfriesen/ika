#include <windows.h>
#include "pixel_matrix.h"
#include "misc.h"

static inline void DoClipping(int& x,int& y,int& xstart,int& xlen,int& ystart,int& ylen,const Rect& rClip)
{
    if (x<rClip.left)
    {
        xlen-=(rClip.left-x);
        xstart+=(rClip.left-x);
        x=rClip.left;
    }
    if (y<rClip.top)
    {
        ylen-=(rClip.top-y);
        ystart+=(rClip.top-y);
        y=rClip.top;
    }
    if (x+xlen>rClip.right)
        xlen=rClip.right-x;
    if (y+ylen>rClip.bottom)
        ylen=rClip.bottom-y;
}

CPixelMatrix::CPixelMatrix()
{
    nWidth=nHeight=16;		// arbitrary
    pData=new RGBA[nWidth*nHeight];
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(int width,int height)
{
    nWidth=width;
    nHeight=height;
    
    pData=new RGBA[nWidth*nHeight];
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(RGBA* pSrc,int width,int height)
{
    nWidth=width;
    nHeight=height;
    pData=new RGBA[width*height];
    memcpy(pData,pSrc,width*height*sizeof(RGBA));
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(u8* data,int width,int height,byte* pal)
{
    nWidth=width;
    nHeight=height;
    pData=new RGBA[nWidth*nHeight];
    for (int i=0; i<width*height; i++)
        pData[i]=RGBA(data[i],pal);
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(const CPixelMatrix& src)
{
    nWidth=src.nWidth;
    nHeight=src.nHeight;
    pData=new RGBA[nWidth*nHeight];
    memcpy(pData,src.pData,nWidth*nHeight*sizeof(RGBA));
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::~CPixelMatrix()
{
    delete[] pData;
}


CPixelMatrix& CPixelMatrix::operator = (const CPixelMatrix& rhs)
{
    if (this==&rhs)
        return *this;
    
    delete[] pData;
    
    nWidth=rhs.nWidth;
    nHeight=rhs.nHeight;
    pData=new RGBA[nWidth*nHeight];
    memcpy(pData,rhs.pData,nWidth*nHeight*sizeof(RGBA));
    
    return *this;
}

bool CPixelMatrix::operator == (const CPixelMatrix& rhs)
{
    if (nWidth!=rhs.nWidth)
        return false;
    if (nHeight!=rhs.nHeight)
        return false;
    
    return !memcmp(pData,rhs.pData,nWidth*nHeight*sizeof(RGBA));
}


void CPixelMatrix::CopyPixelData(RGBA* data,int width,int height)
{
    delete[] pData;
    
    nWidth=width;
    nHeight=height;
    
    pData=new RGBA[width*height];
    memcpy(pData,data,width*height*sizeof(RGBA));

    cliprect=Rect(0,0,width,height);
}

void CPixelMatrix::CopyPixelData(u8* data,int width,int height,byte* pal)
{
    delete[] pData;
    
    nWidth=width;
    nHeight=height;
    
    pData=new RGBA[width*height];
    for (int i=0; i<width*height; i++)
        pData[i]=RGBA(data[i],pal);
}

RGBA CPixelMatrix::GetPixel(int x,int y)
{
    if (x<0 || x>=nWidth)
        return RGBA(0,0,0,0);
    if (y<0 || y>=nHeight)
        return RGBA(0,0,0,0);
    
    return pData[y*nWidth+x];
}

void CPixelMatrix::SetPixel(int x,int y,RGBA c)
{
    if (x<0 || x>=nWidth)
        return;
    if (y<0 || y>=nHeight)
        return;
    
    pData[y*nWidth+x]=c;
}

void CPixelMatrix::Clear(RGBA colour)
{
    // I feel like being nasty right now >:D
    RGBA* p=pData+nWidth*nHeight-1;
    
    while (p>=pData)
        *p--=colour;
}

void CPixelMatrix::Rotate()
{
    CPixelMatrix temp(*this);
    
    for (int y=0; y<nHeight; y++)
        for (int x=0; x<nWidth; x++)
            SetPixel(y,x,temp.GetPixel(x,y));
}

void CPixelMatrix::Flip()
{
    CPixelMatrix temp(*this);
    
    for (int y=0; y<nHeight; y++)
        for (int x=0; x<nWidth; x++)
            SetPixel(x,y,temp.GetPixel(x,nHeight-y-1));
}

void CPixelMatrix::Mirror()
{
    CPixelMatrix temp(*this);
    
    for (int y=0; y<nHeight; y++)
        for (int x=0; x<nWidth; x++)
            SetPixel(x,y,temp.GetPixel(nWidth-x-1,y));
}

void CPixelMatrix::Resize(int x,int y)
{
    if (x<1 || y<1)
        return;
    
    RGBA* pTemp=new RGBA[x*y];
    
    RGBA* pSrc=pData;
    RGBA* pDest=pTemp;
    
    int ylen=nHeight<y?nHeight:y;
    int nCopywidth=sizeof(RGBA)* (nWidth<x ? nWidth : x);							// smaller of the two
    while (ylen--)
    {
        memcpy(pDest,pSrc,nCopywidth);
        pDest+=x;
        pSrc+=nWidth;
    }
    
    delete[] pData;
    pData=pTemp;
    nWidth=x;
    nHeight=y;

    SetClipRect(Rect(0,0,nWidth,nHeight));
}

void CPixelMatrix::SetClipRect(Rect& r)
{
    cliprect.left=max(0,r.left);
    cliprect.top=max(0,r.top);
    cliprect.right=min(nWidth,r.right);
    cliprect.bottom=min(nHeight,r.bottom);

    if (r.Width()<0)    swap(r.left,r.right);
    if (r.Height()<0)   swap(r.top,r.bottom);
}