#include "gfx_soft16.h"
#include "misc.h"
#include "blits.h"
#include "log.h"
#include "types.h"

// DirectDraw for fullscreen rendering
LPDIRECTDRAW		lpdd=NULL;						// Main DirectDraw object
LPDIRECTDRAWSURFACE	mainsurface=NULL;				// front buffer
LPDIRECTDRAWSURFACE backsurf=NULL;					// back buffer for flipping
LPDIRECTDRAWCLIPPER	mainclip=NULL;					// clippin
LPDIRECTDRAWGAMMACONTROL	lpgamma=NULL;			// gamma controller, for palettemorph
DDGAMMARAMP			normalgamma;					// typical gamma settings
DDGAMMARAMP			currentgamma;					// active gamma settings

// DIB for windowed rendering

HBITMAP hBitmap;
void*	pBackbuffer;
HDC		hDC;

int zoomfactor=1;									// zoom multiplier

HWND hCurWnd;

int nBytesperpixel;
int xres,yres;
bool bFullscreen;
handle hScreen;										// Handle to the screen image
handle hRenderdest;									// handle to the image that we blit to
RECT maincliprect;

bool b15bpp;
u16 blend_mask;

int nScreensurfacewidth,nScreensurfaceheight;		// width/height of the screen surface. (we have to remember this)

static int nImagecount=0;							// Running count of images, for debugging purposes
static bool bInited=false;

inline void ValidateClipRect(handle& img)
{
    if (img->rClip.left<0) img->rClip.left=0;
    if (img->rClip.top<0)  img->rClip.top=0;
    if (img->rClip.right>img->nWidth) img->rClip.right=img->nWidth;
    if (img->rClip.bottom>img->nHeight) img->rClip.bottom=img->nHeight;
}

inline u16 make16bpp(u32 c)
{
    int r=(c>>16)&255;
    int g=(c>>8)&255;
    int b=c&255;
    
    if (!b15bpp)
        return (u16)( ((b>>3)<<11) | ((g>>2)<<5) | (r>>3) );
    else
        return (u16)( ((b>>3)<<10) | ((g>>3)<<5) | (r>>3) );
}


/////////////////// Code ////////////////////////////////

int gfxGetVersion()	{	return 2;	}

