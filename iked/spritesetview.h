
#pragma once

#include <wx/wx.h>
#include <wx/grid.h>

#include "docview.h"
#include "graph.h"
#include "spriteset.h"
#include "imagearraypanel.h"

namespace iked {

    class MovescriptEditor;

    struct SpriteSetView : DocumentPanel {
        SpriteSetView(MainWindow* parent, Document* doc, const std::string& fileName);
        ~SpriteSetView();

    private:
        void Render();  // blech

        void onSave(wxCommandEvent& event);
        void onSaveAs(wxCommandEvent& event);
        void onRightClickFrame(int frameIndex);
        //void onDoubleClickFrame(wxCommandEvent& event);
        void onEditFrame(wxCommandEvent& event);

        void onZoomIn(wxCommandEvent& event);
        void onZoomOut(wxCommandEvent& event);
        void onZoomNormal(wxCommandEvent& event);

        void onShowMovescriptEditor(wxCommandEvent& event);
        void onImportFrames(wxCommandEvent& event);

        void updateScrollbar();

        void init();
        void initMenu();
        void initAccelerators();

        SpriteSet*          getSprite();

        wxTextCtrl*         hotXEdit;
        wxTextCtrl*         hotYEdit;
        wxTextCtrl*         hotWidthEdit;
        wxTextCtrl*         hotHeightEdit;
        wxListBox*          animScriptGrid;
        wxListBox*          metaDataGrid;
        ImageArrayPanel*    imagePanel;
        
        wxMenu*             contextMenu;
        //MovescriptEditor*  moveScriptEditor;

        DECLARE_EVENT_TABLE()
    };

}
