
#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

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
        virtual void deleteDocument(Document* doc);

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

        void refresh();             // copies sprite data onto the form
        void refreshSprite();       // copies form data into the dialog

        SpriteSet*          getSprite();

        wxTextCtrl*         hotXEdit;
        wxTextCtrl*         hotYEdit;
        wxTextCtrl*         hotWidthEdit;
        wxTextCtrl*         hotHeightEdit;
        wxListCtrl*          animScriptGrid;
        wxListCtrl*          metaDataGrid;
        ImageArrayPanel*    imagePanel;
        
        wxMenu*             contextMenu;
        //MovescriptEditor*  moveScriptEditor;

        DECLARE_EVENT_TABLE()
    };

}
