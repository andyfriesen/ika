#ifndef NEWMAPDLG_H
#define NEWMAPDLG_H

#include <wx / wx.h>
#include <string>

class NewMapDlg : public wxDialog
{
    void OnOK(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);
public:
    NewMapDlg(wxWindow* parent);

    // too lazy to encapsulate this shit.
    int width;
    int height;
    std::string tilesetname;

    DECLARE_EVENT_TABLE()
};

#endif