#include "tileset.h"

#include "common/log.h"
#include "common/misc.h"

#include "video/Driver.h"
#include "video/Image.h"

CTileSet::CTileSet(const char* fname, Video::Driver* v)
    : video(v)
    , nAnimtimer(0)
{
    CDEBUG("ctileset::loadvsp");
    VSP vsp;
    
    if (!vsp.Load(fname))
        throw TileSetException();
    
    nFrames=vsp.NumTiles();
    nFramex=vsp.Width();
    nFramey=vsp.Height();
    
    try
    {
        hFrame.resize(nFrames);
        for (int i=0; i<nFrames; i++)
        {
            hFrame[i] = video->CreateImage(vsp.GetTile(i));
        }
    }
    catch(...)
    {	
        throw TileSetException();
    }
    
    // Next up, set up the tile animation stuff
    nTileidx.resize(nFrames);                                   // Make the vectors fit
    bFlip.resize(nFrames);
    for (int i=0; i<nFrames; i++)
    {
        nTileidx[i]=i;                                          // set initial values for the vectors
        bFlip[i]=false;
    }

    animstate.resize(100);                                      // urk.  Magic number.
    for (int j=0; j<100; j++)
    {
        animstate[j]=vsp.Anim(j);                               // copy the animation data
        animstate[j].nCount=animstate[j].nDelay;                // init the counter
    }
}

CTileSet::~CTileSet()
{
    for (int i=0; i<nFrames; i++)
        delete hFrame[i];
}

Video::Image* CTileSet::GetTile(int index)
{
    index = nTileidx[index];

    return hFrame[index];
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
            nTileidx[i]=Random(anim.nStart, anim.nFinish);
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
