
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
    struct CTile
    {
        CImage* pImg;
        bool bAltered;

        CTile() : pImg(0),bAltered(false) {}
    };

    VSP*                pVsp;
    vector<CTile>       bitmaps;    // hardware dependant copies of the tiles
    public:    
    void Sync();                    // updates bitmaps to mirror the VSP
    void SyncAll();                 // Deallocates the bitmaps, then reconstructs them all.
    void FreeBitmaps();             // Deallocates the bitmaps vector

public:

    CTileSet();
    ~CTileSet();

    bool Load(const char* fname);
    bool Save(const char* fname);

    const CPixelMatrix&   GetTile(int tileidx);                 // returns the pixel data for the tile
    void SetTile(int tileidx,const CPixelMatrix& newtile);      // assigns the pixel data for the tile

    void DrawTile(int x,int y,int tileidx,CGraphFrame& dest);   // draws the tile on the frame, updating the copy as needed

    // Don't use this unless you're doing something that has nothing to do with the tile images.
    // Use the CTileSet interface instead, as it will keep things synch'd.
    VSP& GetVSP() { return *pVsp; }

    // TODO: methods for inserting/deleting tiles, and moving them around
};

#endif