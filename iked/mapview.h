
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
#include <wx\wx.h>

class Map;

class CMainWnd;
class CGraphFrame;
class CTileSet;

class wxSashLayoutWindow;
class wxCheckListBox;
class wxScrolledWindow;

class CMapView : public wxMDIChildFrame
{
    CMainWnd*    pParentwnd;

    wxSashLayoutWindow* pLeftbar;
    wxSashLayoutWindow* pRightbar;
    wxScrolledWindow*   pScrollwin;
    CGraphFrame*        pGraph;
    wxCheckListBox*     pLayerlist;

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

    // Stuff that's not directly related to the UI

private:
    int xwin,ywin;

    void Render();
    void RenderLayer(int lay);

    bool Save(const char* fname);
};

#endif