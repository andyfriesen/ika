#include <wx/xrc/xmlres.h>
#include "newmapdlg.h"

BEGIN_EVENT_TABLE(NewMapDlg, wxDialog)
    EVT_CLOSE(NewMapDlg::OnClose)
END_EVENT_TABLE()

NewMapDlg::NewMapDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_newmap");
}

void NewMapDlg::OnClose()
{
    width         = atoi(XRCCTRL(*this, "edit_width", wxTextCtrl)->GetValue().c_str());
    height        = atoi(XRCCTRL(*this, "edit_height", wxTextCtrl)->GetValue().c_str());
    loadtileset   = XRCCTRL(*this, "radio_loadtileset", wxRadioBox)->GetSelection() != 0;
    tilesetname   = XRCCTRL(*this, "edit_tilesetfilename", wxTextCtrl)->GetValue().c_str();
    tilesetwidth  = atoi(XRCCTRL(*this, "edit_tilesetwidth", wxTextCtrl)->GetValue().c_str());
    tilesetheight = atoi(XRCCTRL(*this, "edit_tilesetheight", wxTextCtrl)->GetValue().c_str());
}