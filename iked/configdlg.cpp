
#include "configdlg.h"
#include "wx/resource.h"
#include "wx/utils.h"
#include "configfile.h"

enum
{
    ID_FULLSCREEN=20001,
    ID_ENABLESOUND,
    ID_ENABLELOGGING,
    ID_PIXELDEPTH,
    ID_GRAPHDRIVER,
};

BEGIN_EVENT_TABLE(CConfigDlg, wxDialog)
    EVT_BUTTON(wxID_OK, CConfigDlg::OnOK)
    EVT_BUTTON(wxID_CANCEL, CConfigDlg::OnCancel)
END_EVENT_TABLE()

// tiny converter thing
#define C(p) ConvertDialogToPixels(p)

CConfigDlg::CConfigDlg(wxWindow* parent,
           wxWindowID id,
           const std::string& name)
           : wxDialog(parent, id, "ika Configuration", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxDIALOG_MODAL, name.c_str()),
             sFilename(name)
       
{
    const wxString driverChoices[] =
    {
        "Software",
        "OpenGL"
    };
    const int numDrivers = 2;

    const wxString pixelDepthChoices[] =
    {
        "1&6bpp", "&32bpp"
    };
    const int pixelDepths = 2;

    SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL));
    SetSize(C(wxSize(225, 100)));

    //LoadFromResource(parent, "ConfigDlg");
    new wxStaticBox(this, -1, "Graphics Driver", C(wxPoint(0, 0)), C(wxSize(113, 85)), 0, "Resolution");
    new wxListBox(this, -1, C(wxPoint(1, 8)), C(wxSize(108, 75)), numDrivers, driverChoices, wxLB_EXTENDED, wxDefaultValidator, "graphdriver");
    new wxCheckBox(this, -1, "Run &Fullscreen", C(wxPoint(116, 9)), C(wxSize(64, 8)), 0, wxDefaultValidator, "fullscreen");
    new wxRadioBox(this, -1, "Pixel Depth", C(wxPoint(116, 33)), C(wxSize(56, 34)), pixelDepths, pixelDepthChoices, wxVERTICAL, wxSIMPLE_BORDER | wxRA_SPECIFY_ROWS, wxDefaultValidator, "pixeldepth");
    
    new wxCheckBox(this, -1, "Enable &Sound", C(wxPoint(116, 17)), C(wxSize(64, 8)), 0, wxDefaultValidator, "enablesound");
    new wxCheckBox(this, -1, "Enable &Logging", C(wxPoint(116, 25)), C(wxSize(64, 8)), 0, wxDefaultValidator, "enablelogging");
    new wxButton(this, -1, "OK", C(wxPoint(116, 72)), C(wxSize(40, 11)), 0, wxDefaultValidator, "ok");
    new wxButton(this, -1, "Cancel", C(wxPoint(164, 72)), C(wxSize(40, 11)), 0, wxDefaultValidator, "cancel");

    pFullscreenbox  =(wxCheckBox*)wxFindWindowByName("fullscreen", this);
    pSoundbox       =(wxCheckBox*)wxFindWindowByName("enablesound", this);
    pLogbox         =(wxCheckBox*)wxFindWindowByName("enablelogging", this);
    pPixeldepthselector
                    =wxFindWindowByName("pixeldepth", this);
    pGraphdriverselector
                    =wxFindWindowByName("graphdriver", this);

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
    cfg.Add("fullscreen", pFullscreenbox->GetValue()?"1":"0");
    cfg.Add("sounddriver", pSoundbox->GetValue()?"sys\\sfx_mikmod.dll":"");
    cfg.Add("log", pLogbox->GetValue()?"1":"0");

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