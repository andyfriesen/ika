
#pragma once

#include "docview.h"
#include "graph.h"

class wxMenu;

namespace iked {
    struct MainWindow;
    struct TileSet;
    struct ImageBank;

    struct TileSetPanel : DocumentPanel {
        TileSetPanel(MainWindow* parentwnd, const std::string& fname);
        TileSetPanel(MainWindow* parentwnd, int width, int height);
        ~TileSetPanel();

        void init();

        virtual void onSave(wxCommandEvent& event);
        void onSaveAs(wxCommandEvent& event);

        void onPaint();
        void onSize(wxSizeEvent& event);
        void onScroll(wxScrollWinEvent& event);

        void onLeftClick(wxMouseEvent& event);
        void onRightClick(wxMouseEvent& event);
        void onMouseWheel(wxMouseEvent& event);

        void onEraseBackground(wxEraseEvent&) {}

        //-----------------------------------

        void onImportTiles(wxCommandEvent&);

        void setZoomFactor(int factor);
        void onCutTile(wxCommandEvent&);
        void onCopyTile(wxCommandEvent&);
        void onInsertTile(wxCommandEvent&);
        void onPasteOver(wxCommandEvent&);
        void onInsertAndPaste(wxCommandEvent&);
        void onEditTile(wxCommandEvent&);
        void onZoomNormal(wxCommandEvent&);
        void onZoomIn(wxCommandEvent&);
        void onZoomOut(wxCommandEvent&);
        void onZoomIn2x(wxCommandEvent&);
        void onZoomOut2x(wxCommandEvent&);
        void onZoomIn4x(wxCommandEvent&);
        void onZoomOut4x(wxCommandEvent&);

        //-----------------------------------

        void render();
        void updateScrollbar();

        /// returns the tile under the specified client coordinates
        int  getTileAt(int x, int y) const;                 

        /// x and y recieve the position at which the specified tile is 
        /// rendered at
        void getTilePos(int t, int& x, int& y) const;        

        DECLARE_EVENT_TABLE()

    private:
        MainWindow*       parent;
        GraphicsFrame*    graph;

        TileSet*        tileSet;

        wxMenu*         contextMenu;

        int ywin;                       // scrollbar position
        bool pad;                       // pixel padding on / off

        int tileIndex;                  // used for the context menu; the tile index of the tile that was right - clicked
    };

}
