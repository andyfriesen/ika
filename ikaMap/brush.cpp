
#include "brush.h"

Brush::Brush() {
    // Temp hack
    /*tiles.Resize(2, 2);
    tiles(0, 0).index = 104;
    tiles(0, 0).mask = true;
    tiles(0, 1).index = 112;
    tiles(0, 1).mask = true;
    tiles(1, 0).mask = false;
    tiles(1, 1).mask = false;*/
}

Brush::Brush(const Matrix<uint>& t) {
    tiles.Resize(t.Width(), t.Height());

    for (uint y = 0; y < Height(); y++) {
        for (uint x = 0; x < Width(); x++) {
            tiles(x, y).index = t(x, y);
            tiles(x, y).obstruct = 0;
            tiles(x, y).mask = true;
        }
    }
}

Brush::Brush(const Map::Layer& layer, const Rect& r) {
    uint width = min(int(r.left + layer.Width()), r.right);
    uint height = min(int(r.top + layer.Height()), r.bottom);

    tiles.Resize(width, height);

    for (uint y = 0; y < height; y++) {
        for (uint x = 0; x < width; x++) {
            Tile& t = tiles(x, y);
            t.index = layer.tiles(x + r.left, y + r.top);
            t.obstruct = layer.obstructions(x, y);
            t.mask = false;
        }
    }
}

Brush::Brush(const Map::Layer& layer, const Rect& r, const Matrix<bool>& mask) {
    uint width = min(int(r.left + layer.Width()), r.right);
    uint height = min(int(r.top + layer.Height()), r.bottom);

    tiles.Resize(width, height);

    for (uint y = 0; y < height; y++) {
        for (uint x = 0; x < width; x++) {
            Tile& t = tiles(x, y);
            if (mask(x, y)) {
                t.mask = true;

            } else {
                t.index = layer.tiles(x + r.left, y + r.top);
                t.obstruct = layer.obstructions(x, y);
                t.mask = false;
            }
        }
    }
}
