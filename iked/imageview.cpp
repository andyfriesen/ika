
#include "imageview.h"
#include "graph.h"

BEGIN_EVENT_TABLE(CImageView, DocumentPanel)
    EVT_PAINT(CImageView::OnPaint)
END_EVENT_TABLE()

CImageView::CImageView(MainWindow* parent, Canvas* img)
    : DocumentPanel(parent, "image"), pData(img)
{
    SetSize(-1, -1, 340, 340);

    pGraph = new GraphicsFrame(this);
    pGraph->SetSize(10, 10, 320, 320);

    pImage = new Image(*img);
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

void CImageView::OnSave(wxCommandEvent&)
{
}

const void* CImageView::GetResource() const
{
    return pData;
}