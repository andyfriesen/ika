#ifndef SCRIPTDLG_H
#define SCRIPTDLG_H

#include "wxinc.h"
#include <vector>

struct MainWindow;
struct Script;

struct ScriptDlg : public wxDialog
{
    std::vector<Script*>& _scripts;
public:
    ScriptDlg(MainWindow* mainWnd);

    void OnAddScript(wxCommandEvent&);
    void OnReloadScript(wxCommandEvent&);
    void OnUnloadScript(wxCommandEvent&);
    void OnActivateScript(wxCommandEvent&);
    void OnSelectScript(wxCommandEvent& event);

private:
    void RefreshScriptList();

    wxListBox*    _scriptList;  // simple ease.  it's better than getting it through the XRC id every time.
    wxStaticText* _scriptDesc;  // ditto

    DECLARE_EVENT_TABLE()
};

#endif