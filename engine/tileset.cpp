#include <stdexcept>
#include "tileset.h"

#include "common/log.h"
#include "common/utility.h"

#include "video/Driver.h"
#include "video/Image.h"

TileSet::TileSet(const std::string& fname, Video::Driver* v)
    : video(v)
    , nAnimtimer(0)
{
    CDEBUG("ctileset::loadvsp");
    vsp = new VSP;
    
    if (!vsp->Load(fname)) {
        throw std::runtime_error("Unable to load VSP file " + fname);
    }
    
    nFrames = vsp->NumTiles();
    nFramex = vsp->Width();
    nFramey = vsp->Height();
    
    try {
        hFrame.resize(nFrames);
        for (int i = 0; i < nFrames; i++) {
            hFrame[i] = video->CreateImage(vsp->GetTile(i));
        }
    }
    catch(...) {	
        throw TileSetException();
    }
    
    // Next up, set up the tile animation stuff
    nTileidx.resize(nFrames);                                   // Make the vectors fit
    bFlip.resize(nFrames);
    for (int i = 0; i < nFrames; i++)
    {
        nTileidx[i] = i;                                         // set initial values for the vectors
        bFlip[i] = false;
    }

    animstate = vsp->vspAnim;
    for (uint j = 0; j < vsp->vspAnim.size(); j++) {
        animstate[j].count = animstate[j].delay;                // init the counter
    }
}

TileSet::~TileSet()
{
    for (int i = 0; i < nFrames; i++)
        video->FreeImage(hFrame[i]);
}

void TileSet::Save(const std::string& fileName) const {
    vsp->Save(fileName);
}

Video::Image* TileSet::GetTile(uint index) const {
    if (hFrame.empty()) {
        return 0;
    }

    if (index < 0 || index >= nTileidx.size()) {
        index = 0;
    }

    index = nTileidx[index];

    if (index < 0 || index >= nTileidx.size()) {
        index = 0;
    }

    return hFrame[index];
}

void TileSet::UpdateAnimation(int time)
{
    int i = time - nAnimtimer;					// how many ticks have elapsed?
    nAnimtimer = time;
    if (i < 1) return;						// not very much, wait a little longer
    if (i > 100) i = 100;   // hack
    
    while (i--)
    {
        for (int j = 0; j < 100; j++)
            if (animstate[j].start != animstate[j].finish)
                AnimateStrand(animstate[j]);
    }
}

void TileSet::AnimateStrand(VSP::AnimState& anim)
{
    int i;
    
    anim.count--;
    
    if (anim.count > 0)	return;
    
    switch(anim.mode)
    {
    case VSP::linear:
        for (i = anim.start; i < anim.finish; i++)
        {
            nTileidx[i]++;
            if (nTileidx[i] > anim.finish)
                nTileidx[i] = anim.start;
        }
        break;

    case VSP::reverse:
        for (i = anim.start; i < anim.finish; i++)
        {
            nTileidx[i]--;
            if (nTileidx[i] < anim.start)
                nTileidx[i] = anim.finish;
        }
        break;

    case VSP::random:
        for (i = anim.start; i < anim.finish; i++)
            nTileidx[i] = Random(anim.start, anim.finish + 1);
        break;

    case VSP::  flip:
        for (i = anim.start; i < anim.finish; i++)
        {
            if (bFlip[i])
            {
                nTileidx[i]++;
                if (nTileidx[i] > anim.finish)
                {
                    nTileidx[i] = anim.start;
                    bFlip[i] = !bFlip[i];
                }
            }
            else
            {
                nTileidx[i]--;
                if (nTileidx[i] < anim.start)
                {
                    nTileidx[i] = anim.finish;
                    bFlip[i] = !bFlip[i];
                }
            }
        }
        break;
    }
    
    anim.count = anim.delay;
}
