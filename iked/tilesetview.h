
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

    int ywin;                                       // scrollbar position
public:
    CTileSetView(CMainWnd* parentwnd,const string& fname);

    void OnSave(wxCommandEvent& event);

    void OnPaint();
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnClose();

    void OnLeftClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);

    DECLARE_EVENT_TABLE()

    //-----------------------------------

    void Render();
    void UpdateScrollbar();

    int  TileAt(int x,int y) const;                 // returns the tile under the specified client coordinates
};

#endif