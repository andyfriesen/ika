#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <stdexcept>

#include "importframesdlg.h"
#include "fileio.h"

BEGIN_EVENT_TABLE(ImportFramesDlg, wxDialog)
    EVT_BUTTON(wxID_OK, ImportFramesDlg::OnOk)
    EVT_BUTTON(XRCID("button_browse"), ImportFramesDlg::OnBrowse)
END_EVENT_TABLE()

void ImportFramesDlg::OnOk(wxCommandEvent& event)
{
    try
    {
        int width = atoi(XRCCTRL(*this, "edit_framewidth", wxTextCtrl)->GetValue());
        int height = atoi(XRCCTRL(*this, "edit_frameheight", wxTextCtrl)->GetValue());
        int numFrames = atoi(XRCCTRL(*this, "edit_numframes", wxTextCtrl)->GetValue());
        int rowSize = atoi(XRCCTRL(*this, "edit_rowsize", wxTextCtrl)->GetValue());
        std::string fileName = XRCCTRL(*this, "edit_filename", wxTextCtrl)->GetValue().c_str();
        bool pad = XRCCTRL(*this, "check_padding", wxCheckBox)->GetValue();
        append = XRCCTRL(*this, "check_append", wxCheckBox)->GetValue();
        // TODO: colour key because dreq is gay.  Also, he asked for it, or something.

        if (!File::Exists(fileName))    throw va("%s does not exist", fileName.c_str());
        if (width < 0 || height < 0)    throw "The frame dimensions must be at least one pixel.";
        if (numFrames < 1)              throw "Importing less than one frame doesn't make much sense.";
        if (rowSize < 1)                throw "There should be at least one frame per row.";

        try
        {
            ImportFrames(width, height, numFrames, rowSize, fileName, pad);
            wxDialog::OnOK(event);
        }
        catch (std::exception ex)
        {
            throw va("Import error: %s", ex.what());
        }
    }
    catch (const char* msg)
    {
        wxMessageBox(msg, "Error", wxOK | wxCENTER, this);
    }
}

void ImportFramesDlg::OnBrowse(wxCommandEvent& event)
{
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
    if (result == wxID_OK)
        XRCCTRL(*this, "edit_filename", wxTextCtrl)->SetValue(dlg.GetPath());
}

void ImportFramesDlg::ImportFrames(int width, int height, int numFrames, int rowSize, const std::string& fileName, bool pad)
{
    Canvas image(fileName.c_str());
    frames.clear(); 

    int xstep = width + (pad ? 1 : 0);
    int ystep = height + (pad ? 1 : 0);

    int xpos = pad ? 1 : 0;
    int ypos = pad ? 1 : 0;
    int curframe = 0;
    int row = 0;
    while (row++, curframe < numFrames)
    {
        for (int col = 0; col < rowSize && curframe < numFrames; col++)
        {
            Canvas frame(width, height);
            CBlitter<Opaque>::Blit(image, frame, -xpos, -ypos);
            frames.push_back(frame);
            
            curframe++;

            xpos += xstep;
        }

        xpos = pad ? 1 : 0;
        ypos += ystep;
    }
}

ImportFramesDlg::ImportFramesDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_importframes");
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
}

int ImportFramesDlg::ShowModal()
{
    return wxDialog::ShowModal();
}

int ImportFramesDlg::ShowModal(int width, int height)
{
    wxTextCtrl* w = XRCCTRL(*this, "edit_framewidth", wxTextCtrl);
    wxTextCtrl* h = XRCCTRL(*this, "edit_frameheight", wxTextCtrl);

    w->SetValue(ToString(width).c_str());   w->SetEditable(false);
    h->SetValue(ToString(height).c_str());  h->SetEditable(false);

    return wxDialog::ShowModal();
}