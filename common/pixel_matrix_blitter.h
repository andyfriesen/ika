
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
            // TODO: :D
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

    static void ScaleBlit(CPixelMatrix& src,CPixelMatrix& dest,int x,int y)
    {
    }
};