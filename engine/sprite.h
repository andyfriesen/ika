#ifndef SPRITE_H
#define SPRITE_H

#include "common/types.h"

#include <list>
using std::list;

namespace Video
{
    class Driver;
    class Image;
}

struct SpriteException{};

/**
    A hardware dependant representation of a .CHR file.
*/
class CSprite
{
    std::vector<std::string>  sScript;                            ///< move scripts
    Video::Driver* video;

    int nFramex, nFramey;                               ///< frame size

public:
    std::string sFilename;

    short int	nHotx, nHoty;		                ///< hotspot position
    short int	nHotw, nHoth;		                ///< hotspot size

    std::vector<Video::Image*> hFrame;                       ///< array of frame images

    CSprite(const std::string& fname, Video::Driver* v);
    virtual ~CSprite();

    Video::Image* GetFrame(uint frame);                 ///< Returns the frame image
  
    inline int Width() const { return nFramex; }
    inline int Height() const { return nFramey; }
    const std::string& Script(uint s) const;
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
        CRefCountedSprite(const std::string& fname, Video::Driver* v)
            : CSprite(fname, v)
            , nRefcount(0)
        {}
    };

    typedef list<CRefCountedSprite*> SpriteList;

    SpriteList sprite;                                      ///< List of allocated sprites

public:
    CSprite* Load(const std::string& fname, Video::Driver* video); ///< loads a CHR file
    void Free(CSprite* s);                                  ///< releases a CHR file

    ~CSpriteController();
};

#endif
