
#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "docview.h"
#include "graph.h"
#include "spriteset.h"
#include "imagearraypanel.h"
#include "commands/spritecommands.h"

namespace iked {

    struct ListControl;

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

        void onNewAnimScript(wxCommandEvent& event);
        void onDestroyAnimScript(wxCommandEvent& event);
        void onSelectAnimScript(wxCommandEvent& event);
        void onEditAnimScript(wxListEvent& event);

        void onNewMetaData(wxCommandEvent& event);
        void onDestroyMetaData(wxCommandEvent& event);
        void onSelectMetaData(wxCommandEvent& event);

        void onDestroyFrame(wxCommandEvent& event);

        void onSpriteSetChanged(SpriteSet* spriteSet);

        void updateScrollbar();

        void init();
        void initMenu();
        void initAccelerators();

        void refresh();             // copies sprite data onto the form
        void refreshSprite();       // copies form data into the dialog

        void importFrames(
            const std::string& fileName, 
            int frameWidth, 
            int frameHeight, 
            int rowSize, 
            int numFrames,
            bool pad,
            commands::ImportMode importMode
        );


        SpriteSet*          getSprite() const;

        wxTextCtrl*         hotXEdit;
        wxTextCtrl*         hotYEdit;
        wxTextCtrl*         hotWidthEdit;
        wxTextCtrl*         hotHeightEdit;
        ListControl*        animScriptGrid;
        ListControl*        metaDataGrid;
        ImageArrayPanel*    imagePanel;
        
        wxMenu*             contextMenu;
        //MovescriptEditor*  moveScriptEditor;

        // state flags because wxListCtrl doesn't have a GetSelection() method. :P
        std::string curAnimScript;
        std::string curMetaData;

        DECLARE_EVENT_TABLE()
    };

}
