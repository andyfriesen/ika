#if 0
#include "main.h"
#include "mapview.h"
#include "entityeditor.h"
#include "map.h"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(EntityEditor, wxDialog)
    EVT_LISTBOX(XRCID("list_entities"), EntityEditor::OnSelectEntity)
    EVT_CLOSE(EntityEditor::OnClose)
    EVT_BUTTON(wxID_OK, EntityEditor::OnClose)
END_EVENT_TABLE()

EntityEditor::EntityEditor(MapView* parent, Map* m)
    : _parent(parent)
    , _map(m)
    , _curEnt(0)
{
    wxXmlResource::Get()->LoadDialog(this, _parent, wxT("dialog_entity"));
    //_entList = XRCCTRL(*this, "list_entities", wxListBox);
    wxSizer* s = XRCCTRL(*this, "panel_main", wxPanel)->GetSizer();

    // Stretch out the entity list to take up the height of the whole dialog.
    //_entList->SetSize(wxSize(_entList->GetSize().GetWidth(), s->GetSize().GetHeight()));
    //UpdateList();

    s->Fit(this);
}

void EntityEditor::InitControls()
{
}

void EntityEditor::UpdateList()
{
    /*int nCurrent = _entList->GetSelection();

    _entList->Clear();

    for (uint i = 0; i < _map->NumEnts(); i++)
    {
        const SMapEntity& e = _map->GetEntity(i);

        _entList->Append(e.name.c_str());
    }

    if (nCurrent != -1)
        _entList->SetSelection(nCurrent);
    nCurentidx=-1;
    UpdateDlg();*/
}

void EntityEditor::UpdateData()
{
    if (!_curEnt)   return;

    Map::Entity& ent    = _map->entities[_curEnt->bluePrint];
    _curEnt->label      = get<wxTextCtrl>("edit_label")->GetValue().c_str();
    ent.spriteName      = get<wxTextCtrl>("edit_sprite")->GetValue().c_str();
    ent.speed           = atoi(get<wxTextCtrl>("edit_speed")->GetValue().c_str());
    ent.moveScript      = get<wxTextCtrl>("edit_movescript")->GetValue().c_str();
    ent.activateScript  = get<wxTextCtrl>("edit_script")->GetValue().c_str();
    ent.obstructsEntities = get<wxCheckBox>("check_obstructs")->GetValue();
    ent.obstructedByEntities = get<wxCheckBox>("check_obstructedbyentities")->GetValue();
    ent.obstructedByMap = get<wxCheckBox>("check_obstructedbymap")->GetValue();
    
    _curEnt->x                 = atoi(get<wxTextCtrl>("edit_x")->GetValue().c_str());
    _curEnt->y                 = atoi(get<wxTextCtrl>("edit_y")->GetValue().c_str());
}

void EntityEditor::UpdateDlg()
{
    if (!_curEnt) return;

    if (_map->entities.count(_curEnt->bluePrint))
    {
        Map::Entity& ent = _map->entities[_curEnt->bluePrint];
        get<wxTextCtrl>("edit_label")->SetValue(                _curEnt->label.c_str());
        get<wxTextCtrl>("edit_sprite")->SetValue(               ent.spriteName.c_str());
        get<wxTextCtrl>("edit_speed")->SetValue(                toString(ent.speed).c_str());
        get<wxTextCtrl>("edit_movescript")->SetValue(           ent.moveScript.c_str());
        get<wxTextCtrl>("edit_script")->SetValue(               ent.activateScript.c_str());
        get<wxCheckBox>("check_obstructs")->SetValue(           ent.obstructsEntities);
        get<wxCheckBox>("check_obstructedbyentities")->SetValue(ent.obstructedByMap);
        get<wxCheckBox>("check_obstructedbymap")->SetValue(     ent.obstructedByEntities);
    }

    get<wxTextCtrl>("edit_x")->SetValue(toString(_curEnt->x).c_str());
    get<wxTextCtrl>("edit_y")->SetValue(toString(_curEnt->y).c_str());
}

void EntityEditor::OnSelectEntity(wxCommandEvent& event)
{
    /*UpdateData();
    nCurentidx = event.GetInt();
    UpdateDlg();*/
}

void EntityEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    OnOK(event);
}

void EntityEditor::Show(Map::Layer::Entity* ent)
{
    _curEnt = ent;
    UpdateDlg();
    wxDialog::Show(true);
}

#endif
