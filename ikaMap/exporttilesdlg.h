#ifndef EXPORTTILESDLG_H
#define EXPORTTILESDLG_H

#include "wxinc.h"
#include <string>
#include <vector>

#include "common/Canvas.h"

struct Executor;

struct ExportTilesDlg : public wxDialog
{
    /// User's chosen settings.
    bool _pad;
    uint _rowSize;
    std::string _fileName;
    Executor* _executor;

    ExportTilesDlg(Executor* executor);

    int ShowModal();

private:

    void OnOk(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);

    void SetDefaultValues();

    DECLARE_EVENT_TABLE()

};

#endif
