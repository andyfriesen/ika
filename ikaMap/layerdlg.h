#ifndef LAYERDLG_H
#define LAYERDLG_H

#include "wxinc.h"
#include "common/map.h"

struct MainWindow;

struct LayerDlg : public wxDialog
{
    template <class T>
    T* Ctrl(const std::string& name, const T* = 0);

    MainWindow* _mainWnd;
    uint _layerIndex;

    void OnOk(wxCommandEvent&);
    void OnResize(wxCommandEvent&);

public:
    LayerDlg(MainWindow* parent, uint layerIndex);

    std::string label;
    bool wrapx;
    bool wrapy;
    int x;
    int y;

    DECLARE_EVENT_TABLE()
};

#endif