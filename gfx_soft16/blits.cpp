#include "gfx_soft16.h"
#include "blits.h"

#define USE_ASM

extern void log(const char* s,...);

void SetBlitAsNull(handle img)
{
	img->Blit=&NullBlit;
	img->ScaleBlit=&NullScaleBlit;
}

void SetBlitAsOpaque(handle img)
{
	img->Blit=&OpaqueBlit;
	img->ScaleBlit=&OpaqueScaleBlit;
}

void SetBlitAsSprite(handle img)
{
	img->Blit=&SpriteBlit;
	img->ScaleBlit=&SpriteScaleBlit;
}

void SetBlitAsAlpha(handle img)
{
	img->Blit=&AlphaBlit;
	img->ScaleBlit=&AlphaScaleBlit;
}

// Figures out which blit is best for the image in question
void ScanImage(handle img)
{
	int x,y;
	bool bIsNull=true;					// true if every pixel has 0 alpha
	bool bIsOpaque=true;				// true if every pixel has 255 alpha
	bool bIsSprite=true;				// true if every pixel has either 0 or 255 alpha

	for (y=0; y<img->nHeight && (bIsNull || bIsOpaque || bIsSprite); y++)
		for (x=0; x<img->nWidth && (bIsNull || bIsOpaque || bIsSprite); x++)
		{
			u8 a=img->pAlpha[y*img->nWidth+x];
			if (a)	bIsNull=false;
			if (a!=255) bIsOpaque=false;
			if (a!=0 && a!=255)
			{
				SetBlitAsAlpha(img);
				return;
			}
		}

	if (bIsNull)
		SetBlitAsNull(img);
	else if (bIsOpaque)
		SetBlitAsOpaque(img);
	else if (bIsSprite)
		SetBlitAsSprite(img);
	else
		SetBlitAsAlpha(img);
}

// adjusts an image's blits, much faster than the above function
void RescanImage(handle img,bool bZero,bool bFull,bool bPartial)
{
	if (bPartial)
	{
		SetBlitAsAlpha(img);
		return;
	}

	if (img->Blit==&NullBlit && bFull)
		SetBlitAsSprite(img);

	if (img->Blit==&OpaqueBlit && bZero)
		if (bZero)
			SetBlitAsSprite(img);
}

bool NullBlit(handle,int,int)
{
	// WHEN YOU PUNCH THEM THEY SWELL
	return true;
}

bool OpaqueBlit(handle img,int x,int y)
{
	int xstart=img->rClip.left;
	int ystart=img->rClip.top;
	int xlen=img->rClip.right-img->rClip.left;
	int ylen=img->rClip.bottom-img->rClip.top;

	DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
	if (xlen<1 || ylen<1)	return true;	// offscreen

	u16* pDest=hRenderdest->pData+(y*hRenderdest->nPitch)+x;
	u16* pSrc =img->pData        +(ystart*img->nPitch)+xstart;
	u8* pSrcalpha=img->pAlpha	 +(ystart*img->nWidth)+xstart;
	u8* pDestalpha=hRenderdest->pAlpha+(y*hRenderdest->nWidth)+x;

	if (!pDest)
	{
		log("!!! %8x",img);
		return false;
	}

	while (ylen)
	{
		memcpy(pDest,pSrc,xlen*sizeof(u16));
		memcpy(pDestalpha,pSrcalpha,xlen);
		pDest+=hRenderdest->nPitch;
		pSrc+=img->nPitch;
		pDestalpha+=hRenderdest->nWidth;
		pSrcalpha+=img->nWidth;
		--ylen;
	}

	SetBlitAsAlpha(hRenderdest);

	return true;
}

