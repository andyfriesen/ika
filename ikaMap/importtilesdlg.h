#ifndef IMPORTTILESDLG_H
#define IMPORTTILESDLG_H

#include "wxinc.h"
#include <string>
#include <vector>

#include "Canvas.h"

struct ImportTilesDlg : public wxDialog
{
public:
    /// User's chosen settings.
    bool _append;
    bool _pad;
    bool _autoCount;
    uint _width;
    uint _height;
    uint _rowSize;
    uint _numTiles;
    std::string _fileName;

    /// The tiles we grabbed.
    std::vector<Canvas> tiles;

private:
    uint _defaultWidth;
    uint _defaultHeight;

public:
    ImportTilesDlg(wxWindow* parent);

    int ShowModal();
    int ShowModal(uint width, uint height); // the defaults for width and height.  If the append option is unchecked, then the width and height are locked to these values.

private:
    void OnOk(wxCommandEvent& event);
    void OnCheckAppend(wxCommandEvent& event);
    void OnCheckAutoCount(wxCommandEvent& event);
    void OnBrowse(wxCommandEvent& event);

    void SetDefaultValues();
    void ImportTiles(int width, int height, int numFrames, int rowSize, const std::string& fileName, bool pad, bool autoCount);

    DECLARE_EVENT_TABLE()

};

#endif