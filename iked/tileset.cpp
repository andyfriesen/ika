#include "tileset.h"
#include "vsp.h"
#include "pixel_matrix.h"
#include "graph.h"

CTileSet::CTileSet()
: pVsp(0),nCurtile(0)
{
}

CTileSet::~CTileSet()
{
    FreeBitmaps();
}

void CTileSet::Sync()
{
    for (std::set<int>::iterator i=altered.begin(); i!=altered.end(); i++)
    {
        bitmaps[*i]->Update(pVsp->GetTile(*i));
    }

    altered.clear();
}

void CTileSet::SyncAll()
{
    FreeBitmaps();

    bitmaps.resize(pVsp->NumTiles());

    for (int i=0; i<pVsp->NumTiles(); i++)
    {
        bitmaps[i]=new CImage(pVsp->GetTile(i));
    }
}

void CTileSet::FreeBitmaps()
{
    for (int i=0; i<bitmaps.size(); i++)
    {
        delete bitmaps[i];
    }

    bitmaps.clear();
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

const CPixelMatrix& CTileSet::GetTile(int tileidx)
{
    return pVsp->GetTile(tileidx);
}

void CTileSet::SetTile(int tileidx,const CPixelMatrix& newtile)
{
    if (tileidx<0 || tileidx>=pVsp->NumTiles())
        return;

    pVsp->GetTile(tileidx)=newtile;
    altered.insert(tileidx);
}

CImage& CTileSet::GetImage(int tileidx) const
{
    if (tileidx<0 || tileidx>=bitmaps.size())
        return *bitmaps[0];

    return *bitmaps[tileidx];
}

int CTileSet::NumTiles() const
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
