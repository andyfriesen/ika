
#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H

#include "wx\wx.h"

class CMapView;
class Map;

class CEntityEditor : public wxDialog
{
    CMapView*   pParent;
    Map*        pMap;
    int         nCurentidx;
    
    // convenience or the listbox
    wxListBox*  pEntlist;

public:
    CEntityEditor(CMapView* parent, Map* m);

private:
    void InitControls();

    template <class T>
        T* get(const char* name, T* = 0)
    {
        return (T*)FindWindowByName(name);
    }

public:
    void UpdateList();
    void UpdateData();
    void UpdateDlg();

    DECLARE_EVENT_TABLE()

    void OnSelectEntity(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
};

#endif