
#include <cassert>
#include "wxinc.h"
#include <wx/xrc/xmlres.h>

#include "mapdlg.h"

#include "mainwindow.h"
#include "command.h"
#include "common/map.h"
#include "tileset.h"
#include "common/utility.h"

BEGIN_EVENT_TABLE(MapDlg, wxDialog)
    EVT_BUTTON(wxID_OK, MapDlg::OnOk)
    EVT_BUTTON(XRCID("button_autosize"), MapDlg::OnAutoSize)
END_EVENT_TABLE()

template <typename T>
T* MapDlg::Ctrl(const std::string& name, const T*)
{
    wxWindow* t = FindWindow(XRCID(name.c_str()));

#ifdef DEBUG
    wxASSERT(wxDynamicCast(t, T));
#endif

    return (T*)t;
}

void MapDlg::OnOk(wxCommandEvent& event)
{
    title  = Ctrl<wxTextCtrl>("edit_title")->GetValue().c_str();
    width  = atoi(Ctrl<wxTextCtrl>("edit_width")->GetValue().c_str());
    height = atoi(Ctrl<wxTextCtrl>("edit_height")->GetValue().c_str());

    Map* map = _mainWnd->GetMap();

    if (width < 1 || height < 1)
        wxMessageBox("Width and height must be positive integers.", "Negative sizes make no sense", wxOK | wxCENTER, this);
    else if (title  != map->title ||
        width  != map->width ||
        height != map->height)
        wxDialog::OnOK(event);
    else
        wxDialog::OnCancel(event);
}

void MapDlg::OnAutoSize(wxCommandEvent&)
{
    Map* map = _mainWnd->GetMap();
    Tileset* tileset = _mainWnd->GetTileset();

    width = height = 0;

    for (uint i = 0; i < map->NumLayers(); i++)
    {
        Map::Layer* lay = map->GetLayer(i);

        width  = max(width,  lay->x + lay->Width()  * tileset->Width());
        height = max(height, lay->y + lay->Height() * tileset->Height());
    }

    Ctrl<wxTextCtrl>("edit_width")->SetValue(va("%i", width));
    Ctrl<wxTextCtrl>("edit_height")->SetValue(va("%i", height));
}

MapDlg::MapDlg(MainWindow* parent)
    : _mainWnd(parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, "dialog_map");

    Map* map = _mainWnd->GetMap();

    Ctrl<wxTextCtrl>("edit_title")->SetValue(map->title.c_str());
    Ctrl<wxTextCtrl>("edit_width")->SetValue(va("%i", map->width));
    Ctrl<wxTextCtrl>("edit_height")->SetValue(va("%i", map->height));

    Ctrl<wxPanel>("panel_main")->GetSizer()->Fit(this);
}