
#ifndef SPRITESET_H
#define SPRITESET_H

#include "imagebank.h"
#include "misc.h"
#include "chr.h"

//class CCHRfile;

struct SpriteSet : ImageBank
{
    SpriteSet();

    void New(int width, int height);
    bool Load(const std::string& fname);
    bool Save(const std::string& fname);

    virtual Canvas& Get(uint idx);
    virtual uint Count() const;   
    int Width() const;
    int Height() const;

    void AppendFrame();
    void AppendFrame(Canvas& p);
    void InsertFrame(int i);
    void InsertFrame(int i, Canvas& p);
    void DeleteFrame(int i);

    void Resize(int width, int height);

    inline CCHRfile* GetCHR() const { return _chr.get(); }

private:
    virtual void SetImage(const Canvas& img, uint idx);

    ScopedPtr<CCHRfile> _chr;
};

#endif