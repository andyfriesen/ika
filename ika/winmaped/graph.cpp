#include "graph.h"

static const int nDIBsize=128;							// stupid MSVC won't let me declare this in the class definition. :(

CGraphView::CGraphView(HWND hwnd,renderfunc pRenderfunc,void* pthis)
{
    hWnd=hwnd;
    Render=pRenderfunc;
    pThis=pthis;
    
    pDib=new CDIB(nDIBsize,nDIBsize,4);
}

CGraphView::~CGraphView()
{
    if (pDib)
	delete pDib;
    pDib=0;
}

void CGraphView::AddBlock(int x,int y)
{
/*	for (int i=0; i<dirtyrects.size(); i++)				// make sure that this rect isn't already on the list
if (dirtyrects[i].x==x && dirtyrects[i].y==y)
    return;*/
    
    dirtyrects.push_back(point(x,y));
}

void CGraphView::DirtyRect(RECT r)
{
    if (r.left>r.right)
	std::swap(r.left,r.right);
    if (r.top>r.bottom)
	std::swap(r.top,r.bottom);
    
    // startx and starty must always round down
    // endx and endy must always round up
    int startx=r.left;						// ROUNDING ERRORS SUCK
    int starty=r.top;
    int endx=r.right+nDIBsize;
    int endy=r.bottom+nDIBsize;
    
    startx/=nDIBsize;	starty/=nDIBsize;
    endx/=nDIBsize;		endy/=nDIBsize;
    
    for (int y=starty; y<=endy; y++)
	for (int x=startx; x<=endx; x++)
	    AddBlock(x,y);
}

inline void CGraphView::DoClipping(int& x,int& y,int& xstart,int& xlen,int& ystart,int& ylen)
{
    if (x<0)
    {
	xlen+=x;
	xstart-=x;
	x=0;
    }
    
    if (x+xlen>nDIBsize)
	xlen=nDIBsize-x;
    if (y<0)
    {
	ylen+=y;
	ystart-=y;
	y=0;
    }
    if (y+ylen>nDIBsize)
	ylen=nDIBsize-y;
}

void CGraphView::Blit(const CPixelMatrix& src,int x,int y,bool trans)
{
    if (trans)
    {
	AlphaBlit(src,x,y);
	return;
    }
    
    const int bpp=4;
    
    int nDestwidth=pDib->Width();
    int nDestheight=pDib->Height();
    
    int xstart=0;
    int ystart=0;
    int xlen=src.Width();
    int ylen=src.Height();
    int ox=x;
    int oy=y;
    
    DoClipping(x,y,xstart,xlen,ystart,ylen);
    
    if (xlen<1 || ylen<1)	return;	// offscreen
    
    u32* pDest=(u32*)(pDib->GetPixelData())+(y*nDestwidth+x);
    u32* pSrc =(u32*)src.GetPixelData() +(ystart*src.Width()+xstart);
    
    while (ylen)
    {
	int x=xlen;
	while (x)
	{
	    // convert from ARGB to ABGR
	    /*
	    u32 c=*pSrc++;
	    u8 r=(u8)((c>>16)&255);
	    u8 b=(u8)(c&255);
	    c&=0xFF00FF00;								// the alpha and green channels don't move, keep them intact						
	    *pDest++=c|(b<<16)|r;
	    /*/
	    // Did I need to convert this to ASM?  Doubt it. ;D
	    __asm
	    {
		mov		ebx,[pSrc]
		    mov		eax,[ebx]
		    
		    mov		ebx,[pDest]
		    mov		[ebx],eax
		    
		    mov		al,[ebx]
		    mov		ah,[ebx+2]
		    mov		[ebx],ah						// have to switch from BGRA to RGBA
		    mov		[ebx+2],al
	    }
	    ++pSrc;	++pDest;
	    //*/
	    x--;
	}
	pDest+=nDestwidth-xlen;
	pSrc+=src.Width()-xlen;
	--ylen;
    }
}	

void CGraphView::AlphaBlit(const CPixelMatrix& src,int x,int y)
{
    const int bpp=4;
    
    int nDestwidth=pDib->Width();
    int nDestheight=pDib->Height();
    
    int xstart=0;
    int ystart=0;
    int xlen=src.Width();
    int ylen=src.Height();
    int ox=x;
    int oy=y;
    
    DoClipping(x,y,xstart,xlen,ystart,ylen);
    
    if (xlen<1 || ylen<1)	return;	// offscreen
    
    u32* pDest=(u32*)(pDib->GetPixelData())+(y*nDestwidth+x);
    u32* pSrc =(u32*)src.GetPixelData() +(ystart*src.Width()+xstart);
    
    while (ylen)
    {
	int x=xlen;
	while (x)
	{
	    // convert from ARGB to ABGR
	    /*
	    u32 c=*pSrc++;
	    u8 r=(u8)((c>>16)&255);
	    u8 b=(u8)(c&255);
	    c&=0xFF00FF00;								// the alpha and green channels don't move, keep them intact						
	    *pDest++=c|(b<<16)|r;
	    /*/
	    // Did I need to convert this to ASM?  Doubt it. ;D
	    __asm
	    {
		mov		ebx,[pSrc]
		    mov		eax,[ebx]
		    
		    test	eax,0xFF000000
		    jz		nodraw
		    
		    mov		ebx,[pDest]
		    mov		[ebx],eax
		    
		    mov		al,[ebx]
		    mov		ah,[ebx+2]
		    mov		[ebx],ah						// have to switch from BGRA to RGBA
		    mov		[ebx+2],al
		    
nodraw:
	    }
	    ++pSrc;	++pDest;
	    //*/
	    x--;
	}
	pDest+=nDestwidth-xlen;
	pSrc+=src.Width()-xlen;
	--ylen;
    }
}

