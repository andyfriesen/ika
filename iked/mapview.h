
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
#include <map>
#include <wx\wx.h>

using std::map;

class Map;

class CMainWnd;
class CGraphFrame;
class CTileSet;
class CLayerVisibilityControl;

class wxSashLayoutWindow;
class wxCheckListBox;
class wxScrolledWindow;

class CMapView : public wxMDIChildFrame
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

    CMainWnd*    pParentwnd;

    wxSashLayoutWindow* pLeftbar;
    wxSashLayoutWindow* pRightbar;
    wxScrolledWindow*   pScrollwin;
    CGraphFrame*        pGraph;
    CLayerVisibilityControl*
                        pLayerlist;

    Map*         pMap;

    CTileSet*    pTileset;

public:
    CMapView(CMainWnd* parent,const string& fname,const wxPoint& position=wxDefaultPosition,const wxSize& size=wxDefaultSize,const long style=wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL);

    void InitLayerVisibilityControl();

    void OnPaint();
    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnClose();

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