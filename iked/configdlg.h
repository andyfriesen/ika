
#include "wx/wx.h"

#include "common/configfile.h"
#include "common/utility.h"

class CConfigDlg : public wxDialog
{
    wxCheckBox* pFullscreenbox;
    wxCheckBox* pSoundbox;
    wxCheckBox* pLogbox;
    wxRadioBox* pPixeldepthselector;
    wxListBox*  pGraphdriverselector;        

    void Update();
    void Load(const std::string& fname);
    void Save(const std::string& fname);

    CConfigFile cfg;
    std::string sFilename;

public:
    CConfigDlg(wxWindow* parent,
               wxWindowID id,
               const std::string& name="");

    void OnOK(wxCommandEvent&);
    void OnCancel(wxCommandEvent&);

    DECLARE_EVENT_TABLE()
};