
#include "spriteset.h"
#include "chr.h"
#include "log.h"

SpriteSet::SpriteSet() 
    : _chr(new CCHRfile)
{
}

void SpriteSet::New(int width, int height)
{
    _chr->New(width, height);
}

bool SpriteSet::Load(const std::string& fname)
{
    CCHRfile* _newChr = new CCHRfile;

    try
    {   _newChr->Load(fname);   }
    catch (std::runtime_error err)
    {
        Log::Write(va("CCHRfile: %s", err.what()));
        return false;
    }

    delete _chr;
    _chr = _newChr;

    SyncAll();
    return true;
}

bool SpriteSet::Save(const std::string& fname)
{
    std::string fileName(fname);
    int pos = fileName.rfind('.');
    
    // If the extension is CHR, then export to the old format.
    if (pos != std::string::npos && Path::Compare("chr", fileName.substr(pos)) == true)
        _chr->SaveOld(fileName);
    else
        _chr->Save(fname);

    return true;
}

void SpriteSet::SetImage(const Canvas& img, int idx)
{
    _chr->GetFrame(idx) = img;
}

Canvas& SpriteSet::Get(int idx)
{
    return _chr->GetFrame(idx);
}

uint SpriteSet::Count() const
{
    return _chr->NumFrames();
}

int SpriteSet::Width() const
{
    return _chr->Width();
}

int SpriteSet::Height() const
{
    return _chr->Height();
}

void SpriteSet::AppendFrame()
{
    InsertFrame(Count());
}

void SpriteSet::AppendFrame(Canvas& p)
{
    InsertFrame(Count(), p);
}

void SpriteSet::InsertFrame(int i)
{
    _chr->InsertFrame(i);
    SyncAll();
}

void SpriteSet::InsertFrame(int i, Canvas& p)
{
    _chr->InsertFrame(i, p);
    SyncAll();
}

void SpriteSet::DeleteFrame(int i)
{
    _chr->DeleteFrame(i);
    SyncAll();
}

void SpriteSet::Resize(int width, int height)
{
    _chr->Resize(width, height);
    SyncAll();
}