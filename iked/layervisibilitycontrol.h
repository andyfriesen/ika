
#include <wx\checklst.h>

class wxListEvent;

class CMapView;

class CLayerVisibilityControl : public wxCheckListBox
{
    CMapView* pMapview;
public:
    CLayerVisibilityControl(wxWindow* parent,int id,CMapView* mapview);

    void OnDrag(wxListEvent& event);
    void OnEndDrag(wxListEvent& event);
    void OnItemSelected(wxListEvent& event);
    void OnKeyDown(wxListEvent& event);
    void OnItemChecked(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};