
#include "wx / wx.h"

#include "configfile.h"
#include "types.h"


class CConfigDlg : public wxDialog
{
    wxCheckBox* pFullscreenbox;
    wxCheckBox* pSoundbox;
    wxCheckBox* pLogbox;
    wxRadioBox* pPixeldepthselector;
    wxListBox*  pGraphdriverselector;        

    void Update();
    void Load(const string& fname);
    void Save(const string& fname);

    CConfigFile cfg;
    std::string sFilename;

public:
    CConfigDlg(wxWindow* parent,
               wxWindowID id,
               const string& name="");

    void OnOK(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};