#if 0
#pragma once

#include <set>
#include "debug.h"
#include "common/utility.h"
#include "imagebank.h"
#include "document.h"

// I cannot sufficiently emphasize how much I hate the name 'VSP'. -- andy
struct VSP;

namespace iked {

    struct TileSet : ImageDocumentResource {
        TileSet(int width, int height, int numTiles);
        TileSet(VSP* v);
        ~TileSet();

	// Resource
	virtual TileSet* clone();
	// --

        void save(const std::string& fname);

        inline const VSP& getVSP() { return *vsp; }

        inline int getCurTile() { return curTile; }
        void setCurTile(int t);

    protected:
        // ImageDocumentResource
        virtual int doGetCount();
        virtual int doGetWidth();
        virtual int doGetHeight();
        virtual const Canvas& doGetCanvas(int index);
        virtual void doSetCanvas(const Canvas& canvas, int index);
        virtual void doInsert(const Canvas& canvas, int position);
        virtual void doRemove(int position);
        // --
    private:      
        DECLARE_INVARIANT();

        VSP* vsp;

        // I don't think this is the Right Thing.  UI stuff doesn't really belong here.
        // However, this has the advantage of making the VSP and map views
        // interoperate with minimum fuss.
        int  curTile;
    };

}
#endif
