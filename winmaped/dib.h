
#ifndef CDIB_H
#define CDIB_H

/*
I get the feeling that I've got a million different abstractions for images. -_-

  Class wrapper for win32 Device Independant Bitmap Sections. (usually referred to as DIB sections)
*/

#include <windows.h>
#include "types.h"
#include "Canvas.h"

class CDIB : public Canvas
{
private:
    HBITMAP hBitmap;                                            // Object handle, not used for much, but still important
    HDC         hDC;                                            // Device context -- used when blitting to/from this DIB
    
    void CreateDIB(int width,int height);
    
public:
    CDIB(int width,int height);
    CDIB(const CDIB& s);
    CDIB(const Canvas& s);
    ~CDIB();
    
    inline HDC GetDC() const { return hDC; }
    inline void* GetPixelData() const { return _pixels; }
    inline int  Width() const { return _width; }
    inline int  Height() const { return _height;        }
    
    CDIB& operator = (const Canvas& s);                         // more efficient than using the constructor implicitly
    
    void SetPixel(int x,int y,RGBA colour);
    
    void CopyPixelData(RGBA* pixels,int width,int height);
};

#endif