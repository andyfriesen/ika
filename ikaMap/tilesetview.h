
#ifndef TILESETVIEW_H
#define TILESETVIEW_H
#include "common/matrix.h"
#include "common/types.h"
#include "wxinc.h"
#include "video.h"

struct Executor;
struct Tileset;
struct TilesetEvent;
struct MapTilesetEvent;

struct TilesetView : VideoFrame
{
private:
    Executor* _executor;
    int  _ywin;  // scroll position, in pixels.
    bool _pad;

public:
    TilesetView(Executor* e, wxWindow* parent);
    ~TilesetView();

    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnPaint(wxPaintEvent&);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    void Render();

    void UpdateScrollBars();

    void OnTilesetChange(const TilesetEvent& event);
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