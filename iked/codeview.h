
#ifndef CODEVIEW_H
#define CODEVIEW_H

#include <string>
#include <wx\wx.h>
#include <wx\stc\stc.h>

class CMainWnd;

class CCodeWnd : public wxMDIChildFrame
{
    wxStyledTextCtrl*   pTextctrl;
    std::string    sFilename;
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
    };

    CCodeWnd(CMainWnd* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,const char* fname=0);
    ~CCodeWnd();

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

    DECLARE_EVENT_TABLE()
};

#endif