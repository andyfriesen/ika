/*
    Since all the OpenGL stuff is windowed, we potentially can take advantage 
    of the fact that all page flipping is done via block copying, and thus we 
    can safely assume that the framebuffer is not erased when "flipping", by 
    implementing dirty rectangle based rendering.
    
    Winmaped used this technique to fantastic effect; it was still immediately
    responsive when maximized at 1600x1200x32 when setting tiles on a map, and
    that was done with software rendering.
*/

#pragma once

#include <map>

#include "common/utility.h"
#include "misc.h"

#include "wx/wx.h"

#include "docview.h"
#include "map.h"

class MainWindow;
class GraphicsFrame;
class Tileset;
class SpriteSet;
class CLayerVisibilityControl;
class EntityEditor;
class ZoneEditor;

class wxSashLayoutWindow;
class wxCheckListBox;
class wxScrolledWindow;

class MapView;

namespace MapEditState
{
    class IEditState
    {
    protected:
        MapView* This;

        IEditState(MapView* t) : This(t){}

        // accessors since subclasses aren't privy to the inner workings of MapView
        Map::Layer* CurLayer() const;
        MainWindow*   Parent() const;
        Tileset*   Tileset() const;
        Point       CameraPos() const;
        Map*        Map() const;
        std::map<std::string, SpriteSet*>& SpriteSets() const;
        void        Render();

    public:
        virtual ~IEditState(){}
        virtual void OnMouseDown(wxMouseEvent&) = 0;
        virtual void OnMouseUp(wxMouseEvent&) = 0;
        virtual void OnMouseMove(wxMouseEvent&) = 0;
        virtual void OnMouseWheel(wxMouseEvent&) = 0;
        virtual void OnDoubleClick(wxMouseEvent&){}
        virtual void OnRender(){}
        virtual void OnKeyPress(wxKeyEvent&){}
    };

    /*class CopyState : public IEditState
    {
        Rect _selection;
    public:
        CopyState(MapView* t, int x, int y)
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
        PasteState(MapView* t, int x, int y);

        virtual void OnMouseDown(wxMouseEvent& e);
        virtual void OnMouseUp(wxMouseEvent& e);
        virtual void OnMouseMove(wxMouseEvent& e);
        virtual void OnMouseWheel(wxMouseEvent& e);
        virtual void OnRender();
    };*/
}

class MapView : public DocumentPanel
{
    friend class EntityEditor;
    friend class MapEditState::IEditState;

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
    MainWindow*           pParentwnd;

    wxSashLayoutWindow* pLeftbar;
    wxSashLayoutWindow* pRightbar;
    wxScrolledWindow*   pScrollwin;
    GraphicsFrame*        _graph;
    CLayerVisibilityControl*
                        _layerList;

protected:
    EntityEditor*      _entityEditor;
    ZoneEditor*         _zoneEditor;
//private:

    Map*                _map;
    Tileset*           _tileset;
    ScopedPtr<MapEditState::IEditState> _editState;

    // Entity spritesets needed for this map.  The key is the filename of the sprite.
    // I hope that using strings to get sprites like this isn't going to be a performance
    // liability.
    typedef std::map<std::string, SpriteSet*> SpriteMap;
    SpriteMap _sprites;

public:
    MapView(MainWindow* parent, int width, int height, const std::string& tilesetname);
    MapView(MainWindow* parent, const std::string& fname);

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

    void SetCurrentLayer(Map::Layer* lay);
    void SetCurrentLayer(const std::string& layerName);

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

    void OnLayerMode(wxCommandEvent&);
    void OnTileMode(wxCommandEvent&);
    void OnEntityMode(wxCommandEvent&);
    void OnZoneMode(wxCommandEvent&);

    void OnMoveLayerUp(wxCommandEvent&);
    void OnMoveLayerDown(wxCommandEvent&);
    void OnDeleteLayer(wxCommandEvent&);
    void OnNewLayer(wxCommandEvent&);

    void OnClose();

    void OnKeyDown(wxKeyEvent&);

//------------------------------------------------------------

//------------------------------------------------------------

    DECLARE_EVENT_TABLE()

public:
    // CLayerVisibilityControl calls these functions
    void OnLayerChange(Map::Layer* lay);
    void OnLayerToggleVisibility(Map::Layer* lay, int newstate);

protected:
    int xwin, ywin;
    Map::Layer* _curLayer;
    // old mouse coords.  To prevent redundant processing.
    int oldx, oldy;
    // TODO: reimplement layer hiding.
    Map::Zone* _curZone;

public:
    void MapToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y);
    void ScreenToTile(int& x, int& y, Map::Layer* lay);

private:
    void HandleMouse(wxMouseEvent& event);
    void HandleMouseWheel(wxMouseEvent& event);
    void UpdateScrollbars();

    void Render();
    void RenderEntities(Map::Layer* lay, int xoffset, int yoffset);
    //void RenderInfoLayer(int lay);
    void RenderLayer(Map::Layer* lay, int xoffset, int yoffset);

public:
    void UpdateLayerList();
    void RenderSelectionRect(Rect r, RGBA colour);

    void Zoom(int nZoomscale);

    bool Save(const char* fname);
};

#endif