
#include "main.h"
#include "mapview.h"
#include "entityeditor.h"
#include "map.h"

//#include "wx\resource.h"

BEGIN_EVENT_TABLE(CEntityEditor, wxDialog)
    EVT_LISTBOX(id_entlist, CEntityEditor::OnSelectEntity)
    EVT_CLOSE(CEntityEditor::OnClose)
END_EVENT_TABLE()

CEntityEditor::CEntityEditor(CMapView* parent, Map* m)
: wxDialog((wxWindow*)parent, -1, "Entities", wxDefaultPosition, wxSize(600, 285))
{
    pParent=parent;

    pMap=m;

    InitControls();

    UpdateList();
}

void CEntityEditor::InitControls()
{
#if 1
    // HUUUUUUUUUUURRRRRRRRRRRRK

    wxString sPatterns[6]=
    {
        "Stopped",
        "Wander",
        "Wander Rect",
        "Wander Zone",
        "Chasing",
        "Scripted"
    };

    const int column[] =
    {
        150, 220, 275, 290
    };

    const int line[] =
    {
        5, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205, 225,
    };

    const wxSize editsize=wxSize(50, 20);
    const wxSize bigedit=wxSize(120, 20);
    const wxSize labelsize=wxSize(70, 20);
    const wxSize smalllabel=wxSize(14, 20);

    pEntlist     = new wxListBox(this, id_entlist, wxPoint(5, 5), wxSize(140, 245));
    pMovepattern = new wxRadioBox(this, -1, "Movement pattern", wxPoint(350, 5), wxSize(140, 140), 6, sPatterns, 1);

    pIsobs  = new wxCheckBox(this, -1, "Entity &Obstructs other entities", wxPoint(347, 150), wxSize(200, 10));
    pMapobs = new wxCheckBox(this, -1, "Entity is obstructed by the &Map", wxPoint(347, 165), wxSize(200, 10));
    pEntobs = new wxCheckBox(this, -1, "Entity is obstructed by other &Entities", wxPoint(347, 180), wxSize(200, 10));

    pName           = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 0]), bigedit);
    pX              = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 1]), editsize);
    pY              = new wxTextCtrl(this, -1, "", wxPoint(column[3], line[ 1]), editsize);
    pSprite         = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 2]), bigedit);
    pSpeed          = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 3]), editsize);
    pActscript      = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 4]), bigedit);
    pMovescript     = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 5]), bigedit);
    pSteps          = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 6]), editsize);
    pDelay          = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 7]), editsize);
    pX1             = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 8]), editsize);
    pY1             = new wxTextCtrl(this, -1, "", wxPoint(column[3], line[ 8]), editsize);
    pX2             = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[ 9]), editsize);
    pY2             = new wxTextCtrl(this, -1, "", wxPoint(column[3], line[ 9]), editsize);
    pZone           = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[10]), editsize);
    pChasetarget    = new wxTextCtrl(this, -1, "", wxPoint(column[1], line[11]), bigedit);

    // Text labels
    new wxStaticText(this, -1, "Name",        wxPoint(column[0], line[0]), labelsize);
    new wxStaticText(this, -1, "X",           wxPoint(column[0], line[1]), smalllabel);
    new wxStaticText(this, -1, "Y",           wxPoint(column[2], line[1]), smalllabel);
    new wxStaticText(this, -1, "Sprite",      wxPoint(column[0], line[2]), labelsize);
    new wxStaticText(this, -1, "Speed",       wxPoint(column[0], line[3]), labelsize);
    new wxStaticText(this, -1, "Script",      wxPoint(column[0], line[4]), labelsize);
    new wxStaticText(this, -1, "MScript",     wxPoint(column[0], line[5]), labelsize);
    new wxStaticText(this, -1, "Steps",       wxPoint(column[0], line[6]), labelsize);
    new wxStaticText(this, -1, "Delay",       wxPoint(column[0], line[7]), labelsize);
    new wxStaticText(this, -1, "X1",          wxPoint(column[0], line[8]), smalllabel);
    new wxStaticText(this, -1, "Y1",          wxPoint(column[2], line[8]), smalllabel);
    new wxStaticText(this, -1, "X2",          wxPoint(column[0], line[9]), smalllabel);
    new wxStaticText(this, -1, "Y2",          wxPoint(column[2], line[9]), smalllabel);
    new wxStaticText(this, -1, "Zone",        wxPoint(column[0], line[10]), labelsize);
    new wxStaticText(this, -1, "Chase Target", wxPoint(column[0], line[11]), labelsize);

