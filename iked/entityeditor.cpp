
#include "entityeditor.h"
#include "main.h"

CEntityEditor::CEntityEditor(CMapView* parent)
: wxDialog((wxWindow*)parent,-1,"Entities",wxDefaultPosition,wxDefaultSize,wxCAPTION | wxDIALOG_MODELESS)
{
    pParent=parent;

    LoadFromResource(this,"ConfigDlg");
}