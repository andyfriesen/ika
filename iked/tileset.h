
#ifndef TILESET_H
#define TILESET_H

#include "types.h"
#include <set>
//#include <wx\wx.h>

class CPixelMatrix;
class CGraphFactory;
class CGraphFrame;
class CImage;

class VSP;

class CTileSet
{
    /*struct CTile
    {
        CImage* pImg;
        bool bAltered;

        CTile() : pImg(0),bAltered(false) {}
    };*/

    VSP*                pVsp;
    vector<CImage*>     bitmaps;    // hardware dependant copies of the tiles

    std::set<int>       altered;    // tiles that have changed since the last sync
public:    
    void Sync();                    // updates bitmaps to mirror the VSP
    void SyncAll();                 // Deallocates the bitmaps, then reconstructs them all.
    void FreeBitmaps();             // Deallocates the bitmaps vector

    // Don't think this is the Right Thing.  UI stuff doesn't belong here.
    // However, this has the advantage of making the VSP and map views
    // innately interoperate.
    int  nCurtile;

public:

    CTileSet();
    ~CTileSet();

    bool Load(const char* fname);
    bool Save(const char* fname);

    const CPixelMatrix&   GetTile(int tileidx);                 // returns the pixel data for the tile
    void SetTile(int tileidx,const CPixelMatrix& newtile);      // assigns the pixel data for the tile
    int  NumTiles() const;
    int  Width() const;
    int  Height() const;

    void DrawTile(int x,int y,int tileidx,CGraphFrame& dest);   // draws the tile on the frame, updating the copy as needed

    CImage& GetImage(int tileidx) const;

    // Don't use this unless you're doing something that has nothing to do with the tile images.
    // Use the CTileSet interface instead, as it will keep things synch'd.
    VSP& GetVSP() { return *pVsp; }

    inline int CurTile() const { return nCurtile; }
    void SetCurTile(int t);

    // TODO: methods for inserting/deleting tiles, and moving them around
};

#endif