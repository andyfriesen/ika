#include "sprite.h"

#include "common/log.h"
#include "common/chr.h"
#include "video/Driver.h"
#include "video/Image.h"

CSprite::CSprite(const char* fname, Video::Driver* v)
    : video(v)
{
    CCHRfile chr;
    bool bResult = chr.Load(fname);
    if (!bResult)
        throw SpriteException();
    
    nFramex=chr.Width();
    nFramey=chr.Height();
    nHotx=chr.HotX();
    nHoty=chr.HotY();
    nHotw=chr.HotW();
    nHoth=chr.HotH();
    
    sScript.resize(chr.sMovescript.size());
    for (uint s = 0; s < chr.sMovescript.size(); s++)
        sScript[s] = chr.sMovescript[s];
    
    hFrame.resize(chr.NumFrames());
    for (uint i = 0; i < chr.NumFrames(); i++)
    {
        hFrame[i] = video->CreateImage(chr.GetFrame(i));
    }
}

CSprite::~CSprite()
{
    for (int i=0; i<hFrame.size(); i++)
        delete hFrame[i];
}

string& CSprite::Script(int s)
{
    static string dummy;

    if (s<0 || s>sScript.size())
        return dummy;

    return sScript[s];
}

Video::Image* CSprite::GetFrame(int frame)
{
    if (frame<0 || frame>hFrame.size())
        return 0;
    
    return hFrame[frame];
}

// -----------------------------------  CSpriteController methods ------------------------

CSprite* CSpriteController::Load(const char* fname, Video::Driver* video)
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
    try
    {
        s=new CRefCountedSprite(fname, video);
    }
    catch (SpriteException)
    {
        return 0;
    }

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
        CRefCountedSprite* t=*i;
        if (a==t)
        {
            (*i)->nRefcount--;

            if ((*i)->nRefcount==0)
            {
                sprite.remove(*i);
                delete s;
            }
            return;
        }
    }

    Log::Write("Unallocated sprite tried to release!!  \"%s\"",((CRefCountedSprite*)s)->sFilename);
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
