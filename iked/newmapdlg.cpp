#include <wx/xrc/xmlres.h>
#include "newmapdlg.h"

BEGIN_EVENT_TABLE(NewMapDlg, wxDialog)
    EVT_BUTTON(wxID_OK, NewMapDlg::OnOK)
END_EVENT_TABLE()

NewMapDlg::NewMapDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_newmap");
}

void NewMapDlg::OnOK(wxCommandEvent& event)
{
    width         = atoi(XRCCTRL(*this, "edit_width", wxTextCtrl)->GetValue().c_str());
    height        = atoi(XRCCTRL(*this, "edit_height", wxTextCtrl)->GetValue().c_str()); 
    loadtileset   = XRCCTRL(*this, "radio_loadtileset", wxRadioButton)->GetValue() != 0;
    tilesetname   = XRCCTRL(*this, "edit_tilesetfilename", wxTextCtrl)->GetValue().c_str();
    tilesetwidth  = atoi(XRCCTRL(*this, "edit_tilesetwidth", wxTextCtrl)->GetValue().c_str());
    tilesetheight = atoi(XRCCTRL(*this, "edit_tilesetheight", wxTextCtrl)->GetValue().c_str());
    wxDialog::OnOK(event);
}