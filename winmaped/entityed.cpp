#include "entityed.h"
#include "miscdlg.h"

void CEntityDlg::Execute(HINSTANCE hinst,HWND hWnd,Map* m,int e)
{
    pMap=m;
    nCurent=e;
    hInst=hinst;
    StartDlg(hinst,hWnd,"EntDlg");
}

void CEntityDlg::UpdateData(HWND hWnd)
{
    char c[255];
    
    GetDlgItemText(hWnd,ID_EDNAME,c,255);
    curdata.sName=c;
    
    curdata.x				= GetDlgItemInt(hWnd,ID_EDX,NULL,false);
    curdata.y				= GetDlgItemInt(hWnd,ID_EDY,NULL,false);
    GetDlgItemText(hWnd,ID_EDCHR,c,255);			curdata.sCHRname		= c;
    curdata.nSpeed			= GetDlgItemInt(hWnd,ID_EDSPEED,NULL,false);
    GetDlgItemText(hWnd,ID_EDMSCRIPT,c,255);		curdata.sMovescript		= c;
    GetDlgItemText(hWnd,ID_EDSCRIPT,c,255);			curdata.sActscript		= c;
    curdata.nWandersteps	= GetDlgItemInt(hWnd,ID_EDSTEPS,NULL,false);
    curdata.nWanderdelay	= GetDlgItemInt(hWnd,ID_EDDELAY,NULL,false);
    curdata.nWanderrect[0]	= GetDlgItemInt(hWnd,ID_EDX1,NULL,false);
    curdata.nWanderrect[1]	= GetDlgItemInt(hWnd,ID_EDY1,NULL,false);
    curdata.nWanderrect[2]	= GetDlgItemInt(hWnd,ID_EDX2,NULL,false);
    curdata.nWanderrect[3]	= GetDlgItemInt(hWnd,ID_EDY2,NULL,false);
    GetDlgItemText(hWnd,IDC_EDMZONE,c,255);			curdata.sZone			= c;
    GetDlgItemText(hWnd,IDC_EDCHASETARGET,c,255);	curdata.sChasetarget	= c;
    curdata.nChasedist		= GetDlgItemInt(hWnd,IDC_EDCHASEDIST,NULL,false);
    
    curdata.bAdjacentactivation = IsDlgButtonChecked(hWnd,IDC_AUTOACTIVATE)?true:false;
    
    curdata.bMapobs=IsDlgButtonChecked(hWnd,ID_ENTOBSMAP)?true:false;
    curdata.bEntobs=IsDlgButtonChecked(hWnd,IDC_ENTOBSENT)?true:false;
    curdata.bIsobs=IsDlgButtonChecked(hWnd,ID_ENTISOBS)?true:false;
}

