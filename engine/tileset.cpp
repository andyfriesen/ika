#include "tileset.h"
#include "log.h"
#include "misc.h"

void CTileSet::Free()
{
    for (int i=0; i<nFrames; i++)
    {
        gfxFreeImage(hFrame[i]);
        hFrame[i]=0;
    }
    
    nFrames=0;
}

void CTileSet::BlitFrame(int x,int y,int frame)
{
    frame=nTileidx[frame];

#ifdef _DEBUG
    if (frame<0 || frame>nFrames)
    {
        Log::Write("CTileSet::BlitFrame : Invalid frame %i specified.",frame);
        return;
    }
#endif
    
    gfxBlitImage(hFrame[frame],x,y,false);
}

void CTileSet::TBlitFrame(int x,int y,int frame)
{
    frame=nTileidx[frame];

#ifdef _DEBUG
    if (frame<0 || frame>nFrames)
    {
        Log::Write("CTileSet::TBlitFrame : Invalid frame %i specified.",frame);
        return;
    }
#endif

    gfxBlitImage(hFrame[frame],x,y,true);
}

bool CTileSet::LoadVSP(const char* fname)
{
    CDEBUG("ctileset::loadvsp");
    VSP vsp;
    
    if (!vsp.Load(fname))
        return false;
    
    Free();
    nFrames=vsp.NumTiles();
    nFramex=vsp.Width();
    nFramey=vsp.Height();
    
    try
    {
        hFrame.resize(nFrames);
        for (int i=0; i<nFrames; i++)
        {
            CPixelMatrix& tile=vsp.GetTile(i);

            hFrame[i]=gfxCreateImage(nFramex,nFramey);
            gfxCopyPixelData(hFrame[i],(u32*)tile.GetPixelData(),nFramex,nFramey);
        }
    }
    catch(...)
    {	
        vsp.Free();
        return false;
    }
    
    // Next up, set up the tile animation stuff
    int i;
    
    nTileidx.resize(nFrames);                                   // Make the vectors fit
    bFlip.resize(nFrames);
    for (i=0; i<nFrames; i++)
    {
        nTileidx[i]=i;                                          // set initial values for the vectors
        bFlip[i]=false;
    }

    animstate.resize(100);                                      // urk.  Magic number.
    for (i=0; i<100; i++)
    {
        animstate[i]=vsp.Anim(i);                               // copy the animation data
        animstate[i].nCount=animstate[i].nDelay;                // init the counter
    }
    
    vsp.Free();
    return true;
}

void CTileSet::UpdateAnimation(int time)
{
    int i=time-nAnimtimer;					// how many ticks have elapsed?
    nAnimtimer=time;
    if (i<1) return;						// not very much, wait a little longer
    if (i>100) i=100;   // hack
    
    while (i--)
    {
        for (int j=0; j<100; j++)
            if (animstate[j].nStart!=animstate[j].nFinish)
                AnimateStrand(animstate[j]);
    }
}


void CTileSet::AnimateStrand(VSP::AnimState& anim)
{
    int i;
    
    anim.nCount--;
    
    if (anim.nCount>0)	return;
    
    switch(anim.mode)
    {
    case VSP::linear:
        for (i=anim.nStart; i<anim.nFinish; i++)
        {
            nTileidx[i]++;
            if (nTileidx[i]>anim.nFinish)
                nTileidx[i]=anim.nStart;
        }
        break;
    case VSP::reverse:
        for (i=anim.nStart; i<anim.nFinish; i++)
        {
            nTileidx[i]--;
            if (nTileidx[i]<anim.nStart)
                nTileidx[i]=anim.nFinish;
        }
        break;
    case VSP::random:
        for (i=anim.nStart; i<anim.nFinish; i++)
            nTileidx[i]=Random(anim.nStart,anim.nFinish);
        break;
    case VSP::  flip:
        for (i=anim.nStart; i<anim.nFinish; i++)
        {
            if (bFlip[i])
            {
                nTileidx[i]++;
                if (nTileidx[i]>anim.nFinish)
                {
                    nTileidx[i]=anim.nStart;
                    bFlip[i]=!bFlip[i];
                }
            }
            else
            {
                nTileidx[i]--;
                if (nTileidx[i]<anim.nStart)
                {
                    nTileidx[i]=anim.nFinish;
                    bFlip[i]=!bFlip[i];
                }
            }
        }
        break;
    }
    
    anim.nCount=anim.nDelay;
}

