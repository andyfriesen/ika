#ifndef TILESET_H
#define TILESET_H

#include "common/utility.h"
#include "common/vsp.h"

namespace Video
{
    struct Driver;
    struct Image;
}

struct TilesetException{};

/**
 * Hardware-dependant, runtime-relevant information concerning a VSP.
 */
struct Tileset {
    Tileset(const std::string& fname, Video::Driver* v);
    ~Tileset();

    void Save(const std::string& fileName) const;

    Video::Image* GetTile(uint index) const;

    inline int NumTiles() const { return frameCount; }  ///< Returns the number of tiles in the tileset.

    inline int Width() const { return frameWidth; }     ///< Width of the tiles in the tileset.
    inline int Height() const { return frameHeight; }   ///< Height of the tiles in the tileset.

    void UpdateAnimation(int time);                     ///< Updates the animation state.  Pass the current time.

private:
    Video::Driver* video;
    ScopedPtr<VSP> vsp;                                 ///< Source tileset.

    std::vector<Video::Image*> hFrame;                  ///< Array of image handles.
    int frameCount;                                     ///< Number of tiles in the tileset.
    int frameWidth, frameHeight;                        ///< Tile dimensions

    std::vector<int>    tileIndex;                      ///< Translation table for actual tiles <--> the tile that should be drawn. (animating tiles)
    std::vector<bool>   flipFlag;                       ///< For tiles in the "flip" mode. (back and forth)
    
    std::vector<VSP::AnimState>    animstate;           ///< Animation states for each tile

    int animTimer;                                      ///< used by UpdateAnimation

    void AnimateStrand(VSP::AnimState& anim);           ///< Updates one tile's animation state.
};

#endif
