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

#ifndef VSP_H
#define VSP_H

#include "types.h"
#include "Canvas.h"

class VSP
{
public:
    enum    // VSP animation modes
    {
        linear,
            reverse,
            random,
            flip
    };
    
    struct AnimState
    {
        int nStart;
        int nFinish;
        int nDelay;
        int mode;
        
        int nCount;
        
        AnimState() 
            : nStart(0), nFinish(0), nDelay(0), mode(linear),
            nCount(0)
        {}
    };
    
private:
    std::vector<Canvas>   tiles;     // tile images
    
    std::string desc;
    std::string name;           // the VSPs filename
    
    std::vector<AnimState>  vspanim;
    
    int _width, _height;
    
public:
    VSP();
    VSP(const std::string& fname);
    ~VSP();
    
    bool Load(const std::string& fname);
    int  Save(const std::string& fname);
    void Free();
    void New(int xsize = 16, int ysize = 16, int numtiles = 100);
    
    void InsertTile(uint pos);
    void DeleteTile(uint pos);
    inline void AppendTile() { AppendTiles(1); }
    void AppendTiles(uint count = 1);
    void CopyTile(Canvas& tb, uint pos);        // Copies the tile into a buffer.
    void PasteTile(const Canvas& tb, uint pos); // pastes the tile from a buffer
    void TPasteTile(Canvas& tb, uint pos);      // transparently pastes the tile from a buffer
    
    void SetPixel(int x, int y, uint tileidx, int c);
    int  GetPixel(int x, int y, uint tileidx);
    
    AnimState& Anim(uint strand);

    Canvas& GetTile(uint tileidx);

    inline int Width() const { return _width; }
    inline int Height() const { return _height; }
    inline uint NumTiles() const { return tiles.size(); }
    
    inline const std::string& Name() const { return name; }
    
private:
    void CreateTilesFromBuffer(u8* data, u8* pal, uint numtiles, int tilex, int tiley);
    void CreateTilesFromBuffer(u16* data, uint numtiles, int tilex, int tiley);
    void CreateTilesFromBuffer(RGBA* data, uint numtiles, int tilex, int tiley);

};

#endif
