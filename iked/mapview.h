
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
#include <map>
#include <wx\wx.h>
#include "docview.h"

using std::map;

class Map;

class CMainWnd;
class CGraphFrame;
class CTileSet;
class CLayerVisibilityControl;

class wxSashLayoutWindow;
class wxCheckListBox;
class wxScrolledWindow;

class CMapView : public IDocView
{
    enum
    {
        lay_entity=-10,
        lay_zone,
        lay_obstruction
    };

    enum
    {
        hidden=0,
        visible,
        darkened,
    };

    enum CursorMode
    {
        mode_normal,
        // copy/paste/etc...
    };

    CMainWnd*           pParentwnd;

    wxSashLayoutWindow* pLeftbar;
    wxSashLayoutWindow* pRightbar;
    wxScrolledWindow*   pScrollwin;
    CGraphFrame*        pGraph;
    CLayerVisibilityControl*
                        pLayerlist;

    Map*                pMap;
    CTileSet*           pTileset;

    CursorMode          csrmode;

public:
    CMapView(CMainWnd* parent,const string& fname);

    void InitLayerVisibilityControl();

    void OnPaint();
    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnSave(wxCommandEvent& event) {}

    void OnClose();

//------------------------------------------------------------

    void HandleMouse(wxMouseEvent& event);

//------------------------------------------------------------

    DECLARE_EVENT_TABLE()

public:
    // CLayerVisibilityControl calls these functions
    void OnLayerChange(int lay);
    void OnLayerToggleVisibility(int lay,int newstate);

    // Stuff that's not directly related to the UI

private:
    int xwin,ywin;
    int nCurlayer;
    map<int,int> nLayertoggle;

    void Render();
    void RenderLayer(int lay);

    bool Save(const char* fname);
};

#endif