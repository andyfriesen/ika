
#ifndef IMAGEBANK_H
#define IMAGEBANK_H

#include "common/utility.h"
#include <set>

class CImage;
class Canvas;

// abstract image bank class.
// Subclassed for VSPs, fonts and such, so they can have hardware - specific copies
// that stay sync'd.
class CImageBank
{
protected:
    std::vector<CImage*> bitmaps;
    std::set<int> altered;

    virtual void SetImage(const Canvas& img, int idx)=0;
public:
    virtual ~CImageBank();

    void Sync();
    void SyncAll();
    void FreeBitmaps();

    virtual Canvas& Get(int idx)=0;
    void Set(const Canvas& img, int idx);

    virtual int Count() const = 0;

    CImage& GetImage(int idx);
};

#endif