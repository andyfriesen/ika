#ifndef TILESET_H
#define TILESET_H

#include "common/types.h"
#include "common/vsp.h"

namespace Video
{
    class Driver;
    class Image;
}

struct TileSetException{};

/*!
  A hardware-dependant representation of a VSP.
*/
class CTileSet
{
    Video::Driver* video;
    std::vector<Video::Image*> hFrame;                       ///< Array of image handles.
    int nFrames;                                        ///< Number of tiles in the tileset.
    int nFramex, nFramey;                                ///< Tile dimensions

    std::vector<int>    nTileidx;                            ///< Translation table for actual tiles <--> the tile that should be drawn. (animating tiles)
    std::vector<bool>   bFlip;                               ///< For tiles in the "flip" mode. (back and forth)
    
    std::vector<VSP::AnimState>    animstate;                ///< Animation states for each tile

    int nAnimtimer;                                     ///< used by updateanimation

    void AnimateStrand(VSP::AnimState& anim);           ///< Updates one tile's animation state.

public:
    CTileSet(const std::string& fname, Video::Driver* v);
    ~CTileSet();

    Video::Image* GetTile(int index);
    Video::Image* operator[](int index) { return GetTile(index); }

    inline int NumTiles() const { return nFrames; }     ///< Returns the number of tiles in the tileset. ;)

    inline int Width() const { return nFramex; }        ///< Width of the tiles in the tileset.
    inline int Height() const { return nFramey; }       ///< Height of the tiles in the tileset.

    void UpdateAnimation(int time);                     ///< Updates the animation state.  Pass the current time.
};

#endif
