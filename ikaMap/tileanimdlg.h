#ifndef TILEANIMDLG_H
#define TILEANIMDLG_H

#include <vector>
#include <map>
#include "wxinc.h"
#include "common/vsp.h"

struct Command;

struct MainWindow;

struct TileAnimDlg : public wxDialog
{
public:
    TileAnimDlg(MainWindow* mainWnd);

    // Opens the dialog, returns a command.  Easy as pie.
    Command* Execute(const std::vector<VSP::AnimState>& animData);

private:
    std::vector<VSP::AnimState>    _oldData;
    std::map<uint, VSP::AnimState> _changedStrands;

    int _curStrand;
    wxListBox* _strandList;

    void UpdateDlg();
    void UpdateData();

    void OnSelectStrand(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
