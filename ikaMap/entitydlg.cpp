#include "entitydlg.h"
#include "mainwindow.h"
#include "misc.h"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(EntityDlg, wxDialog)
    EVT_BUTTON(wxID_OK, EntityDlg::OnOK)
END_EVENT_TABLE()

EntityDlg::EntityDlg(MainWindow* mw, uint layer, uint index)
    : _mainWnd(mw)
    , _layer(layer)
    , _index(index)
{
    wxXmlResource::Get()->LoadDialog(this, mw, wxT("dialog_entity"));
    
    wxSizer* s = get<wxPanel>("panel_main")->GetSizer();
    wxASSERT(s);
    s->Fit(this);

    Map* map = mw->GetMap();

    newData = map->GetLayer(layer).entities[index];
    newBlueprint = map->entities[newData.bluePrint];

    UpdateDlg();
}

void EntityDlg::UpdateData()
{
    newData.label      = get<wxTextCtrl>("edit_label")->GetValue().c_str();
    newBlueprint.spriteName      = get<wxTextCtrl>("edit_sprite")->GetValue().c_str();
    newBlueprint.speed           = atoi(get<wxTextCtrl>("edit_speed")->GetValue().c_str());
    newBlueprint.moveScript      = get<wxTextCtrl>("edit_movescript")->GetValue().c_str();
    newBlueprint.activateScript  = get<wxTextCtrl>("edit_script")->GetValue().c_str();
    newBlueprint.obstructsEntities = get<wxCheckBox>("check_obstructs")->GetValue();
    newBlueprint.obstructedByEntities = get<wxCheckBox>("check_obstructedbyentities")->GetValue();
    newBlueprint.obstructedByMap = get<wxCheckBox>("check_obstructedbymap")->GetValue();
    
    newData.x                 = atoi(get<wxTextCtrl>("edit_x")->GetValue().c_str());
    newData.y                 = atoi(get<wxTextCtrl>("edit_y")->GetValue().c_str());
}

void EntityDlg::UpdateDlg()
{
    get<wxTextCtrl>("edit_label")->SetValue(                newData.label.c_str());
    get<wxTextCtrl>("edit_sprite")->SetValue(               newBlueprint.spriteName.c_str());
    get<wxTextCtrl>("edit_speed")->SetValue(                ToString(newBlueprint.speed).c_str());
    get<wxTextCtrl>("edit_movescript")->SetValue(           newBlueprint.moveScript.c_str());
    get<wxTextCtrl>("edit_script")->SetValue(               newBlueprint.activateScript.c_str());
    get<wxCheckBox>("check_obstructs")->SetValue(           newBlueprint.obstructsEntities);
    get<wxCheckBox>("check_obstructedbyentities")->SetValue(newBlueprint.obstructedByMap);
    get<wxCheckBox>("check_obstructedbymap")->SetValue(     newBlueprint.obstructedByEntities);

    get<wxTextCtrl>("edit_x")->SetValue(                    ToString(newData.x).c_str());
    get<wxTextCtrl>("edit_y")->SetValue(                    ToString(newData.y).c_str());
}

void EntityDlg::OnOK(wxCommandEvent& event)
{
    UpdateData();
    wxDialog::OnOK(event);
}