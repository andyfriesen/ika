#include <stdexcept>

#include "corona.h"

#include "Canvas.h"
#include "misc.h"

static inline void DoClipping(int& x, int& y, int& xstart, int& xlen, int& ystart, int& ylen, const Rect& rClip)
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

Canvas::Canvas()
{
    _width=_height=16;		// arbitrary
    _pixels=new RGBA[_width*_height];
    _cliprect=Rect(0, 0, _width, _height);
}

Canvas::Canvas(int width, int height)
{
    _width=width;
    _height=height;
    
    _pixels=new RGBA[_width*_height];
    _cliprect=Rect(0, 0, _width, _height);
}

Canvas::Canvas(RGBA* pSrc, int width, int height)
{
    _width=width;
    _height=height;
    _pixels=new RGBA[width*height];
    memcpy(_pixels, pSrc, width*height*sizeof(RGBA));
    _cliprect=Rect(0, 0, _width, _height);
}

Canvas::Canvas(u8* data, int width, int height, u8* pal)
{
    _width=width;
    _height=height;
    _pixels=new RGBA[_width*_height];
    for (int i=0; i<width*height; i++)
        _pixels[i]=RGBA(data[i], pal);
    _cliprect=Rect(0, 0, _width, _height);
}

Canvas::Canvas(const Canvas& src)
{
    _width = src._width;
    _height = src._height;
    _pixels = new RGBA[_width*_height];
    memcpy(_pixels, src._pixels, _width*_height*sizeof(RGBA));
    _cliprect = Rect(0, 0, _width, _height);
}

Canvas::Canvas(const char* fname)
{
    corona::Image* img = corona::OpenImage(fname, corona::FF_AUTODETECT, corona::PF_R8G8B8A8);
    if (!img)
        throw std::runtime_error(std::string("Unable to load image file ") + fname);

    RGBA* src = (RGBA*)img->getPixels();
    _width  = img->getWidth();
    _height = img->getHeight();
    _pixels = new RGBA[_width * _height];
    _cliprect = Rect(0, 0, _width, _height);

    std::copy(src, src + _width * _height, _pixels);

    delete img;
}

Canvas::~Canvas()
{
    delete[] _pixels;
}


void Canvas::Save(const char* fname)
{
    corona::Image* img = corona::CreateImage(_width, _height, corona::PF_R8G8B8A8);
    RGBA* dest = (RGBA*)img->getPixels();
    std::copy(_pixels, _pixels + _width * _height, dest);
    corona::SaveImage(fname, corona::FF_PNG, img);
    delete img;
}

Canvas& Canvas::operator = (const Canvas& rhs)
{
    if (this==&rhs)
        return *this;
    
    delete[] _pixels;
    
    _width=rhs._width;
    _height=rhs._height;
    _pixels=new RGBA[_width*_height];
    memcpy(_pixels, rhs._pixels, _width*_height*sizeof(RGBA));
    
    return *this;
}

bool Canvas::operator == (const Canvas& rhs)
{
    if (_width!=rhs._width)
        return false;
    if (_height!=rhs._height)
        return false;
    
    return !memcmp(_pixels, rhs._pixels, _width*_height*sizeof(RGBA));
}


void Canvas::CopyPixelData(RGBA* data, int width, int height)
{
    delete[] _pixels;
    
    _width=width;
    _height=height;
    
    _pixels=new RGBA[width*height];
    memcpy(_pixels, data, width*height*sizeof(RGBA));

    _cliprect=Rect(0, 0, width, height);
}

void Canvas::CopyPixelData(u8* data, int width, int height, u8* pal)
{
    delete[] _pixels;
    
    _width=width;
    _height=height;
    
    _pixels=new RGBA[width*height];
    for (int i=0; i<width*height; i++)
        _pixels[i]=RGBA(data[i], pal);
}

RGBA Canvas::GetPixel(int x, int y)
{
    if (x<0 || x>=_width)
        return RGBA(0, 0, 0, 0);
    if (y<0 || y>=_height)
        return RGBA(0, 0, 0, 0);
    
    return _pixels[y*_width+x];
}

void Canvas::SetPixel(int x, int y, RGBA c)
{
    if (x<0 || x>=_width)
        return;
    if (y<0 || y>=_height)
        return;
    
    _pixels[y*_width+x]=c;
}

void Canvas::Clear(RGBA colour)
{
    // I feel like being nasty right now >:D
    RGBA* p=_pixels+_width*_height-1;
    
    while (p>=_pixels)
        *p--=colour;
}

void Canvas::Rotate()
{
    Canvas temp(*this);
    
    for (int y=0; y<_height; y++)
        for (int x=0; x<_width; x++)
            SetPixel(_height - y, x, temp.GetPixel(x, y));
}

void Canvas::Flip()
{
    Canvas temp(*this);
    
    for (int y=0; y<_height; y++)
        for (int x=0; x<_width; x++)
            SetPixel(x, y, temp.GetPixel(x, _height-y-1));
}

void Canvas::Mirror()
{
    Canvas temp(*this);
    
    for (int y=0; y<_height; y++)
        for (int x=0; x<_width; x++)
            SetPixel(x, y, temp.GetPixel(_width-x-1, y));
}

void Canvas::Resize(int x, int y)
{
    if (x<1 || y<1)
        return;
    
    RGBA* pTemp=new RGBA[x*y];
    
    RGBA* pSrc=_pixels;
    RGBA* pDest=pTemp;
    
    int ylen=_height<y?_height:y;
    int nCopywidth=sizeof(RGBA)* (_width<x ? _width : x);							// smaller of the two
    while (ylen--)
    {
        memcpy(pDest, pSrc, nCopywidth);
        pDest+=x;
        pSrc+=_width;
    }
    
    delete[] _pixels;
    _pixels=pTemp;
    _width=x;
    _height=y;

    _cliprect = Rect(0, 0, _width, _height);
}

void Canvas::SetClipRect(const Rect& r)
{
    _cliprect.left=max(0, r.left);
    _cliprect.top=max(0, r.top);
    _cliprect.right=min(_width, r.right);
    _cliprect.bottom=min(_height, r.bottom);
}
