
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "types.h"
#include <wx\wx.h>

class Map;

class CMainWnd;
class CGraphFrame;
class CTileSet;

class CMapView : public wxMDIChildFrame
{
    CMainWnd*    pParentwnd;
    CGraphFrame* pGraph;
    Map*         pMap;

    CTileSet*    pTileset;

public:
    CMapView(CMainWnd* parent,const string& fname,const wxPoint& position=wxDefaultPosition,const wxSize& size=wxDefaultSize,const long style=wxDEFAULT_FRAME_STYLE);
    ~CMapView();

    void OnPaint();
    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif