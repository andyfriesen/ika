
#ifndef ZONEPROPERTIESDLG_H
#define ZONEPROPERTIESDLG_H

#include "wx/wx.h"
#include "common/utility.h"

struct Executor;
struct Map;

struct ZonePropertiesDlg : public wxDialog {
private:
    Executor*   _executor;
    uint        _layerIndex;
    uint        _zoneIndex;

    wxListBox*  _bluePrintList;
    wxTextCtrl* _editLabel;
    wxTextCtrl* _editScript;
    wxTextCtrl* _editX;
    wxTextCtrl* _editY;
    wxTextCtrl* _editWidth;
    wxTextCtrl* _editHeight;

    Map*    GetMap() const;

    void UpdateList();
    void UpdateData();
    void UpdateDlg();
    void OnSelectBlueprint(wxCommandEvent&);
    void OnClose(wxCloseEvent& event);
    void OnNewBlueprint(wxCommandEvent&);
    void OnDeleteBlueprint(wxCommandEvent&);
    void OnRenameBlueprint(wxCommandEvent&);
    void OnDeleteZone(wxCommandEvent&);
    void OnOk(wxCommandEvent&);

public:
    ZonePropertiesDlg(Executor* e, uint layerIndex, uint zoneIndex);

    DECLARE_EVENT_TABLE()
};

#endif