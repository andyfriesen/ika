/*
    OpenGL stuff isn't as fast as I had hoped it'd be.

    Since all the OpenGL stuff is windowed, we can take advantage of the fact
    that all page flipping is done via block copying, and thus we can safely
    assume that the framebuffer is not erased when "flipping", by implementing
    dirty rectangle based rendering.
    
    Winmaped used this technique to fantastic effect; it was still immediately
    responsive when maximized at 1600x1200x32 when setting tiles on a map, and
    that was done with software rendering.
*/

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
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

class CMapView : public IDocView
{
    friend class CEntityEditor;     // -_-;

    enum
    {
        lay_entity=-10,
        lay_zone,
        lay_obstruction
    };

    enum
    {
        hidden = 0,
        visible,
        darkened,
    };

    enum CursorMode
    {
        mode_normal,
        mode_select,
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
    CEntityEditor*      pEntityeditor;
    ZoneEditor*         _zoneeditor;
private:

    Map*                pMap;
    CTileSet*           pTileset;

    std::vector < CSpriteSet*>   pSprite;                             // entity spritesets needed for this map.  The indeces of this vector coincide with the entities which use them.

    int  nZoom;                                                     // in 16ths (ie 16 is 1:1, while a value of 1 means 1:16)

    Rect _selection;

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

    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);
    void OnZoomIn2x(wxCommandEvent& event);
    void OnZoomIn4x(wxCommandEvent& event);
    void OnZoomOut2x(wxCommandEvent& event);
    void OnZoomOut4x(wxCommandEvent& event);
    void OnZoomNormal(wxCommandEvent& event);

    void OnShowEntityEditor(wxCommandEvent& event);
    void OnShowZoneEditor(wxCommandEvent& event);
    void OnNewLayer(wxCommandEvent& event);
    void OnShowVSP(wxCommandEvent& event);
    void OnShowScript(wxCommandEvent& event);

    void OnClose();

//------------------------------------------------------------

//------------------------------------------------------------

    DECLARE_EVENT_TABLE()

public:
    // CLayerVisibilityControl calls these functions
    void OnLayerChange(int lay);
    void OnLayerToggleVisibility(int lay, int newstate);

    // Stuff that's not directly related to the UI

private:
    int xwin, ywin;
    int nCurlayer;
    // old mouse coords.  To prevent redundant processing.
    int oldx, oldy;
    CursorMode          csrmode;
    std::map < int, int> nLayertoggle;

    void ScreenToMap(int& x, int& y);
    void LayerEdit(wxMouseEvent& event);
    void HandleMouse(wxMouseEvent& event);
    void UpdateScrollbars();
    void UpdateLayerList();

    void Render();
    void RenderEntities();
    void RenderInfoLayer(int lay);
    void RenderLayer(int lay);
    void RenderSelectionRect();

    void Zoom(int nZoomscale);

    bool Save(const char* fname);
};

#endif