
#include "configdlg.h"
#include <wx\resource.h>
#include <wx\utils.h>
#include "configfile.h"

enum
{
    ID_FULLSCREEN=20001,
    ID_ENABLESOUND,
    ID_ENABLELOGGING,
    ID_PIXELDEPTH,
    ID_GRAPHDRIVER,
};

BEGIN_EVENT_TABLE(CConfigDlg,wxDialog)
    EVT_BUTTON(wxID_OK,CConfigDlg::OnOK)
    EVT_BUTTON(wxID_CANCEL,CConfigDlg::OnCancel)
END_EVENT_TABLE()

CConfigDlg::CConfigDlg(wxWindow* parent,
           wxWindowID id,
           const std::string& name)
           : wxDialog(parent,id,"ika Configuration",wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL,name.c_str()),
             sFilename(name)
       
{
    LoadFromResource(parent,"ConfigDlg");

    pFullscreenbox  =(wxCheckBox*)wxFindWindowByName("fullscreen",this);
    pSoundbox       =(wxCheckBox*)wxFindWindowByName("enablesound",this);
    pLogbox         =(wxCheckBox*)wxFindWindowByName("enablelogging",this);
    pPixeldepthselector
                    =wxFindWindowByName("pixeldepth",this);
    pGraphdriverselector
                    =wxFindWindowByName("graphdriver",this);

    if (name!="")
        Load(name.c_str());

    Update();
}

void CConfigDlg::Load(const string& fname)
{
    cfg.Load(fname.c_str());
    Update();
}

void CConfigDlg::Save(const string& fname)
{
    cfg.Add("fullscreen",pFullscreenbox->GetValue()?"1":"0");
    cfg.Add("sounddriver",pSoundbox->GetValue()?"sys\\sfx_mikmod.dll":"");
    cfg.Add("log",pLogbox->GetValue()?"1":"0");

    cfg.Save(fname.c_str());
}

void CConfigDlg::Update()
{
    pFullscreenbox->SetValue(cfg.Int("fullscreen")!=0);
    pSoundbox->SetValue     (cfg["sounddriver"]!="");
    pLogbox->SetValue       (cfg.Int("log")!=0);
}

void CConfigDlg::OnOk(wxCommandEvent&)
{
    Save(sFilename);
}

void CConfigDlg::OnCancel(wxCommandEvent&)
{
    EndModal(wxID_CANCEL);
}