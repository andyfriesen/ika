
#include "configfile.h"
#include <wx\wx.h>
#include <string>

class CConfigDlg : public wxDialog
{
    wxCheckBox* pFullscreenbox;
    wxCheckBox* pSoundbox;
    wxCheckBox* pLogbox;
    wxWindow*   pPixeldepthselector;
    wxWindow*   pGraphdriverselector;        

    void Update();
    void Load(const std::string& fname);
    void Save(const std::string& fname);

    CConfigFile cfg;
    std::string sFilename;

public:
    CConfigDlg(wxWindow* parent,
               wxWindowID id,
               const std::string& name="");

    void OnOk(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};