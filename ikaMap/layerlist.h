
#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <vector>
#include "wxinc.h"
#include "listener.h"
#include "events.h"

struct Map;
struct Executor;
class wxWindow;

struct LayerBox : public wxWindow
{
    LayerBox(wxWindow* parent, wxPoint = wxDefaultPosition, wxSize = wxDefaultSize); // also temp

    void SetVisibilityIcon(wxIcon& bmp);
    void SetActiveIcon(wxIcon& icon);

    void SetLabel(const std::string& label);

    void DoToggleVisibility(wxMouseEvent&);
    void DoActivateLayer(wxMouseEvent&);
    void DoRightDown(wxMouseEvent& event);

    DECLARE_EVENT_TABLE();

private:
    enum
    {
        VIS_ICON,
        ACTIVE_ICON
    };

    wxBitmapButton* _visibilityIcon;
    wxBitmapButton* _activeIcon;
    wxStaticText*   _label;
};

struct LayerList : public wxScrolledWindow
{
public:
    LayerList(Executor* executor, wxWindow* parent, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize);

    // These happen in response to events from executor.
    void OnMapLayersChanged(const MapEvent& event);
    void OnVisibilityChanged(const MapEvent& event);
    void OnLayerActivated(uint index);

    // These we recieve from the LayerBoxes.
    void OnToggleVisibility(wxCommandEvent& event);
    void OnActivateLayer(wxCommandEvent& event);

    void Update(Map* map);
    void UpdateIcons();

    DECLARE_EVENT_TABLE();
private:

    std::vector<LayerBox*> _boxes;  // little UI widget box thingies.
    Executor* _executor;            // bleh. -_-;

    wxBoxSizer* _sizer;

    wxIcon _visibleIcon;
    wxIcon _activeIcon;
    wxIcon _blankIcon;
};

#endif
