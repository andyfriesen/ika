/* 
    the Speed Bump's Spiffy VSP class object thingie (tm)
    Copyright (c) 2000 the Speed Bump
    Based on some crap that vecna made way back when. (c) 199x

    Ihis is my first real attempt to make anything object - oriented, so it may
    look icky, but I'm kinda proud of it right now.
      
    This has got to be the oldest code chunk I have that I still
    use. ^_~
    --tSB May 2001
        
    And it sucked!  Major revamp.
    --andy Nov 2001
*/

// vsp.h

#pragma once

#include "common/utility.h"
#include "Canvas.h"

struct VSP {
    /// VSP animation modes
    enum {
        linear,
        reverse,
        random,
        flip
    };

    struct AnimState {
        uint start;
        uint finish;
        int delay;
        int mode;

        int count;

        AnimState() 
            : start(0)
            , finish(0)
            , delay(0)
            , mode(linear)
            , count(0)
        {}
    };

    VSP();
    VSP(const std::string& fname);
    
    bool Load(const std::string& fname);
    int  Save(const std::string& fname);
    void Free();
    void New(int xsize = 16, int ysize = 16, int numtiles = 100);

    void InsertTile(uint pos);
    void DeleteTile(uint pos);
    inline void AppendTile() { AppendTiles(1); }
    void AppendTiles(uint count);
    void CopyTile(Canvas& tb, uint pos);        // Copies the tile into a buffer.
    void PasteTile(const Canvas& tb, uint pos); // pastes the tile from a buffer
    void TPasteTile(Canvas& tb, uint pos);      // transparently pastes the tile from a buffer
    
    void SetSize(int w, int h);
    
    std::vector<AnimState>& vspAnim;            // I see no reason to hide this, really.

    Canvas& GetTile(uint tileidx);

    inline int Width() const { return _width; }
    inline int Height() const { return _height; }
    inline uint NumTiles() const { return tiles.size(); }
    
    inline const std::string& Name() const { return name; }
    
private:
    VSP(VSP& v);
    VSP& operator =(VSP& rhs);

    void CreateTilesFromBuffer(u8* data, u8* pal, uint numtiles, int tilex, int tiley);
    void CreateTilesFromBuffer(u16* data, uint numtiles, int tilex, int tiley);
    void CreateTilesFromBuffer(RGBA* data, uint numtiles, int tilex, int tiley);

    std::vector<Canvas>   tiles;     // tile images
    
    std::string desc;
    std::string name;           // the VSPs filename
    
    std::vector<AnimState>  _vspanim;
    
    int _width, _height;
};

