#include "tileset.h"
#include "vsp.h"
#include "pixel_matrix.h"
#include "graph.h"

CTileSet::CTileSet()
: pVsp(0)
{
}

CTileSet::~CTileSet()
{
    FreeBitmaps();
}

void CTileSet::Sync()
{
    for (int i=0; i<bitmaps.size(); i++)
    {
        CTile& tile=bitmaps[i];

        if (tile.bAltered)
            tile.pImg->Update(pVsp->GetTile(i));
    }
}

void CTileSet::SyncAll()
{
    FreeBitmaps();

    bitmaps.resize(pVsp->NumTiles());

    for (int i=0; i<pVsp->NumTiles(); i++)
    {
        bitmaps[i].bAltered=false;
        bitmaps[i].pImg=new CImage(pVsp->GetTile(i));
    }
}

void CTileSet::FreeBitmaps()
{
    for (int i=0; i<bitmaps.size(); i++)
    {
        delete bitmaps[i].pImg;
        bitmaps[i].pImg=NULL;
    }
}

bool CTileSet::Load(const char* fname)
{
    VSP* pNewvsp=new VSP;

    if(!pNewvsp->Load(fname))
    {
        delete pNewvsp;
        return false;
    }

    delete pVsp;
    pVsp=pNewvsp;

    SyncAll();

    return true;
}

bool CTileSet::Save(const char* fname)
{
    return false;
}

void CTileSet::DrawTile(int x,int y,int tileidx,CGraphFrame& dest)
{
    if (tileidx<0 || tileidx>=bitmaps.size())
        return;

    dest.Blit(*bitmaps[tileidx].pImg,x,y,true);
}