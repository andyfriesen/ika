#ifndef IMPORTTILESDLG_H
#define IMPORTTILESDLG_H

#include "wxinc.h"
#include <string>
#include <vector>

#include "Canvas.h"

class ImportTilesDlg : public wxDialog
{
public:
    bool append;                // nothing to do with this dialog.  We expose it so that whoever is using it can interpret the tiles properly.
    std::vector<Canvas> tiles;  // the resultant tiles
    uint width;
    uint height;

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
    void OnBrowse(wxCommandEvent& event);

    void ImportTiles(int width, int height, int numFrames, int rowSize, const std::string& fileName, bool pad);

    DECLARE_EVENT_TABLE()

};

#endif