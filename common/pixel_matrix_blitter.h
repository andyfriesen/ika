// This is pretty cool stuff.  Basically, I'm using C++ templates to inline the pixel blending functions.
// Not a lot of code doing a whole lot of work. :)

namespace
{
    struct Opaque
    {
        static inline RGBA Blend(RGBA src,RGBA dest)
        {
            return src;
        }
    };

    struct Matte
    {
        static inline RGBA Blend(RGBA src,RGBA dest)
        {
            return src.a?src:dest;
        }
    };

    struct Alpha
    {
        static inline RGBA Blend(RGBA src,RGBA dest)
        {
#if !defined(MSVC) || !defined(INLINE_ASM)
            // Not endian independant.  Must fix.

	    u32 c;
	    d=(u32*)*pDest;
	    
	    s=(u32*)*pSrc;
	    
	    c =( a*((s&255)-(d&255))/256 + (d&255) );
	    s>>=8; d>>=8;
	    c|=( a*((s&255)-(d&255))/256 + (d&255) )<<8;
	    s>>=8; d>>=8;
	    c|=( a*((s&255)-(d&255))/256 + (d&255) )<<16;
            return RGBA(c);
#else
            // not portable at all, but pretty fast
            u32 s,d,c;
            u8 a=src.a;
            __asm
            {
                    mov         ebx,[src]
                    mov		s,ebx			// s=*pSrc
                    mov         eax,[dest]
//                    mov		edx,[pDest]
//                    mov		eax,dword ptr[edx]
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
            }
            return RGBA(c);
#endif
        }
    };
};

template <typename Blender>
class CBlitter : public Blender
{
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

public:

    static void Blit(CPixelMatrix& src,CPixelMatrix& dest,int x,int y)
    {
        const Rect& r=src.GetClipRect();

        int xstart=r.left;
        int ystart=r.top;
        int xlen=r.Width();
        int ylen=r.Height();
        
        DoClipping(x,y,xstart,xlen,ystart,ylen,dest.GetClipRect());
        if (xlen<1 || ylen<1)	return;                         // offscreen
        
        RGBA* pSrc  = src.GetPixelData() +(ystart*src.Width())+xstart;
        RGBA* pDest = dest.GetPixelData() +(y*dest.Width())+x;
        int  srcinc= src.Width()-xlen;
        int  destinc=dest.Width()-xlen;

        while (ylen)
        {
            int x=xlen;
            while (x--)
            {
                *pDest=Blend(*pSrc,*pDest);
                pDest++;
                pSrc++;
            }

            pSrc+=srcinc;
            pDest+=destinc;
            --ylen;
        }
    }

    static void ScaleBlit(CPixelMatrix& src,CPixelMatrix& dest,int cx,int cy,int w,int h)
    {
        int	x,y;		// current pixel position
        int ix,iy;		// current image location (fixed point 16.16)
        int xinc,yinc;	// Increment to ix,iy per screen pixel (fixed point)
        
        int xstart,ystart;	// position of first pixel to copy (on the image)
        int xlen,ylen;		// number of screen pixels to copy along each axis

        if (w==0 || h==0)   return;
        
        ix=iy=0;
        x=cx; y=cy;

        Rect& srcclip=src.GetClipRect();
        
        xinc=((srcclip.Width())<<16)/w;
        yinc=((srcclip.Height())<<16)/h;
        
        xstart=srcclip.left;
        ystart=srcclip.top;
        xlen=w;
        ylen=h;
        
        DoClipping(x,y,xstart,xlen,ystart,ylen,dest.GetClipRect());
        if (xlen<1 || ylen<1)	return;	// image is entirely offscreen
        
        int xs=xinc*xstart;
        int ys=yinc*ystart;
        
        ix=xs;
        iy=ys&0xFFFF;
        
        RGBA* pSrc  =src.GetPixelData()+((ys>>16)*src.Width());
        RGBA* pDest =dest.GetPixelData()+(y*dest.Width())+x;
        
        x=0;
        y=h;
        
        while (ylen)
        {
            while (x<xlen)
            {
                pDest[x]=Blend(pDest[x],pSrc[ix>>16]);
                ix+=xinc;
                ++x;
            }
            
            x=0;
            ix=xs;
            
            pDest+=dest.Width();
            
            iy+=yinc;
            pSrc+=(iy>>16)*src.Width();
            iy&=0xFFFF;
            --ylen;
        }
    }
};