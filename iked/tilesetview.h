
#ifndef TILESETVIEW_H
#define TILESETVIEW_H

#include "docview.h"
#include "graph.h"

class CMainWnd;
class CTileSet;
class wxMenu;

class CTileSetView : public IDocView
{
    CMainWnd*       pParent;
    CGraphFrame*    pGraph;

    CTileSet*       pTileset;

    wxMenu*         pContextmenu;

    int ywin;                                       // scrollbar position
    bool bPad;                                      // pixel padding on/off

    int nTile;                                      // used for the context menu; the tile index of the tile that was right-clicked
    int zoom;                                       // zoom scale.  16 is normal.  1 is 1/16th scale.

public:
    CTileSetView(CMainWnd* parentwnd,const string& fname);
    ~CTileSetView();

    void OnSave(wxCommandEvent& event);

    void Paint();
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnClose();

    void OnLeftClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    void OnEraseBackground(wxEraseEvent&) {}

    //-----------------------------------

    void Zoom(int factor);
    void OnEditTile(wxCommandEvent&);
    void OnZoomNormal(wxCommandEvent&);
    void OnZoomIn(wxCommandEvent&);
    void OnZoomOut(wxCommandEvent&);
    void OnZoomIn2x(wxCommandEvent&);
    void OnZoomOut2x(wxCommandEvent&);
    void OnZoomIn4x(wxCommandEvent&);
    void OnZoomOut4x(wxCommandEvent&);

    //-----------------------------------

    void Render();
    void UpdateScrollbar();

    int  TileAt(int x,int y) const;                 // returns the tile under the specified client coordinates
    void TilePos(int t,int& x,int& y) const;        // x and y are set to the position at which the specified tile is rendered at

    DECLARE_EVENT_TABLE()
};

#endif