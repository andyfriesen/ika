
#ifndef TILESETVIEW_H
#define TILESETVIEW_H

#include "docview.h"
#include "graph.h"

class CMainWnd;
class CTileSet;

class CTileSetView : public IDocView
{
    CMainWnd*       pParent;
    CGraphFrame*    pGraph;

    CTileSet*       pTileset;

    int ywin;                   // scrollbar position
public:
    CTileSetView(CMainWnd* parentwnd,const string& fname);

    void OnSave(wxCommandEvent& event);

    void OnClick(wxMouseEvent& event);

    void OnPaint();
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollEvent& event);

    void OnClose();

    DECLARE_EVENT_TABLE()
};

#endif