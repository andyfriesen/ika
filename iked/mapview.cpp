#include "mapview.h"
#include "main.h"
#include "graph.h"
#include "tileset.h"
#include <gl\glu.h>

#include <wx\laywin.h>
#include <wx\sashwin.h>

/*
    blegh, didn't want this to get complicated.

    wxMDIChildFrame
        |----------------------|
        |                      |
    wxSashLayoutWindow  wxSashLayoutWindow
        |                      |
    CGraphFrame         Layer visibility stuff, not finalized

*/

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

    pLeftbar=new wxSashLayoutWindow(this,-1);
    pLeftbar->SetAlignment(wxLAYOUT_LEFT);
    pLeftbar->SetOrientation(wxLAYOUT_VERTICAL);
    pLeftbar->SetDefaultSize(wxSize(100,100));
    pLeftbar->SetSashVisible(wxSASH_RIGHT,true);

    pRightbar=new wxSashLayoutWindow(this,-1);
    pRightbar->SetAlignment(wxLAYOUT_RIGHT);

    pRightbar->SetScrollbar(wxVERTICAL,0,10,10000);
    pRightbar->SetScrollbar(wxHORIZONTAL,0,10,10000);

    pGraph=new CGraphFrame(pRightbar);
    Show();

    // Get resources
    pMap=pParentwnd->map.Load(fname);
    pTileset=pParentwnd->vsp.Load(pMap->GetVSPName());
}

void CMapView::OnPaint()
{
    wxPaintDC paintdc(this);

    pGraph->SetCurrent();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    xwin=ywin=0;
    RenderLayer(0);

    glFlush();
    pGraph->ShowPage();
}

void CMapView::OnSize(wxSizeEvent& event)
{
    wxLayoutAlgorithm layout;
    layout.LayoutWindow(this,pRightbar);
    int w,h;
//    pScrollwin->GetClientSize(&w,&h);
    pRightbar->GetClientSize(&w,&h);
    pGraph->SetSize(w,h);
}

void CMapView::OnScroll(wxScrollWinEvent& event)
{
    switch (event.GetOrientation())
    {
    case wxHORIZONTAL:  xwin=event.GetPosition();   break;
    case wxVERTICAL:    ywin=event.GetPosition();   break;
    }
}

void CMapView::OnClose()
{
    pParentwnd->map.Release(pMap);
    pParentwnd->vsp.Release(pTileset);
    Destroy();
}

// ------------------------------ Core logic -------------------------

void CMapView::RenderLayer(int lay)
{
    int nWidth,nHeight;

    pGraph->GetClientSize(&nWidth,&nHeight);

    int tx=pTileset->Width();
    int ty=pTileset->Height();

    int nFirstx=xwin/tx;
    int nFirsty=ywin/ty;
    
    int nLenx=nWidth/tx+1;
    int nLeny=nHeight/ty+1;

    int nAdjx=xwin%tx;
    int nAdjy=ywin%ty;

    for (int y=0; y<nLeny; y++)
    {
        for (int x=0; x<nLenx; x++)
        {
            int t=pMap->GetTile(x+nFirstx, y+nFirsty, lay);

            pTileset->DrawTile(x*tx+nAdjx, y*ty+nAdjy, t, *pGraph);
        }
    }
}