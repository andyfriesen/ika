#include "mapview.h"
#include "main.h"
#include "graph.h"
#include <gl\glu.h>

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
    EVT_PAINT(CMapView::OnPaint)
    EVT_ERASE_BACKGROUND(CMapView::OnErase)
    EVT_SIZE(CMapView::OnSize)
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

    glRotatef(10,0,1,0);

    glBegin(GL_TRIANGLES);
    glColor3f(1,0,0);   glVertex2i(0,-1);
    glColor3f(0,1,0);   glVertex2i(-1,1);
    glColor3f(0,0,1);   glVertex2i(1,1);
    glEnd();
/*
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glNormal3f( 0.0F, 0.0F, 1.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);
    glVertex3f(-0.5F,-0.5F, 0.5F); glVertex3f( 0.5F,-0.5F, 0.5F);
    
    glNormal3f( 0.0F, 0.0F,-1.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f(-0.5F, 0.5F,-0.5F);
    glVertex3f( 0.5F, 0.5F,-0.5F); glVertex3f( 0.5F,-0.5F,-0.5F);
    
    glNormal3f( 0.0F, 1.0F, 0.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f( 0.5F, 0.5F,-0.5F);
    glVertex3f(-0.5F, 0.5F,-0.5F); glVertex3f(-0.5F, 0.5F, 0.5F);
    
    glNormal3f( 0.0F,-1.0F, 0.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f( 0.5F,-0.5F,-0.5F);
    glVertex3f( 0.5F,-0.5F, 0.5F); glVertex3f(-0.5F,-0.5F, 0.5F);
    
    glNormal3f( 1.0F, 0.0F, 0.0F);
    glVertex3f( 0.5F, 0.5F, 0.5F); glVertex3f( 0.5F,-0.5F, 0.5F);
    glVertex3f( 0.5F,-0.5F,-0.5F); glVertex3f( 0.5F, 0.5F,-0.5F);
    
    glNormal3f(-1.0F, 0.0F, 0.0F);
    glVertex3f(-0.5F,-0.5F,-0.5F); glVertex3f(-0.5F,-0.5F, 0.5F);
    glVertex3f(-0.5F, 0.5F, 0.5F); glVertex3f(-0.5F, 0.5F,-0.5F);
    glEnd();*/

    glFlush();
    pGraph->ShowPage();
}

void CMapView::OnSize(wxSizeEvent& event)
{
    int w=event.GetSize().GetWidth();
    int h=event.GetSize().GetHeight();

    pGraph->SetSize(w,h);
    pGraph->SetCurrent();
/*    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, w,h, 0.0f);
/*    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/

    glViewport(0,0,w,h);

    glScissor(0, 0, w,h);
}