
#include "types.h"
#include <wx\wx.h>

class CMainWnd;

class CMapView : public wxMDIChildFrame
{
public:
    CMapView(CMainWnd* parent,const wxString& title="",const wxPoint& position=wxDefaultPosition,const wxSize& size=wxDefaultSize,const long style=wxDEFAULT_FRAME_STYLE,const char* fname=0);

    DECLARE_EVENT_TABLE()
};