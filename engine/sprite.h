/*
    A CSprite is a hardware dependant representation of a .CHR file.
*/

#ifndef SPRITE_H
#define SPRITE_H

#pragma warning (disable:4786)

#include <vector>
#include <list>
#include "graph.h"
#include "fileio.h"
#include "strk.h"

class CSprite
{
    std::vector<string_k>	sScript;		// move scripts

    int nFramex,nFramey;                                // frame size

public:
    string_k sFilename;

    short int	nHotx,nHoty;		                // hotspot position
    short int	nHotw,nHoth;		                // hotspot size

    std::vector<handle> hFrame;

    CSprite() {}
    CSprite(const char* fname);
    virtual ~CSprite();
    void Free();

    void BlitFrame(int x,int y,int frame);
  
    bool LoadCHR(const char* fname);

    inline int Width() const { return nFramex; }
    inline int Height() const { return nFramey; }
    string_k& Script(int s);
};

class CSpriteController
{
    struct CRefCountedSprite : public CSprite
    {
        int nRefcount;

        CRefCountedSprite(const char* fname) : CSprite(fname) {}
    };

    typedef std::list<CRefCountedSprite*> SpriteList;

    SpriteList sprite;

public:
    CSprite* Load(const char* fname);                       // loads a CHR file
    void Free(CSprite* s);                                  // releases a CHR file

    ~CSpriteController();
};

#endif