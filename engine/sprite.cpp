#include <cassert>

#include "sprite.h"

#include "common/log.h"
#include "common/chr.h"
#include "common/misc.h"
#include "video/Driver.h"
#include "video/Image.h"

Sprite::Sprite(const std::string& fname, Video::Driver* v)
    : video(v)
{
    CCHRfile chr;
    chr.Load(fname);
        
    nFramex = chr.Width();
    nFramey = chr.Height();
    nHotx = chr.HotX();
    nHoty = chr.HotY();
    nHotw = chr.HotW();
    nHoth = chr.HotH();
    
    sScript.resize(chr.moveScripts.size());
    for (uint s = 0; s < chr.moveScripts.size(); s++)
        sScript[s] = chr.moveScripts[s];
    
    _frames.resize(chr.NumFrames());
    for (uint i = 0; i < chr.NumFrames(); i++)
    {
        _frames[i] = video->CreateImage(chr.GetFrame(i));
    }
}

Sprite::~Sprite()
{
    for (uint i = 0; i < _frames.size(); i++)
        video->FreeImage(_frames[i]);
}

const std::string& Sprite::Script(uint s) const
{
    static std::string dummy;

    if (s >= sScript.size())
        return dummy;

    return sScript[s];
}

Video::Image* Sprite::GetFrame(uint frame)
{
    if (frame < 0 || frame > _frames.size())
        return 0;
    
    return _frames[frame];
}

// -----------------------------------  CSpriteController methods ------------------------

Sprite* CSpriteController::Load(const std::string& fname, Video::Driver* video)
{
    CDEBUG("ccharactercontroller::load");

    if (sprite.count(fname) != 0)
    {
        Sprite* s = sprite[fname];
        s->ref();
        return s;
    }

    // Not already loaded, we'll have to do that now.
    Sprite* s = new Sprite(fname, video);
    s->_fileName=fname;
    s->ref();
    sprite[fname] = s;

    return s;
}

void CSpriteController::Free(Sprite* s)
{
    if (sprite.count(s->_fileName))
    {
        if (s->getRefCount() == 1)
        {
            Log::Write("Deallocating %s", s->_fileName.c_str());
            sprite.erase(s->_fileName);
        }

        s->unref();
    }
    else
        Log::Write("Unallocated sprite tried to release!!  \"%s\"", s->_fileName.c_str());
}

CSpriteController::~CSpriteController()
{
    for (SpriteMap::iterator i = sprite.begin(); i != sprite.end(); i++)
    {
        if (i->second->getRefCount() != 1)
        {
            Log::Write("Internal Warning: Leaking sprite %s.  Refcount is still %i.", i->second->_fileName.c_str(), i->second->getRefCount());
        }

        i->second->unref();
    }

    sprite.clear();
}
