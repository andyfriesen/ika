
#include "brush.h"

Brush::Brush() {
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

uint Brush::Width() const {
    return tiles.Width();
}

uint Brush::Height() const {
    return tiles.Height();
}
