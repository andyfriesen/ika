
#include "main.h"
#include "mapview.h"
#include "entityeditor.h"
#include "map.h"

#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(CEntityEditor, wxDialog)
    EVT_LISTBOX(XRCID("list_entities"), CEntityEditor::OnSelectEntity)
    EVT_CLOSE(CEntityEditor::OnClose)
END_EVENT_TABLE()

CEntityEditor::CEntityEditor(CMapView* parent, Map* m)
    : pParent(parent)
    , pMap(m)
{
    wxXmlResource::Get()->LoadDialog(this, pParent, wxT("dialog_entity"));
    pEntlist = XRCCTRL(*this, "list_entities", wxListBox);
    wxSizer* s = XRCCTRL(*this, "panel_main", wxPanel)->GetSizer();

    // Stretch out the entity list to take up the height of the whole dialog.
    pEntlist->SetSize(wxSize(pEntlist->GetSize().GetWidth(), s->GetSize().GetHeight()));
    UpdateList();

    s->Fit(this);
}

void CEntityEditor::InitControls()
{
}

void CEntityEditor::UpdateList()
{
    int nCurrent = pEntlist->GetSelection();

    pEntlist->Clear();

    for (int i = 0; i < pMap->NumEnts(); i++)
    {
        const SMapEntity& e = pMap->GetEntity(i);

        pEntlist->Append(e.name.c_str());
    }

    if (nCurrent != -1)
        pEntlist->SetSelection(nCurrent);
    nCurentidx=-1;
    UpdateDlg();
}

void CEntityEditor::UpdateData()
{
    if (nCurentidx==-1) return;

    SMapEntity& e = pMap->GetEntity(nCurentidx);

    if (e.name!=get < wxTextCtrl>("edit_name")->GetValue().c_str())
    {
        wxString bleh = get < wxTextCtrl>("edit_name")->GetValue().c_str();
        pEntlist->InsertItems(1, &bleh, nCurentidx);
        pEntlist->Delete(nCurentidx + 1);
    }

    e.name              = get < wxTextCtrl>("edit_name")->GetValue().c_str();
    e.x                 = atoi(get < wxTextCtrl>("edit_x")->GetValue().c_str());
    e.y                 = atoi(get < wxTextCtrl>("edit_y")->GetValue().c_str());
    e.sCHRname          = get < wxTextCtrl>("edit_sprite")->GetValue().c_str();
    e.nSpeed            = atoi(get < wxTextCtrl>("edit_y")->GetValue().c_str());
    e.sActscript        = get < wxTextCtrl>("edit_script")->GetValue().c_str();
    e.sMovescript       = get < wxTextCtrl>("edit_movescript")->GetValue().c_str();
    e.nWandersteps      = atoi(get < wxTextCtrl>("edit_steps")->GetValue().c_str());
    e.nWanderdelay      = atoi(get < wxTextCtrl>("edit_delay")->GetValue().c_str());
    e.wanderrect.left   = atoi(get < wxTextCtrl>("edit_x1")->GetValue().c_str());
    e.wanderrect.top    = atoi(get < wxTextCtrl>("edit_y1")->GetValue().c_str());
    e.wanderrect.right  = atoi(get < wxTextCtrl>("edit_x2")->GetValue().c_str());
    e.wanderrect.bottom = atoi(get < wxTextCtrl>("edit_y2")->GetValue().c_str());
    e.sZone             = get < wxTextCtrl>("edit_zone")->GetValue().c_str();
    e.sChasetarget      = get < wxTextCtrl>("edit_chasetarget")->GetValue().c_str();
    e.bIsobs            = get < wxCheckBox>("check_isobs")->GetValue();
    e.bMapobs           = get < wxCheckBox>("check_mapobs")->GetValue();
    e.bEntobs           = get < wxCheckBox>("check_entobs")->GetValue();
    e.state             = (MoveCode)get < wxRadioBox>("radio_movepattern")->GetSelection();
}

void CEntityEditor::UpdateDlg()
{
    const SMapEntity& e = pMap->GetEntity(pEntlist->GetSelection());

    get < wxTextCtrl>("edit_name")->SetValue(e.name.c_str());
    get < wxTextCtrl>("edit_x")->SetValue(ToString(e.x).c_str());
    get < wxTextCtrl>("edit_y")->SetValue(ToString(e.y).c_str());
    get < wxTextCtrl>("edit_sprite")->SetValue(e.sCHRname.c_str());
    get < wxTextCtrl>("edit_speed")->SetValue(ToString(e.nSpeed).c_str());
    get < wxTextCtrl>("edit_script")->SetValue(e.sActscript.c_str());
    get < wxTextCtrl>("edit_movescript")->SetValue(e.sMovescript.c_str());
    get < wxTextCtrl>("edit_steps")->SetValue(ToString(e.nWandersteps).c_str());
    get < wxTextCtrl>("edit_delay")->SetValue(ToString(e.nWanderdelay).c_str());
    get < wxTextCtrl>("edit_x1")->SetValue(ToString(e.wanderrect.left).c_str());
    get < wxTextCtrl>("edit_y1")->SetValue(ToString(e.wanderrect.top).c_str());
    get < wxTextCtrl>("edit_x2")->SetValue(ToString(e.wanderrect.right).c_str());
    get < wxTextCtrl>("edit_y2")->SetValue(ToString(e.wanderrect.bottom).c_str());
    get < wxTextCtrl>("edit_zone")->SetValue(e.sZone.c_str());
    get < wxTextCtrl>("edit_chasetarget")->SetValue(e.sChasetarget.c_str());
    get < wxCheckBox>("check_isobs")->SetValue(e.bIsobs);
    get < wxCheckBox>("check_mapobs")->SetValue(e.bMapobs);
    get < wxCheckBox>("check_entobs")->SetValue(e.bEntobs);
    get < wxRadioBox>("radio_movepattern")->SetSelection((int)e.state);
}

void CEntityEditor::OnSelectEntity(wxCommandEvent& event)
{
    UpdateData();
    nCurentidx = event.GetInt();
    UpdateDlg();
}

void CEntityEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    Show(false);
}