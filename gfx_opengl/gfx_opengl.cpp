/*
OpenGL rendering engine
*/

#include <windows.h>
#include <gl\gl.h>
#include <gl\glaux.h>
#include <gl\glu.h>
#include <math.h>
#include "log.h"
#include "pixel_matrix.h"
#include "types.h"

#ifdef WIN32
#  define EXPORT __cdecl
#else
#  define EXPORT
#endif
typedef struct SImage* handle;

struct SImage
{
    CPixelMatrix pixels;
    const int& nWidth;
    const int& nHeight;
    const Rect& cliprect;
    
    GLuint	hTex;
    int nTexwidth,nTexheight;
    
    bool bAltered;
    bool bIsscreen;
    
    SImage():
	hTex(0),
	nTexwidth(0),nTexheight(0),
	bAltered(false),bIsscreen(false),
        nWidth(pixels.Width()), nHeight(pixels.Height()),
        cliprect(pixels.GetClipRect())
    {}

    ~SImage()
    {
        if (hTex)
            glDeleteTextures(1,&hTex);
    }
};

//------------------------------------- Prototypes --------------------------------------

bool EXPORT gfxShutdown();
bool EXPORT gfxSetRenderDest(handle);
handle EXPORT gfxCreateImage(int x,int y);
void MakeClientFit();

//-------------------------------------- Globals ----------------------------------------

HWND hGLWnd;
int xres,yres,gbpp;
bool bFullscreen;
HDC  hDC;
HGLRC hRC;

int nImages=0;																			// number of allocated images at any given moment.  Handy for debugging.
int nMaxtexsize;																		// The biggest texture the card can handle

handle hScreen;
handle hRenderdest;

//--------------------------------------- Code -------------------------------------------

#include "stuff.h"
#include "hardblits.h"

int EXPORT gfxGetVersion()
{
    return 2;
}

bool EXPORT gfxInit(HWND hWnd,int x,int y,int bpp,bool fullscreen)
{
    // TODO: replace with SDL?
    GLuint nPixelformat;
    RECT   rClient;

    hGLWnd=hWnd;
    
    xres=x;  yres=y;
    gbpp=bpp;
    
    rClient.top=rClient.left=0;
    rClient.right=xres;  rClient.bottom=yres;
    
    bFullscreen=fullscreen;
    
    try
    {
	if (fullscreen)
	{
	    DEVMODE mode;
	    ZeroMemory(&mode,sizeof mode);
	    mode.dmSize=sizeof mode;
	    mode.dmPelsWidth=xres;
	    mode.dmPelsHeight=yres;
	    mode.dmBitsPerPel=bpp;
	    mode.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	    
	    int result=ChangeDisplaySettings(&mode,CDS_FULLSCREEN);
	    if (result!=DISP_CHANGE_SUCCESSFUL)
		throw "ChangeDisplaySettings failed";
	    
	    SetWindowLong(hWnd,GWL_STYLE,WS_POPUP);
	    SetWindowLong(hWnd,GWL_EXSTYLE,WS_EX_APPWINDOW);
	    SetWindowPos(hWnd,HWND_TOPMOST,0,0,xres,yres,SWP_SHOWWINDOW);
	}
	else
	{
	    MoveWindow(hWnd,0,0,xres,yres,true);
	    MakeClientFit();
	}
	
	PIXELFORMATDESCRIPTOR pfd;
	int result;		
	
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
	    PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = bpp;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;		
	
	hDC=GetDC(hWnd);
	if (!hDC) throw "GetDC failed";
	
	nPixelformat=ChoosePixelFormat(hDC,&pfd);
	if (!nPixelformat) throw "ChoosePixelFormat failed";
	
	result=SetPixelFormat(hDC,nPixelformat,&pfd);
	if (!result) throw "SetPixelFormat failed";
	
	hRC=wglCreateContext(hDC);
	if (!hRC) throw "CreateContect failed";
	
	result=wglMakeCurrent(hDC,hRC);
	if (!result) throw "wglMakeCurrent failed";
	
	ShowWindow(hWnd,SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	SizeWindow(xres,yres);
	
	hScreen=gfxCreateImage(xres,yres);
	hScreen->bIsscreen=true;

        gfxSetRenderDest(hScreen);
	
	InitGL();
    }
    catch (const char* s)
    {
	gfxShutdown();
	MessageBox(hWnd,s,"OpenGL error",0);
	return false;
    }
    
    // Get the max texture size.
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&nMaxtexsize);
    
    return true;
}

bool EXPORT gfxShutdown()
{
    if (bFullscreen)
    {
	DEVMODE dm;
	
	EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&dm);
	dm.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY;
	ChangeDisplaySettings(&dm,0);
	ShowCursor(TRUE);
    }
    
    if (hRC)
    {
	wglMakeCurrent(NULL,NULL);
	
	wglDeleteContext(hRC);
	hRC=0;
    }
    
    if (hDC)    ReleaseDC(hGLWnd,hDC);      hDC=0;
    
    if (nImages!=0)
    {
	// asdjfa;sldkfjas;dlf WTF?
    }
    
    return true;
}

bool EXPORT gfxSwitchToWindowed()
{
    return false;
}

bool EXPORT gfxSwitchToFullScreen()
{
    return false;
}

handle EXPORT gfxCreateImage(int x,int y)
{
    nImages++;
    
    SImage* i=new SImage;

    i->pixels.Resize(x,y);

    SynchTexture(i);
    
    return i;
}

bool EXPORT gfxFreeImage(handle img)
{
    if (img)
    {
	nImages--;
	delete img;
	return true;
    }
    return false;
}

