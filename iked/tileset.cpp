#include "tileset.h"
#include "vsp.h"
#include "pixel_matrix.h"
#include "graph.h"

CTileSet::CTileSet()
: pVsp(0),nCurtile(0)
{
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
    pVsp->Save(fname);
    return true;
}

const CPixelMatrix& CTileSet::Get(int tileidx)
{
    return pVsp->GetTile(tileidx);
}

void CTileSet::SetImage(const CPixelMatrix& newtile,int tileidx)
{
    pVsp->GetTile(tileidx)=newtile;
}

int CTileSet::Count() const
{
    return pVsp?pVsp->NumTiles() : 0;
}

int CTileSet::Width() const
{
    return pVsp?pVsp->Width() : 0;
}

int CTileSet::Height() const
{
    return pVsp?pVsp->Height():0;
}

void CTileSet::SetCurTile(int t)
{
    if (t<0 || t>=pVsp->NumTiles()) return;
    nCurtile=t;
}
