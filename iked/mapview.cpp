#include "mapview.h"
#include "main.h"
#include "graph.h"
#include "tileset.h"
#include "log.h"
#include "layervisibilitycontrol.h"
#include <gl\glu.h>

#include <wx\laywin.h>
#include <wx\sashwin.h>
#include <wx\checklst.h>

/*

    blegh, didn't want this to get complicated.

    wxMDIChildFrame
        |----------------------|
        |                      |
    CMapSash            wxSashLayoutWindow
        |                      |
    CGraphFrame         wxCheckListBox
    (Map rendering)     (Layer visibility)

*/

namespace
{
    // wxSashLayoutWindow tweak that passes size events to its parent.
    class CMapSash : public wxSashLayoutWindow
    {
    public:
        CMapSash(wxWindow* parent, int id)
            : wxSashLayoutWindow(parent,id)
        {}

        void ScrollRel(wxScrollWinEvent& event,int amount)
        {
            int max=GetScrollRange(event.GetOrientation());
            int thumbsize=GetScrollThumb(event.GetOrientation());

            amount+=GetScrollPos(event.GetOrientation());
            if (amount<0) amount=0;
            if (amount>max-thumbsize)
                amount=max-thumbsize;

            ((CMapView*)GetParent())->OnScroll(wxScrollWinEvent(-1,amount,event.GetOrientation()));
        }

        void ScrollTo(wxScrollWinEvent& event,int pos)
        {
            int max=GetScrollRange(event.GetOrientation());
            int thumbsize=GetScrollThumb(event.GetOrientation());

            if (pos<0) pos=0;
            if (pos>max-thumbsize)
                pos=max-thumbsize;

            ((CMapView*)GetParent())->OnScroll(wxScrollWinEvent(-1,pos,event.GetOrientation()));
        }

        void ScrollTop(wxScrollWinEvent& event)         {   ScrollTo(event,0);    }
        void ScrollBottom(wxScrollWinEvent& event)      {   ScrollTo(event,GetScrollRange(event.GetOrientation()));    }
        
        void ScrollLineUp(wxScrollWinEvent& event)      {   ScrollRel(event,-1);       }
        void ScrollLineDown(wxScrollWinEvent& event)    {   ScrollRel(event,+1);    }

        void ScrollPageUp(wxScrollWinEvent& event)      {   ScrollRel(event,-GetScrollThumb(event.GetOrientation()));    }
        void ScrollPageDown(wxScrollWinEvent& event)    {   ScrollRel(event,+GetScrollThumb(event.GetOrientation()));    }

        void OnScroll(wxScrollWinEvent& event)
        {
            ((CMapView*)GetParent())->OnScroll(event);
           
        }

        DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(CMapSash,wxSashLayoutWindow)
        EVT_SCROLLWIN_TOP(CMapSash::ScrollTop)
        EVT_SCROLLWIN_BOTTOM(CMapSash::ScrollBottom)
        EVT_SCROLLWIN_LINEUP(CMapSash::ScrollLineUp)
        EVT_SCROLLWIN_LINEDOWN(CMapSash::ScrollLineDown)
        EVT_SCROLLWIN_PAGEUP(CMapSash::ScrollPageUp)
        EVT_SCROLLWIN_PAGEDOWN(CMapSash::ScrollPageDown)
        EVT_SCROLLWIN_THUMBTRACK(CMapSash::OnScroll)
        EVT_SCROLLWIN_THUMBRELEASE(CMapSash::OnScroll)
    END_EVENT_TABLE()
};

//-------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
    EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)
    EVT_SCROLLWIN(CMapView::OnScroll)

    EVT_CLOSE(CMapView::OnClose)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent,const string& fname,const wxPoint& position,const wxSize& size,const long style)
: wxMDIChildFrame(parent,-1,fname.c_str(),position,size,style)
{
    pParentwnd=parent;

    int w,h;
    GetClientSize(&w,&h);

    // Left side -- layer properties
    pLeftbar=new wxSashLayoutWindow(this,-1);
    pLeftbar->SetAlignment(wxLAYOUT_LEFT);
    pLeftbar->SetOrientation(wxLAYOUT_VERTICAL);
    pLeftbar->SetDefaultSize(wxSize(100,100));
    pLeftbar->SetSashVisible(wxSASH_RIGHT,true);

    pLayerlist=new CLayerVisibilityControl(pLeftbar,-1,this);

    // Right side -- Map view
    pRightbar=new CMapSash(this,-1);
    pRightbar->SetAlignment(wxLAYOUT_RIGHT);

    pGraph=new CGraphFrame(pRightbar);

    // Get resources
    pMap=pParentwnd->map.Load(fname);                                   // load the map
    pTileset=pParentwnd->vsp.Load(pMap->GetVSPName());                  // load the VSP

    pRightbar->SetScrollbar(wxVERTICAL,0,w,pMap->Height()*pTileset->Height());
    pRightbar->SetScrollbar(wxHORIZONTAL,0,h,pMap->Width()*pTileset->Width());
    xwin=ywin=0;

    InitLayerVisibilityControl();

    Show();
}

