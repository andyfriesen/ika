/*

  CTileSet is a hardware-dependant representation of a VSP.

*/

#ifndef TILESET_H
#define TILESET_H

#include "graph.h"
#include "vsp.h"
#include <vector>

class CTileSet
{
    std::vector<handle> hFrame;
    int nFrames;
    int nFramex,nFramey;

    enum
    {
        linear,
        reverse,
        random,
        flip
    };

    std::vector<int>    nTileidx;
    std::vector<bool>   bFlip;
    
    // tile animation data
    std::vector<VSP::AnimState>    animstate;

    int nAnimtimer;                                     // used by updateanimation
   
public:
    CTileSet() : nFrames(0),nFramex(0),nFramey(0) {}
    ~CTileSet() { Free(); }

    void BlitFrame(int x,int y,int frame);
    void TBlitFrame(int x,int y,int frame);

    bool LoadVSP(const char* fname);
    void Free();

    inline int NumTiles() const { return nFrames; }

    inline int Width() const { return nFramex; }
    inline int Height() const { return nFramey; }

    void UpdateAnimation(int time);
    void AnimateStrand(VSP::AnimState& anim);
};

#endif