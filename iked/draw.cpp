
#include "draw.h"
#include "misc.h"

void gfx::Blit(wxImage& dest,int x,int y,int w,int h,RGBA* src,bool trans)
{
	int xstart=0;
	int ystart=0;
	int xlen=w;
	int ylen=h;

	int iw=dest.GetWidth();
	int ih=dest.GetHeight();

	if (x<0)
	{
		xstart=-x;
		xlen+=x;	// x is negative, so xlen gets smaller
		x=0;
	}
	if (x+w>iw)
	{
		xlen=iw-x;
	}
	if (y<0)
	{
		ystart=-x;
		ylen+=x;
		y=0;
	}
	if (y+h>ih)
	{
		ylen=ih-y;
	}

	if (xlen<1 || ylen<1)
		return;

	RGB*	d=(RGB*)dest.GetData()+(y*iw)+x;
	RGBA*	s=src+(ystart*w)+xstart;

	int destinc=iw-xlen;
	int srcinc=w-xlen;

	while (ylen--)
	{
		int curx=xlen;
		while (curx--)
			*d++=*s++;

		d+=destinc;
		s+=srcinc;
	}
}

void gfx::BlitTile(wxImage& dest,int x,int y,int tileidx,VSP& src,bool trans)
{
	Blit(dest,x,y,src.TileX(),src.TileY(),(RGBA*)src.GetPixelData(tileidx),trans);
}

void gfx::Stipple(wxImage& dest,int sx,int sy,int w,int h,RGBA colour)
{
    int imgsize=dest.GetWidth();
    if (sx<0)			{		w+=sx;		sx=0;	}
    if (sy<0)			{		h+=sy;		sy=0;	}
    if (sx+w>imgsize)	w=imgsize-sx;
    if (sy+h>imgsize)	h=imgsize-sy;
    
    if (w<1 || h<1)		return;
    
    for (int y=0; y<h; y++)
	for (int x=0; x<w; x++)
	    if ((x+y)&1)
		((RGB*)dest.GetData())[ (y+sy)*imgsize + sx + x ]=colour;
}

// Primatives

void gfx::HLine(wxImage& dest,int x1,int x2,int y,RGBA colour)
{
    if (x1>x2)
	swap<int>(x1,x2);

    int w=dest.GetWidth();
    
    if (y<0 || y>=w)	return;
    if (x1<0)				x1=0;
    if (x2<0)				return;
    if (x1>=w)		return;
    if (x2>=w)		x2=w-1;
    
    RGB* p=(RGB*)dest.GetData()+(y*w)+x1;
    int xlen=x2-x1;
    
    while (xlen--)
	*p++=colour;
}

void gfx::VLine(wxImage& dest,int x,int y1,int y2,RGBA colour)
{
    if (y1>y2)
	swap<int>(y1,y2);

    int w=dest.GetWidth();
    
    if (x<0 || x>=w)    return;
    if (y1<0)           y1=0;
    if (y2<0)           return;
    if (y1>=w)          return;
    if (y2>=w)          y2=w-1;
    
    RGB* p=(RGB*)dest.GetData()+(y1*w)+x;
    int ylen=y2-y1;
    while (ylen--)
    {
	*p=colour;
	p+=w;
    }
}

void gfx::Rect(wxImage& dest,int x1,int y1,int x2,int y2,RGBA colour)
{
    HLine(dest,x1,x2,y1,colour);
    HLine(dest,x1,x2,y2,colour);
    VLine(dest,x1,y1,y2,colour);
    VLine(dest,x2,y1,y2,colour);
}