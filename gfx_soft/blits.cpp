#include "gfx_soft.h"
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
	    u8 a=(u8)(img->pData[y*img->nPitch+x].a);
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
    
    RGBA* pDest=hRenderdest->pData+(y*hRenderdest->nPitch)+x;
    RGBA* pSrc =img->pData        +(ystart*img->nPitch)+xstart;

#ifdef _DEBUG
    if (!pDest)
    {
	log("!!! %8x",img);
	return false;
    }
#endif

#ifndef USE_ASM
    while (ylen)
    {
	memcpy(pDest,pSrc,xlen*nBytesperpixel);
	pDest+=hRenderdest->nPitch;
	pSrc+=img->nPitch;
	--ylen;
    }
#else
    /*
    esi - source
    edi - dest
    ecx - x loop counter
    edx - y loop counter
    */
    int srcinc=(img->nPitch-xlen)*nBytesperpixel;
    int destinc=(hRenderdest->nPitch-xlen)*nBytesperpixel;
    __asm
    {
	mov	esi,pSrc
	    mov	edi,pDest
	    
	    mov	edx,ylen
	    
yloop:
	mov	ecx,xlen
xloop:
	lodsd			// grab a pixel
	    stosd			// plot it
	    dec		ecx
	    jnz		xloop	// loop
	    
	    add		esi,srcinc
	    add		edi,destinc
	    
	    dec		edx
	    jnz		yloop
    }
#endif
    
    //	RescanImage(hRenderdest,false,true,false);
    //	ScanImage(hRenderdest);	// have to redo the whole whack, since we're copying alpha and all
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
    
    RGBA* pDest=hRenderdest->pData+(y*hRenderdest->nPitch)+x;
    RGBA* pSrc =img->pData        +(ystart*img->nPitch)+xstart;
    
#ifndef USE_ASM
    int destinc=hRenderdest->nPitch-xlen;
    int srcinc=img->nPitch-xlen;
#endif
    
    while (ylen)
    {
#ifndef USE_ASM
	int ix=xlen;
	while (ix)
	{
	    if (pSrc->a)	// alpha?
		*pDest=*pSrc;
	    ++pSrc;
	    ++pDest;
	    
	    --ix;
	}
	pDest+=destinc;
	pSrc+=srcinc;
#else
	// esi - source
	// edi - dest
	// ecx - x loop
	// ebx - used to hold the alpha value (for testing)
	__asm
	{
	    mov		ecx,xlen
		mov		esi,pSrc
		mov		edi,pDest
		
xloop:
	    mov	eax,[esi]				// grab a pixel
		
		test eax,0xFF000000			// any alpha?	(how's THAT for efficiency?)
		jz	nodraw
		
		mov	[edi],eax
		
nodraw:
	    add edi,4
		add	esi,4
		
		dec	ecx
		jnz xloop				
	}
	pDest+=hRenderdest->nPitch;
	pSrc+=img->nPitch;
#endif
	--ylen;
    }
    
    RescanImage(hRenderdest,true,true,false);
    
    return true;
}

