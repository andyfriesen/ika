/*
#ifndef TILESET_H
#define TILESET_H

#include "types.h"
#include <wx\wx.h>

//#include "vsp.h"
class VSP;
class CPixelMatrix;

template <class Image>
class CTileSet
{
    VSP*                pVsp;
    vector<bool>        bAltered;   // true if a tile has been tweaked
    vector<Image*>      bitmaps;    // hardware dependant copies of the tiles

    void Sync();                    // updates bitmaps to mirror the VSP
    void FreeBitmaps();             // Deallocates the bitmaps vector

public:

    CTileSet();
    ~CTileSet();

    bool Load(const char* fname);
    bool Save(const char* fname);

    const CPixelMatrix&   GetTile(int tileidx);                 // returns the pixel data for the tile
    void SetTile(int tileidx,const CPixelMatrix& newtile);      // assigns the pixel data for the tile

    void DrawTile(int x,int y,int tileidx,wxDC* dest);          // draws the tile on the DC, updating the copy as needed

    // TODO: methods for inserting/deleting tiles, and moving them around
};

#endif*/