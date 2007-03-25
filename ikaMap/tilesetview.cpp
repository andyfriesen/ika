#include <cassert>

#include "tilesetview.h"

#include "editstate.h"
#include "mainwindow.h"
#include "tileset.h"
#include "common/log.h"

#include "common/utility.h"
#include "common/matrix.h"

#include "common/types.h"

BEGIN_EVENT_TABLE(TilesetView, VideoFrame)
    EVT_SIZE(TilesetView::OnSize)
    EVT_SCROLLWIN(TilesetView::OnScroll)
    EVT_PAINT(TilesetView::OnPaint)
    EVT_LEFT_DOWN(TilesetView::OnMouseDown)
    EVT_RIGHT_DOWN(TilesetView::OnMouseDown)
    EVT_LEFT_UP(TilesetView::OnMouseUp)
    EVT_RIGHT_UP(TilesetView::OnMouseUp)
    EVT_MOTION(TilesetView::OnMouseMove)
END_EVENT_TABLE()

TilesetView::TilesetView(Executor* e, wxWindow* parent)
    : VideoFrame(parent)
    , _executor(e)
    , _ywin(0)
    , _pad(true)
    , _drawBrushSelection(true)
{}

TilesetView::~TilesetView()
{}

void TilesetView::OnSize(wxSizeEvent& event) {
    UpdateScrollBars();
    VideoFrame::OnSize(event);
}

void TilesetView::OnScroll(wxScrollWinEvent& event) {
    struct Local {
        static void HandleEvent(int& val, int min, int max, int page, int pos, wxEventType et) {
            // Can't switch/case this. :\
            // Luckily, my regex-fu is developed enough that I could convert this
            // from a switch/case to what you see in about 2 seconds. :D
                 if (et == wxEVT_SCROLLWIN_TOP)          { val = min;       }
            else if (et == wxEVT_SCROLLWIN_BOTTOM)       { val = max;       }
            else if (et == wxEVT_SCROLLWIN_LINEUP)       { val--;           }
            else if (et == wxEVT_SCROLLWIN_LINEDOWN)     { val++;           }
            else if (et == wxEVT_SCROLLWIN_PAGEUP)       { val -= page;     }
            else if (et == wxEVT_SCROLLWIN_PAGEDOWN)     { val += page;     }
            else if (et == wxEVT_SCROLLWIN_THUMBTRACK)   { val = pos;       }
            else if (et == wxEVT_SCROLLWIN_THUMBRELEASE) { val = pos;       }

            val = clamp(val, min, max);
        }
    };

    const Tileset* ts = _executor->GetTileset();

    wxSize size = GetSize();

    Local::HandleEvent(
        _ywin,
        0,
        NumTileRows() * (ts->Height() + (_pad ? 1 : 0)) - LogicalHeight(),
        LogicalHeight() - size.GetHeight(),
        event.GetPosition(),
        event.GetEventType()
    );

    UpdateScrollBars();
    Render();
    ShowPage();
}

void TilesetView::OnPaint(wxPaintEvent&) {
    wxPaintDC dc(this);

    Render();
    ShowPage();
}

void TilesetView::OnMouseDown(wxMouseEvent& event) {
    _dragging = true;
    _drawBrushSelection = true;

    uint index = PointToTile(event.m_x, event.m_y);
    _executor->SetCurrentTile(index);

    // Set tileset view rectangle origin.
    int x = event.m_x;
    int y = event.m_y;

    PointToCoords(x, y);

    _originX = x;
    _originY = y;

    SetSelection(x, y, 1, 1);

    Render();
    ShowPage();
}

void TilesetView::OnMouseMove(wxMouseEvent& event) {
    if (_dragging) {
        // Update the rectangle.
        int x = event.m_x;
        int y = event.m_y;

        PointToCoords(x, y);

        // Check if new x is less than origin x.
        if (x < _originX) {
            _selected.left = x;
            _selected.right = _originX;
        } else {
            _selected.left = _originX;
            _selected.right = x;
        }

        // Check if new y is less than origin y.
        if (y < _originY) {
            _selected.top = y;
            _selected.bottom = _originY;

        } else {
            _selected.top = _originY;
            _selected.bottom = y;
        }



        Render();
        ShowPage();
    }
}

void TilesetView::OnMouseUp(wxMouseEvent& event) {
    _dragging = false;

    VideoFrame::OnMouseEvent(event);

    // Grab tiles within selection area.
    _selected.Normalize();
    int w = _selected.Width() + 1;
    int h = _selected.Height() + 1;

    Tileset* ts = _executor->GetTileset();
    int tileWidth = ts->Width() + (_pad ? 1 : 0);
    int tileHeight = ts->Height() + (_pad ? 1 : 0);

    Brush newBrush;
    newBrush.tiles.Resize(w, h);

    for (int y = 0; y < h; y ++) {
        for (int x = 0; x < w; x ++) {
            Brush::Tile& t = newBrush.tiles(x, y);
            t.index = PointToTile((_selected.left + x) * tileWidth, (_selected.top + y) * tileHeight - _ywin);
            t.mask = true;
        }
    }

    _executor->SetCurrentBrush(newBrush);
    _drawBrushSelection = true;
}

