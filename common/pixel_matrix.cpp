#include <windows.h>
#include "pixel_matrix.h"
#include "pixel_matrix_blitter.h"

static inline void DoClipping(int& x,int& y,int& xstart,int& xlen,int& ystart,int& ylen,const Rect& rClip)
{
    if (x<rClip.left)
    {
        xlen-=(rClip.left-x);
        xstart+=(rClip.left-x);
        x=rClip.left;
    }
    if (y<rClip.top)
    {
        ylen-=(rClip.top-y);
        ystart+=(rClip.top-y);
        y=rClip.top;
    }
    if (x+xlen>rClip.right)
        xlen=rClip.right-x;
    if (y+ylen>rClip.bottom)
        ylen=rClip.bottom-y;
}

CPixelMatrix::CPixelMatrix()
{
    nWidth=nHeight=16;		// arbitrary
    pData=new RGBA[nWidth*nHeight];
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(int width,int height)
{
    nWidth=width;
    nHeight=height;
    
    pData=new RGBA[nWidth*nHeight];
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(RGBA* pSrc,int width,int height)
{
    nWidth=width;
    nHeight=height;
    pData=new RGBA[width*height];
    memcpy(pData,pSrc,width*height*sizeof(RGBA));
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(u8* data,int width,int height,byte* pal)
{
    nWidth=width;
    nHeight=height;
    pData=new RGBA[nWidth*nHeight];
    for (int i=0; i<width*height; i++)
        pData[i]=RGBA(data[i],pal);
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::CPixelMatrix(const CPixelMatrix& src)
{
    nWidth=src.nWidth;
    nHeight=src.nHeight;
    pData=new RGBA[nWidth*nHeight];
    memcpy(pData,src.pData,nWidth*nHeight*sizeof(RGBA));
    cliprect=Rect(0,0,nWidth,nHeight);
}

CPixelMatrix::~CPixelMatrix()
{
    delete[] pData;
}


CPixelMatrix& CPixelMatrix::operator = (const CPixelMatrix& rhs)
{
    if (this==&rhs)
        return *this;
    
    delete[] pData;
    
    nWidth=rhs.nWidth;
    nHeight=rhs.nHeight;
    pData=new RGBA[nWidth*nHeight];
    memcpy(pData,rhs.pData,nWidth*nHeight*sizeof(RGBA));
    
    return *this;
}

bool CPixelMatrix::operator == (const CPixelMatrix& rhs)
{
    if (nWidth!=rhs.nWidth)
        return false;
    if (nHeight!=rhs.nHeight)
        return false;
    
    return !memcmp(pData,rhs.pData,nWidth*nHeight*sizeof(RGBA));
}


void CPixelMatrix::CopyPixelData(RGBA* data,int width,int height)
{
    delete[] pData;
    
    nWidth=width;
    nHeight=height;
    
    pData=new RGBA[width*height];
    memcpy(pData,data,width*height*sizeof(RGBA));
}

void CPixelMatrix::CopyPixelData(u8* data,int width,int height,byte* pal)
{
    delete[] pData;
    
    nWidth=width;
    nHeight=height;
    
    pData=new RGBA[width*height];
    for (int i=0; i<width*height; i++)
        pData[i]=RGBA(data[i],pal);
}

RGBA CPixelMatrix::GetPixel(int x,int y)
{
    if (x<0 || x>=nWidth)
        return RGBA(0,0,0,0);
    if (y<0 || y>=nHeight)
        return RGBA(0,0,0,0);
    
    return pData[y*nWidth+x];
}

void CPixelMatrix::SetPixel(int x,int y,RGBA c)
{
    if (x<0 || x>=nWidth)
        return;
    if (y<0 || y>=nHeight)
        return;
    
    pData[y*nWidth+x]=c;
}

void CPixelMatrix::Clear(RGBA colour)
{
    // I feel like being nasty right now >:D
    RGBA* p=pData+nWidth*nHeight-1;
    
    while (p>=pData)
        *p--=colour;
}

void CPixelMatrix::Rotate()
{
    CPixelMatrix temp(*this);
    
    for (int y=0; y<nHeight; y++)
        for (int x=0; x<nWidth; x++)
            SetPixel(y,x,temp.GetPixel(x,y));
}

void CPixelMatrix::Flip()
{
    CPixelMatrix temp(*this);
    
    for (int y=0; y<nHeight; y++)
        for (int x=0; x<nWidth; x++)
            SetPixel(x,y,temp.GetPixel(x,nHeight-y-1));
}

void CPixelMatrix::Mirror()
{
    CPixelMatrix temp(*this);
    
    for (int y=0; y<nHeight; y++)
        for (int x=0; x<nWidth; x++)
            SetPixel(x,y,temp.GetPixel(nWidth-x-1,y));
}

void CPixelMatrix::Resize(int x,int y)
{
    if (x<1 || y<1)
        return;
    
    RGBA* pTemp=new RGBA[x*y];
    
    RGBA* pSrc=pData;
    RGBA* pDest=pTemp;
    
    int ylen=nHeight<y?nHeight:y;
    int nCopywidth=sizeof(RGBA)* (nWidth<x ? nWidth : x);							// smaller of the two
    while (ylen--)
    {
        memcpy(pDest,pSrc,nCopywidth);
        pDest+=x;
        pSrc+=nWidth;
    }
    
    delete[] pData;
    pData=pTemp;
    nWidth=x;
    nHeight=y;
}

// ............ this sucks
void CPixelMatrix::Blit(CPixelMatrix& dest,int x,int y)
{
    CBlitter<Opaque>::Blit(*this,dest,x,y);
    return;

#if 0
    int xstart=cliprect.left;
    int ystart=cliprect.top;
    int xlen=cliprect.right-cliprect.left;
    int ylen=cliprect.bottom-cliprect.top;
    
    DoClipping(x,y,xstart,xlen,ystart,ylen,dest.cliprect);
    if (xlen<1 || ylen<1)	return;                         // offscreen
    
    u32* pSrc  = ((u32*)pData) +(ystart*nWidth)+xstart;
    u32* pDest = ((u32*)dest.pData) +(y*dest.nWidth)+x;
    int  srcinc= nWidth-xlen;
    int  destinc=dest.nWidth-xlen;
    
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

#endif
}

void CPixelMatrix::OpaqueBlit(CPixelMatrix& dest,int x,int y)
{
    CBlitter<Opaque>::Blit(*this,dest,x,y);
    return;

#if 0
    int xstart=cliprect.left;
    int ystart=cliprect.top;
    int xlen=cliprect.right-cliprect.left;
    int ylen=cliprect.bottom-cliprect.top;
    
    DoClipping(x,y,xstart,xlen,ystart,ylen,dest.cliprect);
    if (xlen<1 || ylen<1)	return;                         // offscreen
    
    u32* pSrc  = ((u32*)pData) +(ystart*nWidth)+xstart;
    u32* pDest = ((u32*)dest.pData) +(y*dest.nWidth)+x;
//    int  srcinc= nWidth-xlen;
//    int  destinc=dest.nWidth-xlen;
    
    y=ylen;
    x=xlen;
    
    while (ylen)
    {
        memcpy(pDest,pSrc,xlen * sizeof(RGBA));
        pDest+=dest.Width();
        pSrc+=Width();
        ylen--;
    }
#endif
}