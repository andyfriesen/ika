
#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H

#include <wx\wx.h>

class CMapView;
class Map;

class CEntityEditor : public wxDialog
{
    CMapView*   pParent;
    Map*        pMap;

    // Convenience pointers to dialog elements
    wxListBox*  pEntlist;

public:
    CEntityEditor(CMapView* parent,Map* m);

    void Update();
};

#endif