
#include "configfile.h"
#include <wx\wx.h>

class CConfigDlg : public wxDialog
{

public:
    CConfigDlg(wxWindow* parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_DIALOG_STYLE,
               const wxString& name="");
};