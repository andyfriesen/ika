
#ifndef TILESETVIEW_H
#define TILESETVIEW_H

#include "wxinc.h"
#include "video.h"

struct Executor;
struct Tileset;
struct TileSetEvent;
struct MapTileSetEvent;

struct TileSetView : VideoFrame
{
private:
    Executor* _executor;
    int  _ywin;  // scroll position, in pixels.
    bool _pad;

public:
    TileSetView(Executor* e, wxWindow* parent);
    ~TileSetView();

    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnPaint(wxPaintEvent&);
    void OnLeftClick(wxMouseEvent& event);

    void Render();

    void UpdateScrollBars();

    void OnTileSetChange(const TileSetEvent& event);
    void OnCurrentTileChange(uint newTile);

    uint PointToTile(int x, int y) const;   // returns the tile under coordinates (x,y)
    void TileToPoint(uint index, int& x, int& y) const;   // returns the position of the tile in (x,y)

private:
    // Simple helper things.
    uint TilesPerRow() const;
    uint NumTileRows() const;

    DECLARE_EVENT_TABLE()
};

#endif