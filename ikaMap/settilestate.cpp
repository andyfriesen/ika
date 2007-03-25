
#include "settilestate.h"
#include "wxinc.h"
#include "executor.h"
#include "mapview.h"
#include "tilesetview.h"
#include "common/map.h"
#include "tileset.h"
#include "command.h"
#include "common/utility.h"
#include "video.h"
#include "wxinc.h"

#include "common/log.h"

#undef USE_GROUP_JUJU

TilesetState::TilesetState(Executor* e)
    : EditState(e, "Pencil")
    , _oldX(0)
    , _oldY(0)
    , _curX(0)
    , _curY(0)
    , _curGroup(0)
{}

void TilesetState::OnRender() {
}

void TilesetState::OnRenderCurrentLayer() {
    MapView* mv = GetMapView();

    int w = GetTileset()->Width();
    int h = GetTileset()->Height();

    mv->GetVideo()->DrawSelectRect(_oldX * w - mv->GetXWin(), _oldY * h - mv->GetYWin(), w, h, RGBA(255, 192, 192, 255));
}

void TilesetState::OnTilesetViewRender() {
    TilesetView* tsv = GetExecutor()->GetTilesetView();

    Tileset* ts = GetExecutor()->GetTileset();
    int x;
    int y;
    int tileWidth  = ts->Width()  + (tsv->IsPadded() ? 1 : 0);
    int tileHeight = ts->Height() + (tsv->IsPadded() ? 1 : 0);

    tsv->TileToPoint(GetExecutor()->GetCurrentTile(), x, y);

    tsv->DrawSelectRect(x - 1, y - 1, ts->Width() + 1, ts->Height() + 1, RGBA(127, 255, 255));
}

void TilesetState::OnMouseDown(wxMouseEvent& event) {
    int x = event.m_x;
    int y = event.m_y;

    if (event.LeftDown() && !event.ShiftDown()) {
        // Left Click to set a single tile
        SetTile(x, y);

    } else if (event.LeftDown() && event.ShiftDown()) {
        // Shift + Left Click to make the tile under the cursor the current one
        GetMapView()->ScreenToTile(x, y);

        SetCurTile(GetCurLayer()->tiles(x, y));

    }
}

void TilesetState::OnMouseUp(wxMouseEvent& event) {
    // odd that the mouse button is able to go up without first going down

#if defined(USE_GROUP_JUJU)
        if (_curGroup) {
            if (_curGroup->GetCount() == 1) {
                GetExecutor()->AddCommand(_curGroup->GetIndex(0));
                delete _curGroup;
            } else {
                GetExecutor()->AddCommand(_curGroup);
            }
        }
        _curGroup = 0;
#endif
}

void TilesetState::OnMouseMove(wxMouseEvent& event) {
    int x = event.m_x;
    int y = event.m_y;
    GetMapView()->ScreenToTile(x, y);

    if (x == _oldX && y == _oldY)   return;
    if (x < 0 || y < 0)             return;

    _oldX = min(x, GetCurLayer()->Width());
    _oldY = min(y, GetCurLayer()->Height());

    if (event.LeftIsDown() && !event.ShiftDown()) {
        SetTile(event.m_x, event.m_y);

    }

    GetMapView()->Refresh();
}

void TilesetState::OnMouseWheel(wxMouseEvent& event) {
    /*
    int i = event.GetWheelRotation() / event.GetWheelDelta();
    uint curTile = GetCurTile();

    // _curTile is unsigned, so we need to check for wraparound before actually doing the math.
    // So we just add the tile count, then subtract it afterwards, if needed
    curTile += GetTileset()->Count();
    curTile += i;

    // If we've gone over, wrap-around.
    while (curTile >= GetTileset()->Count())
        curTile -= GetTileset()->Count();

    SetCurTile(curTile);
    */
}

void TilesetState::SetTile(int x, int y) {
    GetMapView()->ScreenToTile(x, y);

    if (GetCurLayer()->tiles(x, y) == GetCurTile()) {
        // Avoid flooding the undo buffer with commands that don't actually do anything.
        return;
    }

    Command* cmd = new SetTileCommand(x, y, GetCurLayerIndex(), GetCurTile());
#if defined(USE_GROUP_JUJU)
    // Create a new group if we need to
    if (!_curGroup) {
        _curGroup = new CompositeCommand();
    }

    // naughty.  execute the command, but don't put it on the undo stack
    // when the mouse button is released, we add the list of commands all in one go.
    cmd->Do(GetExecutor());
    _curGroup->Append(cmd);
#else
    HandleCommand(cmd);
#endif
}

uint TilesetState::GetCurTile() const {
    return GetExecutor()->GetCurrentTile();
}

void TilesetState::SetCurTile(uint newTile) {
    GetExecutor()->SetCurrentTile(newTile);
}

