
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
    wxRadioBox* pMovepattern;
    wxCheckBox* pIsobs;
    wxCheckBox* pMapobs;
    wxCheckBox* pEntobs;

    wxTextCtrl* pY;
    wxTextCtrl* pX;
    wxTextCtrl* pSprite;
    wxTextCtrl* pSpeed;
    wxTextCtrl* pActscript;
    wxTextCtrl* pMovescript;
    wxTextCtrl* pSteps;
    wxTextCtrl* pDelay;
    wxTextCtrl* pX1;
    wxTextCtrl* pY1;
    wxTextCtrl* pX2;
    wxTextCtrl* pY2;
    wxTextCtrl* pZone;
    wxTextCtrl* pChasetarget;
    wxListBox*  pActmethod;

public:
    CEntityEditor(CMapView* parent,Map* m);

private:
    void InitControls();

public:
    void Update();
};

#endif