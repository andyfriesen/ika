
#include "wxinc.h"
#include "common/map.h"
#include "mapview.h"
#include "video.h"
#include "command.h"
#include "zonepropertiesdlg.h"

#include "zoneeditstate.h"

ZoneEditState::ZoneEditState(Executor* e)
    : EditState(e, "Zones")
    , _curZoneIndex(-1)
    , _dragging(false)
{}

void ZoneEditState::OnMouseDown(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToLayer(x, y);
    uint zoneIndex = GetMapView()->ZoneAt(x, y, GetCurLayerIndex());

    if (zoneIndex == -1 && event.LeftDown() && event.ShiftDown())   // shift+left drag to create a zone
    {
        _dragging = true;
        _selection = Rect(x, y, x, y);
    }
    else if (event.LeftDClick() && zoneIndex != -1)
    {
        _curZoneIndex = zoneIndex;
        ZonePropertiesDlg dlg(GetExecutor(), GetCurLayerIndex(), _curZoneIndex);

        // Show the zone properties dialog
        dlg.ShowModal();

        GetMapView()->Refresh();
    }
}

void ZoneEditState::OnMouseUp(wxMouseEvent& event)
{
    if (_dragging)
    {
        _dragging = false;

        _selection.Normalize();
        if (_selection.Width() > 1 && _selection.Height() > 1)
        {
            // Need an intuitive way to set the default blueprint.
            HandleCommand(new PlaceZoneCommand(_selection, GetCurLayerIndex(), ""));
        }
    }
}

void ZoneEditState::OnMouseMove(wxMouseEvent& event)
{
    int x = event.GetX();
    int y = event.GetY();
    GetMapView()->ScreenToLayer(x, y);

    if (_dragging)
    {
        _selection.right = x;
        _selection.bottom = y;
        GetMapView()->Refresh();
    }
}

void ZoneEditState::OnRender()
{
    Map::Layer* layer = GetCurLayer();
    std::vector<Map::Layer::Zone>& zones = GetCurLayer()->zones;
    VideoFrame* video = GetMapView()->GetVideo();

    for (uint i = 0; i < zones.size(); i++)
    {
        Map::Layer::Zone& zone = zones[i];

        int x = zone.position.left + layer->x - GetMapView()->GetXWin();
        int y = zone.position.top + layer->y  - GetMapView()->GetYWin();
        int w = zone.position.Width();
        int h = zone.position.Height();

        RGBA colour =
            (i == _curZoneIndex) ?
                RGBA(192, 192, 255, 128) :
                RGBA(  0, 192, 255, 128);

        video->DrawRectFill(x, y, w, h, colour);

        colour.a = 255;
        video->DrawRect    (x, y, w, h, RGBA(0, 192, 255, 255));
    }

    if (_dragging)
    {
        int x = _selection.left + layer->x - GetMapView()->GetXWin();
        int y = _selection.top + layer->y  - GetMapView()->GetYWin();
        int w = _selection.Width();
        int h = _selection.Height();

        video->DrawRectFill(x, y, w, h, RGBA(255, 192, 192, 128));
        video->DrawRect    (x, y, w, h, RGBA(255, 192, 192, 255));
    }
}

void ZoneEditState::OnSwitchLayers(uint oldLayer, uint newLayer)
{
    _curZoneIndex = -1;
}