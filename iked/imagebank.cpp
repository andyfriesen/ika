#include "imagebank.h"
#include "graph.h"

CImageBank::~CImageBank()
{
    FreeBitmaps();
}

void CImageBank::Sync()
{
    for (std::set<int>::iterator i = altered.begin(); i!=altered.end(); i++)
    {
        bitmaps[*i]->Update(Get(*i));
    }

    altered.clear();
}

void CImageBank::SyncAll()
{
    FreeBitmaps();

    bitmaps.resize(Count());
}

void CImageBank::FreeBitmaps()
{
    for (uint i = 0; i < bitmaps.size(); i++)
    {
        CImage* img = bitmaps[i];
        delete img;
    }
    bitmaps.clear();
}

void CImageBank::Set(const Canvas& img, int idx)
{
    if (idx < 0 || idx >= Count())
        return;

    SetImage(img, idx);
    altered.insert(idx);
}

CImage& CImageBank::GetImage(int idx)
{
    if (altered.find(idx) != altered.end() && bitmaps[idx])   // has this image been altered?
    {
        bitmaps[idx]->Update(Get(idx));     // sync it
        altered.erase(idx);
    }

    if (idx < 0 || idx >= Count())
        idx = 0;

    // lazy allocation
    if (bitmaps[idx] == 0)
        bitmaps[idx] = new CImage(Get(idx));

    return *bitmaps[idx];
}