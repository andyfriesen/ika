#include "tileset.h"
#include "common/vsp.h"
#include "common/Canvas.h"
#include "video.h"

Tileset::Tileset()
    : pVsp(0)
{
}

Tileset::Tileset(VSP* v)
    : pVsp(v)
{
    SyncAll();
}

Tileset::~Tileset() {
    delete pVsp;
}

bool Tileset::Load(const std::string& fileName) {
    VSP* pNewvsp = new VSP;

    if(!pNewvsp->Load(fileName)) {
        delete pNewvsp;
        return false;
    }

    delete pVsp;
    pVsp = pNewvsp;

    SyncAll();

    return true;
}

bool Tileset::Save(const std::string& fileName) {
    pVsp->Save(fileName);
    return true;
}

void Tileset::New(int width, int height) {
    if (!pVsp) {
        pVsp = new VSP;
    }

    pVsp->New(width, height, 0);
    SyncAll();
}

Canvas& Tileset::Get(uint tileidx) {
    return pVsp->GetTile(tileidx);
}

void Tileset::SetImage(const Canvas& newtile, uint tileidx) {
    pVsp->GetTile(tileidx)=newtile;
}

uint Tileset::Count() const {
    return pVsp ? pVsp->NumTiles() : 0;
}

int Tileset::Width() const {
    return pVsp ? pVsp->Width() : 0;
}

int Tileset::Height() const {
    return pVsp ? pVsp->Height() : 0;
}

void Tileset::AppendTile() {
    pVsp->AppendTile();
    SyncAll();
}

void Tileset::AppendTile(Canvas& c) {
    pVsp->AppendTile();
    pVsp->PasteTile(c, pVsp->NumTiles() - 1);
    SyncAll();
}

void Tileset::InsertTile(uint pos) {
    pVsp->InsertTile(pos);
    SyncAll();
}

void Tileset::InsertTile(uint pos, Canvas& c) {
    pVsp->InsertTile(pos);
    pVsp->PasteTile(c, pos);
    SyncAll();
}

void Tileset::DeleteTile(uint pos) {
    pVsp->DeleteTile(pos);
    SyncAll();
}

std::vector<VSP::AnimState>& Tileset::GetAnim() {
    return pVsp->vspAnim;
}
