
#include "entityeditor.h"
#include "map.h"

#include "main.h"

#include <wx\resource.h>

/*
    ack.

    wxWindows doesn't seem to like me using wxr resources, so I get to make all this crap manually. :x
*/

CEntityEditor::CEntityEditor(CMapView* parent,Map* m)
: wxDialog((wxWindow*)parent,-1,"Entities",wxDefaultPosition,wxSize(600,400))
{
    pParent=parent;

    pMap=m;

    InitControls();

    Update();
}

void CEntityEditor::InitControls()
{
//*
    pEntlist=new wxListBox(this,-1,wxPoint(5,5),wxSize(140,230));

    wxString sPatterns[6]=
    {
        "Stopped",
        "Wander",
        "Wander Rect",
        "Wander Zone",
        "Chasing",
        "Scripted"
    };

    pMovepattern=new wxRadioBox(this,-1,"Movement pattern",wxPoint(350,6),wxSize(70,75),6,sPatterns,1);

    pIsobs=new wxCheckBox(this,-1,"Entity &Obstructs other entities",wxPoint(347,92),wxSize(107,10));
    pMapobs=new wxCheckBox(this,-1,"Entity is obstructed by the &Map",wxPoint(347,102),wxSize(107,10));
    pEntobs=new wxCheckBox(this,-1,"Entity is obstructed by other &Entities",wxPoint(347,112),wxSize(107,10));

/*/
    LoadFromResource((wxWindow*)parent,"dialog1");
    pEntlist=(wxListBox*)wxFindWindowByName("EntList",this);    
//*/
}

void CEntityEditor::Update()
{
    pEntlist->Clear();

    for (int i=0; i<pMap->NumEnts(); i++)
    {
        SMapEntity e;
        pMap->GetEntInfo(e,i);

        pEntlist->Append(e.sName.c_str());
    }
}