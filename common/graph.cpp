/*
    Graphics DLL interface crapola.
*/

#include "graph.h"
#include "pixel_matrix.h"
#include "importpng.h"
#include "fileio.h"
#include "log.h"

// =====================================Interface=======================================
int (*gfxGetVersion)();                                                                                     // returns the version # of the driver

bool (*gfxInit)(HWND hWnd,int xres,int yres,int bpp,bool fullscreen);                                       // sets up the graphics mode at the specified resolution (if you want, you can ignore bpp)
void (*gfxShutdown)();                                                                                      // cleans up, IT IS THE APP'S RESPONSIBILITY TO FREE IMAGES, the DLL doesn't have to.
bool (*gfxSwitchToFullScreen)();
bool (*gfxSwitchToWindowed)();
bool (*gfxSwitchResolution)(int x,int y);

handle (*gfxCreateImage)(int x,int y);                                                                      // Creates a new image of the specified size, and returns the handle
bool (*gfxFreeImage)(handle img);                                                                           // deallocates the image
handle (*gfxCopyImage)(handle img);                                                                         // returns a copy of the image passed
bool (*gfxCopyPixelData)(handle img,u32* data,int width,int height);                                        // Give it an array of RGBA quads, and this function will copy the whole of it to the specified image.
bool (*gfxClipWnd)(int x1,int y1,int x2,int y2);

bool (*gfxClipImage)(handle img,Rect& r);                                                                   // All subsequent blit ops will only blit the specified subsection
handle (*gfxGetScreenImage)();

bool (*gfxBlitImage)(handle img,int x,int y,bool transparent);                                              // blits the image on the current renderdest
bool (*gfxScaleBlitImage)(handle img,int x,int y,int width,int height,bool transparent);                    // scale blit
bool (*gfxDistortBlitImage)(handle img,int x[4],int y[4],bool transparent);                                  // rotate/scale blit
bool (*gfxCopyChan)(handle src,int nSrcchan,handle dest,int nDestchan);
bool (*gfxSetPixel)(handle img,int x,int y,int colour);
int  (*gfxGetPixel)(handle img,int x,int y);
bool (*gfxLine)(handle img,int x1,int y1,int x2,int y2,int colour);
bool (*gfxRect)(handle img,int x1,int y1,int x2,int y2,int colour,bool filled);
bool (*gfxEllipse)(handle img,int cx,int cy,int rx,int ry,int colour,bool filled);
bool (*gfxFlatPoly)(handle img,int x[3],int y[3],int colour[3]);
// bool (*gfxTMapPoly)

bool (*gfxSetRenderDest)(handle newrenderdest);                                                             // All subsequent blits go to this image, if it's valid
handle (*gfxGetRenderDest)();

bool (*gfxShowPage)();                                                                                      // copies the back buffer to the front.

int  (*gfxImageWidth)(handle img);
int  (*gfxImageHeight)(handle img);

bool (*gfxPaletteMorph)(int r,int g,int b);

// ==================================Implementation=====================================

HINSTANCE hGraph=0;

// idea ripped from Sphere (thanks, Aegis!)
template<typename T>
static void Assign(T& dest,void* src)
{
    if (!src)
        throw 0;                    // This saves so much typing.
    dest=(T&)src;
}

bool SetUpGraphics(const string& dllname)
{
    hGraph=LoadLibrary(dllname.c_str());
    if (!hGraph)
        return false;
    
    try
    {
        Assign(gfxGetVersion            ,GetProcAddress(hGraph,"gfxGetVersion"));
        Assign(gfxInit                  ,GetProcAddress(hGraph,"gfxInit"));
        Assign(gfxShutdown              ,GetProcAddress(hGraph,"gfxShutdown"));
        Assign(gfxSwitchToFullScreen    ,GetProcAddress(hGraph,"gfxSwitchToFullScreen"));
        Assign(gfxSwitchToWindowed      ,GetProcAddress(hGraph,"gfxSwitchToWindowed"));
        Assign(gfxSwitchResolution      ,GetProcAddress(hGraph,"gfxSwitchResolution"));
        
        Assign(gfxCreateImage           ,GetProcAddress(hGraph,"gfxCreateImage"));
        Assign(gfxFreeImage             ,GetProcAddress(hGraph,"gfxFreeImage"));
        Assign(gfxCopyImage             ,GetProcAddress(hGraph,"gfxCopyImage"));
        Assign(gfxCopyPixelData         ,GetProcAddress(hGraph,"gfxCopyPixelData"));
        Assign(gfxClipWnd               ,GetProcAddress(hGraph,"gfxClipWnd"));
        
        Assign(gfxClipImage             ,GetProcAddress(hGraph,"gfxClipImage"));
        Assign(gfxGetScreenImage        ,GetProcAddress(hGraph,"gfxGetScreenImage"));
        
        Assign(gfxBlitImage             ,GetProcAddress(hGraph,"gfxBlitImage"));
        Assign(gfxScaleBlitImage        ,GetProcAddress(hGraph,"gfxScaleBlitImage"));
        Assign(gfxDistortBlitImage      ,GetProcAddress(hGraph,"gfxDistortBlitImage"));
        Assign(gfxSetPixel              ,GetProcAddress(hGraph,"gfxSetPixel"));
        Assign(gfxGetPixel              ,GetProcAddress(hGraph,"gfxGetPixel"));
        Assign(gfxLine                  ,GetProcAddress(hGraph,"gfxLine"));
        Assign(gfxRect                  ,GetProcAddress(hGraph,"gfxRect"));
        Assign(gfxEllipse               ,GetProcAddress(hGraph,"gfxEllipse"));
        
        Assign(gfxSetRenderDest         ,GetProcAddress(hGraph,"gfxSetRenderDest"));
        Assign(gfxGetRenderDest         ,GetProcAddress(hGraph,"gfxGetRenderDest"));
        Assign(gfxShowPage              ,GetProcAddress(hGraph,"gfxShowPage"));
        Assign(gfxImageWidth            ,GetProcAddress(hGraph,"gfxImageWidth"));
        Assign(gfxImageHeight           ,GetProcAddress(hGraph,"gfxImageHeight"));
        Assign(gfxPaletteMorph          ,GetProcAddress(hGraph,"gfxPaletteMorph"));
        
        Assign(gfxCopyChan              ,GetProcAddress(hGraph,"gfxCopyChan"));
        Assign(gfxFlatPoly              ,GetProcAddress(hGraph,"gfxFlatPoly"));
    }
    catch(int)
    {
        FreeLibrary(hGraph);
        hGraph=0;
        return false;
    }
    
    return true;
}

void UnloadGraphics()
{
    if (hGraph)
    {
        gfxShutdown();
        FreeLibrary(hGraph);
        hGraph=NULL;
    }
}

bool gfxLoadPNG(handle hImage,const char* fname)
{
    CPixelMatrix png;
    
    bool result=PNG::Load(png,fname);
    
    if (!result)
        return false;
    
    gfxCopyPixelData(hImage,(u32*)png.GetPixelData(),png.Width(),png.Height());
    return true;
}
