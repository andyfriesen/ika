#if 0
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "newmapdlg.h"
#include "fileio.h"
#include "misc.h"

BEGIN_EVENT_TABLE(NewMapDlg, wxDialog)
    EVT_BUTTON(wxID_OK, NewMapDlg::OnOK)
    EVT_BUTTON(XRCID("button_browse"), NewMapDlg::OnBrowse)
END_EVENT_TABLE()

NewMapDlg::NewMapDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_newmap");
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
}

void NewMapDlg::OnOK(wxCommandEvent& event)
{
    width         = atoi(XRCCTRL(*this, "edit_width", wxTextCtrl)->GetValue().c_str());
    height        = atoi(XRCCTRL(*this, "edit_height", wxTextCtrl)->GetValue().c_str()); 
    tilesetname   = XRCCTRL(*this, "edit_tileset", wxTextCtrl)->GetValue().c_str();

    if (width < 0 || height < 0)
        wxMessageBox("Width and height must be at least one pixel.", "Error", wxOK | wxCENTER, this);
    else if (!File::Exists(tilesetname))
        wxMessageBox(va("%s does not exist!", tilesetname.c_str()), "Error", wxOK | wxCENTER, this);
    else
        wxDialog::OnOK(event);
}

void NewMapDlg::OnBrowse(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Open Tileset",
        "",
        "",
        "VSP Tilesets (*.vsp)|*.vsp|"
        "All files (*.*)|*.*",
        wxOPEN
        );

    int result = dlg.ShowModal();
    if (result == wxID_OK)
        XRCCTRL(*this, "edit_tileset", wxTextCtrl)->SetValue(dlg.GetPath());
}
#endif
