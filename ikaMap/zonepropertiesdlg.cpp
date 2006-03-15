
#include "wxinc.h"
#include "zonepropertiesdlg.h"
#include "common/map.h"
#include "mainwindow.h"
#include "common/utility.h"
#include "command.h"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(ZonePropertiesDlg, wxDialog)
    EVT_LISTBOX(XRCID("list_blueprints"), ZonePropertiesDlg::OnSelectBlueprint)
    EVT_CLOSE(ZonePropertiesDlg::OnClose)

    EVT_BUTTON(XRCID("button_new"), ZonePropertiesDlg::OnNewBlueprint)
    EVT_BUTTON(XRCID("button_delete"), ZonePropertiesDlg::OnDeleteBlueprint)
    EVT_BUTTON(XRCID("button_rename"), ZonePropertiesDlg::OnRenameBlueprint)
    EVT_BUTTON(XRCID("button_deletezone"), ZonePropertiesDlg::OnDeleteZone)
    EVT_BUTTON(wxID_OK, ZonePropertiesDlg::OnOk)
END_EVENT_TABLE()

Map* ZonePropertiesDlg::GetMap() const {
    return _executor->GetMap();
}

void ZonePropertiesDlg::UpdateList() {
    Map* map = GetMap();
    std::string pos = _bluePrintList->GetStringSelection().c_str();

    _bluePrintList->Clear();
    if (map->zones.empty()) {
        return;
    }

    for (Map::ZoneMap::iterator
        iter  = map->zones.begin();
        iter != map->zones.end();
        iter++
    ) {
        _bluePrintList->Append(iter->second.label.c_str());
    }

    if (!pos.empty() && _bluePrintList->FindString(pos.c_str()) != -1) {
        _bluePrintList->SetStringSelection(pos.c_str());
    }
}

// So much work for something that should be so simple. -_-
void ZonePropertiesDlg::UpdateData() {
    Map* map = GetMap();
    Map::ZoneMap& bPrints = map->zones; // not hungarian. -_-

    Map::Layer::Zone& zone = GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex];
    Map::Layer::Zone newZone = zone;

    std::vector<Command*> commands;
    if (zone.label.empty()) {
        // -- cheap hack --

        const char* def = "Default Zone";
        // Create a default blueprint, if necessary
        if (!bPrints.count(def)) {
            Map::Zone bp;
            bp.label = def;
            commands.push_back(new DefineZoneBluePrintCommand(bp, 0));

            // EEP.  Need to do this, otherwise the dialog won't update the command.
            // (because it hasn't been performed yet)
            // Probably some issues with undo here, as the Do() method typically
            // reads the current data, and stores it.  When ikaMap itself handles
            // these commands, it will Do() them again, erasing the previous (real)
            // values.
            commands.back()->Do(_executor);
        }

        UpdateList();
        _bluePrintList->SetStringSelection(def);

        newZone.label = def;
    }

    if (_editScript->GetValue().c_str() != bPrints[newZone.label].scriptName) {
        Map::Zone& oldBp = bPrints[zone.label];
        Map::Zone newBp = oldBp;
        newBp.scriptName = _editScript->GetValue().c_str();
        commands.push_back(new DefineZoneBluePrintCommand(newBp, oldBp));
    }

    newZone.position.left   = atoi(_editX->GetValue().c_str());
    newZone.position.top    = atoi(_editY->GetValue().c_str());
    newZone.position.right  = newZone.position.left + atoi(_editWidth->GetValue().c_str());
    newZone.position.bottom = newZone.position.top  + atoi(_editHeight->GetValue().c_str());
    newZone.position.Normalize();   // just in case
    commands.push_back(new ChangeZoneCommand(_layerIndex, _zoneIndex, newZone));

    _executor->HandleCommand(new CompositeCommand(commands));
}

void ZonePropertiesDlg::UpdateDlg() {
    Map::Layer::Zone& zone = GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex];
    if (!zone.label.empty() && GetMap()->zones.count(zone.label) != 0) {
        Map::Zone& bluePrint =   GetMap()->zones[zone.label];

        _editLabel->SetValue(bluePrint.label.c_str());
        _editScript->SetValue(bluePrint.scriptName.c_str());
        _bluePrintList->SetStringSelection(bluePrint.label.c_str());
    }

    _editX->SetValue(toString(zone.position.left).c_str());
    _editY->SetValue(toString(zone.position.top).c_str());
    _editWidth->SetValue(toString(zone.position.Width()).c_str());
    _editHeight->SetValue(toString(zone.position.Height()).c_str());
}

ZonePropertiesDlg::ZonePropertiesDlg(Executor* e, uint layerIndex, uint zoneIndex)
    : _executor(e)
    , _layerIndex(layerIndex)
    , _zoneIndex(zoneIndex)
{
    wxXmlResource::Get()->LoadDialog(this, e->GetParentWindow(), "dialog_zone");

    wxSizer* sizer = XRCCTRL(*this, "panel_main", wxPanel)->GetSizer();
    sizer->Fit(this);

    _bluePrintList = XRCCTRL(*this, "list_blueprints", wxListBox);
    _editLabel = XRCCTRL(*this, "edit_label", wxTextCtrl);
    _editScript = XRCCTRL(*this, "edit_script", wxTextCtrl);
    _editX = XRCCTRL(*this, "edit_x", wxTextCtrl);
    _editY = XRCCTRL(*this, "edit_y", wxTextCtrl);
    _editWidth = XRCCTRL(*this, "edit_width", wxTextCtrl);
    _editHeight = XRCCTRL(*this, "edit_height", wxTextCtrl);

    UpdateList();
    UpdateDlg();
}