bool SpriteBlit(handle img,int x,int y)
{
	int xstart=img->rClip.left;
	int ystart=img->rClip.top;
	int xlen=img->rClip.right-img->rClip.left;
	int ylen=img->rClip.bottom-img->rClip.top;

	DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
	if (xlen<1 || ylen<1)	return true;	// offscreen

	u16* pDest=hRenderdest->pData+(y*hRenderdest->nPitch)+x;
	u16* pSrc =img->pData        +(ystart*img->nPitch)+xstart;
	u8* pSrcAlpha=img->pAlpha	 +(ystart*img->nWidth)+xstart;
	u8* pDestAlpha=hRenderdest->pAlpha+(y*hRenderdest->nWidth)+x;

	int destinc=hRenderdest->nPitch-xlen;
	int srcinc=img->nPitch-xlen;
	int salphinc=img->nWidth-xlen;
	int dalphinc=hRenderdest->nWidth-xlen;
	
#ifndef USE_ASM
	while (ylen)
	{
		int ix=xlen;
		while (ix)
		{
			if (*pSrcAlpha)	// alpha?
				*pDest=*pSrc,
				*pDestAlpha=*pSrcAlpha;
			++pSrc;
			++pDest;
			++pSrcAlpha;
			++pDestAlpha;

			--ix;
		}
		pDest+=destinc;
		pSrc+=srcinc;
		pSrcAlpha+=salphinc;
		pDestAlpha+=dalphinc;

		--ylen;
	}
#else
	// esi - source
	// edi - dest
	// ecx - x loop counter
	// edx - y loop counter
	srcinc<<=1;
	destinc<<=1;
	__asm
	{
		mov esi,pSrc
		mov edi,pDest
		mov edx,ylen

		yloop:
			mov ecx,xlen

			xloop:
				mov eax,pSrcAlpha
				mov	bl,[eax]
				test bl,bl
				jz	nodraw

					mov	ax,[esi]
					mov	[edi],ax
					mov byte ptr pDestAlpha,bl

				nodraw:
				add	esi,2
				add edi,2
				inc pSrcAlpha
				inc pDestAlpha

			dec	ecx
			jnz	xloop

			add	esi,srcinc
			add edi,destinc

			mov	eax,salphinc
			mov	ebx,dalphinc
			add pSrcAlpha,eax
			add pDestAlpha,ebx

		dec	edx
		jnz yloop
	}
#endif
	RescanImage(hRenderdest,true,true,false);

	return true;
}

u16 Blend_Pixels(u16 c1,u16 c2);

bool AlphaBlit(handle img,int x,int y)
// This is so slow it makes my head hurt.  MMX?  OpenGL more likely.
{
	int xstart=img->rClip.left;
	int ystart=img->rClip.top;
	int xlen=img->rClip.right-img->rClip.left;
	int ylen=img->rClip.bottom-img->rClip.top;

	DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
	if (xlen<1 || ylen<1)	return true;	// offscreen

	u16* pSrc  = img->pData			+(ystart*img->nPitch)+xstart;
	u16* pDest = hRenderdest->pData	+(y*hRenderdest->nPitch)+x;
	u8*  pAlpha= img->pAlpha		+(ystart*img->nWidth)+xstart;
	int  srcinc= img->nPitch-xlen;
	int  destinc=hRenderdest->nPitch-xlen;
	int	 alphinc=img->nWidth-xlen;

	// Wee... the scariest, most arcane blit loop ever.  Should be pretty quick, for an alpha blit, though.

	while (ylen)
	{
		int ix=xlen;
		while (ix)
		{
			if (*pAlpha==255)	// alpha?
				*pDest=*pSrc;
			else if (*pAlpha)
				*pDest=Blend_Pixels(*pDest,*pSrc);
			++pSrc;
			++pDest;
			++pAlpha;

			--ix;
		}
		pDest+=destinc;
		pSrc+=srcinc;
		pAlpha+=alphinc;
		--ylen;
	}

	SetBlitAsAlpha(hRenderdest);
	return true;
}

//------------------------------------------------------------------------------------------------------------------

bool NullScaleBlit(handle,int,int,int,int)
{
	// THE OOS ARE UNRULY.  PLEASE DISENGAGE.
	return true;
}

bool OpaqueScaleBlit(handle img,int cx,int cy,int w,int h)
{
	int	x,y;		// current pixel position
	int ix,iy;		// current image location (fixed point 16.16)
	int xinc,yinc;	// Increment to ix,iy per screen pixel (fixed point)

	int xstart,ystart;	// position of first pixel to copy (on the image)
	int xlen,ylen;		// number of screen pixels to copy along each axis

	ix=iy=0;
	x=cx; y=cy;

	xinc=((img->rClip.right-img->rClip.left)<<16)/w;
	yinc=((img->rClip.bottom-img->rClip.top)<<16)/h;

	xstart=img->rClip.left;
	ystart=img->rClip.top;
	xlen=w;
	ylen=h;

	DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
	if (xlen<1 || ylen<1)	return false;	// image is entirely offscreen

	int xs=xinc*xstart;
	int ys=yinc*ystart;

	ix=xs;
	iy=ys&0xFFFF;

	u16* src	=img->pData+((ys>>16)*img->nPitch);
	u16* dest	=hRenderdest->pData+(y*hRenderdest->nPitch)+x;

	x=0;
	y=h;

	while (ylen)
	{
		while (x<xlen)
		{
			dest[x]=src[ix>>16];
			ix+=xinc;
			++x;
		}

		x=0;
		ix=xs;

		dest+=hRenderdest->nPitch;

		iy+=yinc;
		src+=(iy>>16)*img->nPitch;
		iy&=0xFFFF;
		--ylen;
	}

	SetBlitAsAlpha(hRenderdest);

	return true;
}

