
#include <cassert>
#include "tileset.h"
#include "common/vsp.h"
#include "common/Canvas.h"
#include "graph.h"

namespace iked {

    TileSet::TileSet(int width, int height, int numTiles)
        : curTile(0)
    {
        vsp = new VSP;
        vsp->New(width, height, numTiles);
        invariant();
    }

    TileSet::TileSet(VSP* v) 
        : curTile(0)
        , vsp(v)
    {
        invariant();
    }

    TileSet::~TileSet() {
        invariant();
        delete vsp;
    }

    TileSet* TileSet::clone() {
        invariant();
	throw std::runtime_error("TileSet::clone Not yet implemented");
    }

    int TileSet::doGetCount() {
        invariant();
        return vsp->NumTiles();
    }

    int TileSet::doGetWidth() {
        invariant();
        return vsp->Width();
    }

    int TileSet::doGetHeight() {
        invariant();
        return vsp->Height();
    }

    const Canvas& TileSet::doGetCanvas(int tileidx) {
        invariant();
        return vsp->GetTile(tileidx);
    }

    void TileSet::doSetCanvas(const Canvas& newtile, int tileidx) {
        vsp->GetTile(tileidx) = newtile;
        invariant();
    }

    void TileSet::doInsert(const Canvas& canvas, int position) {
        vsp->InsertTile(position);
        vsp->PasteTile(canvas, position);
        invariant();
    }

    void TileSet::doRemove(int pos) {
        vsp->DeleteTile(pos);
        pos = max(pos, getCount() - 1);
        invariant();
    }

    void TileSet::save(const std::string& fname) {
        invariant();
        vsp->Save(fname);
    }

    void TileSet::setCurTile(int t) {
        if (0 <= t && t < getCount()) {
            curTile = t;
        }
    }

    DEBUG_BLOCK(
        void TileSet::invariant() {
            assert(vsp != 0);
            assert(0 <= curTile && curTile < getCount());
        }
    )

}
