#include "mapview.h"
#include "main.h"
#include "graph.h"

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
    EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,const char* fname)
: wxMDIChildFrame(parent,-1,title,position,size,style)
{
    int w,h;
    GetSize(&w,&h);

    pGraph=new CGraphFrame(this);
    pGraph->SetSize(w,h);
    Show();
}

CMapView::~CMapView()
{
}

void CMapView::OnPaint()
{
    wxPaintDC paintdc(this);

    pGraph->SetCurrent();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pGraph->RectFill(0,0,100,100,RGBA(255,0,0));

    glTranslatef(0,0,4);
    glBegin(GL_TRIANGLES);
    glVertex2f(0,-50);
    glVertex2f(-50,50);
    glVertex2f(50,50);
    glEnd();

    pGraph->ShowPage();
}