
#include "types.h"
#include <wx\wx.h>

class CMainWnd;
class CGraphFrame;

class CMapView : public wxMDIChildFrame
{
    CGraphFrame* pGraph;
public:
    CMapView(CMainWnd* parent,const wxString& title="",const wxPoint& position=wxDefaultPosition,const wxSize& size=wxDefaultSize,const long style=wxDEFAULT_FRAME_STYLE,const char* fname=0);
    ~CMapView();

    void OnPaint();
    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);

    DECLARE_EVENT_TABLE()
};