#else
    // I REALLY REALLY REALLY wish this worked. ;)
    //LoadFromResource((wxWindow*)parent, "dialog1");
    pEntlist = (wxListBox*)wxFindWindowByName("EntList", this);    
#endif
}

void CEntityEditor::UpdateList()
{
    int nCurrent=pEntlist->GetSelection();

    pEntlist->Clear();

    for (int i=0; i<pMap->NumEnts(); i++)
    {
        const SMapEntity& e=pMap->GetEntity(i);

        pEntlist->Append(e.sName.c_str());
    }

    if (nCurrent != -1)
        pEntlist->SetSelection(nCurrent);
    nCurentidx=-1;
    UpdateDlg();
}

void CEntityEditor::UpdateData()
{
    if (nCurentidx==-1) return;

    SMapEntity& e=pMap->GetEntity(nCurentidx);

    if (e.sName!=pName->GetValue().c_str())
    {
        wxString bleh=pName->GetValue().c_str();
        pEntlist->InsertItems(1, &bleh, nCurentidx);
        pEntlist->Delete(nCurentidx+1);
    }

    e.sName             =pName->GetValue().c_str();
    e.x                 =atoi(pX->GetValue().c_str());
    e.y                 =atoi(pY->GetValue().c_str());
    e.sCHRname          =pSprite->GetValue().c_str();
    e.nSpeed            =atoi(pY->GetValue().c_str());
    e.sActscript        =pActscript->GetValue().c_str();
    e.sMovescript       =pMovescript->GetValue().c_str();
    e.nWandersteps      =atoi(pSteps->GetValue().c_str());
    e.nWanderdelay      =atoi(pDelay->GetValue().c_str());
    e.wanderrect.left   =atoi(pX1->GetValue().c_str());
    e.wanderrect.top    =atoi(pY1->GetValue().c_str());
    e.wanderrect.right  =atoi(pX2->GetValue().c_str());
    e.wanderrect.bottom =atoi(pY2->GetValue().c_str());
    e.sZone             =pZone->GetValue().c_str();
    e.sChasetarget      =pChasetarget->GetValue().c_str();
    e.bIsobs            =pIsobs->GetValue();
    e.bMapobs           =pMapobs->GetValue();
    e.bEntobs           =pEntobs->GetValue();
}

void CEntityEditor::UpdateDlg()
{
    const SMapEntity& e=pMap->GetEntity(pEntlist->GetSelection());

    pName       ->SetValue(e.sName.c_str());
    pX          ->SetValue(ToString(e.x).c_str());
    pY          ->SetValue(ToString(e.y).c_str());
    pSprite     ->SetValue(e.sCHRname.c_str());
    pSpeed      ->SetValue(ToString(e.nSpeed).c_str());
    pActscript  ->SetValue(e.sActscript.c_str());
    pMovescript ->SetValue(e.sMovescript.c_str());
    pSteps      ->SetValue(ToString(e.nWandersteps).c_str());
    pDelay      ->SetValue(ToString(e.nWanderdelay).c_str());
    pX1         ->SetValue(ToString(e.wanderrect.left).c_str());
    pY1         ->SetValue(ToString(e.wanderrect.top).c_str());
    pX2         ->SetValue(ToString(e.wanderrect.right).c_str());
    pY2         ->SetValue(ToString(e.wanderrect.bottom).c_str());
    pZone       ->SetValue(e.sZone.c_str());
    pChasetarget->SetValue(e.sChasetarget.c_str());
    pIsobs      ->SetValue(e.bIsobs);
    pMapobs     ->SetValue(e.bMapobs);
    pEntobs     ->SetValue(e.bEntobs);
}

void CEntityEditor::OnSelectEntity(wxCommandEvent& event)
{
    UpdateData();
    nCurentidx=event.GetInt();
    UpdateDlg();
}

void CEntityEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    Show(false);
}