bool gfxInit(HWND hWnd,int x,int y,int,bool fullscreen)
{
#ifdef _DEBUG
    initlog("gfx_soft16.log");
#endif
    
    if (bInited)
    {
        log("redundant gfxInit's");
        return false;
    }
    
    xres=x; yres=y;
    nBytesperpixel=2;
    hCurWnd=hWnd;
    bFullscreen=fullscreen;
    
    if (fullscreen)
    {
        HRESULT result;
        DDSURFACEDESC ddsd; 
        
        if (!lpdd)
        {
            result=DirectDrawCreate(NULL,&lpdd,NULL);
            if (result!=DD_OK) return false; // poo
        }
        try
        {
            result=lpdd->SetCooperativeLevel(hWnd,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
            if (FAILED(result)) throw result;
            
            result=lpdd->SetDisplayMode(x,y,16);
            if (FAILED(result)) throw result;
            
            ZeroMemory(&ddsd,sizeof ddsd);
            ddsd.dwSize=sizeof ddsd;
            ddsd.dwFlags=DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
            ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
            ddsd.dwBackBufferCount=1;
            result=lpdd->CreateSurface(&ddsd,&mainsurface,NULL);
            if (FAILED(result)) throw result;
            
            DDSCAPS ddscaps;
            ZeroMemory(&ddscaps,sizeof ddscaps);
            ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
            result=mainsurface->GetAttachedSurface(&ddscaps,&backsurf);
            if (FAILED(result)) throw result;
            
            log("Initing gamma stuff...");
            result=mainsurface->QueryInterface(IID_IDirectDrawGammaControl,(void**)&lpgamma);
            if (result!=S_OK)
            {
                lpgamma=NULL;
                log("gfxInit: gamma init stuff failed!! :o");
            }
            else
            {
                log("Gamma stuff inited okay");
                lpgamma->GetGammaRamp(0,&normalgamma);
                lpgamma->GetGammaRamp(0,&currentgamma);
            }
        }
        catch(HRESULT hr)
        {
            logdderr(hr);
            gfxShutdown();
            return false;
        }
    }
    else
    {
        try
        {
            hDC=CreateCompatibleDC(NULL);
            if (!hDC)
                throw "Couldn't create DC";
            
            BITMAPINFO bmi;
            BITMAPINFOHEADER& bmih = bmi.bmiHeader;
            ZeroMemory(&bmi,sizeof bmi);
            bmih.biSize=sizeof(bmih);
            bmih.biWidth=xres;
            bmih.biHeight=-yres;
            bmih.biPlanes=1;
            bmih.biBitCount=16;
            bmih.biCompression=BI_RGB;
            
            hBitmap=CreateDIBSection(hDC,&bmi,DIB_RGB_COLORS,&pBackbuffer,NULL,0);
            if (!hBitmap)
                throw "Couldn't create DIB section";
            
            SelectObject(hDC,hBitmap);
            
            MakeClientFit();
        }
        catch(const char* msg)
        {
            log("gfxInit error: %s",msg);
            gfxShutdown();
            return false;
        }
    }
    
    GetPixelFormat();
    
    hScreen=gfxCreateImage(xres,yres);
    
    hRenderdest=hScreen;
    
    gfxClipWnd(0,0,x-1,y-1);
    
    log("gfxinit finished");
    
    bInited=true;
    return true;
}

void gfxShutdown()
{
    if (!bInited)
        return;
    
    bInited=false;
    
    gfxFreeImage(hScreen);
    
    if (bFullscreen)
    {
        lpdd->RestoreDisplayMode();
        if (mainsurface) mainsurface->Release();
        if (mainclip)    mainclip->Release();
        if (lpgamma)	 {	lpgamma->SetGammaRamp(0,&normalgamma);	lpgamma->Release();	}
        if (lpdd)        lpdd->Release();
        
        mainsurface=NULL;
        mainclip=NULL;
        lpgamma=NULL;
        lpdd=NULL;
    }
    else
    {
        DeleteDC(hDC);
        DeleteObject(hBitmap);
    }
    
    if (nImagecount!=0)
        log("gfxShutdown:: %i image%s unaccounted for!!!",nImagecount,nImagecount==1?"s":"");
}

bool gfxSwitchToFullScreen()
{
    return 0;
}

bool gfxSwitchToWindowed()
{
    return 0;
}

bool gfxSwitchResolution(int x,int y)		// OHFUCK: if the display is 565 fullscreen, then switching to 15bpp in mid-run will result in graphics strangeness
{
    //	if (hScreen)
    //		return false;
    
    log("-------resize------------");
    if (bFullscreen)
    {
        // NYI
        GetPixelFormat();
    }
    else
    {
        xres=x;
        yres=y;
        
        DeleteObject(hBitmap);
        
        BITMAPINFO bmi;
        BITMAPINFOHEADER& bmih = bmi.bmiHeader;
        ZeroMemory(&bmi,sizeof bmi);
        bmih.biSize=sizeof(bmih);
        bmih.biWidth=xres;
        bmih.biHeight=-yres;
        bmih.biPlanes=1;
        bmih.biBitCount=16;
        bmih.biCompression=BI_RGB;
        
        hBitmap=CreateDIBSection(GetDC(NULL),&bmi,DIB_RGB_COLORS,&pBackbuffer,NULL,0);
        
        SelectObject(hDC,hBitmap);
        GetPixelFormat();
    }
    return false;
}

handle gfxCreateImage(int x,int y)
{
    handle temp;
    
    temp=new SImage;
    
    temp->pData=new u16[x*y];
    temp->pAlpha=new u8[x*y];
    ZeroMemory(temp->pData,x*y*sizeof(u16));
    ZeroMemory(temp->pAlpha,x*y*sizeof(u8));
    temp->nWidth=x;
    temp->nHeight=y;
    temp->nPitch=x;
    temp->Blit=&NullBlit;
    temp->ScaleBlit=&NullScaleBlit;
    
    gfxClipImage(temp, Rect(0,0,x,y) );
    nImagecount++;
    
    return temp;
}

bool gfxFreeImage(handle img)
{
    if (!img)
        return false;
    
    if (img==hScreen)
        log("Free screen");
    
    delete[] img->pData;
    img->pData=NULL;
    img->nWidth=img->nHeight=img->nPitch=0;
    nImagecount--;
    
    delete img;
    
    return true;
}

handle gfxCopyImage(handle img)
{
    bool gfxCopyPixelData(handle,u32*,int,int);
    
    handle i=new SImage;
    
    i->nWidth=img->nWidth;
    i->nHeight=img->nHeight;
    i->nPitch=img->nPitch;
    
    i->pData=new u16[i->nWidth*i->nHeight];
    memcpy(i->pData,img->pData,i->nWidth*i->nHeight*sizeof(u16));
    
    i->pAlpha=new u8[i->nWidth*i->nHeight];
    memcpy(i->pAlpha,img->pAlpha,i->nWidth*i->nHeight*sizeof(u8));
    
    gfxClipImage(i, Rect(0,0,img->nWidth-1,img->nHeight-1) );
    
    ScanImage(i);
    
    nImagecount++;
    
    return i;
}

bool gfxCopyPixelData(handle img,u32* data,int width,int height)
{
    delete[] img->pData;
    delete[] img->pAlpha;
    
    img->pData=new u16[width*height];
    img->pAlpha=new u8[width*height];
    img->nWidth=width;
    img->nHeight=height;
    img->nPitch=width;
    img->rClip=Rect(0,0,width,height);
    
    for (int y=0; y<height; y++)
        for (int x=0; x<width; x++)
        {
            u32 c=*data++;
            
            img->pData[y*width+x]=make16bpp(c);
            img->pAlpha[y*width+x]=(u8)((c>>24)&255);
        }
        
        ScanImage(img);
        
        return 0;
}

bool gfxClipWnd(int x1,int y1,int x2,int y2)
{
    maincliprect.left=x1;
    maincliprect.top=y1;
    maincliprect.right=x2;
    maincliprect.bottom=y2;
    return true;
}

bool gfxClipImage(handle img,Rect& r)
{
    img->rClip=r;
    ValidateClipRect(img);
    return 0;
}

handle gfxGetScreenImage()
{
    return hScreen;
}

bool gfxShowPage()
{
    if (bFullscreen)
    {
        HRESULT result;
        DDSURFACEDESC ddsd;
        
        // First, copy the screen to the back buffer
        ZeroMemory(&ddsd,sizeof ddsd);
        ddsd.dwSize=sizeof ddsd;
        result=backsurf->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_WRITEONLY,NULL);
        if (FAILED(result))	{ log("lockfail"); return false; }
        
        u16* pSrc =hScreen->pData;
        u8* pDest=(u8*)ddsd.lpSurface;
        
        int xlen=xres;
        xlen*=sizeof(u16);
        for (int y=yres; y; y--)
        {
            memcpy(pDest,pSrc,xlen);
            pSrc+=hScreen->nWidth;
            pDest+=ddsd.lPitch;
        }
        
        result=backsurf->Unlock(ddsd.lpSurface);
        if (FAILED(result)) return false;
        
        result=mainsurface->Flip(NULL,DDFLIP_WAIT);
        if (FAILED(result))
        {
            log("flipfail");
            return false;
        }
        
        return true;
    }
    else
    {
        int xlen=xres*sizeof(u16);
        int ylen=yres;
        
        u8* pDest=(u8*)pBackbuffer;
        u8* pSrc=(u8*)hScreen->pData;
        
        while (ylen--)
        {
            memcpy(pDest,pSrc,xlen);
            pDest+=xlen;			pSrc+=xlen;
        }
        
        HDC hRenderDC=GetDC(hCurWnd);
        BitBlt(hRenderDC,0,0,xres,yres,hDC,0,0,SRCCOPY);
        ReleaseDC(hCurWnd,hRenderDC);
        
        return true;
    }
    return 0;
}

