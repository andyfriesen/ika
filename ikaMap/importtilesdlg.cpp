#include "wxinc.h"
#include <wx/xrc/xmlres.h>
#include <stdexcept>

#include "importtilesdlg.h"
#include "fileio.h"

BEGIN_EVENT_TABLE(ImportTilesDlg, wxDialog)
    EVT_BUTTON(wxID_OK, ImportTilesDlg::OnOk)
    EVT_BUTTON(XRCID("button_browse"), ImportTilesDlg::OnBrowse)
    EVT_CHECKBOX(XRCID("check_append"), ImportTilesDlg::OnCheckAppend)
END_EVENT_TABLE()

ImportTilesDlg::ImportTilesDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_importframes");
    XRCCTRL(*this, "panel_main", wxPanel)->GetSizer()->Fit(this);
}

int ImportTilesDlg::ShowModal()
{
    SetDefaultValues();

    return wxDialog::ShowModal();
}

int ImportTilesDlg::ShowModal(uint width, uint height)
{
    _width = width;
    _height = height;

    SetDefaultValues();

    return wxDialog::ShowModal();
}

void ImportTilesDlg::OnOk(wxCommandEvent& event)
{
    try
    {
        _width = atoi(XRCCTRL(*this, "edit_framewidth", wxTextCtrl)->GetValue());
        _height = atoi(XRCCTRL(*this, "edit_frameheight", wxTextCtrl)->GetValue());
        _numTiles = atoi(XRCCTRL(*this, "edit_numframes", wxTextCtrl)->GetValue());
        _rowSize = atoi(XRCCTRL(*this, "edit_rowsize", wxTextCtrl)->GetValue());
        _fileName = XRCCTRL(*this, "edit_filename", wxTextCtrl)->GetValue().c_str();
        _pad = XRCCTRL(*this, "check_padding", wxCheckBox)->GetValue();
        _append = XRCCTRL(*this, "check_append", wxCheckBox)->GetValue();
    
        if (!File::Exists(_fileName))   throw va("%s does not exist", _fileName.c_str());
        if (_width < 0 || _height < 0)  throw "The frame dimensions must be at least one pixel.";
        if (_numTiles < 1)              throw "Importing less than one frame doesn't make much sense.";
        if (_rowSize < 1)               throw "There should be at least one frame per row.";

        try
        {
            ImportTiles(_width, _height, _numTiles, _rowSize, _fileName, _pad);
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

void ImportTilesDlg::OnCheckAppend(wxCommandEvent& event)
{
    wxTextCtrl* editWidth = XRCCTRL(*this, "edit_framewidth", wxTextCtrl);
    wxTextCtrl* editHeight = XRCCTRL(*this, "edit_frameheight", wxTextCtrl);

    if (event.GetInt())
    {
        editWidth->Disable();
        editWidth->SetValue(ToString(_defaultWidth).c_str());
        editHeight->Disable();
        editHeight->SetValue(ToString(_defaultHeight).c_str());
    }
    else
    {
        editWidth->Enable();
        editHeight->Enable();
    }
}

void ImportTilesDlg::OnBrowse(wxCommandEvent& event)
{
    wxFileDialog dlg(
        this,
        "Import tiles from image",
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

void ImportTilesDlg::SetDefaultValues()
{
    wxTextCtrl* w = XRCCTRL(*this, "edit_framewidth", wxTextCtrl);
    wxTextCtrl* h = XRCCTRL(*this, "edit_frameheight", wxTextCtrl);

    w->SetValue(ToString(_width).c_str());   w->Enable(!_append);
    h->SetValue(ToString(_height).c_str());  h->Enable(!_append);

    XRCCTRL(*this, "check_append", wxCheckBox)->SetValue(_append);
    XRCCTRL(*this, "check_padding", wxCheckBox)->SetValue(_pad);

    XRCCTRL(*this, "edit_numframes", wxTextCtrl)->SetValue(ToString(_numTiles).c_str());
    XRCCTRL(*this, "edit_rowsize", wxTextCtrl)->SetValue(ToString(_rowSize).c_str());
    XRCCTRL(*this, "edit_filename", wxTextCtrl)->SetValue(_fileName.c_str());
}

void ImportTilesDlg::ImportTiles(int width, int height, int numFrames, int rowSize, const std::string& fileName, bool pad)
{
    Canvas image(fileName.c_str());
    tiles.clear(); 

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
            tiles.push_back(frame);
            
            curframe++;

            xpos += xstep;
        }

        xpos = pad ? 1 : 0;
        ypos += ystep;
    }
}
