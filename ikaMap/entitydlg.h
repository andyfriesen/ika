
#ifndef ENTITYDLG_H
#define ENTITYDLG_H

#include "wxinc.h"

#include "common/map.h"

struct Executor;

struct EntityDlg : public wxDialog
{
    Executor* _executor;
    uint _layer;
    uint _index;

private:
    template <typename T>
        T* get(const char* name, T* = 0)
    {
        T* t = (T*)FindWindowByName(name);
        wxASSERT(t);
        return t;
    }

public:
    EntityDlg(Executor* e, uint layer, uint index);

    void UpdateData();
    void UpdateDlg();

    void OnOK(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);

    Map::Entity newData;

    DECLARE_EVENT_TABLE()
};

#endif