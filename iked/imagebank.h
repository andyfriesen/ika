
#ifndef IMAGEBANK_H
#define IMAGEBANK_H

#include "types.h"
#include <set>

class CImage;
class CPixelMatrix;

// abstract image bank class.
// Subclassed for VSPs, fonts and such, so they can have hardware-specific copies
// that stay sync'd.
class CImageBank
{
    vector<CImage*> bitmaps;
    std::set<int> altered;

    virtual void SetImage(const CPixelMatrix& img,int idx)=0;
public:
    virtual ~CImageBank();

    void Sync();
    void SyncAll();
    void FreeBitmaps();

    virtual const CPixelMatrix& Get(int idx)=0;
    void Set(CPixelMatrix& img,int idx);

    virtual int Count() const=0;

    CImage& GetImage(int idx);
};

#endif