bool SpriteScaleBlit(handle img,int cx,int cy,int w,int h)
{
	int	x,y;		// current pixel position
	int ix,iy;		// current image location (fixed point 16.16)
	int xinc,yinc;	// Increment to ix,iy per screen pixel (fixed point)

	int xstart,ystart;	// position of first pixel to copy (on the image)
	int xlen,ylen;		// number of screen pixels to copy along each axis

	ix=iy=0;
	x=cx; y=cy;

	xinc=((img->rClip.right-img->rClip.left)<<16)/w;
	yinc=((img->rClip.bottom-img->rClip.top)<<16)/h;

	xstart=img->rClip.left;
	ystart=img->rClip.top;
	xlen=w;
	ylen=h;

	DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
	if (xlen<1 || ylen<1)	return false;	// image is entirely offscreen

	int xs=xinc*xstart;
	int ys=yinc*ystart;

	ix=xs;
	iy=ys&0xFFFF;

	u16* src	=img->pData+((ys>>16)*img->nPitch);
	u8*  pAlpha	=img->pAlpha+((ys>>16)*img->nWidth);
	u16* dest	=hRenderdest->pData+(y*hRenderdest->nPitch)+x;

	x=0;
	y=h;

	while (ylen)
	{
		if (pAlpha[ix>>16])				// any alpha at all?
			dest[x]=src[ix>>16];
		ix+=xinc;
		if (++x<w)
			continue;

		x=0;
		ix=xs;

		dest+=hRenderdest->nPitch;

		iy+=yinc;
		src+=(iy>>16)*img->nPitch;
		pAlpha+=(iy>>16)*img->nWidth;
		iy&=0xFFFF;
		--ylen;
	}

	RescanImage(hRenderdest,true,true,false);

	return true;
}

// X_+
bool AlphaScaleBlit(handle img,int cx,int cy,int w,int h)
{
	int	x,y;		// current pixel position
	int ix,iy;		// current image location (fixed point 16.16)
	int xinc,yinc;	// Increment to ix,iy per screen pixel (fixed point)

	int xstart,ystart;	// position of first pixel to copy (on the image)
	int xlen,ylen;		// number of screen pixels to copy along each axis

	ix=iy=0;
	x=cx; y=cy;

	xinc=((img->rClip.right-img->rClip.left)<<16)/w;
	yinc=((img->rClip.bottom-img->rClip.top)<<16)/h;

	xstart=img->rClip.left;
	ystart=img->rClip.top;
	xlen=w;
	ylen=h;

	DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
	if (xlen<1 || ylen<1)	return false;	// image is entirely offscreen

	int xs=xinc*xstart;
	int ys=yinc*ystart;

	ix=xs;
	iy=ys&0xFFFF;

	u16* src	=img->pData+((ys>>16)*img->nPitch);
	u8*  pAlpha	=img->pAlpha+((ys>>16)*img->nWidth);
	u16* dest	=hRenderdest->pData+(y*hRenderdest->nPitch)+x;

	x=0;
	y=h;

	while (ylen)
	{
		if (pAlpha[ix>>16]==0xFF)			// full alpha?
			dest[x]=src[ix>>16];
		else if (pAlpha[ix>>16])			// any alpha at all?
			dest[x]=Blend_Pixels(src[ix>>16],dest[x]);

		ix+=xinc;
		if (++x<xlen)
			continue;

		x=0;
		ix=xs;

		dest+=hRenderdest->nPitch;

		iy+=yinc;
		src   +=(iy>>16)*img->nPitch;
		pAlpha+=(iy>>16)*img->nWidth;
		iy&=0xFFFF;
		--ylen;
	}

	RescanImage(img,false,false,true);

	return true;
}