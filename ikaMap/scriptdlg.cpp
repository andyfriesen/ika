#include "scriptdlg.h"
#include "mainwindow.h"
#include "script.h"
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(ScriptDlg, wxDialog)
    EVT_BUTTON(XRCID("button_addscript"), ScriptDlg::OnAddScript)
    EVT_BUTTON(XRCID("button_reloadscript"), ScriptDlg::OnReloadScript)
    EVT_BUTTON(XRCID("button_unloadscript"), ScriptDlg::OnUnloadScript)
    EVT_LISTBOX(XRCID("list_scripts"), ScriptDlg::OnSelectScript)
END_EVENT_TABLE()

ScriptDlg::ScriptDlg(MainWindow* mainWnd)
    : _scripts(mainWnd->GetScripts())
{
    wxXmlResource::Get()->LoadDialog(this, mainWnd, wxT("dialog_scripts"));

    _scriptList = XRCCTRL(*this, "list_scripts", wxListBox);
    _scriptDesc = XRCCTRL(*this, "static_scriptdesc", wxStaticText);

    assert(_scriptList);
    assert(_scriptDesc);

    RefreshScriptList();
}

void ScriptDlg::OnAddScript(wxCommandEvent&)
{
    wxFileDialog dlg(
        this,
        "Import script",
        "", // default dir
        "", // default filename
        "Python Scripts (*.py)|*.py|All Files (*.*)|*.*",
        wxOPEN | wxFILE_MUST_EXIST
        );

    int result = dlg.ShowModal();
    if (result == wxID_OK)
    {
        try
        {
            Script* newScript = new Script(dlg.GetPath().c_str());
            _scripts.push_back(newScript);
            RefreshScriptList();
        }
        catch (std::runtime_error err)
        {
            ::wxMessageBox(err.what(), "Import error", wxOK | wxCENTER | wxICON_ERROR, this);
        }
    }
}

void ScriptDlg::OnReloadScript(wxCommandEvent&)
{
    ::wxMessageBox("Not yet implemented", "Sorry!");
}

void ScriptDlg::OnUnloadScript(wxCommandEvent&)
{
    int index = _scriptList->GetSelection();
    if (index < 0 || index > _scriptList->GetCount())   return;

    Script* script = reinterpret_cast<Script*>(_scriptList->GetClientData(index));

    for (std::vector<Script*>::iterator
        iter = _scripts.begin();
        iter != _scripts.end();
        iter++)
    {
        if (script == *iter)
        {
            _scripts.erase(iter);
            delete script;
            script = 0;
            break;
        }
    }

    wxASSERT(script == 0);

    RefreshScriptList();
}

void ScriptDlg::OnSelectScript(wxCommandEvent& event)
{
    Script* script = reinterpret_cast<Script*>(_scriptList->GetClientData(event.GetInt()));

    _scriptDesc->SetLabel(script->GetDesc().c_str());
}

void ScriptDlg::RefreshScriptList()
{
    _scriptList->Clear();

    for (uint i = 0; i < _scripts.size(); i++)
    {
        _scriptList->Append(_scripts[i]->GetName().c_str(), _scripts[i]);
    }

    _scriptDesc->SetLabel("");
}