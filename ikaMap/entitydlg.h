
#ifndef ENTITYDLG_H
#define ENTITYDLG_H

#include "wxinc.h"

#include "map.h"

class MainWindow;

class EntityDlg : public wxDialog
{
    MainWindow* _mainWnd;
    uint _layer;
    uint _index;

private:
    template <class T>
        T* get(const char* name, T* = 0)
    {
        T* t = (T*)FindWindowByName(name);
        wxASSERT(t);
        return t;
    }

public:
    EntityDlg(MainWindow* mw, uint layer, uint index);

    void UpdateData();
    void UpdateDlg();

    void OnOK(wxCommandEvent& event);

    Map::Entity newBlueprint;
    Map::Layer::Entity newData;
    
    DECLARE_EVENT_TABLE()
};

#endif