
#include <cassert>

#include "docview.h"
#include "main.h"

namespace iked {

    BEGIN_EVENT_TABLE(DocumentPanel, wxMDIChildFrame)
        EVT_CLOSE(DocumentPanel::onClose)
    END_EVENT_TABLE()

    DocumentPanel::DocumentPanel(MainWindow* parentWindow, Document* doc, const std::string& fname, int style)
        : wxMDIChildFrame(parentWindow, -1, fname.c_str(), wxDefaultPosition, wxDefaultSize)
        , parent(parentWindow)
        , document(doc)
        , name(fname)
        , isChanged(false)
    {
        assert(document != 0);
        document->ref();

        SetTitle(fname.length()?
                fname.c_str()   :   "Untitled"
        );
    }

    DocumentPanel::~DocumentPanel() {
        // ??? :P
    }

    void DocumentPanel::setName(const std::string& newName) {
        name = newName;
    }

    const std::string& DocumentPanel::getName() const {
        return name;
    }

    Document* DocumentPanel::getDocument() { 
        return document; 
    }

    void DocumentPanel::saveDocument(const std::string& fileName) {
        document->save(fileName);
    }

    void DocumentPanel::onClose(wxCloseEvent& event) {
        if (document == 0) { // it can happen!!! @_@;
            return;
        }

        if (isChanged) {
            // FIXME: wtf.  Why make a temporary instance like this? :P
            wxMessageDialog msgdlg(
                    this,
                    "This file has been modified. Save?",
                    name.c_str(),
                    wxYES_NO | wxCANCEL | wxICON_QUESTION,
                    wxDefaultPosition
                );

            int result = msgdlg.ShowModal();

            if (result == wxID_YES) {
                // Yes: save.  Let the event go
                wxCommandEvent dummy;
                onSave(dummy);
            } else if (result == wxID_NO) {
                // No: just go.
                ;
            } else if (result == wxID_CANCEL) {
                // Cancel: suppress the event.
                event.Veto();
                return;
            } else {
                // wtf: panic.
                wxASSERT(false);
            }
        }
        
        deleteDocument(document);
        document = 0;
        parent->OnChildClose(this);
        event.Skip();
    }


    void DocumentPanel::onSave(wxCommandEvent& event) {
    }
}