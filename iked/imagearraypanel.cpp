
#include "imagearraypanel.h"
#include "imagebank.h"
#include "wx_events.h"

namespace iked {

    BEGIN_EVENT_TABLE(ImageArrayPanel, GraphicsFrame)
        EVT_PAINT(ImageArrayPanel::onPaint)
        EVT_ERASE_BACKGROUND(ImageArrayPanel::onEraseBackground)
        EVT_LEFT_UP(ImageArrayPanel::onLeftClick)
        EVT_RIGHT_UP(ImageArrayPanel::onRightClick)
        EVT_LEFT_DCLICK(ImageArrayPanel::onDoubleClick)
    END_EVENT_TABLE()

    ImageArrayPanel::ImageArrayPanel(wxWindow* parent, ImageArrayDocument* doc)
        : GraphicsFrame(parent)
        , document(doc)
        , selectedImage(0)
        , scrollPos(0)
        , imagePadSize(0)
    {}

    int ImageArrayPanel::getImageAtPos(int x, int y) {
        y += scrollPos;

        int xstep = document->getWidth() + imagePadSize;
        int ystep = document->getHeight() + imagePadSize;
        int cols = max(LogicalWidth() / xstep, 1);

        x /= xstep;
        y /= ystep;

        int index = y * cols + x;

        if (0 < index && index <= document->getCount()) {
            return index;
        } else {
            return -1;
        }
    }

    void ImageArrayPanel::getImagePos(int index, int* x, int* y) {
        int xstep = document->getWidth()  + imagePadSize;
        int ystep = document->getHeight() + imagePadSize;

        int cols = max(LogicalWidth() / xstep, 1);

        *x = index % cols;
        *y = index / cols - scrollPos;

        *x *= xstep;
        *y *= ystep;
        *y += scrollPos;
    }

    void ImageArrayPanel::onSize(wxSizeEvent& event) {
        Refresh();
    }

    void ImageArrayPanel::onScroll(wxScrollWinEvent& event) {
        if (event.m_eventType == wxEVT_SCROLLWIN_TOP) {
            scrollPos = 0;
        } else if (event.m_eventType == wxEVT_SCROLLWIN_BOTTOM) {
            // guaranteed to be too big.  Will be corrected in updateScrollbar
            scrollPos = document->getCount();
        } else if (event.m_eventType == wxEVT_SCROLLWIN_LINEUP) {
            scrollPos--;
        } else if (event.m_eventType == wxEVT_SCROLLWIN_LINEDOWN) {
            scrollPos++;
        } else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEUP) {
            scrollPos -= GetScrollThumb(wxVERTICAL);
        } else if (event.m_eventType == wxEVT_SCROLLWIN_PAGEDOWN) {
            scrollPos += GetScrollThumb(wxVERTICAL);
        } else {
            scrollPos = event.GetPosition();
        }

        updateScrollbar();
        Refresh();
    }

    void ImageArrayPanel::onLeftClick(wxMouseEvent& event) {
        int x;
        int y;
        event.GetPosition(&x, &y);

        int index = getImageAtPos(x, y);
        if (index != -1 && index != selectedImage) {
            wxCommandEvent event(EVT_IMAGE_SELECT, GetId());
            event.SetInt(index);
            ProcessEvent(event);
            selectedImage = index;
            Refresh();
        }
    }

    void ImageArrayPanel::onRightClick(wxMouseEvent& event) {
        int x;
        int y;
        event.GetPosition(&x, &y);

        int index = getImageAtPos(x, y);
        if (index != -1) {
            wxCommandEvent evt(EVT_IMAGE_RIGHT_CLICK, GetId());
            evt.SetInt(index);
            AddPendingEvent(evt);
            selectedImage = index; // is this the right thing?
        }
    }

    void ImageArrayPanel::onDoubleClick(wxMouseEvent& event) {
        int x;
        int y;
        event.GetPosition(&x, &y);

        int index = getImageAtPos(x, y);
        if (index != -1) {
            wxCommandEvent evt(EVT_IMAGE_DOUBLE_CLICK, GetId());
            evt.SetInt(index);
            AddPendingEvent(evt);
            selectedImage = index;
        }
    }
    
    void ImageArrayPanel::onPaint(wxPaintEvent& event){
        wxPaintDC paintDC(this);
        if (!document) {
            return;
        }

        int width = LogicalWidth();

        int framex = document->getWidth();
        int framey = document->getHeight();

        int xstep = framex + imagePadSize;
        int ystep = framey + imagePadSize;

        int cols = max(1, width / xstep);

        SetCurrent();
        Clear();

        int x = 0;
        int y = 0;
        int index = scrollPos * cols;
        while (index < document->getCount()) {
            Blit(document->getImage(index), x * xstep, y * ystep, true);

            index++;
            x++;
            if (x > cols) {
                x = 0;
                y++;
            }
        }

        int x2, y2;
        framex = document->getWidth();
        framey = document->getHeight();
        getImagePos(selectedImage, &x2, &y2);

        Rect(x2 - 1, y2 - 1, framex + 1, framey + 1, RGBA(255, 255, 255));

        ShowPage();
    }

    void ImageArrayPanel::updateScrollbar() {
        const int width = LogicalWidth();
        const int height = LogicalHeight();

        int cols = max(1, width / document->getWidth());
        int rows = height / document->getHeight();

        int totalHeight = document->getCount() / cols + 1;

        scrollPos = clamp(scrollPos, 0, rows);

        SetScrollbar(wxVERTICAL, scrollPos, rows, totalHeight, true);
    }

    int ImageArrayPanel::getNumCols() {
        return max(LogicalWidth() / (document->getWidth() + imagePadSize), 1);
    }

    int ImageArrayPanel::getNumRows() {
        return max(1, document->getCount() / getNumCols());
    }

}
