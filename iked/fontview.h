
#ifndef FONTVIEW_H
#define FONTVIEW_H

#include <wx\wx.h>

#include "types.h"
#include "docview.h"
#include "graph.h"
#include "fontfile.h"

class CFontView : public IDocView
{
    public:

        CFontView(CMainWnd* parentwnd,const string& fname);
        ~CFontView() {}

        void OnSave(wxCommandEvent&);
        void OnSize(wxSizeEvent&);
        void OnPaint();
        void OnEraseBackground(wxEraseEvent&) {}
        void OnScroll(wxScrollWinEvent&);
        void OnClose();
        void OnLeftClick(wxMouseEvent&);
        void OnRightClick(wxMouseEvent&);
        void Render();

    private:

        CMainWnd* pParent;
        CGraphFrame* pGraph;
        CFontFile* pFontfile;

        int ywin;
        int nCurfont;
        string sFilename;

        void FontPos(int fontidx,int& x,int& y) const;
        int FontAt(int x,int y) const;
        void UpdateScrollbar();


        DECLARE_EVENT_TABLE()

};

#endif // FONTVIEW_H
        