#include "misc.h"

bool __cdecl gfxSetPixel(class SImage* img,int x,int y,u32 colour)
{
	if (x<0 || x>=img->nWidth ||
		y<0 || y>=img->nHeight)
		return false;

	u16 c=make16bpp(colour);

	img->pData[y*img->nPitch+x]=Blend_Pixels(c,img->pData[y*img->nPitch+x]);
	img->pAlpha[y*img->nWidth+x]=255;		// max out alpha

	RescanImage(img,colour>>24==0,colour>>24==255,colour>>24!=0 && colour>>24!=255);

	return true;
}

//bool __cdecl gfxSetPixel(class SImage *,int,int,int,bool)
//{return false;}

int gfxGetPixel(handle img,int x,int y)
{
	if (x<0 || x>=img->nWidth ||
		y<0 || y>=img->nHeight)
		return 0;
	return img->pData[y*img->nPitch+x];
}

bool gfxRect(handle img,int x1,int y1,int x2,int y2,u32 colour,bool filled)
{
	if (filled)
	{
		int ydir=y1<y2?1:-1;
		for (int y=y1; y!=y2; y+=ydir)
			HLine(img,x1,x2,y,colour);
	}
	else
	{
		if (abs(y1-y2)>1)
		{
			VLine(img,x1,y1+1,y2-1,colour);
			VLine(img,x2,y1+1,y2-1,colour);
		}
		else
		{
			HLine(img,x1,x2,y1,colour);
			return true;
		}

		if (abs(x1-x2)>1)
		{
			HLine(img,x1,x2,y1,colour);
			HLine(img,x1,x2,y2,colour);
		}
		else
		{
			VLine(img,x1,y1,y2,colour);
			return true;
		}
	}

	RescanImage(img,colour>>24==0,colour>>24==255,colour>>24!=0 || colour>>24!=255);
	
	return true;
}

bool gfxEllipse(handle img,int cx,int cy,int radx,int rady,u32 colour,bool fill)
{
	int mx1,my1,mx2,my2;
	int aq,bq;
	int dx,dy;
	int r,rx,ry;
	int x;

	if (fill)
		HLine(img,cx-radx,cx+radx,cy,colour);
	else
	{
		gfxSetPixel(img,cx+radx,cy,colour);
		gfxSetPixel(img,cx-radx,cy,colour);
	}
	
	mx1=cx-radx;	my1=cy;
	mx2=cx+radx;	my2=cy;
	
	aq=radx*radx;
	bq=rady*rady;
	dx=aq<<1;
	dy=bq<<1;
	r=radx*bq;
	rx=r<<1;
	ry=0;
	x=radx;
	
	while (x>0)
	{
		if (r>0)
		{
			my1++;
			my2--;
			ry+=dx;
			r-=ry;
		}
		if (r<=0)
		{
			x--;
			mx1++;
			mx2--;
			rx-=dy;
			r+=rx;
		}
		if (fill)
		{
			HLine(img,mx1,mx2,my1,colour);
			HLine(img,mx1,mx2,my2,colour);
		}
		else
		{
			gfxSetPixel(img,mx1,my1,colour);
			gfxSetPixel(img,mx2,my1,colour);
			gfxSetPixel(img,mx1,my2,colour);
			gfxSetPixel(img,mx2,my2,colour);
		}
	}

	return true;
}

//bool gfxFlatPoly(handle img,int x[3],int y[3],u32 colour[3])
bool gfxFlatPoly(handle,int[3],int[3],u32[3])
{
	return false;
}

inline void keepinrange(int& i,int min,int max)
{
	if (i<min) i=min;
	if (i>=max) i=max-1;
}

void HLine(handle img,int x1,int x2,int y,u32 colour)
{
	if (!(colour>>24))
		return;

	if (x1>x2)
	{
		int a=x1;
		x1=x2; x2=a;
	}

	if (y<img->rClip.top || y>=img->rClip.bottom)
		return;
	keepinrange(x1,img->rClip.left,img->rClip.right);
	keepinrange(x2,img->rClip.left,img->rClip.right);

	u16* p=img->pData+(y*img->nPitch)+x1;

	u16 c=make16bpp(colour);

	int xlen=x2-x1;

	if (colour>>24==255)
		do
		{
			*p=c;
			p++;
		}
		while (xlen--);
	else if (colour>>24)
		do
		{
			*p=Blend_Pixels(c,*p);
			p++;
		}
		while (xlen--);
}

void VLine(handle img,int x,int y1,int y2,u32 colour)
{
	u8 alpha=(u8)(colour>>24);
	if (!alpha)
		return;

	if (x<img->rClip.left || x>img->rClip.right) return;
	keepinrange(y1,img->rClip.top,img->rClip.bottom-1);
	keepinrange(y2,img->rClip.top,img->rClip.bottom-1);

	if (y1>y2)
	{
		int a=y1;
		y1=y2;
		y2=a;
	}

	u16* p=img->pData+(y1*img->nPitch)+x;
	int yinc=img->nWidth;

	u16 c=make16bpp(colour);

	int ylen=y2-y1;

	if (alpha==255)
		do
		{
			*p=c;
			p+=yinc;
		}
		while (ylen--);
	else
		do
		{
			*p=Blend_Pixels(c,*p);
			p+=yinc;
		}
		while (ylen--);
}

bool gfxLine(handle img,int x1, int y1, int x2, int y2, u32 colour)
{
	u16 c=make16bpp(colour);
	
	//check for the cases in which the line is vertical or horizontal or only one pixel big
	//we can do those faster through other means
	if(y1==y2&&x1==x2)
	{
		gfxSetPixel(img,x1,y1,colour);
		return true;
	}
	if(y1==y2)
	{
		HLine(img,x1,x2,y1,colour);
		return true;
	}
	if(x1==x2)
	{
		VLine(img,x1,y1,y2,colour);
		return true;
	}
	
	//its good to have these handy
	int cx1=img->rClip.left;
	int cx2=img->rClip.right-1;
	int cy1=img->rClip.top;
	int cy2=img->rClip.bottom-1;
	
	//variables for the clipping code
	int v1=0,v2=0;
//	int diff;
//	int j;
	
	//here begins what is apparently the Cohen-Sutherland line clipping algorithm
	//there's a doc on it here: http://reality.sgi.com/tomcat_asd/algorithms.html#clipping
	if(y1<cy1) v1|=8;
	if(y1>cy2) v1|=4;
	if(x1>cx2) v1|=2;
	if(x1<cx1) v1|=1;
	if(y2<cy1) v2|=8;
	if(y2>cy2) v2|=4;
	if(x2>cx2) v2|=2;
	if(x2<cx1) v2|=1;
	
	while((v1&v2)==0&&(v1|v2)!=0)
	{
		if(v1)
		{
			if(v1&8) //clip above
			{
				x1-=((x1-x2)*(cy1-y1))/(y2-y1+1);
				y1=cy1;
			}
			else if(v1&4) //clip below
			{
				x1-=((x1-x2)*(y1-cy2))/(y1-y2+1);
				y1=cy2;
			}
			else if(v1&2) //clip right
			{
				y1-=((y1-y2)*(x1-cx2))/(x1-x2+1);
				x1=cx2;
			}
			else //clip left
			{
				y1-=((y1-y2)*(cx1-x1))/(x2-x1+1);
				x1=cx1;
			}
			v1=0;
			if(y1<cy1) v1|=8;
			if(y1>cy2) v1|=4;
			if(x1>cx2) v1|=2;
			if(x1<cx1) v1|=1;
		}
		else
		{
			if(v2&8) //clip above
			{
				x2-=((x2-x1)*(cy1-y2))/(y1-y2+1);
				y2=cy1;
			}
			else if(v2&4) //clip below
			{
				x2-=((x2-x1)*(y2-cy2))/(y2-y1+1);
				y2=cy2;
			}
			else if(v2&2) //clip right
			{
				y2-=((y2-y1)*(x2-cx2))/(x2-x1+1);
				x2=cx2;
			}
			else //clip left
			{
				y2-=((y2-y1)*(cx1-x2))/(x1-x2+1);
				x2=cx1;
			}
			v2=0;
			if(y2<cy1) v2|=8;
			if(y2>cy2) v2|=4;
			if(x2>cx2) v2|=2;
			if(x2<cx1) v2|=1;
		}
	}
	
	//this tells us if the line was clipped in its entirety. yum!
	if(v1&v2)
		return true;
	
	//make these checks again
	if(y1==y2&&x1==x2)
	{
		gfxSetPixel(img,x1,y1,c);
		return true;
	}
	if(y1==y2)
	{
		HLine(img,x1,x2,y1,c);
		return true;
	}
	if(x1==x2)
	{
		VLine(img,x1,y1,y2,c);
		return true;
	}
	
	//for the line renderer
	int xi,yi,xyi;
	int d,dir,diu,dx,dy;
	
	//two pointers because we render the line from both ends
	u16* w1=&((u16*)img->pData)[img->nPitch*y1+x1];
	u16* w2=&((u16*)img->pData)[img->nPitch*y2+x2];
	
	//start algorithm presently.
	xi=1;
	if((dx=x2-x1)<0)
	{
		dx=-dx;
		xi=-1;
	}
	
	yi=img->nPitch;
	if((dy=y2-y1)<0)
	{
		yi=-yi;
		dy=-dy;
	}
	
	xyi=xi+yi;
	
	if(dy<dx)
	{
		dir=dy*2;
		d=-dx;
		diu=2*d;
		dy=dx/2;
		
		for(;;)
		{
			*w1=c;
			*w2=c;
			if((d+=dir)<=0)
			{
				w1+=xi;
				w2-=xi;
				if((--dy)>0) continue;
				*w1=c;
				if((dx&1)==0) return true;
				*w2=c;
				return true;
			}
			else
			{
				w1+=xyi;
				w2-=xyi;
				d+=diu;
				if((--dy)>0) continue;
				*w1=c;
				if((dx&1)==0) return true;
				*w2=c;
				return true;
			}
		}
	}
	else
	{
		dir=dx*2;
		d=-dy;
		diu=d*2;
		dx=dy/2;
		for(;;)
		{
			*w1=c;
			*w2=c;
			if((d+=dir)<=0)
			{
				w1+=yi;
				w2-=yi;
				if((--dx)>0) continue;
				*w1=c;
				if((dy&1)==0) return true;
				*w2=c;
				return true;
			}
			else
			{
				w1+=xyi;
				w2-=xyi;
				d+=diu;
				if((--dx)>0) continue;
				*w1=c;
				if((dy&1)==0) return true;
				*w2=c;
				return true;
			}
		}
	}

	RescanImage(hRenderdest,colour>>24==0,colour>>24==255,(colour>>24!=255 && colour>>24!=0));

	return true;
}
