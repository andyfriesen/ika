
#include "layervisibilitycontrol.h"
#include <wx\wx.h>
#include <wx\listctrl.h>
#include "mapview.h"

BEGIN_EVENT_TABLE(CLayerVisibilityControl,wxCheckListBox)
    EVT_LIST_BEGIN_DRAG(-1,CLayerVisibilityControl::OnDrag)
//    EVT_LIST_END_DRAG(-1,CLayerVisibilityControl::OnEndDrag)
    EVT_LISTBOX(-1,CLayerVisibilityControl::OnItemSelected)
    EVT_LIST_KEY_DOWN(-1,CLayerVisibilityControl::OnKeyDown)
    EVT_CHECKLISTBOX(-1,CLayerVisibilityControl::OnItemChecked)
END_EVENT_TABLE()

CLayerVisibilityControl::CLayerVisibilityControl(wxWindow* parent,int id,CMapView* mapview)
: wxCheckListBox(parent,id), pMapview(mapview)
{}

void CLayerVisibilityControl::AppendItem(const std::string& name,int idx)
{
    layidx.push_back(idx);

    Append(name.c_str());
}

void CLayerVisibilityControl::CheckItem(int idx)
{
    for (int i=0; i<layidx.size(); i++)
    {
        if (layidx[i]==idx)
        {
            Check(i);
            return;
        }
    }
}

void CLayerVisibilityControl::OnDrag(wxListEvent& event)
{
}

void CLayerVisibilityControl::OnEndDrag(wxListEvent& event)
{
}

void CLayerVisibilityControl::OnItemSelected(wxListEvent& event)
{
    pMapview->OnLayerChange( layidx[event.GetInt()] );
}

void CLayerVisibilityControl::OnKeyDown(wxListEvent& event) {}

void CLayerVisibilityControl::OnItemChecked(wxCommandEvent& event)
{
    int idx=-1;
    for (int i=0; i<layidx.size(); i++)
        if (layidx[i]==event.GetInt())
        {   idx=i;  break;  }

    if (idx==-1) return;

    pMapview->OnLayerToggleVisibility(event.GetInt(),IsChecked( layidx[event.GetInt()] ));
}
