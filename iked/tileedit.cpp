
#include "tileedit.h"
#include "graph.h"
#include "main.h"

#include "tileset.h"

CTileEdit::CTileEdit(CMainWnd* parent,CTileSet* tileset,int tileidx)
:   CImageView(parent,&tileset->Get(tileidx)),           // base class constructor
    pParent(parent),pTileset(tileset),nTile(tileidx)
{
}
