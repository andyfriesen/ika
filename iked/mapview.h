/*
    Since all the OpenGL stuff is windowed, we potentially can take advantage 
    of the fact that all page flipping is done via block copying, and thus we 
    can safely assume that the framebuffer is not erased when "flipping", by 
    implementing dirty rectangle based rendering.
    
    Winmaped used this technique to fantastic effect; it was still immediately
    responsive when maximized at 1600x1200x32 when setting tiles on a map, and
    that was done with software rendering.
*/

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
#include "misc.h"
#include <map>
#include "wx\wx.h"
#include "docview.h"

class Map;

class CMainWnd;
class CGraphFrame;
class CTileSet;
class CSpriteSet;
class CLayerVisibilityControl;
class CEntityEditor;
class ZoneEditor;

class wxSashLayoutWindow;
class wxCheckListBox;
class wxScrolledWindow;

class CMapView;

namespace MapEditState
{
    class IEditState
    {
    protected:
        CMapView* This;

        IEditState(CMapView* t) : This(t){}

    public:
        virtual ~IEditState(){}
        virtual void OnMouseDown(wxMouseEvent& e) = 0;
        virtual void OnMouseUp(wxMouseEvent& e) = 0;
        virtual void OnMouseMove(wxMouseEvent& e) = 0;
        virtual void OnMouseWheel(wxMouseEvent& e) = 0;
        virtual void OnRender(){}
    };

    class TileSetState : public IEditState
    {
        int oldx, oldy;

        void LayerEdit(wxMouseEvent& e);
        void UpdateStatBar(wxMouseEvent& e, int x, int y);
    public:
        TileSetState(CMapView* t) 
            : IEditState(t)
            , oldx(0)
            , oldy(0)
        {}

        virtual void OnMouseDown(wxMouseEvent& e);
        virtual void OnMouseUp(wxMouseEvent& e);
        virtual void OnMouseMove(wxMouseEvent& e);
        virtual void OnMouseWheel(wxMouseEvent& e);
    };

    class CopyState : public IEditState
    {
        Rect _selection;
    public:
        CopyState(CMapView* t, int x, int y)
            : IEditState(t)
            , _selection(x, y, x, y) {}

        virtual void OnMouseDown(wxMouseEvent& e);
        virtual void OnMouseUp(wxMouseEvent& e);
        virtual void OnMouseMove(wxMouseEvent& e);
        virtual void OnMouseWheel(wxMouseEvent& e);
        virtual void OnRender();
    };

    class PasteState : public IEditState
    {
        Rect _selection;
    public:
        PasteState(CMapView* t, int x, int y);

        virtual void OnMouseDown(wxMouseEvent& e);
        virtual void OnMouseUp(wxMouseEvent& e);
        virtual void OnMouseMove(wxMouseEvent& e);
        virtual void OnMouseWheel(wxMouseEvent& e);
        virtual void OnRender();
    };
}

class CMapView : public IDocView
{
    friend class CEntityEditor;     // -_-;

    // ...
    friend class MapEditState::TileSetState;
    friend class MapEditState::CopyState;
    friend class MapEditState::PasteState;

    enum
    {
        hidden = 0,
        visible,
        darkened,
    };

    enum CursorMode
    {
        mode_normal,
        mode_copy,
        mode_paste,
        // copy / paste / etc...
    };

private:
    CMainWnd*           pParentwnd;

    wxSashLayoutWindow* pLeftbar;
    wxSashLayoutWindow* pRightbar;
    wxScrolledWindow*   pScrollwin;
    CGraphFrame*        pGraph;
    CLayerVisibilityControl*
                        pLayerlist;

protected:
    CEntityEditor*      _entityEditor;
    ZoneEditor*         _zoneEditor;
//private:

    Map*                _map;
    CTileSet*           pTileset;
    ScopedPtr<MapEditState::IEditState> _editState;

    std::vector<CSpriteSet*>   pSprite;                             // entity spritesets needed for this map.  The indeces of this vector coincide with the entities which use them.

public:
    CMapView(CMainWnd* parent, int width, int height, const string& tilesetname);
    CMapView(CMainWnd* parent, const string& fname);

    void InitAccelerators();
    void InitMenu();
    void Init();

    void Paint();
    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    virtual void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);

    virtual const void* GetResource() const;

    void GoNextTile(wxEvent&);
    void GoPrevTile(wxEvent&);

    void OnZoomIn(wxCommandEvent&);
    void OnZoomOut(wxCommandEvent&);
    void OnZoomIn2x(wxCommandEvent&);
    void OnZoomIn4x(wxCommandEvent&);
    void OnZoomOut2x(wxCommandEvent&);
    void OnZoomOut4x(wxCommandEvent&);
    void OnZoomNormal(wxCommandEvent&);

    void OnShowEntityEditor(wxCommandEvent&);
    void OnShowZoneEditor(wxCommandEvent&);
    void OnShowVSP(wxCommandEvent&);
    void OnShowScript(wxCommandEvent&);

    void OnMoveLayerUp(wxCommandEvent&);
    void OnMoveLayerDown(wxCommandEvent&);
    void OnDeleteLayer(wxCommandEvent&);
    void OnNewLayer(wxCommandEvent&);

    void OnClose();

//------------------------------------------------------------

//------------------------------------------------------------

    DECLARE_EVENT_TABLE()

public:
    // CLayerVisibilityControl calls these functions
    void OnLayerChange(int lay);
    void OnLayerToggleVisibility(int lay, int newstate);

protected:
    int xwin, ywin;
    int _curLayer;
    // old mouse coords.  To prevent redundant processing.
    int oldx, oldy;
    std::map<int, int> nLayertoggle;
    uint _curZone;
    Rect _selection;

    void ScreenToMap(int& x, int& y);
    void MapToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y, int layidx);
    void HandleMouse(wxMouseEvent& event);
    void HandleMouseWheel(wxMouseEvent& event);
    void UpdateScrollbars();
    void UpdateLayerList();

    void Render();
    void RenderEntities();
    void RenderInfoLayer(int lay);
    void RenderLayer(int lay);
    void RenderSelectionRect(Rect r, RGBA colour);

    void Zoom(int nZoomscale);

    bool Save(const char* fname);
};

#endif