
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "wxinc.h"
#include "video.h"
#include "map.h"
#include "editstate.h"
#include "settilestate.h"
#include "obstructionstate.h"

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
    ObstructionState _obstructionState;
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
    void RenderObstructions(Map::Layer* lay, int xoffset, int yoffset);

    void UpdateScrollBars();

    void ScreenToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y, uint layer);

    void TileToScreen(int& x, int& y);
    void TileToScreen(int& x, int& y, uint layer);

    uint GetCurLayer() const { return _curLayer; }
    void SetCurLayer(uint i);
    int  GetXWin() const { return _xwin; }
    int  GetYWin() const { return _ywin; }

    /*
    andys evil clone: erg.
        private:  SetTileState _setTileState;
        public:  SetSetTileState();
    aegisz: O_O
    andys evil clone: yeah.  I dislike that. :)
    aegisz: TileStateSetter?
    aegisz: Then you have the whole noun thing going
    andys evil clone: That doesn't sound like a very good name for the state itself.
    aegisz: Oh, yeah.
    andys evil clone: er... it should be "void SetSetTileState();"
        (actually it shouldn't be that either because that's a horrible name)
    aegisz: TileSetterState
    aegisz: lol
    aegisz: just call it cock() and be done with it
    andys evil clone: hah.
    */
    void Cock();    // Sets the default tile-setting state.  Thanks aegis!
    void SetObstructionState();

    DECLARE_EVENT_TABLE()
};

#endif