#include "entitydlg.h"
#include "executor.h"
#include "common/utility.h"
#include "command.h"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(EntityDlg, wxDialog)
    EVT_BUTTON(wxID_OK, EntityDlg::OnOK)
    EVT_BUTTON(XRCID("button_delete"), EntityDlg::OnDelete)
END_EVENT_TABLE()

EntityDlg::EntityDlg(Executor* e, uint layer, uint index)
    : _executor(e)
    , _layer(layer)
    , _index(index)
{
    wxXmlResource::Get()->LoadDialog(this, e->GetParentWindow(), wxT("dialog_entity"));
    
    wxSizer* s = get<wxPanel>("panel_main")->GetSizer();
    wxASSERT(s);
    s->Fit(this);

    Map* map = e->GetMap();

    newData = map->GetLayer(layer)->entities[index];

    UpdateDlg();
}

void EntityDlg::UpdateData()
{
    newData.label           = get<wxTextCtrl>("edit_label")->GetValue().c_str();
    newData.spriteName      = get<wxTextCtrl>("edit_sprite")->GetValue().c_str();
    newData.speed           = atoi(get<wxTextCtrl>("edit_speed")->GetValue().c_str());
    newData.moveScript      = get<wxTextCtrl>("edit_movescript")->GetValue().c_str();
    newData.activateScript  = get<wxTextCtrl>("edit_script")->GetValue().c_str();
    newData.obstructsEntities = get<wxCheckBox>("check_obstructs")->GetValue();
    newData.obstructedByEntities = get<wxCheckBox>("check_obstructedbyentities")->GetValue();
    newData.obstructedByMap = get<wxCheckBox>("check_obstructedbymap")->GetValue();
    
    newData.x                 = atoi(get<wxTextCtrl>("edit_x")->GetValue().c_str());
    newData.y                 = atoi(get<wxTextCtrl>("edit_y")->GetValue().c_str());
}

void EntityDlg::UpdateDlg()
{
    get<wxTextCtrl>("edit_label")->SetValue(                newData.label.c_str());
    get<wxTextCtrl>("edit_sprite")->SetValue(               newData.spriteName.c_str());
    get<wxTextCtrl>("edit_speed")->SetValue(                toString(newData.speed).c_str());
    get<wxTextCtrl>("edit_movescript")->SetValue(           newData.moveScript.c_str());
    get<wxTextCtrl>("edit_script")->SetValue(               newData.activateScript.c_str());
    get<wxCheckBox>("check_obstructs")->SetValue(           newData.obstructsEntities);
    get<wxCheckBox>("check_obstructedbyentities")->SetValue(newData.obstructedByMap);
    get<wxCheckBox>("check_obstructedbymap")->SetValue(     newData.obstructedByEntities);

    get<wxTextCtrl>("edit_x")->SetValue(                    toString(newData.x).c_str());
    get<wxTextCtrl>("edit_y")->SetValue(                    toString(newData.y).c_str());
}

void EntityDlg::OnOK(wxCommandEvent& event)
{
    UpdateData();
    wxDialog::OnOK(event);
}

void EntityDlg::OnDelete(wxCommandEvent& event)
{
    if (wxMessageBox("Are you sure you want to destroy this entity?", "DESTRUCTOMATIC CONFIRMATION CODE REQUIRED", wxYES_NO, this))
    {
        _executor->HandleCommand(new DestroyEntityCommand(_layer, _index));
        EndModal(wxID_CANCEL);
    }
}