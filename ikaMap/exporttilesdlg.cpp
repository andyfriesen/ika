
#include "exporttilesdlg.h"

#include "executor.h"
#include <wx/xrc/xmlres.h>
#include <stdexcept>

#include "tileset.h"

BEGIN_EVENT_TABLE(ExportTilesDlg, wxDialog)
    EVT_BUTTON(wxID_OK, ExportTilesDlg::OnOk)
    EVT_BUTTON(XRCID("button_browse"), ExportTilesDlg::OnBrowse)
END_EVENT_TABLE()

ExportTilesDlg::ExportTilesDlg(Executor* executor)
    : _executor(executor)
    , _pad(true)
    , _rowSize(18)
{
    wxXmlResource::Get()->LoadDialog(this, executor->GetParentWindow(), "dialog_exportframes");
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
}

int ExportTilesDlg::ShowModal()
{
    SetDefaultValues();

    return wxDialog::ShowModal();
}

void ExportTilesDlg::OnOk(wxCommandEvent& event)
{
    _pad = XRCCTRL(*this, "check_pad", wxCheckBox)->GetValue();
    _rowSize = atoi(XRCCTRL(*this, "edit_rowsize", wxTextCtrl)->GetValue().c_str());
    _fileName = XRCCTRL(*this, "edit_filename", wxTextCtrl)->GetValue().c_str();
    EndModal(wxID_OK);
}

void ExportTilesDlg::OnBrowse(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Export tiles to image",
        "",
        "",
        "Portable Network Graphics (*.png)|*.png|"
        "All files (*.*)|*.*",
        wxSAVE | wxOVERWRITE_PROMPT
        );

    int result = dlg.ShowModal();
    if (result == wxID_OK)
        XRCCTRL(*this, "edit_filename", wxTextCtrl)->SetValue(dlg.GetPath());
}

void ExportTilesDlg::SetDefaultValues()
{
    XRCCTRL(*this, "edit_filename", wxTextCtrl)->SetValue(_fileName.c_str());
    XRCCTRL(*this, "check_pad", wxCheckBox)->SetValue(_pad);
    XRCCTRL(*this, "edit_rowsize", wxTextCtrl)->SetValue(ToString(_rowSize).c_str());
}
