
#include "draw.h"

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