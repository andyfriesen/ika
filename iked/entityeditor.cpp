
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
    pEntlist=new wxListBox(this,-1,wxPoint(5,5),wxSize(140,365));

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
        0, 220, 0, 290
    };

    const int line[] =
    {
        5, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205, 225, 245, 265, 285, 305
    };

    const wxSize editsize=wxSize(50,20);

    pMovepattern=new wxRadioBox(this,-1,"Movement pattern",wxPoint(350,5),wxSize(140,140),6,sPatterns,1);

    pIsobs=new wxCheckBox(this,-1,"Entity &Obstructs other entities",wxPoint(347,150),wxSize(200,10));
    pMapobs=new wxCheckBox(this,-1,"Entity is obstructed by the &Map",wxPoint(347,165),wxSize(200,10));
    pEntobs=new wxCheckBox(this,-1,"Entity is obstructed by other &Entities",wxPoint(347,180),wxSize(200,10));

    pX          =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 0]),editsize);
    pY          =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[ 0]),editsize);
    pSprite     =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 1]),editsize);
    pSpeed      =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 2]),editsize);
    pActscript  =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 3]),editsize);
    pMovescript =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 4]),editsize);
    pSteps      =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 5]),editsize);
    pDelay      =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 6]),editsize);
    pX1         =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 7]),editsize);
    pY1         =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[ 7]),editsize);
    pX2         =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 8]),editsize);
    pY2         =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[ 8]),editsize);
    pZone       =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[ 9]),editsize);
    pChasetarget=new wxTextCtrl(this,-1,"",wxPoint(column[1],line[10]),editsize);

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