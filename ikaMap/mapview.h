
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "wxinc.h"
#include "video.h"
#include "map.h"
#include "editstate.h"
#include "settilestate.h"

class MainWindow;

class MapView : public wxPanel
{
private:
    MainWindow* _mainWnd;
    VideoFrame* _video;

    int _xwin, _ywin;
    uint _curLayer;

    // used for middlemouse-button scrolling.
    int _scrollX, _scrollY;

    // The current state
    EditState* _editState;

    // Instances of the various edit states.  We only ever create one of each.
    TileSetState _tileSetState;
    //-

public:
    MapView(MainWindow* mw, wxWindow* parent);
    ~MapView();

    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent&){}
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    void Render();
    void ShowPage();

    void RenderLayer(Map::Layer* lay, int xoffset, int yoffset);
    void RenderEntities(Map::Layer* lay, int xoffset, int yoffset);

    void UpdateScrollBars();

    void ScreenToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y, uint layer);

    void TileToScreen(int& x, int& y);
    void TileToScreen(int& x, int& y, uint layer);

    uint GetCurLayer() const { return _curLayer; }
    void SetCurLayer(uint i);

    DECLARE_EVENT_TABLE()
};

#endif