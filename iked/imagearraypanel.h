
#pragma once

#include "graph.h"
#include "common/listener.h"

namespace iked {

    struct ImageArrayDocument;
    struct Document;

    /**
     * Control that displays an array of images.
     * Used for tilesets, spritesets, and fonts.
     */
    struct ImageArrayPanel : GraphicsFrame {
        ImageArrayPanel(wxWindow* parent, ImageArrayDocument* resource);
        
        /**
         * @return Returns the image under x,y.  -1 if no image is there.
         */
        int getImageAtPos(int x, int y);

        /**
         * x and y recieve the position of the image with the given index.
         */
        void getImagePos(int index, int* x, int* y);

        ImageArrayDocument* getDocument() { return document; }

        Listener<int> leftClickImage;
        Listener<int> rightClickImage;
        Listener<int> doubleClickImage;

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

        void onDocumentDestroyed(Document* doc); // gross

        void updateScrollbar();

        int getNumCols();
        int getNumRows();

        ImageArrayDocument* document;
        int imagePadSize;
        int scrollPos;
        int selectedImage;
    };

}
