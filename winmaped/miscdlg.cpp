#include "miscdlg.h"

// -------------------------------New Map--------------------------------------

void CNewMapDlg::UpdateInfo(HWND hWnd)
{
    info.tilex=GetDlgItemInt(hWnd,IDC_EDVSPX,NULL,false);
    info.tiley=GetDlgItemInt(hWnd,IDC_EDVSPY,NULL,false);
    info.mapx =GetDlgItemInt(hWnd,IDC_EDMAPX,NULL,false);
    info.mapy =GetDlgItemInt(hWnd,IDC_EDMAPY,NULL,false);
    
    if (info.mapx<1)
        info.mapx=1;
    if (info.mapy<1)
        info.mapy=1;
    
    char c[255];
    GetDlgItemText(hWnd,IDC_EDVSPNAME,c,255);
    info.sVspname=c;
}

bool CNewMapDlg::InitProc(HWND hWnd)
{
    CheckDlgButton(hWnd,IDC_NEWVSP,BST_CHECKED);
    // set the default dimensions to 100x100
    SetDlgItemInt(hWnd,IDC_EDMAPX,100,false);
    SetDlgItemInt(hWnd,IDC_EDMAPY,100,false);
    
    // set the default tile dimensions to 16x16
    SetDlgItemInt(hWnd,IDC_EDVSPX,16,false);
    SetDlgItemInt(hWnd,IDC_EDVSPY,16,false);
    
    return true;
}

int CNewMapDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            UpdateInfo(hWnd);
            result=true;
            
            info.bLoadvsp=IsDlgButtonChecked(hWnd,IDC_LOADVSP)==BST_CHECKED;
            
            
            EndDialog(hWnd,0);
            break;
        case IDCANCEL:
            result=false;
            EndDialog(hWnd,0);
            break;
        }
    }
    return 0;
}

bool CNewMapDlg::Execute(HINSTANCE hInst,HWND hWnd)
{
    StartDlg(hInst,hWnd,"NEWMAPDLG");
    return result;
}

// ---------------------------VSP Animation Strand Editor----------------------

void CVSPAnimDlg::UpdateData(HWND hWnd)
{
    curdata.nStart=GetDlgItemInt(hWnd,IDC_EDFIRSTTILE,NULL,false);
    curdata.nFinish=GetDlgItemInt(hWnd,IDC_EDLASTTILE,NULL,false);
    curdata.nDelay=GetDlgItemInt(hWnd,IDC_EDDELAY,NULL,false);
    
    pVsp->Anim(nCurstrand)=curdata;
}

void CVSPAnimDlg::UpdateDialog(HWND hWnd)
{
    curdata=pVsp->Anim(nCurstrand);
    
    SetDlgItemInt(hWnd,IDC_EDFIRSTTILE,curdata.nStart,false);
    SetDlgItemInt(hWnd,IDC_EDLASTTILE,curdata.nFinish,false);
    SetDlgItemInt(hWnd,IDC_EDDELAY,curdata.nDelay,false);
    
    int radioidx;
    switch (curdata.mode)
    {
    case linear:	radioidx=IDC_ANIMFORWARDS;		break;
    case reverse:	radioidx=IDC_ANIMBACKWARDS;		break;
    case random:	radioidx=IDC_ANIMRANDOM;		break;
    case flip:		radioidx=IDC_ANIMFLIP;		break;
    }
    CheckRadioButton(hWnd,IDC_ANIMFORWARDS,IDC_ANIMFLIP,radioidx);
}

int CVSPAnimDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_ANIMFORWARDS:	curdata.mode=linear;	break;
        case IDC_ANIMBACKWARDS:	curdata.mode=reverse;	break;
        case IDC_ANIMRANDOM:	curdata.mode=random;	break;
        case IDC_ANIMFLIP:	curdata.mode=flip;		break;
            
        case IDC_NEXT:
            UpdateData(hWnd);
            if (nCurstrand<255)											// gah, magic number
                nCurstrand++;
            else
                nCurstrand=0;
            UpdateDialog(hWnd);
            
            SetDlgItemText(hWnd,IDC_CURSTRAND,va("Strand %i",nCurstrand));
            break;
            
        case IDC_PREV:
            UpdateData(hWnd);
            if (nCurstrand)
                nCurstrand--;
            else
                nCurstrand=255;
            UpdateDialog(hWnd);
            
            SetDlgItemText(hWnd,IDC_CURSTRAND,va("Strand %i",nCurstrand));
            break;
            
        case IDOK:
            UpdateData(hWnd);
        case IDCANCEL:
            EndDialog(hWnd,0);
            break;
        }
    }
    return 0;
}

bool CVSPAnimDlg::InitProc(HWND hWnd)
{
    UpdateDialog(hWnd);
    return true;
}

void CVSPAnimDlg::Execute(HINSTANCE hInst,HWND hwnd,VSP* pvsp,int curstrand)
{
    pVsp=pvsp;
    nCurstrand=curstrand;
    
    StartDlg(hInst,hwnd,"VSPANIMDLG");
}

// ------------------------------Config----------------------------------------

int CConfigDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            config->bSavev2vsps= IsDlgButtonChecked(hWnd,IDC_SAVEV2VSPS)?true:false;
        case IDCANCEL:
            EndDialog(hWnd,0);
            break;
        }
    }
    return 0;
}

bool CConfigDlg::InitProc(HWND hWnd)
{
    CheckDlgButton(hWnd,IDC_SAVEV2VSPS,config->bSavev2vsps?BST_CHECKED:BST_UNCHECKED);
    return true;
}

bool CConfigDlg::Execute(HINSTANCE hInst,HWND hWnd,SMapEdConfig& cfg)
{
    config=&cfg;
    
    StartDlg(hInst,hWnd,"CONFIGDLG");
    return true;
}