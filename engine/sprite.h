#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"
#include "graph.h"
#include "fileio.h"

#include <list>
using std::list;

/**
    A hardware dependant representation of a .CHR file.
*/
class CSprite
{
    vector<string>  sScript;                        ///< move scripts

    int nFramex,nFramey;                            ///< frame size

public:
    string sFilename;

    short int	nHotx,nHoty;		                ///< hotspot position
    short int	nHotw,nHoth;		                ///< hotspot size

    vector<Video::Image*> hFrame;                   ///< array of frame images

    CSprite() {}
    CSprite(const char* fname);
    virtual ~CSprite();
    void Free();

    void BlitFrame(int x,int y,int frame);          ///< Draws a frame of the sprite at the specified location.
  
    bool LoadCHR(const char* fname);                ///< Loads data from the specified file.

    inline int Width() const { return nFramex; }
    inline int Height() const { return nFramey; }
    string& Script(int s);
};

/**
    Responsible for handling sprite allocation and deallocation.
    CSpriteController also keeps tabs on redundant requests for the same sprite, and
    refcounts accordingly.
*/
class CSpriteController
{
    struct CRefCountedSprite : public CSprite
    {
        int nRefcount;
    };

    typedef list<CRefCountedSprite*> SpriteList;

    SpriteList sprite;                                      ///< List of allocated sprites

public:
    CSprite* Load(const char* fname);                       ///< loads a CHR file
    void Free(CSprite* s);                                  ///< releases a CHR file

    ~CSpriteController();
};

#endif