
#pragma once

#include "graph.h"
#include "wx_events.h"

namespace iked {

    struct ImageArrayDocument;

    /**
     * Control that displays an array of images.
     * Used for tilesets, spritesets, and fonts.
     */
    struct ImageArrayPanel : GraphicsFrame {
        ImageArrayPanel(wxWindow* parent, ImageArrayDocument* resource);
        
#if 0
        virtual void onSelectImage(int index) = 0;
        virtual void onRightClickImage(int index) = 0;
        virtual void onDoubleClickImage(int index) = 0;
#endif

        /**
         * @return Returns the image under x,y.  -1 if no image is there.
         */
        int getImageAtPos(int x, int y);

        /**
         * x and y recieve the position of the image with the given index.
         */
        void getImagePos(int index, int* x, int* y);

        ImageArrayDocument* getDocument() { return document; }

        DECLARE_EVENT_TABLE();
    private:
        void onSize(wxSizeEvent& event);
        void onScroll(wxScrollWinEvent& event);
        void onLeftClick(wxMouseEvent& event);
        void onRightClick(wxMouseEvent& event);
        void onDoubleClick(wxMouseEvent& event);
        void onMouseWheel(wxMouseEvent& event);

        void onPaint(wxPaintEvent& event);
        void onEraseBackground(wxEraseEvent&) { }

        void updateScrollbar();

        int getNumCols();
        int getNumRows();

        ImageArrayDocument* document;
        int imagePadSize;
        int scrollPos;
        int selectedImage;
    };

}
