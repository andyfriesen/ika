#if 0
#pragma once

#include "common/utility.h"
#include <wx/wx.h>
#include <wx\fdrepdlg.h>

#include <wx\stc\stc.h>
#include "docview.h"

class MainWindow;

class CTextView : public DocumentPanel
{
    public:

        CTextView(MainWindow* parent, const std::string& name);

        void OnStyleNeeded(wxStyledTextEvent& event);
        void OnCharAdded(wxStyledTextEvent& event);
    
        virtual void OnSave(wxCommandEvent& event);
        void OnSaveAs(wxCommandEvent& event);

        virtual const void* GetResource() const;

        void OnUndo(wxCommandEvent& event);
        void OnRedo(wxCommandEvent& event);
        void OnCopy(wxCommandEvent& event);
        void OnCut (wxCommandEvent& event);
        void OnPaste(wxCommandEvent& event);
        void OnSelectAll(wxCommandEvent& event);
        void OnFind(wxCommandEvent& event);
        void OnReplace(wxCommandEvent& event);

    private:

        void DoFind(wxFindDialogEvent& event);

        enum
        {
            id_ed = 100,
            id_filesave,
            id_filesaveas,
            id_fileclose,

            id_editundo,
            id_editredo,
            id_editcopy,
            id_editcut,
            id_editpaste,
            id_editselectall,
            id_editfind,
            id_editreplace
        };

        wxStyledTextCtrl*   pTextctrl;

        void InitTextControl();
        void InitAccelerators();

        DECLARE_EVENT_TABLE()
};
#endif
