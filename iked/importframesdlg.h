#pragma once

#include <wx/wx.h>
#include <string>
#include <vector>

#include "common/Canvas.h"

namespace iked {
    struct ImportFramesDlg : public wxDialog {
        ImportFramesDlg(wxWindow* parent);

        int ShowModal();
        int ShowModal(int width, int height); // locks the width and height, then does the usual ShowModal thing.

        std::string fileName;
        int frameWidth;
        int frameHeight;
        int frameCount;
        int rowSize;
        bool pad;
        bool append;

    private:
        void OnOk(wxCommandEvent& event);
        void OnBrowse(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
    };
}
