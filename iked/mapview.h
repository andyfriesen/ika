
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <wx\wx.h>
#include <wx\image.h>
#include "types.h"
#include "map.h"
#include "vsp.h"

class CMapWnd : public wxMDIChildFrame
{
public:
    CMapWnd(wxMDIParentFrame* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,Map* m,VSP* v);
    virtual ~CMapWnd();
    
    //	DECLARE_EVENT_TABLE();
private:
    
    class CMapWidget* pMapwidget;
    
    // ika stuff
    Map* pMap;
    VSP* pVsp;
};

class CMapWidget : public wxScrolledWindow
{
public:
    CMapWidget(wxWindow* parent,const wxPoint& position,const wxSize& size,const long style,Map* m,VSP* v);
    ~CMapWidget();
    
    void OnPaint(wxPaintEvent& p);
    
    DECLARE_EVENT_TABLE()
        
private:
    wxImage* pBackbuffer;
    
    Map* pMap;
    VSP* pVsp;
    
    void RenderLayer(int lay,bool transparent,const wxRect& r);
    void DrawObstructions(const wxRect& r);
    void DrawZones(const wxRect& r);
    void DrawEntities(const wxRect& r);
    void DrawSelection(const wxRect& r);
    void Render(const wxRect& r);
    void RenderAll();
    
private:
    enum
    {
        mode_normal };
    wxRect curselection;
    int cursormode;
    // UI
    std::vector<bool> bLayertoggle;
    bool bObstoggle,bZonetoggle,bEnttoggle;
    int xwin,ywin;
};

#endif