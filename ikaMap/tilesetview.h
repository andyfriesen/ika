
#ifndef TILESETVIEW_H
#define TILESETVIEW_H

#include "wxinc.h"
#include "video.h"

class MainWindow;
class TileSet;

class TileSetView : public VideoFrame
{
private:
    MainWindow* _mainWnd;
    int  _ywin;  // scroll position, in pixels.
    bool _pad;

    uint  _curTile;  // The current tile.

public:
    TileSetView(MainWindow* mw, wxWindow* parent);
    ~TileSetView();

    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);
    void OnPaint(wxPaintEvent&);
    void OnLeftClick(wxMouseEvent& event);

    void Render();

    uint GetCurTile() const;
    void SetCurTile(uint t);

    void UpdateScrollBars();

    uint PointToTile(int x, int y) const;   // returns the tile under coordinates (x,y)
    void TileToPoint(uint index, int& x, int& y) const;   // returns the position of the tile in (x,y)

private:
    // Simple helper things.
    uint TilesPerRow() const;
    uint NumTileRows() const;

    DECLARE_EVENT_TABLE()
};

#endif