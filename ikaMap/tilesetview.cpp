#include <cassert>

#include "tilesetview.h"

#include "mainwindow.h"
#include "tileset.h"
#include "misc.h"

BEGIN_EVENT_TABLE(TileSetView, VideoFrame)
    EVT_SIZE(TileSetView::OnSize)
    EVT_SCROLLWIN(TileSetView::OnScroll)
    EVT_PAINT(TileSetView::OnPaint)
    EVT_LEFT_DOWN(TileSetView::OnLeftClick)
END_EVENT_TABLE()

TileSetView::TileSetView(MainWindow* mw, wxWindow* parent)
    : VideoFrame(parent)
    , _mainWnd(mw)
    , _ywin(0)
    , _curTile(0)
    , _pad(true)
{}

TileSetView::~TileSetView()
{}

void TileSetView::OnSize(wxSizeEvent& event)
{
    UpdateScrollBars();
    VideoFrame::OnSize(event);
}

void TileSetView::OnScroll(wxScrollWinEvent& event)
{
    struct Local
    {
        static void HandleEvent(int& val, int min, int max, int page, int pos, wxEventType et)
        {
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

    const TileSet* ts = _mainWnd->GetTileSet();

    Local::HandleEvent(
        _ywin, 
        0, 
        NumTileRows() * (_mainWnd->GetTileSet()->Height() + (_pad ? 1 : 0)), 
        LogicalHeight(), 
        event.GetPosition(), 
        event.GetEventType());

    UpdateScrollBars();
    Render();
    ShowPage();
}

void TileSetView::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);

    Render();
    ShowPage();
}

void TileSetView::OnLeftClick(wxMouseEvent& event)
{
    VideoFrame::OnMouseEvent(event);    // compensate for zoom.

    SetCurTile(PointToTile(event.m_x, event.m_y + _ywin));
    Render();
    ShowPage();
}

void TileSetView::Render()
{
    SetCurrent();
    Clear();

    TileSet* ts = _mainWnd->GetTileSet();
    assert(ts != 0);

    if (ts->Count() == 0)
        return;

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

    for (uint y = 0; y < numCols; y++)
    {
        for (uint x = 0; x < rowWidth; x++)
        {
            Image& tile = ts->GetImage(curTile);

            Blit(tile, curX, curY);

            curX += tileWidth;
            curTile++;

            if (curTile >= ts->Count())
                goto breakLoop;
        }
        curY += tileHeight;
        curX = startX;
     }

breakLoop:;

    // Highlight the current tile.
    {
        int x, y;
        TileToPoint(_curTile, x, y);

        Rect(x + 1, y, ts->Width(), ts->Height(), RGBA(255, 255, 255));
    }
}

uint TileSetView::GetCurTile() const
{
    return _curTile;
}

void TileSetView::SetCurTile(uint t)
{
    _curTile = clamp<uint>(t, 0, _mainWnd->GetTileSet()->Count() - 1);
    _mainWnd->GetStatusBar()->SetStatusText(va("Tile: %i", _curTile), 2);
    Render();
    ShowPage();
}

void TileSetView::UpdateScrollBars()
{
    const TileSet* ts = _mainWnd->GetTileSet();

    uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
    uint tileHeight = ts->Height() + (_pad ? 1 : 0);

    SetScrollbar(wxVERTICAL, _ywin, LogicalHeight(), NumTileRows() * tileHeight);
}

uint TileSetView::PointToTile(int x, int y) const
{
    const TileSet* ts = _mainWnd->GetTileSet();

    uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
    uint tileHeight = ts->Height() + (_pad ? 1 : 0);

    //y += _ywin;

    x /= tileWidth;
    y /= tileHeight;
    
    return y * TilesPerRow() + x;
}

void TileSetView::TileToPoint(uint index, int& x, int& y) const
{
    if (index > _mainWnd->GetTileSet()->Count())
        x = y = -1;
    else
    {
        const TileSet* ts = _mainWnd->GetTileSet();

        uint tileWidth  = ts->Width()  + (_pad ? 1 : 0);
        uint tileHeight = ts->Height() + (_pad ? 1 : 0);
        uint tilesPerRow = TilesPerRow();

        if (tilesPerRow == 0)
        {
            x = y = -1;
            return;
        }

        y = (index / tilesPerRow) * tileHeight - _ywin;
        x = (index % tilesPerRow) * tileWidth;
    }
}

uint TileSetView::TilesPerRow() const
{
    if (_mainWnd->GetTileSet()->Count() == 0)
        return 1;
    else
    {
        int i = LogicalWidth() / (_mainWnd->GetTileSet()->Width() + (_pad ? 1 : 0));
        return max(1, i);
    }
}

uint TileSetView::NumTileRows() const
{
    if (_mainWnd->GetTileSet()->Count() == 0)
        return 0;
    else
        return _mainWnd->GetTileSet()->Count() / TilesPerRow() + 1;
}