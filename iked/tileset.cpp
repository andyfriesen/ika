
#include <cassert>
#include "tileset.h"
#include "common/vsp.h"
#include "common/Canvas.h"
#include "graph.h"

namespace iked {

    Tileset::Tileset(int width, int height, int numTiles)
        : curTile(0)
    {
        vsp = new VSP;
        vsp->New(width, height, numTiles);
        invariant();
    }

    Tileset::Tileset(VSP* v) 
        : curTile(0)
        , vsp(v)
    {
        invariant();
    }

    Tileset::~Tileset() {
        invariant();
        delete vsp;
    }

    Tileset* Tileset::clone() {
        invariant();
	throw std::runtime_error("Tileset::clone Not yet implemented");
    }

    int Tileset::doGetCount() {
        invariant();
        return vsp->NumTiles();
    }

    int Tileset::doGetWidth() {
        invariant();
        return vsp->Width();
    }

    int Tileset::doGetHeight() {
        invariant();
        return vsp->Height();
    }

    const Canvas& Tileset::doGetCanvas(int tileidx) {
        invariant();
        return vsp->GetTile(tileidx);
    }

    void Tileset::doSetCanvas(const Canvas& newtile, int tileidx) {
        vsp->GetTile(tileidx) = newtile;
        invariant();
    }

    void Tileset::doInsert(const Canvas& canvas, int position) {
        vsp->InsertTile(position);
        vsp->PasteTile(canvas, position);
        invariant();
    }

    void Tileset::doRemove(int pos) {
        vsp->DeleteTile(pos);
        pos = max(pos, getCount() - 1);
        invariant();
    }

    void Tileset::save(const std::string& fname) {
        invariant();
        vsp->Save(fname);
    }

    void Tileset::setCurTile(int t) {
        if (0 <= t && t < getCount()) {
            curTile = t;
        }
    }

    DEBUG_BLOCK(
        void Tileset::invariant() {
            assert(vsp != 0);
            assert(0 <= curTile && curTile < getCount());
        }
    )

}
