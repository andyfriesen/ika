
#include "layerlist.h"
#include "mainwindow.h"
#include "map.h"
#include "log.h"

namespace
{
    const int RIDICULOUSLY_HUGE_CONSTANT = 1000;

    enum
    {
        id_showLayer = wxID_HIGHEST + 1,
        id_activateLayer = id_showLayer + RIDICULOUSLY_HUGE_CONSTANT,
        id_layerProperties
    };
}

BEGIN_EVENT_TABLE(LayerBox, wxWindow)
    EVT_BUTTON(LayerBox::VIS_ICON, LayerBox::DoToggleVisibility)
    EVT_BUTTON(LayerBox::ACTIVE_ICON, LayerBox::DoActivateLayer)
    EVT_COMMAND_RANGE(0, RIDICULOUSLY_HUGE_CONSTANT, wxEVT_COMMAND_RIGHT_CLICK, LayerBox::DoRightDown)
END_EVENT_TABLE()

LayerBox::LayerBox(wxWindow* parent, wxPoint position, wxSize size)
    : wxWindow(parent, -1, position, size, wxSUNKEN_BORDER)
{
    wxIcon blankIcon("blankicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16);

    _visibilityIcon = new wxBitmapButton(this, VIS_ICON, blankIcon, wxDefaultPosition, wxSize(16, 16), 0);
    _activeIcon     = new wxBitmapButton(this, ACTIVE_ICON, blankIcon, wxDefaultPosition, wxSize(16, 16), 0);
    _label = new wxStaticText(this, -1, "-");

    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(_visibilityIcon, 0, wxBOTTOM, 4);
    sizer->Add(_activeIcon, 0, wxBOTTOM, 4);
    sizer->Add(_label, 1, wxALIGN_CENTER_VERTICAL);
    SetSizer(sizer);
    sizer->Fit(this);
}

void LayerBox::SetVisibilityIcon(wxIcon& icon)
{
    _visibilityIcon->SetLabel(icon);
}

void LayerBox::SetActiveIcon(wxIcon& icon)
{
    _activeIcon->SetLabel(icon);
}

void LayerBox::DoToggleVisibility(wxMouseEvent&)
{
    GetParent()->AddPendingEvent(wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, id_showLayer + GetId()));
    Log::Write("Toggle! %s", _label->GetLabel().c_str());
}

void LayerBox::DoActivateLayer(wxMouseEvent&)
{
    GetParent()->AddPendingEvent(wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, id_activateLayer + GetId()));
    Log::Write("Activate! %s", _label->GetLabel().c_str());
}

void LayerBox::DoRightDown(wxMouseEvent& event)
{
    GetParent()->AddPendingEvent(wxCommandEvent(wxEVT_COMMAND_BUTTON_CLICKED, id_layerProperties + GetId()));
    Log::Write("Right click %s", _label->GetLabel().c_str());
}

void LayerBox::SetLabel(const std::string& label)
{
    _label->SetLabel(label.c_str());
    Layout();
}

BEGIN_EVENT_TABLE(LayerList, wxScrolledWindow)
    EVT_COMMAND_RANGE(id_showLayer,     id_showLayer     + RIDICULOUSLY_HUGE_CONSTANT - 1, wxEVT_COMMAND_BUTTON_CLICKED, LayerList::OnToggleVisibility)
    EVT_COMMAND_RANGE(id_activateLayer, id_activateLayer + RIDICULOUSLY_HUGE_CONSTANT - 1, wxEVT_COMMAND_BUTTON_CLICKED, LayerList::OnActivateLayer)
    EVT_COMMAND_RANGE(id_layerProperties, id_layerProperties + RIDICULOUSLY_HUGE_CONSTANT - 1, wxEVT_COMMAND_BUTTON_CLICKED, LayerList::OnShowLayerMenu)
END_EVENT_TABLE()

LayerList::LayerList(Executor* executor, wxWindow* parent, wxPoint position, wxSize size)
    : wxScrolledWindow(parent, -1, position, size)
    , _executor(executor)
    , _visibleIcon("zoomicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16)
    , _activeIcon("brushicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16)
    , _blankIcon("blankicon", wxBITMAP_TYPE_ICO_RESOURCE, 16, 16)
{
    _sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(_sizer);
    Layout();
    SetScrollRate(0, 1);
}

void LayerList::OnMapLayersChanged(const MapEvent& event)
{
    // probably not a good idea.  Better to only do this when 
    // layers have been moved around, destroyed, created, etc.
    // But what the fuck.
    Update(event.map);
}

void LayerList::OnVisibilityChanged(const MapEvent& event)
{
    Freeze();
    UpdateIcons();
    Thaw();
}

void LayerList::OnLayerActivated(uint index)
{
    Freeze();
    UpdateIcons();
    Thaw();
}

void LayerList::OnToggleVisibility(wxCommandEvent& event)
{
    int layerIndex = event.GetId() - id_showLayer;

    wxASSERT(layerIndex >= 0 && layerIndex < _executor->GetMap()->NumLayers());

    bool b = !_executor->IsLayerVisible(layerIndex);
    _executor->ShowLayer(layerIndex, b);
    /*_boxes[layerIndex]->SetVisibilityIcon(
        b ? _visibleIcon
          : _blankIcon);*/
}

void LayerList::OnActivateLayer(wxCommandEvent& event)
{
    int layerIndex = event.GetId() - id_activateLayer;

    wxASSERT(layerIndex >= 0 && layerIndex < _executor->GetMap()->NumLayers());

    _executor->SetCurrentLayer(layerIndex);
}

void LayerList::OnShowLayerMenu(wxCommandEvent& event)
{
    Log::Write("Layer Menu %i", event.GetId());
}

void LayerList::Update(Map* map)
{
    // shorthand.
    const int n = map->NumLayers();

    Freeze();

    // Remove extra boxes that we don't want any more.
    for (int i = _boxes.size() - 1; i >= n; i--)
    {
        _sizer->Remove(i);

        LayerBox* box = _boxes[i];
        bool b = box->Destroy();
        wxASSERT(b);
        _boxes.pop_back();   // remove from our vector
    }

    // Add extra boxes.
    for (int i = _boxes.size(); i < n; i++)
    {
        LayerBox* box = new LayerBox(this);
        _boxes.push_back(box);
        _sizer->Add(box, 0, wxEXPAND);
    }

    // Now we're sure we have the right number of controls,
    // we can rename them, and set properties and junk.
    for (int i = 0; i < n; i++)
    {
        _boxes[i]->SetLabel(map->GetLayer(i)->label);
        _boxes[i]->SetId(i);
    }

    UpdateIcons();

    _sizer->Layout();
    Thaw();
}

void LayerList::UpdateIcons()
{
    const curLayer = _executor->GetCurrentLayer();

    for (uint i = 0; i < _boxes.size(); i++)
    {
        _boxes[i]->SetVisibilityIcon(
            _executor->IsLayerVisible(i)
                ? _visibleIcon
                : _blankIcon
            );
        _boxes[i]->SetActiveIcon(
            curLayer == i
                ? _activeIcon
                : _blankIcon
            );
    }
}