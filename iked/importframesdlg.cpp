#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/xrc/xmlres.h>
#include <stdexcept>

#include "importframesdlg.h"
#include "common/fileio.h"

namespace iked {

    BEGIN_EVENT_TABLE(ImportFramesDlg, wxDialog)
        EVT_BUTTON(wxID_OK, ImportFramesDlg::OnOk)
        EVT_BUTTON(XRCID("button_browse"), ImportFramesDlg::OnBrowse)
    END_EVENT_TABLE()

    void ImportFramesDlg::OnOk(wxCommandEvent& event) {
        try {
            frameWidth = atoi(XRCCTRL(*this, "edit_framewidth", wxTextCtrl)->GetValue());
            frameHeight = atoi(XRCCTRL(*this, "edit_frameheight", wxTextCtrl)->GetValue());
            frameCount = atoi(XRCCTRL(*this, "edit_numframes", wxTextCtrl)->GetValue());
            rowSize = atoi(XRCCTRL(*this, "edit_rowsize", wxTextCtrl)->GetValue());
            fileName = XRCCTRL(*this, "edit_filename", wxTextCtrl)->GetValue().c_str();
            pad = XRCCTRL(*this, "check_padding", wxCheckBox)->GetValue();
            append = XRCCTRL(*this, "check_append", wxCheckBox)->GetValue();
            // TODO: colour key because dreq is gay.  Also, he asked for it, or something.

            if (!File::Exists(fileName))    throw va("%s does not exist", fileName.c_str());
            if (frameWidth < 0 || frameHeight < 0)    throw "The frame dimensions must be at least one pixel.";
            if (frameCount < 1)              throw "Importing less than one frame doesn't make much sense.";
            if (rowSize < 1)                throw "There should be at least one frame per row.";

            wxDialog::OnOK(event);
        }
        catch (const char* msg) {
            wxMessageBox(msg, "Error", wxOK | wxCENTER, this);
        }
    }

    void ImportFramesDlg::OnBrowse(wxCommandEvent& event) {
        wxFileDialog dlg(
            this,
            "Import frames from image",
            "",
            "",
            "All images (*.png;*.jpg;*.jpeg;*.gif;*.bmp;*.pcx;*.tga)|*.png;*.jpg;*.jpeg;*.gif;*.bmp;*.pcx;*.tga|"
            "Portable Network Graphics (*.png)|*.png|"
            "JPEG images (*.jpg;*.jpeg)|*.jpg;*.jpeg|"
            "GIF images (*.gif)|*.gif|"
            "PCX images (*.pcx)|*.pcx|"
            "Targa images (*.tga)|*.tga|"
            "All files (*.*)|*.*",
            wxOPEN
        );

        int result = dlg.ShowModal();
        if (result == wxID_OK) {
            XRCCTRL(*this, "edit_filename", wxTextCtrl)->SetValue(dlg.GetPath());
        }
    }

    ImportFramesDlg::ImportFramesDlg(wxWindow* parent) {
        wxXmlResource::Get()->LoadDialog(this, parent, "dialog_importframes");
        XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
    }

    int ImportFramesDlg::ShowModal() {
        return wxDialog::ShowModal();
    }

    int ImportFramesDlg::ShowModal(int width, int height) {
        wxTextCtrl* w = XRCCTRL(*this, "edit_framewidth", wxTextCtrl);
        wxTextCtrl* h = XRCCTRL(*this, "edit_frameheight", wxTextCtrl);

        w->SetValue(toString(width).c_str());   w->SetEditable(false);
        h->SetValue(toString(height).c_str());  h->SetEditable(false);

        return wxDialog::ShowModal();
    }
}
