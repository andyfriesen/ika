
#include "spriteset.h"
#include "chr.h"

CSpriteSet::CSpriteSet() : pCHR(0)
{
}

bool CSpriteSet::Load(const char* fname)
{
    CCHRfile* pNewchr=new CCHRfile;

    if (!pNewchr->Load(fname))
        return false;

    delete pCHR;
    pCHR=pNewchr;

    SyncAll();
    return true;
}

bool CSpriteSet::Save(const char* fname)
{
    pCHR->Save(fname);

    return true;
}

void CSpriteSet::SetImage(const CPixelMatrix& img,int idx)
{
    pCHR->GetFrame(idx)=img;
}

CPixelMatrix& CSpriteSet::Get(int idx)
{
    return pCHR->GetFrame(idx);
}

int CSpriteSet::Count() const
{
    return pCHR->NumFrames();
}

int CSpriteSet::Width() const
{
    return pCHR?pCHR->Width():0;
}

int CSpriteSet::Height() const
{
    return pCHR?pCHR->Height():0;
}
