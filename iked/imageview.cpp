
#include "imageview.h"
#include "graph.h"

BEGIN_EVENT_TABLE(CImageView,IDocView)
    EVT_PAINT(CImageView::OnPaint)
END_EVENT_TABLE()

CImageView::CImageView(CMainWnd* parent,CImage* img)
    : IDocView(parent,"Image"), pImage(img)
{
    SetSize(-1,-1,340,340);

    pGraph=new CGraphFrame(this);
    pGraph->SetSize(10,10,330,330);
}

void CImageView::OnPaint()
{
    wxPaintDC bleh(this);

    pGraph->SetCurrent();
    pGraph->Clear();
    pGraph->ShowPage();
}