bool AlphaBlit(handle img,int x,int y)
// This is so slow it makes my head hurt.  MMX?  OpenGL more likely.
{
    int xstart=img->rClip.left;
    int ystart=img->rClip.top;
    int xlen=img->rClip.right-img->rClip.left;
    int ylen=img->rClip.bottom-img->rClip.top;
    
    DoClipping(x,y,xstart,xlen,ystart,ylen,hRenderdest->rClip);
    if (xlen<1 || ylen<1)	return true;	// offscreen
    
    u32* pSrc  = (u32*)img->pData            +(ystart*img->nPitch)+xstart;
    u32* pDest = (u32*)hRenderdest->pData    +(y*hRenderdest->nPitch)+x;
    int   srcinc= img->nPitch-xlen;
    int   destinc=hRenderdest->nPitch-xlen;
    
    y=ylen;
    x=xlen;
    // Wee... the scariest, most arcane blit loop ever.  Should be pretty quick, for an alpha blit, though.
    u32 s,d;
    
#ifndef USE_ASM
    unsigned char a;
    while (ylen)
    {
	a=*pSrc>>24;
	
	if (a==255)
	    *pDest=*pSrc;
	else if (a)
	{
	    u32 c;
	    d=*pDest;
	    
	    s=*pSrc;
	    
	    c =( a*((s&255)-(d&255))/256 + (d&255) );
	    s>>=8; d>>=8;
	    c|=( a*((s&255)-(d&255))/256 + (d&255) )<<8;
	    s>>=8; d>>=8;
	    c|=( a*((s&255)-(d&255))/256 + (d&255) )<<16;
	    
	    *pDest=c;
	}
	++pDest;
	++pSrc;
	if (--x)
	    continue;
	
	pDest+=destinc;
	pSrc+=srcinc;
	x=xlen;
	--ylen;
    }
    return 0;
#else
    // evil ASM loop
    // This is quicker, and more arcane still.
    srcinc*=4;
    destinc*=4;
    unsigned char a;
    u32 c;
    __asm
    {
beginloop:
        mov		eax,[pSrc]
	mov		eax,dword ptr[eax]		// eax=*pSrc
	mov		ebx,eax			// ebx=*pSrc
	shr		eax,24			// al = alpha
	mov		a,al
	
	cmp		al,255
	jne		alphablend
	
	mov		eax,[pDest]
	mov		dword ptr [eax],ebx		// *pDest=*pSrc
	jmp		skipalpha
	
alphablend:
        cmp		al,0			// alpha=0, no blend at all
	je		skipalpha
	
	mov		s,ebx			// s=*pSrc
	mov		edx,[pDest]
	mov		eax,dword ptr[edx]
	mov		d,eax			// d=*pDest
	
	// Mix the blue channel
	mov		ecx,eax			// ebx=s  ecx=d		
	and		ebx,255			// ebx=s&255
	and		ecx,255			// ecx=d&255
	
	xor		ah,ah
	mov		al,a
	sub		bx,cx			// bx=(s&255-d&255)
	mul		bx				// eax=bx*ax (where bx is set above, and ax is equal to the alpha)
	
	shr		ax,8			// ax= a*(s&255-d&255)/256
	add		ax,cx			// al= a*(s&255-d&255)/256+d&255
	and		eax,255
	
	mov		c,eax			// save the blue chan
	
	// Green
	mov		ebx,s			// ebx=s
	mov		ecx,d			// ecx=d
	shr		ebx,8
	shr		ecx,8
	mov		s,ebx			// s>>=8;
	mov		d,ecx			// d>>=8;
	
	and		ebx,255			// ebx=s&255
	and		ecx,255			// ecx=d&255
	
	xor		ax,ax
	mov		al,a
	sub		bx,cx			// bl=(s&255-d&255)
	imul	bx				// ax=bl*al (where bl is set above, and al is equal to the alpha)
	
	shl		cx,8
	add		ax,cx			// ax= a*(s&255-d&255)+(d&255*256)
	and		eax,0xFF00
	
	or		c,eax			// mix in the green chan
	
	// Red
	mov		ebx,s			// ebx=s
	mov		ecx,d			// ecx=d
	shr		ebx,8
	shr		ecx,8
	
	and		ebx,255			// ebx=s&255
	and		ecx,255			// ecx=d&255
	
	xor		ax,ax
	mov		al,a
	sub		bx,cx			// bl=(s&255-d&255)
	imul	bx				// ax=bl*al (where bl is set above, and al is equal to the alpha)
	
	shl		cx,8
	add		ax,cx			// ax= a*(s&255-d&255)+(d&255*256)
	and		eax,0xFF00
	shl		eax,8
	
	or		c,eax			// mix in the red chan
	
	mov		eax,c
	mov		ecx,[pDest]
	mov		dword ptr[ecx],	eax	// plot the pixel
	
skipalpha:
    
        add		pSrc,4			// ++pDest
	add		pDest,4			// ++pSrc
	mov		eax,x			// --x
	dec		eax
	mov		x,eax
	cmp		eax,0			// if (--x)
	jne		beginloop		//   continue;
	
	mov		eax,destinc		// pDest+=destinc;
	add		pDest,eax
	
	mov		eax,srcinc		// pSrc+=srcinc;
	add		pSrc,eax
	
	mov		eax,xlen		// x+=xlen
	mov		x,eax
	dec		y
	jnz		beginloop		// if (--y) continue
	}
#endif
	
	//	RescanImage(hRenderdest,false,false,true);
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
    
    if (w==0   || h==0)         return true;        // to avoid /0

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
    
    RGBA* src	=img->pData+((ys>>16)*img->nPitch);
    RGBA* dest	=hRenderdest->pData+(y*hRenderdest->nPitch)+x;
    
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
    
    if (w==0   || h==0)         return true;        // to avoid /0

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
    
    RGBA* src	=img->pData+((ys>>16)*img->nPitch);
    RGBA* dest	=hRenderdest->pData+(y*hRenderdest->nPitch)+x;
    
    x=0;
    y=h;
    
    while (ylen)
    {
	if (src[ix>>16].a)		// any alpha at all?
	    dest[x]=src[ix>>16];
	ix+=xinc;
	if (++x<xlen)
	    continue;
	
	x=0;
	ix=xs;
	
	dest+=hRenderdest->nPitch;
	
	iy+=yinc;
	src+=(iy>>16)*img->nPitch;
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
    
    if (w==0   || h==0)         return true;        // to avoid /0

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
    
    u32* src	=(u32*)img->pData+((ys>>16)*img->nPitch);
    u32* dest	=(u32*)hRenderdest->pData+(y*hRenderdest->nPitch)+x;
    
    x=0;
    y=h;
    
    while (ylen)
    {
	u32 s=src[ix>>16];
	u8 alpha=(u8)(s>>24);
	if (alpha==255)						// full?
	    dest[x]=s;
	else if (alpha)						// any alpha at all?
	{
	    //			u32 s=src[ix>>16];
	    u32 d=dest[x];
	    u32 c;
	    int a=s>>24;
	    
	    c =( a*((s&255)-(d&255))/256 + (d&255) );
	    s>>=8; d>>=8;
	    c|=( a*((s&255)-(d&255))/256 + (d&255) )<<8;
	    s>>=8; d>>=8;
	    c|=( a*((s&255)-(d&255))/256 + (d&255) )<<16;
	    
	    dest[x]=c|(255<<24);;//src[ix>>16];
	}
	
	ix+=xinc;
	if (++x<xlen)
	    continue;
	
	x=0;
	ix=xs;
	
	dest+=hRenderdest->nPitch;
	
	iy+=yinc;
	src+=(iy>>16)*img->nPitch;
	iy&=0xFFFF;
	--ylen;
    }
    
    RescanImage(img,false,false,true);
    
    return true;
}