
#include "layerlist.h"
#include "mainwindow.h"
#include "map.h"

LayerBox::LayerBox(wxWindow* parent, const char* label, wxPoint position, wxSize size)
    : wxTextCtrl(parent, -1, label, position, size)
{}

LayerList::LayerList(MainWindow* mainWnd, wxWindow* parent, wxPoint position, wxSize size)
    : wxScrolledWindow(parent, -1, position, size)
    , _mainWnd(mainWnd)
{
    _sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(_sizer);
}

void LayerList::OnMapChange(const MapEvent& event)
{
    // probably not a good idea.  Better to only do this when 
    // layers have been moved around, destroyed, created, etc.
    // But what the fuck.
    Update(event.map);
}

void LayerList::Update(Map* map)
{
    // shorthand.
    const int n = map->NumLayers();

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
        LayerBox* box = new LayerBox(this, "-"); // leave the layer blank for the moment.
        _boxes.push_back(box);
        _sizer->Add(box, 0, wxEXPAND);
    }

    // inefficient?
    _sizer->Layout();

    // Now we're sure we have the right number of controls,
    // we can rename them, and set properties and junk.
    for (int i = 0; i < n; i++)
    {
        _boxes[i]->SetValue(map->GetLayer(i)->label.c_str());
        // TODO: show visibility icons etc
    }
}
