#include "tileset.h"
#include "vsp.h"
#include "pixel_matrix.h"
#include "graph.h"

CTileSet::CTileSet(CGraphFactory* p)
: pGraphfactory(p)
{
}

CTileSet::~CTileSet()
{
    FreeBitmaps();
}

void CTileSet::Sync()
{
    for (int i=0; i<bitmaps.size(); i++)
        ;
}

void CTileSet::FreeBitmaps()
{
    for (int i=0; i<bitmaps.size(); i++)
    {
        delete bitmaps[i];
        bitmaps[i]=NULL;
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
    FreeBitmaps();

    bAltered.resize(pVsp->NumTiles());
    bitmaps.resize(pVsp->NumTiles());

    for (int i=0; i<pVsp->NumTiles(); i++)
    {
        bitmaps[i]=pGraphfactory->CreateImage(pVsp->GetTile(i));
    }

    return true;
}

bool CTileSet::Save(const char* fname)
{
    return false;
}