#include "zoneeditor.h"
#include "map.h"
#include "main.h"

#include <wx / xrc / xmlres.h>

BEGIN_EVENT_TABLE(ZoneEditor, wxDialog)
    EVT_LISTBOX(XRCID("list_zones"), ZoneEditor::OnSelectZone)
    EVT_CLOSE(ZoneEditor::OnClose)

    EVT_BUTTON(XRCID("button_newzone"), ZoneEditor::OnNewZone)
    EVT_BUTTON(XRCID("button_delzone"), ZoneEditor::OnDelZone)
END_EVENT_TABLE()

void ZoneEditor::UpdateList()
{
    int pos = _zonelist->GetSelection();
    if (pos == -1) pos = 0;

    _zonelist->Clear();

    for (uint i = 0; i < _map->Zones().size(); i++)
        _zonelist->Append(_map->Zones()[i].name.c_str());

    _zonelist->SetSelection(pos);
    UpdateDlg();
}

void ZoneEditor::UpdateData()
{
    SMapZone& zone = _map->Zones()[_curzone];

    if (zone.name != _nameedit->GetValue().c_str())
        _zonelist->SetString(_curzone, _nameedit->GetValue().c_str());

    zone.name                  = _nameedit->GetValue().c_str();
    zone.sDescription           = _descedit->GetValue().c_str();
    zone.sActscript             = _scriptedit->GetValue().c_str();
    //zone.sEntactscript          = _entscriptedit->GetValue().c_str();
    zone.nActchance             = atoi(_chanceedit->GetValue().c_str());
    zone.nActdelay              = atoi(_delayedit->GetValue().c_str());
    zone.bAdjacentactivation    = _adjacentactivate->GetValue();
}

void ZoneEditor::UpdateDlg()
{
    SMapZone& zone = _map->Zones()[_curzone];

    _nameedit->SetValue(zone.name.c_str());
    _descedit->SetValue(zone.sDescription.c_str());
    _scriptedit->SetValue(zone.sActscript.c_str());
    _chanceedit->SetValue(ToString(zone.nActchance).c_str());
    _delayedit->SetValue(ToString(zone.nActdelay).c_str());
    _adjacentactivate->SetValue(zone.bAdjacentactivation);
}

ZoneEditor::ZoneEditor(CMapView* parent, Map* m)
    : _parent(parent)
    , _map(m)
    , _curzone(0)
{
    wxXmlResource::Get()->LoadDialog(this, (wxWindow*)parent, "dialog_zone");
    _zonelist = XRCCTRL(*this, "list_zones", wxListBox);
    _nameedit = XRCCTRL(*this, "edit_name", wxTextCtrl);
    _descedit = XRCCTRL(*this, "edit_description", wxTextCtrl);
    _scriptedit = XRCCTRL(*this, "edit_script", wxTextCtrl);
    _chanceedit = XRCCTRL(*this, "edit_chance", wxTextCtrl);
    _delayedit = XRCCTRL(*this, "edit_delay", wxTextCtrl);
    _adjacentactivate = XRCCTRL(*this, "check_adjactivate", wxCheckBox);

    wxSizer* sizer = XRCCTRL(*this, "panel_main", wxPanel)->GetSizer();
    _zonelist->SetSize(wxSize(_zonelist->GetSize().GetWidth(), sizer->GetSize().GetHeight()));
    sizer->Fit(this);
    
    UpdateList();
}

void ZoneEditor::OnSelectZone(wxCommandEvent& event)
{
    UpdateData();
    _curzone = _zonelist->GetSelection();
    UpdateDlg();
}

void ZoneEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    Show(false);
}

void ZoneEditor::OnNewZone(wxCommandEvent& event)
{
    SMapZone zone;
    zone.name = "New Zone";
    _map->Zones().push_back(zone);
    
    UpdateData();
    UpdateList();
    _curzone = _map->Zones().size() - 1;
    _zonelist->SetSelection(_curzone);
    UpdateDlg();
}

void ZoneEditor::OnDelZone(wxCommandEvent& event)
{
    int result = wxMessageBox("Are you sure?", "DANGER WILL ROBINSON", wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
    if (result != wxYES)
        return;

    if (_curzone != 0)
    {
        _map->Zones().erase(_map->Zones().begin() + _curzone);
        if (_curzone >= _map->Zones().size())
        {
            _curzone = 0;
            UpdateDlg();
            _zonelist->SetSelection(0);
        }
        UpdateList();

        // TODO: Have the map re - adjust itself so that all the indeces are still correct.
    }
    else
    {
        // Not allowed to delete the first zone.  Re - initialize it instead.
        SMapZone& zone = _map->Zones()[0];
        zone.name = "Default zone";
        zone.sDescription = "";
        zone.sActscript = "";
        zone.nActchance = 0;
        zone.nActdelay = 0;
        zone.bAdjacentactivation = false;

        UpdateDlg();
    }
}