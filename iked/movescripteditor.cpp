
#include "movescripteditor.h"
#include "spriteset.h"
#include "spritesetview.h"
#include "chr.h"

//#include <wx\resource.h>

BEGIN_EVENT_TABLE(CMovescriptEditor,wxDialog)
    EVT_CLOSE(CMovescriptEditor::OnClose)
END_EVENT_TABLE()


CMovescriptEditor::CMovescriptEditor(CSpriteSetView* parent,CSpriteSet* sprite,int idx):
wxDialog((wxWindow*)parent,-1,"CHR Properties",wxDefaultPosition,wxSize(400,225)),
pParent(parent),
pSprite(sprite),
nCurframe(idx)
{

    InitControls();
    UpdateDlg();

}

void CMovescriptEditor::InitControls()
{
#if 1
    // AGHAGHAGHAGHAGHAGHAGH

    const int column[] =
    {
        10, 85, 180, 245
    };

    const int line[] =
    {
        5, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205, 225,
    };

    const wxSize editsize=wxSize(50,20);
    const wxSize bigedit=wxSize(300,20);
    const wxSize labelsize=wxSize(68,20);
    const wxSize smalllabel=wxSize(14,20);

    for (int i=0; i<4; i++)
        movescript.push_back(new wxTextCtrl(this,-1,"",wxPoint(column[1],line[i]),bigedit));

    pDesc       =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[4]),bigedit);

    pHotx       =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[5]),editsize);
    pHoty       =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[5]),editsize);
    pHotw       =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[6]),editsize);
    pHoth       =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[6]),editsize);

    pLeft       =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[7]),editsize);
    pRight      =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[7]),editsize);
    pUp         =new wxTextCtrl(this,-1,"",wxPoint(column[1],line[8]),editsize);
    pDown       =new wxTextCtrl(this,-1,"",wxPoint(column[3],line[8]),editsize);

    new wxStaticText(this,-1,"Moving right",        wxPoint(column[0],line[0]),labelsize);
    new wxStaticText(this,-1,"Moving left",         wxPoint(column[0],line[1]),labelsize);
    new wxStaticText(this,-1,"Moving up",           wxPoint(column[0],line[2]),labelsize);
    new wxStaticText(this,-1,"Moving down",         wxPoint(column[0],line[3]),labelsize);

    new wxStaticText(this,-1,"Description",         wxPoint(column[0],line[4]),labelsize);
    new wxStaticText(this,-1,"Hot X",               wxPoint(column[0],line[5]),labelsize);
    new wxStaticText(this,-1,"Hot Y",               wxPoint(column[2],line[5]),labelsize);
    new wxStaticText(this,-1,"Hot Width",           wxPoint(column[0],line[6]),labelsize);
    new wxStaticText(this,-1,"Hot Height",          wxPoint(column[2],line[6]),labelsize);

    new wxStaticText(this,-1,"Idle left",           wxPoint(column[0],line[7]),labelsize);
    new wxStaticText(this,-1,"Idle right",          wxPoint(column[2],line[7]),labelsize);
    new wxStaticText(this,-1,"Idle up",             wxPoint(column[0],line[8]),labelsize);
    new wxStaticText(this,-1,"Idle down",           wxPoint(column[2],line[8]),labelsize);

#else
    // LoadFromResource((wxWindow*)parent,"dialog1");
    // pMovlist=(wxListBox*)wxFindWindowByName("Movescript",this);    
#endif

}

void CMovescriptEditor::UpdateDlg()
{
    CCHRfile& chr=pSprite->GetCHR();

    for (int i=0; i<movescript.size(); i++)
    {
        wxTextCtrl* p=movescript[i];
        
        p->SetValue(chr.sMovescript[i].c_str());
    }

    pDesc->SetValue(chr.sDescription.c_str());

    pHotx->SetValue(ToString(chr.HotX(nCurframe)).c_str());
    pHoty->SetValue(ToString(chr.HotY(nCurframe)).c_str());
    pHotw->SetValue(ToString(chr.HotW(nCurframe)).c_str());
    pHoth->SetValue(ToString(chr.HotH(nCurframe)).c_str());

    pLeft->SetValue(chr.sMovescript[8+face_left].c_str());
    pRight->SetValue(chr.sMovescript[8+face_right].c_str());
    pUp->SetValue(chr.sMovescript[8+face_up].c_str());
    pDown->SetValue(chr.sMovescript[8+face_down].c_str());

}

void CMovescriptEditor::UpdateData()
{
    CCHRfile& chr=pSprite->GetCHR();

    for (int i=0; i<movescript.size(); i++)
        chr.sMovescript[i]=movescript[i]->GetValue().c_str();

    
    chr.sDescription=pDesc->GetValue().c_str();

    chr.sMovescript[8+face_left]    =   pLeft->GetValue().c_str();
    chr.sMovescript[8+face_right]   =   pRight->GetValue().c_str();
    chr.sMovescript[8+face_up]      =   pUp->GetValue().c_str();
    chr.sMovescript[8+face_down]    =   pDown->GetValue().c_str();

    int& hotx=chr.HotX(nCurframe);
    int& hoty=chr.HotY(nCurframe);
    int& hotw=chr.HotW(nCurframe);
    int& hoth=chr.HotH(nCurframe);

    hotx=atoi(pHotx->GetValue().c_str());
    hoty=atoi(pHoty->GetValue().c_str());
    hotw=atoi(pHotw->GetValue().c_str());
    hoth=atoi(pHoth->GetValue().c_str());

}

void CMovescriptEditor::OnClose(wxCommandEvent& event)
{
    UpdateData();
    Show(false);
}



