
#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H

#include <wx\wx.h>

class CMapView;
class Map;

class CEntityEditor : public wxDialog
{
    enum
    {
        id_entlist=100,
    };

    CMapView*   pParent;
    Map*        pMap;
    int         nCurentidx;

    // Convenience pointers to dialog elements
    wxListBox*  pEntlist;
    wxRadioBox* pMovepattern;
    wxCheckBox* pIsobs;
    wxCheckBox* pMapobs;
    wxCheckBox* pEntobs;

    wxTextCtrl* pName;
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
    void UpdateList();
    void UpdateData();
    void UpdateDlg();

    DECLARE_EVENT_TABLE()

    void OnSelectEntity(wxCommandEvent& event);
};

#endif