#include "wxinc.h"
#include <wx/xrc/xmlres.h>

#include "newmapdlg.h"
#include "fileio.h"
#include "misc.h"

BEGIN_EVENT_TABLE(NewMapDlg, wxDialog)
    EVT_BUTTON(wxID_OK, NewMapDlg::OnOK)
    EVT_BUTTON(XRCID("button_browse"), NewMapDlg::OnBrowse)
    EVT_CHECKBOX(XRCID("check_newtileset"), NewMapDlg::OnCheckNewTileSet)
END_EVENT_TABLE()

// I like the look of this better than the XRCCTRL macro. :>
template <typename T>
T* NewMapDlg::Ctrl(const std::string& name, const T*)
{
    wxWindow* t = FindWindow(XRCID(name.c_str()));

#ifdef DEBUG
    wxASSERT(wxDynamicCast(t, T));
#endif

    return (T*)t;
}

NewMapDlg::NewMapDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_newmap");
    Ctrl<wxPanel>("panel_main")->GetSizer()->Fit(this);
}

void NewMapDlg::OnOK(wxCommandEvent& event)
{
    width         = atoi(Ctrl<wxTextCtrl>("edit_width")->GetValue().c_str());
    height        = atoi(Ctrl<wxTextCtrl>("edit_height")->GetValue().c_str());
    tileSetName   = Ctrl<wxTextCtrl>("edit_tilesetname")->GetValue().c_str();
    newTileSet    = Ctrl<wxCheckBox>("check_newtileset")->IsChecked();
    tileWidth     = atoi(Ctrl<wxTextCtrl>("edit_tilewidth")->GetValue().c_str());
    tileHeight    = atoi(Ctrl<wxTextCtrl>("edit_tileheight")->GetValue().c_str());

    if (width < 0 || height < 0)
        wxMessageBox("Width and height must be at least one pixel.", "Error", wxOK | wxCENTER, this);
    else if (!File::Exists(tileSetName))
        wxMessageBox(va("%s does not exist!", tileSetName.c_str()), "Error", wxOK | wxCENTER, this);
    else if (newTileSet && (tileWidth < 0 || tileHeight < 0))
        wxMessageBox("Tile width and height must each be at least one pixel.", "Error", wxOK | wxCENTER, this);
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
        Ctrl<wxTextCtrl>("edit_tilesetname")->SetValue(dlg.GetPath());
}

void NewMapDlg::OnCheckNewTileSet(wxCommandEvent& event)
{
    bool b = Ctrl<wxCheckBox>("check_newtileset")->IsChecked();

    // These only make sense if the check box is checked.
    Ctrl<wxTextCtrl>("edit_tilewidth")->Enable(b);
    Ctrl<wxTextCtrl>("edit_tileheight")->Enable(b);
}
