#if 0
#pragma once

#include "imageview.h"

class MainWindow;
class TileSet;

class CTileEdit : CImageView
{
    MainWindow* pParent;
    TileSet* pTileset;
    int nTile;

public:
    CTileEdit(MainWindow* parent, TileSet* tileset, int tileidx);
};
#endif
