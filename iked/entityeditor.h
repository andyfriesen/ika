
#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H

#include "wx\wx.h"

class MapView;
struct Map;

class EntityEditor : public wxDialog
{
    MapView*     _parent;
    Map*         _map;
    Map::Layer::Entity* _curEnt;
    
    // convenience or the listbox
    //wxListBox*   _entList; // TODO: re-enable this

public:
    EntityEditor(MapView* parent, Map* m);

private:
    void InitControls();

    template <class T>
        T* get(const char* name, T* = 0)
    {
        T* t = (T*)FindWindowByName(name);
        assert(t);
        return t;
    }

public:
    void UpdateList();
    void UpdateData();
    void UpdateDlg();

    void OnSelectEntity(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);

    void Show(Map::Layer::Entity* ent);

    DECLARE_EVENT_TABLE()
};

#endif