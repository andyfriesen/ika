
#include "spriteset.h"
#include "chr.h"
#include "log.h"

CSpriteSet::CSpriteSet() : pCHR(0)
{
}

void CSpriteSet::New(int width, int height)
{
    pCHR = new CCHRfile;
    pCHR->New(width, height);
}

bool CSpriteSet::Load(const char* fname)
{
    CCHRfile* pNewchr = new CCHRfile;

    try
    {   pNewchr->Load(fname);   }
    catch (std::runtime_error err)
    {
        Log::Write(va("CCHRfile: %s", err.what()));
        return false;
    }

    delete pCHR;
    pCHR = pNewchr;

    SyncAll();
    return true;
}

bool CSpriteSet::Save(const char* fname)
{
    pCHR->Save(fname);

    return true;
}

void CSpriteSet::SetImage(const Canvas& img, int idx)
{
    pCHR->GetFrame(idx)=img;
}

Canvas& CSpriteSet::Get(int idx)
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

void CSpriteSet::AppendFrame()
{
    InsertFrame(Count());
}

void CSpriteSet::AppendFrame(Canvas& p)
{
    InsertFrame(Count(), p);
}

void CSpriteSet::InsertFrame(int i)
{
    pCHR->InsertFrame(i);
    SyncAll();
}

void CSpriteSet::InsertFrame(int i, Canvas& p)
{
    pCHR->InsertFrame(i, p);
    SyncAll();
}

void CSpriteSet::DeleteFrame(int i)
{
    pCHR->DeleteFrame(i);
    SyncAll();
}

void CSpriteSet::Resize(int width, int height)
{
    pCHR->Resize(width, height);
    SyncAll();
}