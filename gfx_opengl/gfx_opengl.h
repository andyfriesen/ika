#ifndef GFX_OPENGL_H
#define GFX_OPENGL_H

#include <windows.h>
#include <gl/gl.h>
#include "SImage.h"

extern HWND hGLWnd;
extern int xres,yres,gbpp;
extern bool bFullscreen;
extern HDC  hDC;
extern HGLRC hRC;

extern int nImages;         // number of allocated images at any given moment.  Handy for debugging.
extern int nMaxtexsize;     // The biggest texture the card can handle

extern IMAGE hScreen;
extern IMAGE hRenderdest;

#endif
