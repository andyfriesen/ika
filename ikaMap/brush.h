
#ifndef BRUSH_H
#define BRUSH_H

#include "common/matrix.h"
#include "common/types.h"

#include "common/map.h"

// TODO: multi-layer brushes
struct Brush {
    /* A brush is essentially an arbitrary grouping of tiles.
     * The user uses these to dump groups of tiles on the map all in one go,
     * sort of like a super-copy/paste
     *
     * They can be edited in basically the same way as a map layer, but
     * have a "mask" (see-through areas).
     *
     * TODO: think about brushes that span layers and include zone information.
     */

    Brush();
    Brush(const Map::Layer& layer, const Rect& r);
    Brush(const Map::Layer& layer, const Rect& r, const Matrix<bool>& mask);

    uint Width() const;
    uint Height() const;

    struct Tile {
        uint index;
        u8   obstruct;
        bool mask;          ///< If true, this tile is excluded from paste operations.
    };

    Matrix<Tile> tiles;
};

#endif