void CMapView::InitLayerVisibilityControl()
{
    // Fill up the layer info bar
    const string& s=pMap->GetRString();
    
    for (int idx=0; idx<s.length(); idx++)
    {
        char c=s[idx];
        if (c>='0' && c<='9')
        {
            c= c=='0' ? 10 : c-'1';

            SMapLayerInfo lay;
            pMap->GetLayerInfo(lay,c);
            pLayerlist->AppendItem(va("Layer %i",c),c);
            pLayerlist->Check(pLayerlist->Number()-1);
        }
        else if (c=='E')
            pLayerlist->AppendItem("Entities",lay_entity);
//        else if (c=='R')
//            pLayerlist->AppendItem("HookRetrace",-1);  // we don't do anything when the hookretrace "layer" is selected.
    }

    pLayerlist->AppendItem("Zones",lay_zone);
    pLayerlist->AppendItem("Obstructions",lay_obstruction);
}

void CMapView::OnPaint()
{
    wxPaintDC paintdc(this);

    pGraph->SetCurrent();
    pGraph->Clear();    

    Render();

    glFlush();
    pGraph->ShowPage();
}

void CMapView::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,pRightbar);

    // FIXME: w is coming out too big; you can scroll right past the end of the map.
    int w,h;
    pRightbar->GetClientSize(&w,&h);

    pRightbar->SetScrollbar(wxVERTICAL,xwin,w,pMap->Height()*pTileset->Height());
    pRightbar->SetScrollbar(wxHORIZONTAL,ywin,h,pMap->Width()*pTileset->Width());  
}

void CMapView::OnScroll(wxScrollWinEvent& event)
{
    switch (event.GetOrientation())
    {
    case wxHORIZONTAL:  xwin=event.GetPosition();   break;
    case wxVERTICAL:    ywin=event.GetPosition();   break;
    }

    pRightbar->SetScrollPos(wxHORIZONTAL,xwin);
    pRightbar->SetScrollPos(wxVERTICAL,ywin);

    Render();
    pGraph->ShowPage();
}

void CMapView::OnClose()
{
    pParentwnd->map.Release(pMap);
    pParentwnd->vsp.Release(pTileset);
    Destroy();
}

void CMapView::OnLayerChange(int lay)
{
    nCurlayer=lay;
    if (nLayertoggle[lay]==hidden)
    {
        nLayertoggle[lay]=visible;
        Render();
        pGraph->ShowPage();
    }
}

void CMapView::OnLayerToggleVisibility(int lay,int newstate)
{
    nLayertoggle[lay]=newstate;
    Render();
    pGraph->ShowPage();
}

// ------------------------------ Rendering -------------------------

void CMapView::Render()
{
    const string& r=pMap->GetRString();

    pGraph->SetCurrent();
    pGraph->Clear();

    for (int i=0; i<r.length(); i++)
    {
        if (r[i]>='0' && r[i]<='9')
        {
            int l=r[i]-'1';
            if (r[i]=='0') l=10;
            
            if (l>=0 && l<pMap->NumLayers() && nLayertoggle[l]!=hidden)
                RenderLayer(l);
        }
    }
}

void CMapView::RenderLayer(int lay)
{
    int nWidth,nHeight;

    pGraph->GetClientSize(&nWidth,&nHeight);

    SMapLayerInfo l;
    pMap->GetLayerInfo(l,lay);

    const int xw=xwin*l.pmulx/l.pdivx;
    const int yw=ywin*l.pmuly/l.pdivy;

    const int tx=pTileset->Width();
    const int ty=pTileset->Height();

    const int nFirstx=xw/tx;
    const int nFirsty=yw/ty;
    
    const int nLenx=nWidth/tx+2;
    const int nLeny=nHeight/ty+2;

    const int nAdjx=xw%tx;
    const int nAdjy=yw%ty;

    for (int y=0; y<nLeny; y++)
    {
        for (int x=0; x<nLenx; x++)
        {
            int t=pMap->GetTile(x+nFirstx, y+nFirsty, lay);
            
            if (lay==0 || t!=0)
                pTileset->DrawTile(x*tx-nAdjx, y*ty-nAdjy, t, *pGraph);
        }
    }
}