
#ifndef TILEEDIT_H
#define TILEEDIT_H

#include "imageview.h"

class CMainWnd;
class CTileSet;

class CTileEdit : CImageView
{
    CMainWnd* pParent;
    CTileSet* pTileset;
    int nTile;

public:
    CTileEdit(CMainWnd* parent,CTileSet* tileset,int tileidx);
};

#endif