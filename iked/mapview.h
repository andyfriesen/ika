
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
#include <wx\wx.h>

class Map;

class CMainWnd;
class CGraphFrame;
class CTileSet;

class wxSashLayoutWindow;
class wxSashWindow;
class wxScrolledWindow;

class CMapView : public wxMDIChildFrame
{
    CMainWnd*    pParentwnd;

    wxSashLayoutWindow* pLeftbar;
    wxSashLayoutWindow* pRightbar;
    wxScrolledWindow*   pScrollwin;
    CGraphFrame*        pGraph;

    Map*         pMap;

    CTileSet*    pTileset;

public:
    CMapView(CMainWnd* parent,const string& fname,const wxPoint& position=wxDefaultPosition,const wxSize& size=wxDefaultSize,const long style=wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL);

    void OnPaint();
    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnClose();

    DECLARE_EVENT_TABLE()


private:

    int xwin,ywin;

    void RenderLayer(int lay);
};

#endif