void CEntityDlg::UpdateDialog(HWND hWnd)
{
    SetDlgItemText(hWnd,ID_EDNAME,curdata.sName.c_str());
    
    SetDlgItemInt(hWnd,ID_EDX,curdata.x,false);
    SetDlgItemInt(hWnd,ID_EDY,curdata.y,false);
    SetDlgItemText(hWnd,ID_EDCHR,curdata.sCHRname.c_str());
    SetDlgItemInt(hWnd,ID_EDSPEED,curdata.nSpeed,false);
    SetDlgItemText(hWnd,ID_EDSCRIPT ,curdata.sActscript.c_str());
    SetDlgItemText(hWnd,ID_EDMSCRIPT,curdata.sMovescript.c_str());
    SetDlgItemInt(hWnd,ID_EDSTEPS,curdata.nWandersteps,false);
    SetDlgItemInt(hWnd,ID_EDDELAY,curdata.nWanderdelay,false);
    SetDlgItemInt(hWnd,ID_EDX1,curdata.nWanderrect[0],false);
    SetDlgItemInt(hWnd,ID_EDY1,curdata.nWanderrect[1],false);
    SetDlgItemInt(hWnd,ID_EDX2,curdata.nWanderrect[2],false);
    SetDlgItemInt(hWnd,ID_EDY2,curdata.nWanderrect[3],false);
    
    CheckDlgButton(hWnd,ID_ENTOBSMAP,curdata.bMapobs?BST_CHECKED:0);
    CheckDlgButton(hWnd,IDC_ENTOBSENT,curdata.bEntobs?BST_CHECKED:0);
    CheckDlgButton(hWnd,ID_ENTISOBS,curdata.bIsobs?BST_CHECKED:0);
    
    SetDlgItemText(hWnd,ID_CURZONE,va("Entity %i",nCurent));
    
    if (!curdata.bAdjacentactivation)
        CheckDlgButton(hWnd,IDC_MANUALACTIVATE,BST_CHECKED);
    else
        CheckDlgButton(hWnd,IDC_AUTOACTIVATE,BST_CHECKED);
    
    //	SetDlgItemText(hWnd,ID_LCHR,pMap->GetCHREntry(curdata.chrindex));
    
    //	CheckDlgButton(hWnd,ID_MSTOPPED+curdata.state,BST_CHECKED);
    
    switch(curdata.state)
    {
    case mc_script:
    case mc_nothing: 
        if (curdata.state==mc_script)
            CheckDlgButton(hWnd,ID_MSCRIPTED,BST_CHECKED);
        else
            CheckDlgButton(hWnd,ID_MSTOPPED,BST_CHECKED);
        
        SendDlgItemMessage(hWnd,ID_EDX1,WM_SHOWWINDOW,SW_HIDE,SW_HIDE);
        SetDlgItemText(hWnd,ID_LSTEPS,"");
        SetDlgItemText(hWnd,ID_LDELAY,"");
        SetDlgItemText(hWnd,ID_LX1,"");
        SetDlgItemText(hWnd,ID_LY1,"");
        SetDlgItemText(hWnd,ID_LX2,"");
        SetDlgItemText(hWnd,ID_LY2,"");
        break;
    case mc_wander:
        CheckDlgButton(hWnd,ID_MWANDER,BST_CHECKED);
        SetDlgItemText(hWnd,ID_LSTEPS,"Steps");
        SetDlgItemText(hWnd,ID_LDELAY,"Delay");
        SetDlgItemText(hWnd,ID_LX1,"");
        SetDlgItemText(hWnd,ID_LY1,"");
        SetDlgItemText(hWnd,ID_LX2,"");
        SetDlgItemText(hWnd,ID_LY2,"");
        break;
    case mc_wanderzone:
        CheckDlgButton(hWnd,ID_MWANDERZONE,BST_CHECKED);
        SetDlgItemText(hWnd,ID_LSTEPS,"Steps");
        SetDlgItemText(hWnd,ID_LDELAY,"Delay");
        SetDlgItemText(hWnd,ID_LX1,"Zone");
        SetDlgItemText(hWnd,ID_LY1,"");
        SetDlgItemText(hWnd,ID_LX2,"");
        SetDlgItemText(hWnd,ID_LY2,"");
        break;
    case mc_wanderrect:
        CheckDlgButton(hWnd,ID_MWANDERRECT,BST_CHECKED);
        SetDlgItemText(hWnd,ID_LSTEPS,"Steps");
        SetDlgItemText(hWnd,ID_LDELAY,"Delay");
        SetDlgItemText(hWnd,ID_LX1,"x1");
        SetDlgItemText(hWnd,ID_LY1,"y1");
        SetDlgItemText(hWnd,ID_LX2,"x2");
        SetDlgItemText(hWnd,ID_LY2,"y2");
        break;
    default:
        SetDlgItemText(hWnd,ID_LSTEPS,"WHAT THE BLOODY FUCK?!");
    }
}

bool CEntityDlg::InitProc(HWND hWnd)
{
    pMap->GetEntInfo(curdata,nCurent);
    UpdateDialog(hWnd);
    return true;
}

BOOL CEntityDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_MSTOPPED:
            curdata.state=mc_nothing;
            UpdateDialog(hWnd);
            return true;
        case ID_MWANDER:
            curdata.state=mc_wander;
            UpdateDialog(hWnd);
            return true;
        case ID_MWANDERZONE:
            curdata.state=mc_wanderzone;
            UpdateDialog(hWnd);
            return true;
        case ID_MWANDERRECT:
            curdata.state=mc_wanderrect;
            UpdateDialog(hWnd);
            return true;
        case ID_MSCRIPTED:
            curdata.state=mc_script;
            UpdateDialog(hWnd);
            return true;
        case ID_DELETE:
            if (MessageBox(NULL,"You are about to completely destroy this entity.\r\nAre you sure you want to proceed?","You sure?",MB_YESNO + MB_DEFBUTTON2 + MB_ICONEXCLAMATION)
                !=IDYES)
                return true;
            pMap->DestroyEntity(nCurent);
            EndDialog(hWnd,IDOK);
            return true;
        case ID_APPLY:
            UpdateData(hWnd);
            pMap->SetEntInfo(curdata,nCurent);
            return true;
        case IDOK:
            UpdateData(hWnd);
            pMap->SetEntInfo(curdata,nCurent);
        case IDCANCEL:
            EndDialog(hWnd,IDOK);
            return true;
        }
        return true;
        case WM_CLOSE:
            EndDialog(hWnd, IDOK);
            return true;
    }
    return false;
}