
#include "imageview.h"
#include "graph.h"

BEGIN_EVENT_TABLE(ImageView, DocumentPanel)
    EVT_PAINT(ImageView::OnPaint)
END_EVENT_TABLE()

ImageView::ImageView(MainWindow* parent, Canvas* img)
    : DocumentPanel(parent, "image"), pData(img)
{
    SetSize(-1, -1, 340, 340);

    pGraph = new GraphicsFrame(this);
    pGraph->SetSize(10, 10, 320, 320);

    pImage = new Image(*img);
}

ImageView::~ImageView() {
    delete pImage;
}

void ImageView::OnPaint() {
    wxPaintDC bleh(this);

    pGraph->SetCurrent();
    pGraph->Clear();
    pGraph->ShowPage();
}

void ImageView::OnSave(wxCommandEvent&) {
}

const void* ImageView::GetResource() const {
    return pData;
}