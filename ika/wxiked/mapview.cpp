
#include "mapview.h"
#include "draw.h"
#include <wx/dcmemory.h>

// CMapWnd's events
//BEGIN_EVENT_TABLE(CMapWnd,wxMDIParentFrame)
//END_EVENT_TABLE()

CMapWnd::CMapWnd(wxMDIParentFrame* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,Map* m,VSP* v)
	:wxMDIChildFrame(parent,-1,title,position,size,style)
{
	// create the map widget thingie
	int width,height;
	GetClientSize(&width,&height);

	pMapwidget=new CMapWidget(this,wxPoint(0,0),wxSize(width,height),0,m,v);
	pMapwidget->SetScrollbars(20,20,50,50);

	pMap=m;
	pVsp=v;

	v->Load("test.vsp");
}

CMapWnd::~CMapWnd()
{
}

//----------------------------------------------------------------------

// CMapWidget's events
BEGIN_EVENT_TABLE(CMapWidget,wxScrolledWindow)
	EVT_PAINT(CMapWidget::OnPaint)
END_EVENT_TABLE()

static const int nBlocksize=256;

CMapWidget::CMapWidget(wxWindow* parent,const wxPoint& position,const wxSize& size,const long style,Map* m,VSP* v)
	: wxScrolledWindow(parent,-1,position,size,style)
{
	pMap=m;
	pVsp=v;

	pBackbuffer=new wxImage(nBlocksize,nBlocksize);
}

CMapWidget::~CMapWidget()
{
	delete pBackbuffer;
}

void CMapWidget::OnPaint(wxPaintEvent& p)
{
	gfx::BlitTile(*pBackbuffer,0,0,0,*pVsp);

	wxPaintDC dc(this);
	wxBitmap b(*pBackbuffer);
	wxMemoryDC tempdc;
	tempdc.SelectObject(b);

	dc.Blit(0,0,30,30,&tempdc,0,0);
}