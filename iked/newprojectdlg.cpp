
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>

#include "NewProjectDlg.h"
#include "misc.h"

BEGIN_EVENT_TABLE(NewProjectDlg, wxDialog)
    EVT_TEXT(-1, NewProjectDlg::UpdatePreview)
    EVT_BUTTON(wxID_OK, NewProjectDlg::OnOk)
END_EVENT_TABLE()

NewProjectDlg::NewProjectDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_newproject");
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
    SetSize(500, -1);
}

void NewProjectDlg::UpdatePreview(wxCommandEvent& event)
{
    name = XRCCTRL(*this, "edit_name", wxTextCtrl)->GetValue();
    path = XRCCTRL(*this, "edit_path", wxTextCtrl)->GetValue();

    wxFileName fname(Trim(path).c_str(), Trim(name).c_str());
    XRCCTRL(*this, "label_preview", wxStaticText)->SetLabel(fname.GetFullPath() + ".ikaprj");
}

void NewProjectDlg::OnOk(wxCommandEvent& event)
{
    if (name.empty() || path.empty())
    {
        wxMessageBox("You must enter a project filename and path.", "", wxOK | wxCENTER, this);
        return;
    }

    if (!wxFileName::DirExists(path.c_str()))
    {
        if (wxMessageBox(va("%s does not exist.  Create it?", path.c_str()), "", wxYES_NO, this) != wxYES)
            return;
    }

    if (wxFileName::FileExists(FileName().c_str()))
    {
        if (wxMessageBox(va("Project %s already exists.  Overwrite it?", FileName().c_str()), "", wxYES_NO, this) != wxYES)
            return;
    }

    wxDialog::OnOK(event);
}

std::string NewProjectDlg::FileName() const
{
    wxFileName fname(Trim(path).c_str(), Trim(name).c_str());
    return (fname.GetFullPath() + ".ikaprj").c_str();
}