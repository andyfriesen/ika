
#include <wx/wx.h>
#include <wx/listctrl.h>

#include "layervisibilitycontrol.h"
#include "mapview.h"
#include "log.h"

BEGIN_EVENT_TABLE(CLayerVisibilityControl, wxCheckListBox)
    EVT_LISTBOX(-1, CLayerVisibilityControl::OnItemSelected)
    EVT_LIST_KEY_DOWN(-1, CLayerVisibilityControl::OnKeyDown)
    EVT_CHECKLISTBOX(-1, CLayerVisibilityControl::OnItemChecked)
END_EVENT_TABLE()

CLayerVisibilityControl::CLayerVisibilityControl(wxWindow* parent, int id, CMapView* mapview)
    : wxCheckListBox(parent, id)
    , pMapview(mapview)
{}

void CLayerVisibilityControl::Clear()
{
    layidx.clear();
    wxCheckListBox::Clear();
}

void CLayerVisibilityControl::AppendLayer(const std::string& name, int idx)
{
    layidx.push_back(idx);

    Append(name.c_str());
}

void CLayerVisibilityControl::CheckLayer(int idx)
{
    for (uint i = 0; i < layidx.size(); i++)
    {
        if (layidx[i] == idx)
        {
            Check(i);
            return;
        }
    }
}

void CLayerVisibilityControl::SelectLayer(int idx)
{
    for (uint i = 0; i < layidx.size(); i++)
    {
        if (layidx[i] == idx)
        {
            SetSelection(i);
            return;
        }
    }
}

void CLayerVisibilityControl::OnItemSelected(wxCommandEvent& event)
{
    pMapview->OnLayerChange( layidx[event.GetInt()] );
}

void CLayerVisibilityControl::OnKeyDown(wxListEvent& event) {}

void CLayerVisibilityControl::OnItemChecked(wxCommandEvent& event)
{
    int idx = layidx[event.GetInt()];

    pMapview->OnLayerToggleVisibility(idx, IsChecked( event.GetInt() ));
}
