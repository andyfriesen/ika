
#include "brushstate.h"
#include "executor.h"
#include "mapview.h"
#include "tileset.h"
#include "tilesetview.h"
#include "command.h"

BrushState::BrushState(Executor* e)
    : EditState(e, "Brush")
    , _oldX(-1)
    , _oldY(-1)
{}

void BrushState::OnMouseDown(wxMouseEvent& event) {
    int x = event.m_x;
    int y = event.m_y;
    GetMapView()->ScreenToTile(x, y);

    HandleCommand(new PasteBrushCommand(x, y, GetCurLayerIndex(), GetExecutor()->GetCurrentBrush()));
}

void BrushState::OnMouseUp(wxMouseEvent& event) {
}

void BrushState::OnMouseMove(wxMouseEvent& event) {
    int x = event.m_x;
    int y = event.m_y;

    GetMapView()->ScreenToTile(x, y);
    if (x == _oldX && y == _oldY) {
        return;
    }

    if (event.LeftIsDown()) {
        HandleCommand(new PasteBrushCommand(x, y, GetCurLayerIndex(), GetExecutor()->GetCurrentBrush()));
    }

    _oldX = x;
    _oldY = y;
    GetMapView()->Refresh();
}

void BrushState::OnRenderCurrentLayer() {
    if (_oldX == -1 || _oldY == -1) {
        return;
    }

    GetMapView()->RenderBrush(_oldX, _oldY);
}

void BrushState::OnTilesetViewRender() {
    //GetExecutor()->GetTilesetView()->SetBrushRender();
    TilesetView* tsv = GetExecutor()->GetTilesetView();

    Tileset* ts = GetExecutor()->GetTileset();
    int tileWidth  = ts->Width()  + (tsv->IsPadded() ? 1 : 0);
    int tileHeight = ts->Height() + (tsv->IsPadded() ? 1 : 0);

    Rect selected = tsv->GetBrushSelection();
    int ywin = tsv->GetYWin();
    tsv->DrawSelectRect(selected.left * tileWidth, selected.top * tileHeight - ywin, tileWidth * (selected.Width() + 1), tileHeight * (selected.Height() + 1), RGBA(127, 255, 255));
}
