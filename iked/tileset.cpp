#include "tileset.h"
#include "vsp.h"
#include "Canvas.h"
#include "graph.h"

CTileSet::CTileSet()
: pVsp(0), nCurtile(0)
{
}

bool CTileSet::Load(const char* fname)
{
    VSP* pNewvsp = new VSP;

    if(!pNewvsp->Load(fname))
    {
        delete pNewvsp;
        return false;
    }

    delete pVsp;
    pVsp = pNewvsp;

    SyncAll();

    return true;
}

bool CTileSet::Save(const char* fname)
{
    pVsp->Save(fname);
    return true;
}

void CTileSet::New(int width, int height)
{
    if (!pVsp)
        pVsp = new VSP;

    pVsp->New(width, height, 0);
    SyncAll();
}

Canvas& CTileSet::Get(int tileidx)
{
    return pVsp->GetTile(tileidx);
}

void CTileSet::SetImage(const Canvas& newtile, int tileidx)
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

void CTileSet::AppendTile()
{
    pVsp->AppendTile();
    SyncAll();
}

void CTileSet::AppendTile(Canvas& c)
{
    pVsp->AppendTile();
    pVsp->PasteTile(c, pVsp->NumTiles() - 1);
    SyncAll();
}

void CTileSet::InsertTile(uint pos)
{
    pVsp->InsertTile(pos);
    SyncAll();
}

void CTileSet::InsertTile(uint pos, Canvas& c)
{
    pVsp->InsertTile(pos);
    pVsp->PasteTile(c, pos);
    SyncAll();
}

void CTileSet::DeleteTile(uint pos)
{
    pVsp->DeleteTile(pos);
    SyncAll();
}

void CTileSet::SetCurTile(uint t)
{
    if (t < 0 || t >= pVsp->NumTiles()) return;
    nCurtile = t;
}
