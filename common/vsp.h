/* 
the Speed Bump's Spiffy VSP class object thingie (tm)
Copyright (c) 2000 the Speed Bump
Based on some crap that vecna made way back when. (c) 199x

Ihis is my first real attempt to make anything object-oriented, so it may
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

#include <vector>
#include "types.h"
#include "pixel_matrix.h"

enum    // VSP animation modes
{
    linear,
    reverse,
    random,
    flip
};

class VSP
{
public:
    
    struct AnimState
    {
        int nStart;
        int nFinish;
        int nDelay;
        int mode;
        
        int nCount;
        
        AnimState() 
            : nStart(0),nFinish(0),nDelay(0),mode(linear),
            nCount(0)
        {}
    };
    
private:
    std::vector<CPixelMatrix>   tiles;  // tile images
    
    char sDesc[64];
    char sName[64];		// the VSPs filename
    
    std::vector<AnimState>  vspanim;
    
    int nTilex,nTiley;
    
public:
    VSP();
    VSP(const char* fname);
    ~VSP();
    
    bool Load(const char* fname);
    int  Save(const char* fname);
    void Free();
    void New(int xsize=16,int ysize=16);
    
    void InsertTile(int pos);
    void DeleteTile(int pos);
    inline void AppendTile() { AppendTiles(1); }
    void AppendTiles(int count=1);
    void CopyTile(CPixelMatrix& tb,int pos); // Copies the tile into a buffer.
    void PasteTile(const CPixelMatrix& tb,int pos); // pastes the tile from a buffer
    void TPasteTile(const CPixelMatrix& tb,int pos); // transparently pastes the tile from a buffer
    void To32bpp();
    
    void SetPixel(int x,int y,int tileidx,int c);
    int  GetPixel(int x,int y,int tileidx);
    
    AnimState& Anim(int strand);

    CPixelMatrix& GetTile(int tileidx);

    inline int Width() const { return nTilex; }
    inline int Height() const { return nTiley; }
    inline int NumTiles() const { return tiles.size(); }
    
    inline const char* Name() const { return (const char*)sName; }
    
    void CreateTilesFromBuffer(u8* data,u8* pal,int numtiles,int tilex,int tiley);
    void CreateTilesFromBuffer(u16* data,int numtiles,int tilex,int tiley);
    void CreateTilesFromBuffer(RGBA* data,int numtiles,int tilex,int tiley);

};

#endif
