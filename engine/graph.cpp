/*
    Graphics DLL interface crapola.
*/

#include "corona.h"

#include "graph.h"
#include "pixel_matrix.h"
#include "fileio.h"
#include "log.h"
#include "FPSCounter.h"

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

//bool (*gfxShowPage)();                                                                                      // copies the back buffer to the front.
bool gfxShowPage();
bool (*_gfxShowPage)();

int  (*gfxImageWidth)(handle img);
int  (*gfxImageHeight)(handle img);

bool (*gfxPaletteMorph)(int r,int g,int b);

// ==================================Implementation=====================================

HINSTANCE hGraph=0;

// idea ripped from Sphere (thanks, Aegis!)
template<typename T>
static void Assign(T& dest,const char* name)
{
    void* p = GetProcAddress(hGraph, name);
    if (!p)
        throw 0;                    // This saves so much typing.
    dest=(T&)p;
}

bool SetUpGraphics(const string& dllname)
{
    hGraph=LoadLibrary(dllname.c_str());
    if (!hGraph)
        return false;
    
    try
    {
        Assign(gfxGetVersion            ,"gfxGetVersion");
        Assign(gfxInit                  ,"gfxInit");
        Assign(gfxShutdown              ,"gfxShutdown");
        Assign(gfxSwitchToFullScreen    ,"gfxSwitchToFullScreen");
        Assign(gfxSwitchToWindowed      ,"gfxSwitchToWindowed");
        Assign(gfxSwitchResolution      ,"gfxSwitchResolution");
        
        Assign(gfxCreateImage           ,"gfxCreateImage");
        Assign(gfxFreeImage             ,"gfxFreeImage");
        Assign(gfxCopyImage             ,"gfxCopyImage");
        Assign(gfxCopyPixelData         ,"gfxCopyPixelData");
        Assign(gfxClipWnd               ,"gfxClipWnd");
        
        Assign(gfxClipImage             ,"gfxClipImage");
        Assign(gfxGetScreenImage        ,"gfxGetScreenImage");
        
        Assign(gfxBlitImage             ,"gfxBlitImage");
        Assign(gfxScaleBlitImage        ,"gfxScaleBlitImage");
        Assign(gfxDistortBlitImage      ,"gfxDistortBlitImage");
        Assign(gfxSetPixel              ,"gfxSetPixel");
        Assign(gfxGetPixel              ,"gfxGetPixel");
        Assign(gfxLine                  ,"gfxLine");
        Assign(gfxRect                  ,"gfxRect");
        Assign(gfxEllipse               ,"gfxEllipse");
        
        Assign(gfxSetRenderDest         ,"gfxSetRenderDest");
        Assign(gfxGetRenderDest         ,"gfxGetRenderDest");
        Assign(_gfxShowPage             ,"gfxShowPage");
        Assign(gfxImageWidth            ,"gfxImageWidth");
        Assign(gfxImageHeight           ,"gfxImageHeight");
        Assign(gfxPaletteMorph          ,"gfxPaletteMorph");
        
        Assign(gfxCopyChan              ,"gfxCopyChan");
        Assign(gfxFlatPoly              ,"gfxFlatPoly");
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

static FPSCounter counter;

bool gfxShowPage()
{
    counter.Update();
    _gfxShowPage();
    return true;
}

int gfxGetFrameRate()
{
    return counter.FPS();
}

bool gfxLoadImage(handle hImage,const char* fname)
{
    corona::Image* i=corona::OpenImage(fname,corona::FF_AUTODETECT,corona::PF_R8G8B8A8);
    if (!i)
        return 0;

    CPixelMatrix img((RGBA*)i->getPixels(),i->getWidth(),i->getHeight());

    delete i;
    
    gfxCopyPixelData(hImage,(u32*)img.GetPixelData(),img.Width(),img.Height());
    return true;
}
