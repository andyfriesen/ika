#if 0
#pragma once

#include "imageview.h"

class MainWindow;
class Tileset;

class CTileEdit : CImageView
{
    MainWindow* pParent;
    Tileset* pTileset;
    int nTile;

public:
    CTileEdit(MainWindow* parent, Tileset* tileset, int tileidx);
};
#endif
