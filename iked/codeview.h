
#ifndef CODEVIEW_H
#define CODEVIEW_H

#include "types.h"
#include "wx\wx.h"
#include "wx\colordlg.h"
#include "wx\fontdlg.h"
#include "wx\fdrepdlg.h"
#include "wx\stc\stc.h"
#include "docview.h"

class CMainWnd;

class CCodeView : public IDocView
{
    wxStyledTextCtrl*   pTextctrl;

    void InitTextControl();
    void InitAccelerators();
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
        id_optionsfont,

        id_viewws,
    };

    CCodeView(CMainWnd* parent,const string& name);

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

    void OnMarginClick(wxStyledTextEvent& event);

    void OnSyntaxHighlighting(wxCommandEvent& event); // syntax highlighting dialog
    void OnViewWhiteSpace(wxCommandEvent& event);
private:

    void SetSyntax(int,wxCommandEvent&);

    void DoFind(wxFindDialogEvent& event);
public:

    DECLARE_EVENT_TABLE()
};

#endif