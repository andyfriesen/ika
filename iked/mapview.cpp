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

    glLoadIdentity();
    glTranslatef(0,0,-10);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glColor3f(1,1,1);    glVertex3f(0,-50,400);
    glColor3f(0,0,0);    glVertex3f(-50,50,-400);
    glColor3f(1,1,1);    glVertex3f(50,50,0);
    glEnd();

    pGraph->ShowPage();
}