#ifndef TILESET_H
#define TILESET_H

#include "types.h"
#include "graph.h"
#include "vsp.h"

struct TileSetException{};

/*!
  A hardware-dependant representation of a VSP.
*/
class CTileSet
{
    Video::Driver* video;
    vector<Video::Image*> hFrame;                       ///< Array of image handles.
    int nFrames;                                        ///< Number of tiles in the tileset.
    int nFramex,nFramey;                                ///< Tile dimensions

    vector<int>    nTileidx;                            ///< Translation table for actual tiles <--> the tile that should be drawn. (animating tiles)
    vector<bool>   bFlip;                               ///< For tiles in the "flip" mode. (back and forth)
    
    vector<VSP::AnimState>    animstate;                ///< Animation states for each tile

    int nAnimtimer;                                     ///< used by updateanimation

    void AnimateStrand(VSP::AnimState& anim);           ///< Updates one tile's animation state.

public:
    CTileSet(const char* fname, Video::Driver* v);
    ~CTileSet();

    Video::Image* GetTile(int index);
    Video::Image* operator[](int index) { return GetTile(index); }

    inline int NumTiles() const { return nFrames; }     ///< Returns the number of tiles in the tileset. ;)

    inline int Width() const { return nFramex; }        ///< Width of the tiles in the tileset.
    inline int Height() const { return nFramey; }       ///< Height of the tiles in the tileset.

    void UpdateAnimation(int time);                     ///< Updates the animation state.  Pass the current time.
};

#endif