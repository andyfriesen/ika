
#include "configfile.h"
#include "types.h"
#include <wx\wx.h>


class CConfigDlg : public wxDialog
{
    wxCheckBox* pFullscreenbox;
    wxCheckBox* pSoundbox;
    wxCheckBox* pLogbox;
    wxWindow*   pPixeldepthselector;
    wxWindow*   pGraphdriverselector;        

    void Update();
    void Load(const string& fname);
    void Save(const string& fname);

    CConfigFile cfg;
    std::string sFilename;

public:
    CConfigDlg(wxWindow* parent,
               wxWindowID id,
               const string& name="");

    void OnOk(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};