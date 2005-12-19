#include "wxinc.h"
#include <wx/xrc/xmlres.h>

#include "newmapdlg.h"
#include "common/fileio.h"
#include "common/utility.h"

BEGIN_EVENT_TABLE(NewMapDlg, wxDialog)
    EVT_BUTTON(wxID_OK, NewMapDlg::OnOK)
    EVT_BUTTON(XRCID("button_browse"), NewMapDlg::OnBrowse)
    EVT_CHECKBOX(XRCID("check_newtileset"), NewMapDlg::OnCheckNewTileset)
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

    // defaults
    Ctrl<wxTextCtrl>("edit_width")->SetValue("20");
    Ctrl<wxTextCtrl>("edit_height")->SetValue("15");
    Ctrl<wxTextCtrl>("edit_tilewidth")->SetValue("16");
    Ctrl<wxTextCtrl>("edit_tileheight")->SetValue("16");
}

void NewMapDlg::OnOK(wxCommandEvent& event)
{
    width         = atoi(Ctrl<wxTextCtrl>("edit_width")->GetValue().c_str());
    height        = atoi(Ctrl<wxTextCtrl>("edit_height")->GetValue().c_str());
    tilesetName   = Ctrl<wxTextCtrl>("edit_tilesetname")->GetValue().c_str();
    newTileset    = Ctrl<wxCheckBox>("check_newtileset")->IsChecked();
    tileWidth     = atoi(Ctrl<wxTextCtrl>("edit_tilewidth")->GetValue().c_str());
    tileHeight    = atoi(Ctrl<wxTextCtrl>("edit_tileheight")->GetValue().c_str());

    try
    {
        if (width < 0 || height < 0)                            throw "Width and height must be at least one pixel.";
        if (!newTileset && !File::Exists(tilesetName))          throw va("%s does not exist!", tilesetName.c_str());
        if (newTileset && File::Exists(tilesetName))
        {
            if (wxMessageBox(va("A tileset named %s already exists.\nOverwrite?", tilesetName.c_str()), "", wxYES_NO, this) == wxID_NO)
                return;
        }
        if (newTileset && (tileWidth < 0 || tileHeight < 0))    throw "The tileset to be created must be at least 1 pixel wide and high.";
        else
            wxDialog::OnOK(event);
    }
    catch (const char* s)
    {
        wxMessageBox(s, "Error", wxOK | wxCENTER, this);
    }
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

void NewMapDlg::OnCheckNewTileset(wxCommandEvent& event)
{
    bool b = Ctrl<wxCheckBox>("check_newtileset")->IsChecked();

    // These only make sense if the check box is checked.
    Ctrl<wxTextCtrl>("edit_tilewidth")->Enable(b);
    Ctrl<wxTextCtrl>("edit_tileheight")->Enable(b);
}