void CGraphView::HLine(int x1,int x2,int y,u32 colour)
{
    if (x1>x2)
	std::swap<int>(x1,x2);
    
    if (y<0 || y>=nDIBsize)	return;
    if (x1<0)				x1=0;
    if (x2<0)				return;
    if (x1>=nDIBsize)		return;
    if (x2>=nDIBsize)		x2=nDIBsize-1;
    
    u32* p=(u32*)pDib->GetPixelData()+(y*pDib->Width())+x1;
    int xlen=x2-x1;
    
    while (xlen--)
	*p++=colour;
}

void CGraphView::VLine(int x,int y1,int y2,u32 colour)
{
    if (y1>y2)
	std::swap<int>(y1,y2);
    
    if (x<0 || x>=nDIBsize)	return;
    if (y1<0)				y1=0;
    if (y2<0)				return;
    if (y1>=nDIBsize)		return;
    if (y2>=nDIBsize)		y2=nDIBsize-1;
    
    u32* p=(u32*)pDib->GetPixelData()+(y1*pDib->Width())+x;
    int ylen=y2-y1;
    while (ylen--)
    {
	*p=colour;
	p+=pDib->Width();
    }
}

void CGraphView::Rect(int x1,int y1,int x2,int y2,u32 colour)
{
    HLine(x1,x2,y1,colour);
    HLine(x1,x2,y2,colour);
    VLine(x1,y1,y2,colour);
    VLine(x2,y1,y2,colour);
}

void CGraphView::Stipple(int sx,int sy,int w,int h,u32 colour)
{
    if (sx<0)			{		w+=sx;		sx=0;	}
    if (sy<0)			{		h+=sy;		sy=0;	}
    if (sx+w>nDIBsize)	w=nDIBsize-sx;
    if (sy+h>nDIBsize)	h=nDIBsize-sy;
    
    if (w<1 || h<1)		return;
    
    for (int y=0; y<h; y++)
	for (int x=0; x<w; x++)
	    if ((x+y)&1)
		((u32*)pDib->GetPixelData())[ (y+sy)*pDib->Width() + sx + x ]=colour;
}

void CGraphView::Clear()
{
    ZeroMemory(pDib->GetPixelData(),pDib->Width()*pDib->Height()*sizeof(u32));
}

void CGraphView::ShowPage()
{
    for (int i=0; i<dirtyrects.size(); i++)
    {
	RECT r;
	r.left=dirtyrects[i].x*nDIBsize;
	r.top=dirtyrects[i].y*nDIBsize;
	r.right=r.left+nDIBsize-1;
	r.bottom=r.top+nDIBsize-1;
	
	Render(pThis,r);
	HDC dc=GetDC(hWnd);
	BitBlt(dc,r.left,r.top,nDIBsize,nDIBsize,pDib->GetDC(),0,0,SRCCOPY);
	ReleaseDC(hWnd,dc);
    }
    dirtyrects.clear();
}

void CGraphView::ShowPage(const RECT& r)
{
    DirtyRect(r);
    ShowPage();
}

void CGraphView::ForceShowPage()
{
    RECT clientrect;
    
    GetClientRect(hWnd,&clientrect);
    
    int xend=(clientrect.right-clientrect.left)/nDIBsize+1;
    int yend=(clientrect.bottom-clientrect.top)/nDIBsize+1;
    
    for (int y=0; y<yend; y++)
	for (int x=0; x<xend; x++)
	{
	    RECT r;
	    r.left=x*nDIBsize;
	    r.top=y*nDIBsize;
	    r.right=r.left+nDIBsize-1;
	    r.bottom=r.top+nDIBsize-1;
	    
	    Render(pThis,r);
	    HDC dc=GetDC(hWnd);
	    BitBlt(dc,x*nDIBsize,y*nDIBsize,nDIBsize,nDIBsize,pDib->GetDC(),0,0,SRCCOPY);
	    ReleaseDC(hWnd,dc);
	}
	
	dirtyrects.clear();
}