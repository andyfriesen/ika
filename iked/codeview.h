
#ifndef CODEVIEW_H
#define CODEVIEW_H

#include "types.h"
#include <wx\wx.h>
#include <wx\colordlg.h>
#include <wx\fontdlg.h>
#ifdef WX232
#include <wx\fdrepdlg.h>
#endif
#include <wx\stc\stc.h>
#include "docview.h"

class CMainWnd;

class CCodeView : public IDocView//wxMDIChildFrame
{
    wxStyledTextCtrl*   pTextctrl;
    bool                bChanged;
public:

    enum        // commands specific to this window
    {
        id_ed=100,
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
        id_editreplace,

        id_options,
        id_optionsws,
        id_optionsfont
    };

    CCodeView(CMainWnd* parent,const string& name);

    void OnStyleNeeded(wxStyledTextEvent& event);
    void OnCharAdded(wxStyledTextEvent& event);
    
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);

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

    void OnMarginClick(wxStyledTextEvent& event);

    void OnSyntaxHighlighting(wxCommandEvent& event); // syntax highlighting dialog
    void OnViewWhiteSpace(wxCommandEvent& event);
private:

    void SetSyntax(int,wxCommandEvent&);

#ifdef WX232
    void DoFind(wxFindDialogEvent& event);
#endif
public:

    DECLARE_EVENT_TABLE()
};

#endif