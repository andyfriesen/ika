
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <cassert>

#include "layervisibilitycontrol.h"
#include "mapview.h"
#include "log.h"

BEGIN_EVENT_TABLE(CLayerVisibilityControl, wxCheckListBox)
    EVT_LISTBOX(-1, CLayerVisibilityControl::OnItemSelected)
    EVT_LIST_KEY_DOWN(-1, CLayerVisibilityControl::OnKeyDown)
    EVT_CHECKLISTBOX(-1, CLayerVisibilityControl::OnItemChecked)
END_EVENT_TABLE()

CLayerVisibilityControl::CLayerVisibilityControl(wxWindow* parent, int id, MapView* mapview)
    : wxCheckListBox(parent, id)
    , pMapview(mapview)
{}

void CLayerVisibilityControl::Clear()
{
    layidx.clear();
    wxCheckListBox::Clear();
}

void CLayerVisibilityControl::AppendLayer(Map::Layer* lay)
{
    layidx.push_back(lay);
    Append(lay->label.c_str());
}

void CLayerVisibilityControl::CheckLayer(Map::Layer* lay)
{
    for (uint i = 0; i < layidx.size(); i++)
    {
        if (lay == layidx[i])
        {
            Check(i);
            return;
        }
    }

    // @_@
    assert(false);
}

void CLayerVisibilityControl::SelectLayer(Map::Layer* lay)
{
    for (uint i = 0; i < layidx.size(); i++)
    {
        if (layidx[i] == lay)
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
    Map::Layer* lay = layidx[event.GetInt()];

    pMapview->OnLayerToggleVisibility(lay, IsChecked( event.GetInt() ));
}
