
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "wxinc.h"
#include "video.h"
#include "map.h"
#include "editstate.h"
#include "settilestate.h"
#include "obstructionstate.h"
#include "entitystate.h"

class SpriteSet;

class MainWindow;

/**
 * This is the panel that holds the actual map image.  It processes events and the like pertaining to
 * it directly (mostly mouse and key events), sending commands to change the map as it goes.
 *
 * _editState points to one of the editor state subclasses, depending on what the current state is.
 * (simple strategy pattern)
 */
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
    EntityState _entityState;
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

    uint GetZoom() const;
    void SetZoom(uint z);
    void IncZoom(int amt);

    void ScreenToMap(int& x, int& y);
    void ScreenToMap(int& x, int& y, uint layer);

    void ScreenToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y, uint layer);

    void TileToScreen(int& x, int& y);
    void TileToScreen(int& x, int& y, uint layer);

    uint GetCurLayer() const { return _curLayer; }
    void SetCurLayer(uint i);
    int  GetXWin() const { return _xwin; }
    int  GetYWin() const { return _ywin; }

    uint EntityAt(int x, int y, uint layer);

    SpriteSet*  GetEntitySpriteSet(Map::Entity* ent) const;
    VideoFrame* GetVideo() const;

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
    void SetEntityState();

    DECLARE_EVENT_TABLE()
};

#endif