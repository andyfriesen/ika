
#ifndef SPRITESET_H
#define SPRITESET_H

#include "imagebank.h"

class CCHRfile;

class CSpriteSet : public CImageBank
{
    virtual void SetImage(const Canvas& img,int idx);

    CCHRfile* pCHR;

public:

    CSpriteSet();

    bool Load(const char* fname);
    bool Save(const char* fname);

    virtual Canvas& Get(int idx);
    virtual int Count() const;   
    int Width() const;
    int Height() const;

    inline CCHRfile& GetCHR() const { return *pCHR; }    

};

#endif