#include "sprite.h"
#include "chr.h"
#include "log.h"

CSprite::CSprite(const char* fname)
{
    LoadCHR(fname);
}

CSprite::~CSprite()
{
    Free();
}

void CSprite::Free()
{
    for (int i=0; i<hFrame.size(); i++)
        gfxFreeImage(hFrame[i]);
    hFrame.clear();
}

string& CSprite::Script(int s)
{
    static string dummy;

    if (s<0 || s>sScript.size())
        return dummy;

    return sScript[s];
}

void CSprite::BlitFrame(int x,int y,int frame)
{
    if (frame<0 || frame>hFrame.size())
        return;
    
    gfxBlitImage(hFrame[frame],x,y,true);
}

bool CSprite::LoadCHR(const char* fname)
{
    CCHRfile chr;
    bool bResult=chr.Load(fname);
    if (!bResult)
        return false;
    
    Free();
    
    nFramex=chr.Width();
    nFramey=chr.Height();
    nHotx=chr.HotX();
    nHoty=chr.HotY();
    nHotw=chr.HotW();
    nHoth=chr.HotH();
    
    sScript.resize(chr.sMovescript.size());
    for (int s=0; s<chr.sMovescript.size(); s++)
        sScript[s]=chr.sMovescript[s];
    
    hFrame.resize(chr.NumFrames());
    for (int i=0; i<chr.NumFrames(); i++)
    {
        hFrame[i]=gfxCreateImage(nFramex,nFramey);
        gfxCopyPixelData(hFrame[i], (u32*)chr.GetFrame(i).GetPixelData(), nFramex, nFramey);
    }
    
    return true;
}

// -----------------------------------  CSpriteController methods ------------------------

CSprite* CSpriteController::Load(const char* fname)
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
    CRefCountedSprite* s=new CRefCountedSprite(fname);
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

    log("Unallocated sprite tried to release!!  \"%s\"",((CRefCountedSprite*)s)->sFilename);
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