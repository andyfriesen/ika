
#include "layervisibilitycontrol.h"
#include <wx\wx.h>
#include <wx\listctrl.h>
#include "mapview.h"

BEGIN_EVENT_TABLE(CLayerVisibilityControl,wxCheckListBox)
    EVT_LIST_BEGIN_DRAG(-1,CLayerVisibilityControl::OnDrag)
//    EVT_LIST_END_DRAG(-1,CLayerVisibilityControl::OnEndDrag)
    EVT_LIST_ITEM_SELECTED(-1,CLayerVisibilityControl::OnItemSelected)
    EVT_LIST_KEY_DOWN(-1,CLayerVisibilityControl::OnKeyDown)
    EVT_CHECKLISTBOX(-1,CLayerVisibilityControl::OnItemChecked)
END_EVENT_TABLE()

CLayerVisibilityControl::CLayerVisibilityControl(wxWindow* parent,int id,CMapView* mapview)
: wxCheckListBox(parent,id), pMapview(mapview)
{}

void CLayerVisibilityControl::AppendItem(const std::string& name,int idx)
{
    sLayxlat[name]=idx;
    Append(name.c_str());
}

void CLayerVisibilityControl::OnDrag(wxListEvent& event)
{
}

void CLayerVisibilityControl::OnEndDrag(wxListEvent& event)
{
}

void CLayerVisibilityControl::OnItemSelected(wxListEvent& event)
{
}

void CLayerVisibilityControl::OnKeyDown(wxListEvent& event) {}
void CLayerVisibilityControl::OnItemChecked(wxCommandEvent& event)
{
    pMapview->OnLayerToggleVisibility(event.GetInt(),IsChecked(event.GetInt()));
}
