
#pragma once

#include "common/utility.h"
#include "wx/wx.h"

namespace iked {

    struct MainWindow;
    struct Document;

    /**
     * Base class for all document windows.
     * Carries with it the notion that a document can be saved, closed, and mutated.
     *
     * Also undo stack etc.
     *
     * TODO: extend wxPanel, not MDIChildFrame
     */
    struct DocumentPanel : public wxMDIChildFrame {
        DocumentPanel(MainWindow* parentWindow, Document* doc, const std::string& fileName, int style=wxDEFAULT_FRAME_STYLE);
        virtual ~DocumentPanel();

        void setName(const std::string& newName);
        const std::string& getName() const;

        virtual Document* getDocument() const;

        virtual void saveDocument(const std::string& fileName);
        void saveDocument();

        void onClose(wxCloseEvent& event);
        virtual void onSave(wxCommandEvent& event);

        void onUndo(wxCommandEvent& event);
        void onRedo(wxCommandEvent& event);

        MainWindow* getParent() { return parent; }

        DECLARE_EVENT_TABLE()

    private:
        virtual void deleteDocument(Document* doc) = 0;

        MainWindow* parent;
        Document* document;
        bool isChanged;

        std::string name;
    };

}