bool gfxBlitImage(handle img,int x,int y,bool transparent)
{
#ifdef _DEBUG
    if (!img->Blit)
    {
        log("as;dlkjfas;dlkjf");
        return false;
    }
#endif
    
    if (!transparent)
        return OpaqueBlit(img,x,y);
    
    return img->Blit(img,x,y);
}

bool gfxScaleBlitImage(handle img,int cx,int cy,int w,int h,bool transparent)
{
#ifdef _DEBUG
    if (!img->ScaleBlit)
    {
        log("dasdasdf");
        return false;
    }
#endif
    
    if (!transparent)
        return OpaqueScaleBlit(img,cx,cy,w,h);
    
    return img->ScaleBlit(img,cx,cy,w,h);
}

bool gfxDistortBlitImage(handle img,int x[4],int y[4],bool transparent)
{
    return false;
}

bool gfxCopyChan(handle src,int nSrcchan,handle dest,int nDestchan)
{
#ifdef _DEBUG
    if (!src || !dest)
    {
        log("dfasdfasdfdfasdfasdfasdf");
        return false;
    }
#endif
    
    int xlen=src->nWidth>dest->nWidth?src->nWidth:dest->nWidth;									// whichever is smaller
    int ylen=src->nHeight>dest->nHeight?src->nHeight:dest->nHeight;
    int srcinc=(src->nPitch-src->nWidth)*4;
    int destinc=(dest->nPitch-dest->nWidth)*4;
    
    u8* pSrc=(u8*)src->pData+nSrcchan;
    u8* pDest=(u8*)dest->pData+nDestchan;
    
    while (ylen--)
    {
        int x=xlen;
        while (x--)
        {
            *pDest=*pSrc;
            pDest+=4;
            pSrc+=4;
        }
        pSrc+=srcinc;
        pDest+=destinc;
    }
    
    return true;
}

