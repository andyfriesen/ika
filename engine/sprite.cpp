#include <cassert>

#include "sprite.h"

#include "common/log.h"
#include "common/chr.h"
#include "video/Driver.h"
#include "video/Image.h"

CSprite::CSprite(const std::string& fname, Video::Driver* v)
    : video(v)
{
    CCHRfile chr;
    chr.Load(fname);
        
    nFramex=chr.Width();
    nFramey=chr.Height();
    nHotx=chr.HotX();
    nHoty=chr.HotY();
    nHotw=chr.HotW();
    nHoth=chr.HotH();
    
    sScript.resize(chr.moveScripts.size());
    for (uint s = 0; s < chr.moveScripts.size(); s++)
        sScript[s] = chr.moveScripts[s];
    
    hFrame.resize(chr.NumFrames());
    for (uint i = 0; i < chr.NumFrames(); i++)
    {
        hFrame[i] = video->CreateImage(chr.GetFrame(i));
    }
}

CSprite::~CSprite()
{
    for (uint i=0; i<hFrame.size(); i++)
        video->FreeImage(hFrame[i]);//delete hFrame[i];
}

const std::string& CSprite::Script(uint s) const
{
    static std::string dummy;

    if (s >= sScript.size())
        return dummy;

    return sScript[s];
}

Video::Image* CSprite::GetFrame(uint frame)
{
    if (frame<0 || frame>hFrame.size())
        return 0;
    
    return hFrame[frame];
}

// -----------------------------------  CSpriteController methods ------------------------

CSprite* CSpriteController::Load(const std::string& fname, Video::Driver* video)
{
    CDEBUG("ccharactercontroller::load");

    for (SpriteList::iterator i=sprite.begin(); i!=sprite.end(); i++)
    {
        if ((*i)->sFilename==fname)
        {
            (*i)->nRefcount++;
            return *i;
        }
    }

    // Not already loaded, we'll have to do that now.
    CRefCountedSprite* s;
    s=new CRefCountedSprite(fname, video);
    
    s->nRefcount=1;
    s->sFilename=fname;

    sprite.push_back(s);

    return s;
}

void CSpriteController::Free(CSprite* s)
{
    CRefCountedSprite* a=(CRefCountedSprite*)s;
    for (SpriteList::iterator i=sprite.begin(); i!=sprite.end(); i++)
    {
        CRefCountedSprite* t = *i;
        if (a==t)
        {
            t->nRefcount--;

            if (t->nRefcount == 0)
            {
                Log::Write("Deallocating %s", t->sFilename.c_str());
                sprite.remove(*i);
                delete s;
            }
            return;
        }
    }

    Log::Write("Unallocated sprite tried to release!!  \"%s\"", ((CRefCountedSprite*)s)->sFilename.c_str());
}

CSpriteController::~CSpriteController()
{
    for (SpriteList::iterator i=sprite.begin(); i!=sprite.end(); i++)
    {
        CRefCountedSprite* s=*i;
        delete s;
    }

    sprite.clear();
}
