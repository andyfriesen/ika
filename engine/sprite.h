#ifndef SPRITE_H
#define SPRITE_H

#include "common/types.h"
#include "common/refcount.h"

#include <map>

namespace Video
{
    class Driver;
    class Image;
}

struct SpriteException{};

/**
 *  A hardware dependant representation of a spriteset file.
 */
struct Sprite : RefCounted
{
    std::string _fileName;

    int	nHotx, nHoty;		                            ///< hotspot position
    int	nHotw, nHoth;		                            ///< hotspot size


    Sprite(const std::string& fname, Video::Driver* v);
    virtual ~Sprite();

    Video::Image* GetFrame(uint frame) const;               ///< Returns the frame image

    inline uint Count()  const { return _frames.size(); }
    inline uint Width()  const { return nFramex; }
    inline uint Height() const { return nFramey; }

    const std::string& Script(const std::string& name);
    const std::string& GetIdleScript(Direction dir);
    const std::string& GetWalkScript(Direction dir);

private:
    std::map<std::string, std::string> _scripts;            ///< all move scripts
    std::string _walkScripts[8];                            ///< walking scripts
    std::string _idleScripts[8];                            ///< idle(standing) scripts
    Video::Driver* video;

    uint nFramex, nFramey;                                  ///< frame size

    std::vector<Video::Image*> _frames;                     ///< frame images
};

/**
 *  Responsible for handling sprite allocation and deallocation.
 *  CSpriteController also keeps tabs on redundant requests for the same sprite, and
 *  refcounts accordingly.
 */
class CSpriteController
{
    typedef std::map<std::string, Sprite*> SpriteMap;

    SpriteMap sprite;                                      ///< List of allocated sprites

public:
    Sprite* Load(const std::string& fname, Video::Driver* video); ///< loads a CHR file
    void Free(Sprite* s);                                  ///< releases a CHR file

    ~CSpriteController();
};

#endif
