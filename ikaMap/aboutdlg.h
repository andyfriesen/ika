#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include "wxinc.h"
#include <vector>

struct MainWindow;

struct AboutDlg : public wxDialog
{
public:
    AboutDlg(MainWindow* mainWnd);

    DECLARE_EVENT_TABLE()
};

#endif