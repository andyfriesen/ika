#if 0

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "newspritedlg.h"

BEGIN_EVENT_TABLE(NewSpriteDlg, wxDialog)
    EVT_BUTTON(wxID_OK, NewSpriteDlg::OnOk)
END_EVENT_TABLE()

NewSpriteDlg::NewSpriteDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_newsprite");
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
}

void NewSpriteDlg::OnOk(wxCommandEvent& event)
{
    width = atoi(XRCCTRL(*this, "edit_width", wxTextCtrl)->GetValue().c_str());
    height = atoi(XRCCTRL(*this, "edit_height", wxTextCtrl)->GetValue().c_str());

    if (width < 0 || height < 0)
        wxMessageBox("Width and height must be at least one pixel.", "Error", wxOK | wxCENTER, this);
    else
        wxDialog::OnOK(event);
}
#endif
