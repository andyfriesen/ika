
#ifndef ZONEEDITOR_H
#define ZONEEDITOR_H

#include "wx/wx.h"

class CMapView;
class Map;

class ZoneEditor : public wxDialog
{
private:
    CMapView*   _parent;
    Map*        _map;
    int         _curzone;

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

    void InitControls();
public:
    ZoneEditor(CMapView* parent, Map* m);
};

#endif