void TilesetView::SetSelection(int x, int y, int w, int h) {
    _selected.left = x;
    _selected.right = x + w - 1;
    _selected.top = y;
    _selected.bottom = y + h - 1;
}

void TilesetView::Render() {
    SetCurrent();
    Clear();

    Tileset* ts = _executor->GetTileset();
    assert(ts != 0);

    if (ts->Count() == 0) {
        return;
    }

    // Size of the area that we're to fill
    int clientWidth  = LogicalWidth();
    int clientHeight = LogicalHeight();

    // Number of pixels reserved for each tile.
    int tileWidth  = ts->Width()  + (_pad ? 1 : 0);
    int tileHeight = ts->Height() + (_pad ? 1 : 0);

    // Number of tiles high and wide to be drawn.
    uint rowWidth = max(1, clientWidth  / tileWidth);
    uint numCols  = max(1, clientHeight / tileHeight + 2);

    // Index of the tile to be drawn in the upperleft corner.
    uint curTile  = (_ywin / tileHeight) * rowWidth;

    // Pixel position to draw the upperleft tile.
    uint startX = _pad ? 1 : 0;
    int startY = -(_ywin % tileHeight);

    // Position to draw the current tile (as we loop)
    uint curX = startX;
    uint curY = startY;

    for (uint y = 0; y < numCols; y++) {
        for (uint x = 0; x < rowWidth; x++) {
            Image& tile = ts->GetImage(curTile);

            Blit(tile, curX, curY);

            curX += tileWidth;
            curTile++;

            if (curTile >= ts->Count()) {
                goto breakLoop;
            }
        }
        curY += tileHeight;
        curX = startX;
     }

breakLoop:;

    // Call the current edit state's tilesetview render function.
    _executor->GetEditState()->OnTilesetViewRender();
}

void TilesetView::UpdateScrollBars() {
    const Tileset* ts = _executor->GetTileset();

    //uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);  // Unused.
    uint tileHeight = ts->Height() + (_pad ? 1 : 0);

    SetScrollbar(wxVERTICAL, _ywin, LogicalHeight(), NumTileRows() * tileHeight);
}

void TilesetView::OnTilesetChange(const TilesetEvent& event) {
    SetSelection(0, 0, 1, 1);

    Refresh();
}

void TilesetView::OnCurrentTileChange(uint newTile) {
    Refresh();
    _executor->SetStatusBar(va("Tile: %3i", newTile), 2);
}

void TilesetView::OnCurrentBrushChange(const Brush& newBrush) {
    _drawBrushSelection = false;
}

uint TilesetView::PointToTile(int x, int y) const {
    const Tileset* ts = _executor->GetTileset();

    y += _ywin;

    uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
    uint tileHeight = ts->Height() + (_pad ? 1 : 0);

    x /= tileWidth;
    y /= tileHeight;

    uint index = y * TilesPerRow() + x;

    return clamp<uint>(index, 0, ts->Count() - 1);
}

void TilesetView::TileToPoint(uint index, int& x, int& y) const {
    const Tileset* ts = _executor->GetTileset();

    if (index > ts->Count()) {
        x = y = -1;
    } else {
        uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
        uint tileHeight = ts->Height() + (_pad ? 1 : 0);
        uint tilesPerRow = TilesPerRow();

        if (tilesPerRow == 0) {
            x = y = -1;
            return;
        }

        y = (index / tilesPerRow) * tileHeight - _ywin;
        x = _pad + (index % tilesPerRow) * tileWidth;
    }
}

void TilesetView::PointToCoords(int& x, int& y) const {
    const Tileset* ts = _executor->GetTileset();

    y += _ywin;

    x /= (ts->Width() + (_pad ? 1 : 0));
    y /= (ts->Height() + (_pad ? 1 : 0));

}

uint TilesetView::TilesPerRow() const {
    if (_executor->GetTileset()->Count() == 0) {
        return 1;
    } else {
        int i = LogicalWidth() / (_executor->GetTileset()->Width() + (_pad ? 1 : 0));
        return max(1, i);
    }
}

uint TilesetView::NumTileRows() const {
    if (_executor->GetTileset()->Count() == 0) {
        return 0;
    } else {
        return _executor->GetTileset()->Count() / TilesPerRow() + 1;
    }
}



bool TilesetView::IsPadded() const {
    return _pad;
}

int TilesetView::GetYWin() const {
    return _ywin;
}

Rect TilesetView::GetBrushSelection() const {
    return _selected;
}