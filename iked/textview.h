
#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "common/utility.h"
#include <wx\wx.h>

#ifdef WX232
    #include <wx\fdrepdlg.h>
#endif

#include <wx\stc\stc.h>
#include "docview.h"

class CMainWnd;

class CTextView : public IDocView
{
    public:

        CTextView(CMainWnd* parent, const std::string& name);

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
#ifdef WX232
        void OnFind(wxCommandEvent& event);
        void OnReplace(wxCommandEvent& event);
#endif

    private:

#ifdef WX232
        void DoFind(wxFindDialogEvent& event);
#endif
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

#endif // TEXTVIEW_H