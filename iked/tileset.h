
#ifndef TILESET_H
#define TILESET_H

#include "types.h"
//#include <wx\wx.h>

class CPixelMatrix;
class CGraphFactory;
class CGraphFrame;
class CImage;

class VSP;

class CTileSet
{
    CGraphFactory*      pGraphfactory;  // we use this to make images, basically
    VSP*                pVsp;
    vector<bool>        bAltered;   // true if a tile has been tweaked
    vector<CImage*>     bitmaps;    // hardware dependant copies of the tiles
    
    void Sync();                    // updates bitmaps to mirror the VSP
    void FreeBitmaps();             // Deallocates the bitmaps vector

public:

    CTileSet(CGraphFactory* f=0);
    ~CTileSet();

    void SetFactory(CGraphFactory* p) { pGraphfactory=p; }

    bool Load(const char* fname);
    bool Save(const char* fname);

    const CPixelMatrix&   GetTile(int tileidx);                 // returns the pixel data for the tile
    void SetTile(int tileidx,const CPixelMatrix& newtile);      // assigns the pixel data for the tile

    void DrawTile(int x,int y,int tileidx,CGraphFrame& dest);   // draws the tile on the DC, updating the copy as needed

    // TODO: methods for inserting/deleting tiles, and moving them around
};

#endif