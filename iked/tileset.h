
#ifndef TILESET_H
#define TILESET_H

#include "types.h"
#include <set>
#include "imagebank.h"
//#include <wx\wx.h>


class VSP;

class CTileSet : public CImageBank
{
    VSP* pVsp;

    virtual void SetImage(const CPixelMatrix& img,int idx);
private:

    // Don't think this is the Right Thing.  UI stuff doesn't belong here.
    // However, this has the advantage of making the VSP and map views
    // interoperate with minimum fuss.
    int  nCurtile;

public:

    CTileSet();

    bool Load(const char* fname);
    bool Save(const char* fname);

    virtual CPixelMatrix&   Get(int idx);                 // returns the pixel data for the tile
    virtual int   Count() const;
    int  Width() const;
    int  Height() const;

    // Don't use this unless you're doing something that has nothing to do with the tile images.
    // Use the CTileSet interface instead, as it will keep things synch'd.
    inline VSP& GetVSP() const { return *pVsp; }

    inline int CurTile() const { return nCurtile; }
    void SetCurTile(uint t);
};

#endif