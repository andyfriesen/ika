
#include "types.h"

#include "wx/wx.h"
#include "wx/checklst.h"
#include <vector>

class wxListEvent;

class CMapView;

class CLayerVisibilityControl : public wxCheckListBox
{
    CMapView* pMapview;

    std::vector<int> layidx;

public:
    CLayerVisibilityControl(wxWindow* parent, int id, CMapView* mapview);

    void Clear();
    void AppendItem(const std::string& name, int idx);
    void CheckItem(int idx);

    void OnItemSelected(wxCommandEvent& event);
    void OnKeyDown(wxListEvent& event);
    void OnItemChecked(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};