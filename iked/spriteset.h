
#ifndef SPRITESET_H
#define SPRITESET_H

#include "imagebank.h"

class CCHRfile;

class CSpriteSet : public CImageBank
{
    virtual void SetImage(const Canvas& img, int idx);

    CCHRfile* pCHR;

public:

    CSpriteSet();

    void New(int width, int height);
    bool Load(const char* fname);
    bool Save(const char* fname);

    virtual Canvas& Get(int idx);
    virtual int Count() const;   
    int Width() const;
    int Height() const;

    void AppendFrame();
    void AppendFrame(Canvas& p);
    void InsertFrame(int i);
    void InsertFrame(int i, Canvas& p);
    void DeleteFrame(int i);

    void Resize(int width, int height);

    inline CCHRfile& GetCHR() const { return *pCHR; }    

};

#endif