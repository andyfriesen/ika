#include <cassert>

#include "tilesetview.h"

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
    , _selection(0, 0, 1, 1)
    , _dragging(false)
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

    Local::HandleEvent(
        _ywin, 
        0, 
        NumTileRows() * (ts->Height() + (_pad ? 1 : 0)), 
        LogicalHeight(), 
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
/*
void TilesetView::OnLeftClick(wxMouseEvent& event) {
    std::string b = "Beta!";
    Log::Write("Click!" + b);
    //exit(1);
    //VideoFrame::OnMouseEvent(event);
    //Tileset* ts = _executor->GetTileset();
    Log::Write("Click");
    _dragging = true;
    exit(1);
    //_selection.left = event.m_x / ts->Width();
    //_selection.top = event.m_y / ts->Height();
}


void TilesetView::OnLeftClick(wxMouseEvent& event) {
    VideoFrame::OnMouseEvent(event);

    Tileset* ts = _executor->GetTileset();

    if (event.ControlDown())

    {
        _selection.right = event.m_x / (ts->Width() + (_pad ? 1 : 0)) + 1;

        _selection.bottom = event.m_y / (ts->Height() + (_pad ? 1 : 0)) + 1;

    }

    else
    
    {
        _selection.left = event.m_x / (ts->Width() + (_pad ? 1 : 0));

        _selection.top = event.m_y / (ts->Height() + (_pad ? 1 : 0));

        _selection.right = _selection.left + 1;

        _selection.bottom = _selection.top + 1;
    }



    // Update brush.
    UpdateBrush();


    Refresh();
}
*/



void TilesetView::OnMouseDown(wxMouseEvent& event)

{

    VideoFrame::OnMouseEvent(event);

    Tileset* ts = _executor->GetTileset();

    if (event.LeftDown() && !_dragging)

    {

        _selection.left = event.GetX() / (ts->Width() + (_pad ? 1 : 0));

        _selection.top = event.GetY() / (ts->Height() + (_pad ? 1 : 0));

        _selection.right = _selection.left + 1;

        _selection.bottom = _selection.top + 1;



        _dragging = true;

    }

}


void TilesetView::OnMouseUp(wxMouseEvent& event)

{

    Tileset* ts = _executor->GetTileset();

    if (_dragging && !event.LeftIsDown())

    {

        _selection.right = event.m_x / (ts->Width() + (_pad ? 1 : 0)) + 1;

        _selection.bottom = event.m_y / (ts->Height() + (_pad ? 1 : 0)) + 1;



        _dragging = false;



        UpdateBrush();

        Refresh();

    }

}



void TilesetView::OnMouseMove(wxMouseEvent& event)

{

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

    // Highlight the current tile.
    {


        uint w = ts->Width();
        uint h = ts->Height();
        DrawSelectRect(_selection.left * (w + 1),
                _selection.top * (h + 1),
                (_selection.right - _selection.left) * (w + 1),
                (_selection.bottom - _selection.top) * (h + 1), 
                RGBA(127, 255, 255));

    }
}

void TilesetView::UpdateScrollBars() {
    const Tileset* ts = _executor->GetTileset();

    uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
    uint tileHeight = ts->Height() + (_pad ? 1 : 0);

    SetScrollbar(wxVERTICAL, _ywin, LogicalHeight(), NumTileRows() * tileHeight);
}


void TilesetView::SetSelectionTile(uint index)

{

    const Tileset* ts = _executor->GetTileset();



    int x, y;

    TileToPoint(index, x, y);

    x /= ts->Width() + (_pad ? 1 : 0);

    y /= ts->Width() + (_pad ? 1 : 0);



    _selection.left = x;

    _selection.top = y;

    _selection.right = x + 1;

    _selection.bottom = y + 1;



    Refresh();

}


void TilesetView::OnTilesetChange(const TilesetEvent& event) {
    _selection.top = 0;

    _selection.left = 0;

    _selection.bottom = 1;

    _selection.right = 1;



    Refresh();
}

void TilesetView::OnCurrentBrushChange(Matrix<uint>& newBrush) {
    Refresh();
    //_executor->SetStatusBar(va("Tile: %3i", newTile), 2);
}
void TilesetView::UpdateBrush() {
    uint h = _selection.bottom - _selection.top;
    uint w = _selection.right - _selection.left;
    Matrix<uint> brush = _executor->GetCurrentBrush();
    brush.Resize(w, h);
    for (uint x = 0; x < w; x ++) {
        for (uint y = 0; y < h; y ++) {
            brush(x, y) = (_selection.top + y) * TilesPerRow() + _selection.left + x;
        }
    }
    _executor->SetCurrentBrush(brush);
}

uint TilesetView::PointToTile(int x, int y) const {
    const Tileset* ts = _executor->GetTileset();

    uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
    uint tileHeight = ts->Height() + (_pad ? 1 : 0);

    x /= tileWidth;
    y /= tileHeight;
    
    return y * TilesPerRow() + x;
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
        x = (index % tilesPerRow) * tileWidth;
    }
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
