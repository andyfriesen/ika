
#include "entityeditor.h"
#include "map.h"

#include "main.h"

#include <wx\resource.h>

CEntityEditor::CEntityEditor(CMapView* parent,Map* m)
: wxDialog((wxWindow*)parent,-1,"Entities",wxDefaultPosition)
{
    pParent=parent;

//*
    pEntlist=new wxListBox(this,-1);
/*/
    LoadFromResource((wxWindow*)parent,"Entities");
    pEntlist=(wxListBox*)wxFindWindowByName("EntList",this);    
//*/

    pMap=m;

    Update();
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