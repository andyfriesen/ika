#ifndef MAPDLG_H
#define MAPDLG_H

#include "wxinc.h"
#include "map.h"

class MainWindow;

class MapDlg : public wxDialog
{
    template <class T>
    T* Ctrl(const std::string& name, const T* = 0);

    MainWindow* _mainWnd;

    void OnOk(wxCommandEvent&);
    void OnAutoSize(wxCommandEvent&);

public:
    MapDlg(MainWindow* parent);

    std::string title;
    uint width;
    uint height;

    DECLARE_EVENT_TABLE()
};

#endif