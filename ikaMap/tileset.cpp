#include "tileset.h"
#include "vsp.h"
#include "Canvas.h"
#include "video.h"

TileSet::TileSet()
: pVsp(0), nCurtile(0)
{
}

TileSet::~TileSet()
{
    delete pVsp;
}

bool TileSet::Load(const std::string& fileName)
{
    VSP* pNewvsp = new VSP;

    if(!pNewvsp->Load(fileName))
    {
        delete pNewvsp;
        return false;
    }

    delete pVsp;
    pVsp = pNewvsp;

    SyncAll();

    return true;
}

bool TileSet::Save(const std::string& fileName)
{
    pVsp->Save(fileName);
    return true;
}

void TileSet::New(int width, int height)
{
    if (!pVsp)
        pVsp = new VSP;

    pVsp->New(width, height, 0);
    SyncAll();
}

Canvas& TileSet::Get(int tileidx)
{
    return pVsp->GetTile(tileidx);
}

void TileSet::SetImage(const Canvas& newtile, int tileidx)
{
    pVsp->GetTile(tileidx)=newtile;
}

uint TileSet::Count() const
{
    return pVsp?pVsp->NumTiles() : 0;
}

int TileSet::Width() const
{
    return pVsp?pVsp->Width() : 0;
}

int TileSet::Height() const
{
    return pVsp?pVsp->Height():0;
}

void TileSet::AppendTile()
{
    pVsp->AppendTile();
    SyncAll();
}

void TileSet::AppendTile(Canvas& c)
{
    pVsp->AppendTile();
    pVsp->PasteTile(c, pVsp->NumTiles() - 1);
    SyncAll();
}

void TileSet::InsertTile(uint pos)
{
    pVsp->InsertTile(pos);
    SyncAll();
}

void TileSet::InsertTile(uint pos, Canvas& c)
{
    pVsp->InsertTile(pos);
    pVsp->PasteTile(c, pos);
    SyncAll();
}

void TileSet::DeleteTile(uint pos)
{
    pVsp->DeleteTile(pos);
    SyncAll();
}

void TileSet::SetCurTile(uint t)
{
    if (t < 0 || t >= pVsp->NumTiles()) return;
    nCurtile = t;
}
