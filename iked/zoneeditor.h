
#ifndef ZONEEDITOR_H
#define ZONEEDITOR_H

#include "wx/wx.h"
#include "types.h"

class MapView;
struct Map;

class ZoneEditor : public wxDialog
{
private:
    MapView*   _parent;
    Map*        _map;
    uint        _curzone;

    wxListBox*  _zonelist;
    wxTextCtrl* _nameedit;
    wxTextCtrl* _descedit;
    wxTextCtrl* _scriptedit;
    wxTextCtrl* _entscriptedit;
    wxTextCtrl* _chanceedit;
    wxTextCtrl* _delayedit;
    wxCheckBox* _adjacentactivate;

    void UpdateList();
    void UpdateData();
    void UpdateDlg();

    DECLARE_EVENT_TABLE()

    void OnSelectZone(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnNewZone(wxCommandEvent& event);
    void OnDelZone(wxCommandEvent& event);

public:
    ZoneEditor(MapView* parent, Map* m);
};

#endif