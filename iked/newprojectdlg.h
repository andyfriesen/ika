#ifndef NEWPROJECTDLG_H
#define NEWPROJECTDLG_H

#include <wx/dialog.h>
#include <string>

class NewProjectDlg : public wxDialog
{
    std::string name;
    std::string path;

    void UpdatePreview(wxCommandEvent& event);
    void OnOk(wxCommandEvent& event);
public:
    NewProjectDlg(wxWindow* parent);

    std::string FileName() const;

    DECLARE_EVENT_TABLE()
};

#endif