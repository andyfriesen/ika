#include "mapview.h"
#include "main.h"
#include "graph.h"
#include "tileset.h"
#include <gl\glu.h>

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
    EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)
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
//    pVsp=pParentwnd->vsp.Load(pMap->GetVSPName());
//    pTileset=new CTileSet(pParentwnd->graphfactory);
    pTileset=pParentwnd->vsp.Load(pMap->GetVSPName());
}

CMapView::~CMapView()
{
    pParentwnd->map.Release(pMap);
    pParentwnd->vsp.Release(pTileset);
    delete pTileset;
}

void CMapView::OnPaint()
{
    wxPaintDC paintdc(this);

    pGraph->SetCurrent();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glColor3f(1,0,0);   glVertex2i(50,0);
    glColor3f(0,1,0);   glVertex2i(0,100);
    glColor3f(0,0,1);   glVertex2i(100,100);
    glEnd();

    glFlush();
    pGraph->ShowPage();
}

void CMapView::OnSize(wxSizeEvent& event)
{
    pGraph->SetSize(event.GetSize());
}