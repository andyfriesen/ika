
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

    CSpriteSetView(CMainWnd* parentwnd, const std::string& fname);
    CSpriteSetView(CMainWnd* parentwnd, int width, int height);
    ~CSpriteSetView();

    virtual void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);

    virtual const void* GetResource() const;

private:
    void OnPaint();
public:
    void Render();  // blech
private:
    void OnSize(wxSizeEvent& event);
    void OnScroll(wxScrollWinEvent& event);

    void OnLeftClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);

    void OnEditFrame(wxCommandEvent& event);

    void OnPreviousFrame(wxCommandEvent& event);
    void OnNextFrame(wxCommandEvent& event);

    void OnZoomIn(wxCommandEvent& event);
    void OnZoomOut(wxCommandEvent& event);
    void OnZoomNormal(wxCommandEvent& event);

    void UpdateScrollbar();

    void OnShowMovescriptEditor(wxCommandEvent& event);
    void OnImportFrames(wxCommandEvent& event);

    void Init();
    void InitMenu();
    void InitAccelerators();
    void SpritePos(int idx, int& x, int& y) const;
    int  FrameAt(int x, int y) const;
    void Zoom(int nZoomscale);

    CMainWnd*           _parent;
    CGraphFrame*        _graph;
    CSpriteSet*         _sprite;
    
    int                 _curFrame;
    int                 _ywin;
    int                 _zoom;
    bool                _pad;

    wxMenu*             _contextMenu;
    CMovescriptEditor*  _moveScriptEditor;

    DECLARE_EVENT_TABLE()
};

#endif

