#if 0
#pragma once

#include <wx/dialog.h>

class NewSpriteDlg : public wxDialog
{
    void OnOk(wxCommandEvent& event);
public:
    NewSpriteDlg(wxWindow* parent);

    int width;
    int height;

    DECLARE_EVENT_TABLE()
};
#endif
