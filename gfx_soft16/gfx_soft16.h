/*
	Interface, foo!  This is our spiffy universal interface poo.

--------------- VERSION 1 ------------------------

If version is >1, then it can have extras, depending on what we feel like doing. ^_^
Methods that aren't present in the DLL must always return false.

Alpha is desired, but ditch it if you want the extra speed.

Also, handles have arbitrary meaning, and they're 4 bytes in size.  Make them actual pointers if you want to, as long 
as you clean up. (and have guts)

TODO: add gfxSwitchToFullScreen and gfxSwitchToWindowed, also, more blit functions and methods to alter images.

Maybe add routines for switching bitdepths.
*/

///////////////////////////////////////////////////////////////////////

#ifndef GFX_SOFT_H
#define GFX_SOFT_H

#include "types.h"

#include <ddraw.h>

typedef class SImage* handle;

class SImage	// A struct with a constructor
{
public:
	int		nWidth,nHeight;
	int		nPitch;				// pitch is in pixels
	u16*	pData;
	byte*	pAlpha;
	RECT	rClip;

	bool (*Blit)(handle img,int x,int y);
	bool (*ScaleBlit)(handle img,int x,int y,int width,int height);
	bool (*RotScaleBlit)(handle img,int cx,int cy,float angle,int scale);

	SImage()
	{	
		pData=0;
		pAlpha=0;
		nWidth=nHeight=nPitch=0;
		Blit=0;
		ScaleBlit=0;
		RotScaleBlit=0;
	}
};

extern handle hRenderdest;									// handle to the image that we blit to

int gfxGetVersion();																					// returns the version # of the driver
	  
bool gfxInit(HWND hWnd,int xres,int yres,int bpp,bool fullscreen);										// sets up the graphics mode at the specified resolution (a driver may or may not pay attention to bpp and/or fullscreen)
void gfxShutdown();																						// cleans up, IT IS THE APP'S RESPONSIBILITY TO FREE IMAGES, the DLL doesn't have to babysit.

bool gfxSwitchToFullScreen();
bool gfxSwitchToWindowed();

handle gfxCreateImage(int x,int y);																		// Creates a new image of the specified size, and returns the handle
bool gfxFreeImage(handle img);
bool gfxCopyPixelData(handle img,u32* data,int width,int height);										// Give it an array of RGBA u32s, and this function will copy the whole of it to the specified image.
bool gfxClipImage(handle img,RECT r);
handle gfxGetScreenImage();																				// returns a handle to the screen image
bool gfxClipWnd(int x1,int y1,int x2,int y2);
bool gfxSwitchResolution(int x,int y);

bool gfxShowPage();																						// copies the back buffer to the front.
bool gfxBlitImage(handle img,int x,int y,bool transparent);												// blits the image on the current renderdest
bool gfxScaleBlitImage(handle img,int x,int y,int width,int height,bool transparent);					// scale blit
bool gfxRotScaleImage(handle img,int cx,int cy,float angle,int scale,bool transparent);					// rotate/scale blit
bool gfxCopyChannel(handle src,int nSrcchan,handle dest,int nDestchan);
bool gfxSetPixel(handle img,int x,int y,u32 colour);
bool gfxLine(handle img,int x1,int y1,int x2,int y2,u32 colour);
bool gfxRect(handle img,int x1,int y1,int x2,int y2,u32 colour,bool filled);
bool gfxEllipse(handle img,int cx,int cy,int rx,int ry,u32 colour,bool filled);
// bool gfxFlatPoly
// bool gfxTMapPoly

bool gfxSetRenderDest(handle newrenderdest);															// All subsequent blits go to this image, if it's valid
handle gfxGetRenderDest();

int  gfxImageWidth(handle img);
int  gfxImageHeight(handle img);


/////////////// Internal stuff ////////////////////////

void HLine(handle img,int x1,int x2,int y,u32 colour);
void VLine(handle img,int x,int y1,int y2,u32 colour);

void MakeClientFit();
void GetPixelFormat();
void ClipWnd(int x1,int y1,int x2,int y2);

inline void DoClipping(int& x,int& y,int& xstart,int& xlen,int& ystart,int& ylen,const RECT& rClip)
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

inline u16 Blend_Pixels(u16 c1,u16 c2)
{
	extern u16 blend_mask;
/*	if (!b15bpp)
		return (u16)(( (c1&0xF7DE) + (c2&0xF7DE) )/2);
	else
		return (u16)(( (c1&0x7BDE) + (c2&0x7BDE) )/2);*/
	return (u16)(( (c1&blend_mask) + (c2&blend_mask) )>>1);
}


#endif