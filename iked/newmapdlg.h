#ifndef NEWMAPDLG_H
#define NEWMAPDLG_H

#include <wx/wx.h>
#include <string>

class NewMapDlg : public wxDialog
{
    void OnOK(wxCommandEvent& event);
public:
    NewMapDlg(wxWindow* parent);

    // too lazy to encapsulate this shit.
    int width;
    int height;
    bool loadtileset;
    std::string tilesetname;
    int tilesetwidth;
    int tilesetheight;

    DECLARE_EVENT_TABLE()
};

#endif