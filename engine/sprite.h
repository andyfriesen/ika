/*
    A CSprite is a hardware dependant representation of a .CHR file.
*/

#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"
#include "graph.h"
#include "fileio.h"

#include <list>
using std::list;

class CSprite
{
    vector<string>  sScript;                            // move scripts

    int nFramex,nFramey;                                // frame size

public:
    string sFilename;

    short int	nHotx,nHoty;		                // hotspot position
    short int	nHotw,nHoth;		                // hotspot size

    vector<handle> hFrame;

    CSprite() {}
    CSprite(const char* fname);
    virtual ~CSprite();
    void Free();

    void BlitFrame(int x,int y,int frame);
  
    bool LoadCHR(const char* fname);

    inline int Width() const { return nFramex; }
    inline int Height() const { return nFramey; }
    string& Script(int s);
};

class CSpriteController
{
    struct CRefCountedSprite : public CSprite
    {
        int nRefcount;

        CRefCountedSprite(const char* fname) : CSprite(fname) {}
    };

    typedef list<CRefCountedSprite*> SpriteList;

    SpriteList sprite;

public:
    CSprite* Load(const char* fname);                       // loads a CHR file
    void Free(CSprite* s);                                  // releases a CHR file

    ~CSpriteController();
};

#endif