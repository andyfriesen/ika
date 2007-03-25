
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "wxinc.h"
#include "video.h"
#include "common/map.h"
#include "editstate.h"
#include "settilestate.h"
#include "brushstate.h"
#include "copypastestate.h"
#include "obstructionstate.h"
#include "entitystate.h"
#include "zoneeditstate.h"
#include "scriptstate.h"

struct Executor;
struct SpriteSet;
struct Script;

struct MapEvent;

/**
 * This is the panel that holds the actual map image.  It processes events and the like pertaining to
 * it directly (mostly mouse and key events), sending commands to change the map as it goes.
 *
 * _editState points to one of the editor state subclasses, depending on what the current state is.
 * (simple strategy pattern)
 */
struct MapView : public wxPanel {
    MapView(Executor* executor, wxWindow* parent);
    ~MapView();

    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent&){}
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    void OnKeyPress(wxKeyEvent& event);

    void OnMapChange(const MapEvent& event);
    void OnCurLayerChange(uint index);

    void Render();
    void ShowPage();

    void RenderLayer(const Matrix<uint>& tiles, int xoffset, int yoffset);
    void RenderLayer(Map::Layer* lay, int xoffset, int yoffset);
    void RenderEntities(Map::Layer* lay, int xoffset, int yoffset);
    void RenderObstructions(Map::Layer* lay, int xoffset, int yoffset);
    void RenderBrush(const Brush& brush, int x, int y);
    void RenderBrushOutline(const Brush& brush, int x, int y);
    void RenderBrush(int tx, int ty);

    void UpdateScrollBars();

    uint GetZoom() const;
    void SetZoom(uint z);
    void IncZoom(int amt);

    // Gah.  Pain in the ass.
    // Screen coordinates are pixels relative to the upperleft corner of the map viewport.
    // Layer coordinates are pixels relative to the upperleft corner of the layer.
    // Tile coordinates are tiles relative to the upperleft corner of the layer.
    void ScreenToMap(int& x, int& y) const;

    void ScreenToLayer(int& x, int& y) const;
    void ScreenToLayer(int& x, int& y, uint layer) const;

    void ScreenToTile(int& x, int& y) const;
    void ScreenToTile(int& x, int& y, uint layer) const;

    void TileToScreen(int& x, int& y) const;
    void TileToScreen(int& x, int& y, uint layer) const;

    void MapToTile(int& x, int& y) const;
    void MapToTile(int& x, int& y, uint layer) const;

    void TileToMap(int& x, int& y) const;
    void TileToMap(int& x, int& y, uint layer) const;

    int  GetXWin() const { return _xwin; }
    void SetXWin(int x) { _xwin = x;    };
    int  GetYWin() const { return _ywin; }
    void SetYWin(int y) { _ywin = y;    };

    uint EntityAt(int x, int y, uint layer);
    uint ZoneAt(int x, int y, uint layer);

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
    aegisz: TilesetterState
    aegisz: lol
    aegisz: just call it cock() and be done with it
    andys evil clone: hah.
    */
    void Cock();    // Sets the default tile-setting state.  Thanks aegis!
    void SetBrushState();
    void SetCopyPasteState();
    void SetObstructionState();
    void SetEntityState();
    void SetZoneState();
    void SetScriptTool(Script* script);

    DECLARE_EVENT_TABLE()

private:
    // It'd be great if we could eradicate the need for this.
    Executor*   _executor;
    VideoFrame* _video;

    int _xwin, _ywin;

    // used for middlemouse-button scrolling.
    int _scrollX, _scrollY;

    // Instances of the various edit states.  We only ever create one of each.
    TilesetState _tilesetState;
    BrushState _brushState;
    CopyPasteState _copyPasteState;
    ObstructionState _obstructionState;
    EntityState _entityState;
    ZoneEditState _zoneEditState;
    ScriptState _scriptState;
    //-
};

#endif
