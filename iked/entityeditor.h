
#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H

#include <wx\wx.h>

class CMapView;

class CEntityEditor : public wxDialog
{
    CMapView* pParent;

public:
    CEntityEditor(CMapView* parent);
};

#endif