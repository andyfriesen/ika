#ifndef GRAPH_H
#define GRAPH_H

#include <windows.h>
#include "importpng.h"

typedef int handle;
typedef unsigned int u32;

extern int (*gfxGetVersion)();																			// returns the version # of the driver
	  
extern bool (*gfxInit)(HWND hWnd,int xres,int yres,int bpp,bool fullscreen);							// sets up the graphics mode at the specified resolution (if you want, you can ignore bpp)
//extern void (*gfxShutdown)();																			// cleans up, IT IS THE APP'S RESPONSIBILITY TO FREE IMAGES, the DLL doesn't have to.
extern bool (*gfxSwitchToFullScreen)();
extern bool (*gfxSwitchToWindowed)();
extern bool (*gfxSwitchResolution)(int x,int y);

extern handle (*gfxCreateImage)(int x,int y);															// Creates a new image of the specified size, and returns the handle
extern bool (*gfxFreeImage)(handle img);
extern handle (*gfxCopyImage)(handle img);																// returns a copy of the image passed
extern bool (*gfxCopyPixelData)(handle img,u32* data,int width,int height);								// Give it an array of RGBA quads, and this function will copy the whole of it to the specified image.
extern bool (*gfxClipWnd)(int x1,int y1,int x2,int y2);

extern bool (*gfxClipImage)(handle img,Rect& r);															// All subsequent blit ops will only blit the specified subsection
extern handle (*gfxGetScreenImage)();
	  
extern bool (*gfxBlitImage)(handle img,int x,int y,bool transparent);									// blits the image on the current renderdest
extern bool (*gfxScaleBlitImage)(handle img,int x,int y,int width,int height,bool transparent);			// scale blit
extern bool (*gfxDistortBlitImage)(handle img,int x[4],int y[4],bool transparent);		// rotate/scale blit
extern bool (*gfxCopyChan)(handle src,int nSrcchan,handle dest,int nDestchan);
extern bool (*gfxSetPixel)(handle img,int x,int y,int colour);
extern int  (*gfxGetPixel)(handle img,int x,int y);
extern bool (*gfxLine)(handle img,int x1,int y1,int x2,int y2,int colour);
extern bool (*gfxRect)(handle img,int x1,int y1,int x2,int y2,int colour,bool filled);
extern bool (*gfxEllipse)(handle img,int cx,int cy,int rx,int ry,int colour,bool filled);
extern bool (*gfxFlatPoly)(handle img,int x[3],int y[3],int colour[3]);

extern bool (*gfxSetRenderDest)(handle newrenderdest);													// All subsequent blits go to this image, if it's valid
extern handle (*gfxGetRenderDest)();

extern bool (*gfxShowPage)();																			// copies the back buffer to the front.

extern int  (*gfxImageWidth)(handle img);
extern int  (*gfxImageHeight)(handle img);

extern bool (*gfxPaletteMorph)(int r,int g,int b);

extern bool SetUpGraphics(const string& dllname);
extern void UnloadGraphics();

extern bool gfxLoadImage(handle hImage,const char* fname);												// loads an image file into the specified image

#endif // ndef graph_h
