
#ifndef SPRITESETVIEW_H
#define SPRITESETVIEW_H

#include <wx\wx.h>

#include "docview.h"
#include "graph.h"
#include "spriteset.h"


class CMovescriptEditor;

class CSpriteSetView : public IDocView
{
    public:

        CSpriteSetView(CMainWnd* parentwnd,const string& fname);
        ~CSpriteSetView();


        void OnSave(wxCommandEvent& event);
        void OnSaveAs(wxCommandEvent& event);
        void OnClose();

        void OnPaint();
        void Render();
        void OnSize(wxSizeEvent& event);
        void OnScroll(wxScrollWinEvent& event);

        void OnLeftClick(wxMouseEvent& event);
        void OnRightClick(wxMouseEvent& event);

        void OnEditFrame(wxCommandEvent&);

        void OnPreviousFrame(wxCommandEvent&);
        void OnNextFrame(wxCommandEvent&);

        void OnZoomIn(wxCommandEvent& event);
        void OnZoomOut(wxCommandEvent& event);
        void OnZoomNormal(wxCommandEvent& event);

        void UpdateScrollbar();
        //void HandleMouse(wxMouseEvent& event);

        void OnShowMovescriptEditor(wxCommandEvent& event);


    private:

        void InitMenu();
        void InitAccelerators();
        void SpritePos(int idx,int& x,int& y) const;
        void Zoom(int nZoomscale);



        CMainWnd*       pParent;
        CGraphFrame*    pGraph;
        CSpriteSet*     pSprite;
        

        int             nCurframe;
        int             ywin;
        int             nZoom;
        string          sSpritename;


        wxMenu*             pContextmenu;
        CMovescriptEditor*  pMovescripteditor;

        



        DECLARE_EVENT_TABLE()


};

#endif