extern void RescanImage(handle img,bool bZero,bool bFull,bool bPartial);

bool gfxSetRenderDest(handle img)		// careful, the driver can't validate this
{
    hRenderdest=img;
    return true;
}

handle gfxGetRenderDest()
{
    return hRenderdest;
}

int  gfxImageWidth(handle img)
{
    return img->nWidth;
}

int  gfxImageHeight(handle img)
{
    return img->nHeight;
}

bool gfxPaletteMorph(int r,int g,int b)
{
    if (bFullscreen)
    {
        int count;
        
        if (lpgamma)
        {
            for (count=0; count<256; count++)
            {
                currentgamma.red[count]=(u16)(count*r);
                currentgamma.green[count]=(u16)(count*g);
                currentgamma.blue[count]=(u16)(count*b);
            }
            
            lpgamma->SetGammaRamp(0,&currentgamma);
            return true;
        }
        else
            return false;
    }
    return false;
}

#include "primatives.h"

////////////////////// Implementation helper function type things /////////////////////////

void MakeClientFit()
{
    RECT client,window,goal;
    int ox,oy; // how far off are we?
    
    if (bFullscreen) return;  // why?
    goal.left=goal.top=0;
    goal.right=xres; goal.bottom=yres;
    
    GetClientRect(hCurWnd,&client);
    GetWindowRect(hCurWnd,&window);
    
    // find out how much adjustment we need to do
    ox=xres-client.right;
    oy=yres-client.bottom;
    
    // do it!
    window.right+=ox;
    window.bottom+=oy;
    
    POINT pt;
    
    pt.x=pt.y=0;
    
    ClientToScreen(hCurWnd,&pt);
    
    MoveWindow(hCurWnd,window.left,window.top,window.right-window.left,window.bottom-window.top,true);
}

void GetPixelFormat(void)
// Polls the hardware for the pixel format of the current display mode.
// This allows us to use packpixel and unpackpixel.
{
    if (bFullscreen)
    {
        DDPIXELFORMAT ddpf;
        
        memset(&ddpf,0,sizeof ddpf);
        ddpf.dwSize=sizeof(DDPIXELFORMAT);
        mainsurface->GetPixelFormat(&ddpf);
        
        b15bpp=(ddpf.dwGBitMask==0x03E0);
    }
    else
        b15bpp=true;
    
    if (b15bpp)
        blend_mask=0x7BDE;
    else
        blend_mask=0xF7DE;
}

