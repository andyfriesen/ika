
#include "imageview.h"
#include "graph.h"

BEGIN_EVENT_TABLE(CImageView,wxDialog)
    //EVT_PAINT(CImageView::OnPaint)
END_EVENT_TABLE()

CImageView::CImageView(CPixelMatrix* img)
    : wxDialog(), pData(img)
{
    SetSize(-1,-1,340,340);

    pGraph=new CGraphFrame(this);
    pGraph->SetSize(10,10,330,330);

    pImage=new CImage(*img);
}

CImageView::~CImageView()
{
    delete pImage;
}

void CImageView::OnPaint()
{
    wxPaintDC bleh(this);

    pGraph->SetCurrent();
    pGraph->Clear();
    pGraph->ShowPage();
}