void ZonePropertiesDlg::OnSelectBlueprint(wxCommandEvent& event) {
    Map* map = GetMap();
    Map::Layer::Zone& zone = map->GetLayer(_layerIndex)->zones[_zoneIndex];
    wxListBox* zoneList = _bluePrintList;

    if (!zone.label.empty())
        UpdateData();
    zone.label = zoneList->GetStringSelection().c_str();
    UpdateDlg();
}

void ZonePropertiesDlg::OnClose(wxCloseEvent&) {
    UpdateData();
    Show(false);
}

void ZonePropertiesDlg::OnNewBlueprint(wxCommandEvent&) {
    Map::ZoneMap& zones = GetMap()->zones;

    // Get a unique blueprint label
    std::string label = "New Zone";
    int i = 0;
    while (zones.count(label) != 0) {
        label = va("New Zone %i", i);
        i++;
    }

    Map::Zone newZone;
    newZone.label = label;

    GetMap()->zones[label] = newZone;
    GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex].label = label;
    UpdateList();
    UpdateDlg();
}

void ZonePropertiesDlg::OnDeleteBlueprint(wxCommandEvent&) {
    int result = wxMessageBox(
        "Are you sure you want to remove this blueprint?\n"
        "This is not the same as deleting the zone!",
        "All the cool kids are doin' it.",
        wxYES_NO | wxCENTRE | wxICON_QUESTION,
        this
    );

    if (result != wxYES) {
        return;
    }

    Map* map = GetMap();
    Map::Layer* layer = map->GetLayer(_layerIndex);
    std::string label = _bluePrintList->GetStringSelection().c_str();

    std::vector<Command*> commands;
    commands.push_back(new DefineZoneBluePrintCommand(0, map->zones[label]));

    uint i = 0;
    int j = 0;
    for (uint i = 0; i < GetMap()->NumLayers(); i++) {
        std::vector<Map::Layer::Zone>& zones = GetMap()->GetLayer(i)->zones;

        for (uint j = 0; j < zones.size(); j++) {
            if (zones[j].label == label) {
                Map::Layer::Zone newZone = zones[j];
                newZone.label = ""; // I guess.  I dunno what else to do, except delete the zone. (which is kind of annoying)
                commands.push_back(new ChangeZoneCommand(i, j, newZone));
            }
        }
    }

    _executor->HandleCommand(new CompositeCommand(commands));

    UpdateDlg();
}

void ZonePropertiesDlg::OnRenameBlueprint(wxCommandEvent&) {
    std::string newLabel = _editLabel->GetValue().c_str();
    Map::Layer::Zone& zone = GetMap()->GetLayer(_layerIndex)->zones[_zoneIndex];

    if (newLabel.empty() || newLabel == zone.label || zone.label == "") { // Homie don't play that game.
        return;
    }

    if (GetMap()->zones.count(newLabel) != 0) {
        wxMessageBox(
            va(
                "The blueprint label %s is already taken.\n"
                "You'll have to pick a unique label",
                newLabel.c_str()
            ),
            "Blueprint labels must be unique",
            wxOK | wxCENTRE,
            this
        );
        return;
    }

    std::string oldLabel = zone.label;

    // weee.
    GetMap()->zones[newLabel] = GetMap()->zones[oldLabel];  // copy
    GetMap()->zones[newLabel].label = newLabel;             // update
    GetMap()->zones.erase(oldLabel);                        // remove the old version

    // Next up, update the zones themselves, so that names point to the right place.
    std::vector<Command*> commands;
    for (uint i = 0; i < GetMap()->NumLayers(); i++) {
        std::vector<Map::Layer::Zone>& zones = GetMap()->GetLayer(i)->zones;
        for (uint j = 0; j < zones.size(); j++) {
            if (zones[j].label == oldLabel) {
                Map::Layer::Zone newZone = zones[j];
                newZone.label = newLabel;
                commands.push_back(new ChangeZoneCommand(i, j, newZone));
            }
        }
    }
    _executor->HandleCommand(new CompositeCommand(commands));

    UpdateList();
    _bluePrintList->SetStringSelection(newLabel.c_str());
}

void ZonePropertiesDlg::OnDeleteZone(wxCommandEvent&) {
    if (wxMessageBox(
            "Are you sure you want to remove this zone?",
            "KILL",
            wxYES_NO | wxCENTER | wxICON_QUESTION,
            this
        ) == wxYES
    ) {
        _executor->HandleCommand(new DestroyZoneCommand(_layerIndex, _zoneIndex));
        EndModal(wxOK);
    }
}

void ZonePropertiesDlg::OnOk(wxCommandEvent& event) {
    UpdateData();
    wxDialog::OnOK(event);
}