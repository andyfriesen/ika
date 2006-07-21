
#include "copypastestate.h"
#include "command.h"
#include "executor.h"
#include "mapview.h"
#include "common/map.h"
#include "tileset.h"

/*
 * Behaviour:
 *     shift+left-clicks anywhere
 *         Add state: allow the user to drag a rect, and add it to the current selection.
 *     ctrl+left-click anywhere
 *         Subtract state: drag a rect.  Remove that from the selection.
 *     left-click somewhere not selected
 *         Add state.
 *     left-click somewhere selected
 *         Switch to "move" state: allow the user to drag the selection around to move it.
 *     right-click anywhere for any reason
 *         TEMP HACK: copy the current selection to the editor's active brush.
 */

CopyPasteState::CopyPasteState(Executor* e)
    : EditState(e, "Selection")
    , _state(NoneState)
    , _dragging(false)
    , _selX(0)
    , _selY(0)
    , _dragX(0)
    , _dragY(0)
    , _offsetX(0)
    , _offsetY(0)
{
    _tiles.tiles.Resize(0, 0);
}

void CopyPasteState::OnMouseDown(wxMouseEvent& event) {
    // const Tileset* ts = GetTileset();            // Unused.
    // const Map::Layer* curLayer = GetCurLayer();  // Unused.
    MapView* mv = GetMapView();

    int x = event.GetX();
    int y = event.GetY();
    mv->ScreenToTile(x, y);

    if (event.LeftDown()) {
        _selection = Rect(x, y, x, y);
        if (event.ShiftDown()) {
            _state = AddState;
        } else if (event.ControlDown()) {
            _state = SubtractState;
        } else {
            return;
        }

    } else if (event.RightDown()) {
        GetExecutor()->SetCurrentBrush(_tiles);
    }
}

void CopyPasteState::OnMouseUp(wxMouseEvent& event) {
    Map::Layer* lay = GetCurLayer();

    if (_state == AddState) {
        _state = NoneState;
        _selection.Normalize();
        
        // Figure out the minimum rect that can contain the union 
        // of the dragged rect and the existing selection.
        Rect newClip;
        if (_tiles.tiles.Empty()) {
            newClip = _selection;
        } else {
            newClip.left = min(_selection.left, _selX);
            newClip.right = max(_selection.right, _selX + int(_tiles.Width()));
            newClip.top = min(_selection.top, _selY);
            newClip.bottom = max(_selection.bottom, _selY + int(_tiles.Height()));
        }

        int ofsX = _selX - newClip.left;
        int ofsY = _selY - newClip.top;

        // FIXME: this is inefficient.

        Brush newBrush;
        newBrush.tiles.Resize(newClip.Width(), newClip.Height());

        // Copy the old bit over.
        for (uint y = 0; y < _tiles.Height(); y++) {
            for (unsigned int x = 0; x < _tiles.Width(); x++) {
                //Brush::Tile& t = newBrush.tiles(x + ofsX, y + ofsY);  // Unused.
                //Brush::Tile& u = _tiles.tiles(x, y);					// Unused.
                newBrush.tiles(x + ofsX, y + ofsY) = _tiles.tiles(x, y);
            }
        }

        // Add the new stuff.
		assert(_selection.top > 0 && _selection.right > 0 &&
			   _selection.bottom > 0 && _selection.left > 0);
        for (unsigned int y = _selection.top; y < _selection.bottom; y++) {
            for (unsigned int x = _selection.left; x < _selection.right; x++) {
                Brush::Tile& t = newBrush.tiles(x - newClip.left, y - newClip.top);
                t.index = lay->tiles(x, y);
                t.mask = true;
            }
        }

        _tiles = newBrush;
        _selX = newClip.left;
        _selY = newClip.top;

        GetMapView()->Refresh();

    } else if (_state == SubtractState) {
        _state = NoneState;

        // Easier to remove than add.  Find the overlap, and flip a few mask bits.
        _selection.Normalize();
        int xofs = _selection.left - _selX;
        int yofs = _selection.top - _selY;

        for (int y = 0; y < _selection.Height(); y++) {
            for (int x = 0; x < _selection.Width(); x++) {
                int sx = x + xofs;
                int sy = y + yofs;
                if (sx == clamp<int>(sx, 0, _tiles.Width()) &&
                    sy == clamp<int>(sy, 0, _tiles.Height())
                ) {
                    _tiles.tiles(sx, sy).mask = false;
                }
            }
        }

        GetMapView()->Refresh();
    }
}

void CopyPasteState::OnMouseMove(wxMouseEvent& event) {
    if (_state != NoneState) {
        _selection.right = event.GetX();
        _selection.bottom = event.GetY();

        GetMapView()->ScreenToTile(_selection.right, _selection.bottom);

        if (_selection.right >= _selection.left)    _selection.right++;
        if (_selection.bottom >= _selection.top)    _selection.bottom++;

        GetMapView()->Refresh();
        // Update the other corner of the selection rect.
    }
}

void CopyPasteState::OnRenderCurrentLayer() {
    const Tileset* ts = GetTileset();
    const Map::Layer* curLayer = GetCurLayer();
    MapView* mv = GetMapView();
    
    if (!_tiles.tiles.Empty()) {
        int xofs = _selX;
        int yofs = _selY;
        mv->TileToScreen(xofs, yofs);
        mv->RenderBrushOutline(_tiles, xofs, yofs);
    }

    if (_state != NoneState) {
        // Draw the current selection rectangle
        int x = _selection.left * ts->Width() - mv->GetXWin() + curLayer->x;
        int y = _selection.top * ts->Height() - mv->GetYWin() + curLayer->y;
        int w = _selection.Width() * ts->Width();
        int h = _selection.Height() * ts->Height();

        mv->GetVideo()->DrawRectFill(x, y, w, h, RGBA(255, 255, 255, 128));
        mv->GetVideo()->DrawRect    (x, y, w, h, RGBA(0, 255, 255));
    }
}

void CopyPasteState::OnEndState() {
    Paste();
}

void CopyPasteState::Paste() {
    if (_tiles.Empty()) return; // :P

    HandleCommand(new PasteBrushCommand(_selX, _selY, GetCurLayerIndex(), _tiles));
    _tiles.tiles.Resize(0, 0);
}
