
#include "types.h"

#include "wx/wx.h"
#include "wx/checklst.h"

#include "map.h"

class wxListEvent;

class MapView;

class CLayerVisibilityControl : public wxCheckListBox
{
    MapView* pMapview;

    std::vector<Map::Layer*> layidx; // lookup table to match layer instances with table indeces

public:
    CLayerVisibilityControl(wxWindow* parent, int id, MapView* mapview);

    void Clear();
    void AppendLayer(Map::Layer* lay);
    void CheckLayer(Map::Layer* lay);
    void SelectLayer(Map::Layer* lay);

    void OnItemSelected(wxCommandEvent& event);
    void OnKeyDown(wxListEvent& event);
    void OnItemChecked(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};