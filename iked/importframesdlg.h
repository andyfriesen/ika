#if 0
#pragma once

#include <wx/dialog.h>
#include <string>
#include <vector>

#include "Canvas.h"

class ImportFramesDlg : public wxDialog
{
    void OnOk(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);

    void ImportFrames(int width, int height, int numFrames, int rowSize, const std::string& fileName, bool pad);
public:
    ImportFramesDlg(wxWindow* parent);

    std::vector<Canvas> frames; // the resultant frames
    bool append;                // nothing to do with this dialog.  We expose it so that whoever is using it can interpret the frames properly.

    int ShowModal();
    int ShowModal(int width, int height); // locks the width and height, then does the usual ShowModal thing.

    DECLARE_EVENT_TABLE()
};
#endif