handle EXPORT gfxCopyImage(handle img)
{
    bool EXPORT gfxCopyPixelData(handle,u32*,int,int);
    
    handle i=gfxCreateImage(img->nWidth,img->nHeight);
    gfxCopyPixelData(i,(u32*)img->pixels.GetPixelData(),img->nWidth,img->nHeight);
    
    nImages++;
    
    return i;
}

bool EXPORT gfxCopyPixelData(handle img,u32* data,int width,int height)
{
    img->pixels.CopyPixelData((RGBA*)data,width,height);
    if (img->hTex)
    {
        glDeleteTextures(1,&img->hTex);
        img->hTex=0;
    }

    img->pixels.SetClipRect(Rect(0,0,img->nWidth,img->nHeight));

    SynchTexture(img);    
    return true;
}

bool EXPORT gfxClipImage(handle img,Rect& r)
{
    img->pixels.SetClipRect(r);
    if(img->bIsscreen) 
	glScissor(r.left,yres-r.top,r.right-r.left,yres-(r.top-r.bottom));
   
    return true;
}

handle EXPORT gfxGetScreenImage()
{
    return hScreen;
}

bool EXPORT gfxClipWnd(int x1,int y1,int x2,int y2)
{
    return false;
}

bool EXPORT gfxSwitchResolution(int x,int y)
{
    if (bFullscreen)
    {
	DEVMODE dm;
	
	EnumDisplaySettings(NULL,ENUM_REGISTRY_SETTINGS,&dm);
	dm.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY;
	ChangeDisplaySettings(&dm,0);
	ShowCursor(TRUE);
    }
    
    if (hRC)
    {
	wglMakeCurrent(NULL,NULL);
	
	wglDeleteContext(hRC);
	hRC=0;
    }
    
    if (hDC)
    {
	ReleaseDC(hGLWnd,hDC);
	hDC=0;
    }
    
    gfxInit( hGLWnd,xres,yres,gbpp,bFullscreen);
    
    return true;
}

bool EXPORT gfxShowPage()
{
    return SwapBuffers(hDC)!=0;
}

bool EXPORT gfxPaletteMorph(int r,int g,int b)
{
    return false;
}

bool EXPORT gfxBlitImage(handle img,int x,int y,bool transparent)
{

    if (img->hTex && hRenderdest==hScreen)
    {
        if (img->bAltered)
        {
            SynchTexture(img);
            img->bAltered=false;
        }

        RenderTexture(img,x,y,transparent);
    }
    else if (img!=hScreen)
    {
        if (transparent)
            CBlitter<Alpha>::Blit(img->pixels, hRenderdest->pixels, x, y);
        else
            CBlitter<Opaque>::Blit(img->pixels, hRenderdest->pixels, x, y);
        hRenderdest->bAltered=true;
    }

    return true;
}

bool EXPORT gfxScaleBlitImage(handle img,int x,int y,int w,int h,bool transparent)
{
    if (img->hTex && hRenderdest==hScreen)
    {
        if (img->bAltered)
        {
            SynchTexture(img);
            img->bAltered=false;
        }

        ScaleRenderTexture(img,x,y,w,h,transparent);
    }
    return true;
}

bool EXPORT gfxRotScaleImage(handle img,int cx,int cy,float angle,int scale,bool transparent)
{
    if (img->hTex && hRenderdest==hScreen)
	RotScaleRenderTexture(img,cx,cy,angle,scale,transparent);
    return true;
}

bool EXPORT gfxCopyChan(handle,int,handle,int)
{
    return false;
}

bool EXPORT gfxSetPixel(handle img,int x,int y,u32 colour)
{
    if (hRenderdest==hScreen)
	HardPoint(img,x,y,colour);
    return false;
}

int EXPORT gfxGetPixel(handle img,int x,int y)
{
    int colour=0;
    if (img==hScreen)
    {
        glReadPixels(x, y, 1, 1, GL_RGBA,  GL_BYTE, &colour);
        return colour;
    }
    else
        return img->pixels.GetPixel(x,y);
}

bool EXPORT gfxLine(handle img,int x1,int y1,int x2,int y2,u32 colour)
{
    if (img==hScreen)
	HardLine(x1,y1,x2,y2,colour);
    else
    {
        CBlitter<Alpha>::Line(img->pixels,x1,y1,x2,y2,RGBA(colour));
        img->bAltered=true;
    }
    return true;
}

bool EXPORT gfxRect(handle img,int x1,int y1,int x2,int y2,u32 colour,bool filled)
{
    if (img==hScreen)
	HardRect(x1,y1,x2,y2,colour,filled);
    else
    {
        RGBA c(colour);

        if (!c.a)   return true;
        if (c.a==255)
            CBlitter<Opaque>::DrawRect(img->pixels,x1,y1,x2,y2,c,filled);
        else
            CBlitter<Alpha>::DrawRect(img->pixels,x1,y1,x2,y2,c,filled);
        img->bAltered=true;
    }
    return true;
}

bool EXPORT gfxEllipse(handle img,int cx,int cy,int rx,int ry,u32 colour,bool filled)
{
    return false;
}

bool EXPORT gfxFlatPoly(handle img,int x[3],int y[3],u32 colour[3])
{
    if (img==hScreen)
	HardPoly(img,x,y,colour);
    return true;
}

bool EXPORT gfxSetRenderDest(handle newrenderdest)
{
    hRenderdest=newrenderdest;
    return true;
}

handle EXPORT gfxGetRenderDest()
{
    return hRenderdest;
}

int EXPORT gfxImageWidth(handle img)
{
    if (img)
	return img->nWidth;
    return 0;
}

int EXPORT gfxImageHeight(handle img)
{
    if (img)
	return img->nHeight;
    return 0;
}	
