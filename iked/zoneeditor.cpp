#include "zoneeditor.h"
#include "map.h"
#include "main.h"

enum
{
    id_zonelist = 100,
    id_newzone,
    id_delzone
};

BEGIN_EVENT_TABLE(ZoneEditor, wxDialog)
    EVT_LISTBOX(id_zonelist, ZoneEditor::OnSelectZone)
    EVT_CLOSE(ZoneEditor::OnClose)

    EVT_BUTTON(id_newzone, ZoneEditor::OnNewZone)
    EVT_BUTTON(id_delzone, ZoneEditor::OnDelZone)
END_EVENT_TABLE()

void ZoneEditor::UpdateList()
{
    int pos = _zonelist->GetSelection();
    if (pos == -1) pos = 0;

    _zonelist->Clear();

    for (uint i = 0; i < _map->Zones().size(); i++)
    {
        _zonelist->Append(_map->Zones()[i].name.c_str());
    }

    _zonelist->SetSelection(pos);
    UpdateDlg();
}

void ZoneEditor::UpdateData()
{
    SMapZone& zone = _map->Zones()[_curzone];

    //if (zone.name != _nameedit->GetValue().c_str())
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

void ZoneEditor::InitControls()
{
    const int column[] =
    {
        150, 220, 275, 290
    };

    const int line[] =
    {
        5, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205, 225,
    };

    const wxSize editsize(50, 20);
    const wxSize bigedit(120, 20);
    const wxSize labelsize(70, 20);
    const wxSize smalllabel(14, 20);

    _zonelist = new wxListBox(this, id_zonelist, wxPoint(0, 0), wxSize(140, 145));
    _nameedit = new wxTextCtrl(this, -1, "",    wxPoint(column[1], line[0]), bigedit);
    _descedit = new wxTextCtrl(this, -1, "",    wxPoint(column[1], line[1]), bigedit);
    _scriptedit = new wxTextCtrl(this, -1, "",  wxPoint(column[1], line[2]), bigedit);
    _chanceedit = new wxTextCtrl(this, -1, "",  wxPoint(column[1], line[3]), bigedit);
    _delayedit = new wxTextCtrl(this, -1, "",   wxPoint(column[1], line[4]), bigedit);
    _adjacentactivate = new wxCheckBox(this, -1, "&Adjacent activation", wxPoint(column[0], line[5]), bigedit);

    new wxStaticText(this, -1, "Name",          wxPoint(column[0], line[0]), labelsize, wxALIGN_RIGHT);
    new wxStaticText(this, -1, "Description",   wxPoint(column[0], line[1]), labelsize, wxALIGN_RIGHT);
    new wxStaticText(this, -1, "Script",        wxPoint(column[0], line[2]), labelsize, wxALIGN_RIGHT);
    //new wxStaticText(this, -1, "Entity Script", wxPoint(column[0], line[3]), labelsize, wxALIGN_RIGHT);
    new wxStaticText(this, -1, "Chance (%)",    wxPoint(column[0], line[3]), labelsize, wxALIGN_RIGHT);
    new wxStaticText(this, -1, "Delay",         wxPoint(column[0], line[4]), labelsize, wxALIGN_RIGHT);

    new wxButton(this, id_newzone, "&New Zone", wxPoint(column[0], line[6]), labelsize);
    new wxButton(this, id_delzone, "&Delete",   wxPoint(column[1], line[6]), labelsize);
}

ZoneEditor::ZoneEditor(CMapView* parent, Map* m)
    : wxDialog((wxWindow*)parent, -1, "Zones", wxDefaultPosition, wxSize(350, 180))
    , _parent(parent)
    , _map(m)
    , _curzone(0)
{
    InitControls();
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

        // TODO: Have the map re-adjust itself so that all the indeces are still correct.
    }
    else
    {
        // Not allowed to delete the first zone.  Re-initialize it instead.
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