
#pragma warning (disable:4786)

#include <wx\checklst.h>
#include <map>
#include <string>

class wxListEvent;

class CMapView;

class CLayerVisibilityControl : public wxCheckListBox
{
    CMapView* pMapview;

    std::map<std::string,int> sLayxlat;

public:
    CLayerVisibilityControl(wxWindow* parent,int id,CMapView* mapview);

    void AppendItem(const std::string& name,int idx);

    void OnDrag(wxListEvent& event);
    void OnEndDrag(wxListEvent& event);
    void OnItemSelected(wxListEvent& event);
    void OnKeyDown(wxListEvent& event);
    void OnItemChecked(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};