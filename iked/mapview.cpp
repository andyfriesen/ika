#include "mapview.h"
#include "main.h"
#include "graph.h"
#include "tileset.h"
#include <gl\glu.h>

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
    EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)

    EVT_CLOSE(CMapView::OnClose)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent,const string& fname,const wxPoint& position,const wxSize& size,const long style)
: wxMDIChildFrame(parent,-1,fname.c_str(),position,size,style)
{
    pParentwnd=parent;

    int w,h;
    GetSize(&w,&h);

    pGraph=new CGraphFrame(this);
    pGraph->SetSize(w,h);
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
    pGraph->SetSize(event.GetSize());
}

void CMapView::OnClose()
{
    pParentwnd->map.Release(pMap);
    pParentwnd->vsp.Release(pTileset);
}

// ------------------------------ Core logic -------------------------

void CMapView::RenderLayer(int lay)
{
    int nWidth,nHeight;

    GetClientSize(&nWidth,&nHeight);

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