
#ifndef LAYERLIST_H
#define LAYERLIST_H

#include <vector>
#include "wxinc.h"
#include "listener.h"
#include "events.h"

struct Map;
struct MainWindow; // coupling.  Gah.
class wxWindow;

struct LayerBox : public wxTextCtrl // temporary
{
public:
    LayerBox(wxWindow* parent, const char* label, wxPoint = wxDefaultPosition, wxSize = wxDefaultSize); // also temp
};

struct LayerList : public wxScrolledWindow
{
public:
    LayerList(MainWindow* mainWnd, wxWindow* parent, wxPoint position = wxDefaultPosition, wxSize size = wxDefaultSize);

    void OnMapChange(const MapEvent& event);

    void Update(Map* map);
private:

    std::vector<LayerBox*> _boxes; // little UI widget box thingies.
    MainWindow* _mainWnd;

    wxBoxSizer* _sizer;
};

#endif
