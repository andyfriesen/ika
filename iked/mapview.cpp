#include "mapview.h"
#include "main.h"
#include "graph.h"

BEGIN_EVENT_TABLE(CMapView,wxMDIChildFrame)
END_EVENT_TABLE()

CMapView::CMapView(CMainWnd* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,const char* fname)
: wxMDIChildFrame(parent,-1,title,position,size,style)
{
    Show();

    CGraphFrame* f=new CGraphFrame(this);
    f->ShowPage();
}