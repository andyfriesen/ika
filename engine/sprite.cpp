#include <cassert>

#include "sprite.h"

#include "common/log.h"
#include "common/chr.h"
#include "common/utility.h"
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

    _scripts = chr.moveScripts;
    
    // Get a list of idle and move scripts.  Since we'll be needing these quite often, it's useful to have them arranged in such a format
    // It's also faster, if that actually matters any.
    const char* dirNames[] = {
        "up", "down", "left", "right",
        "upleft", "upright", "downleft", "downright"
    };
    const int numDirs = sizeof dirNames / sizeof dirNames[0];
    for (uint i = 0; i < numDirs; i++)
    {
        std::string s;
        
        s = "idle_";    s += dirNames[i];   _idleScripts[i] = _scripts[s];
        s = "walk_";    s += dirNames[i];   _walkScripts[i] = _scripts[s];
    }
    
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

const std::map<std::string, std::string>& Sprite::GetAllScripts() const
{
    return _scripts;
}

const std::string& Sprite::GetScript(const std::string& name)
{
    static std::string dummy;

    if (!_scripts.count(name))
        return dummy;

    return _scripts[name];
}

const std::string& Sprite::GetIdleScript(Direction dir)
{
    assert(int(dir) >= 0 && int(dir) < 8);

    return _idleScripts[int(dir)];
}

const std::string& Sprite::GetWalkScript(Direction dir)
{
    assert(int(dir) >= 0 && int(dir) < 8);

    return _walkScripts[int(dir)];
}

Video::Image* Sprite::GetFrame(uint frame) const
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
        assert(s->getRefCount() > 0);

        if (s->getRefCount() == 1)
            sprite.erase(s->_fileName);

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
