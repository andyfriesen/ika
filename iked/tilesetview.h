
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
public:
    CTileSetView(CMainWnd* parentwnd,const string& fname);

    void OnClick(wxMouseEvent& event);

    void OnPaint();
    void OnClose();

    void OnSave(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif