#include "imagebank.h"
#include "video.h"

ImageBank::~ImageBank()
{
    FreeBitmaps();
}

void ImageBank::Sync()
{
    for (std::set<int>::iterator i = altered.begin(); i!=altered.end(); i++)
    {
        bitmaps[*i]->Update(Get(*i));
    }

    altered.clear();
}

void ImageBank::SyncAll()
{
    FreeBitmaps();

    bitmaps.resize(Count());
}

void ImageBank::FreeBitmaps()
{
    for (uint i = 0; i < bitmaps.size(); i++)
    {
        Image* img = bitmaps[i];
        delete img;
    }
    bitmaps.clear();
}

void ImageBank::Set(const Canvas& img, uint idx)
{
    if (idx >= Count())
        return;

    SetImage(img, idx);
    altered.insert(idx);
}

Image& ImageBank::GetImage(uint idx)
{
    if (altered.find(idx) != altered.end() && bitmaps[idx])   // has this image been altered?
    {
        bitmaps[idx]->Update(Get(idx));     // sync it
        altered.erase(idx);
    }

    if (idx >= Count())
        idx = 0;

    // lazy allocation
    if (bitmaps[idx] == 0)
        bitmaps[idx] = new Image(Get(idx));

    return *bitmaps[idx];
}