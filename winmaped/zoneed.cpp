#include "zoneed.h"

int CZoneEdDlg::Execute(HINSTANCE hInst,HWND hWnd,int zone, Map* m)
{
        pMap=m;
        nCurzone=zone;
        // HACK
        pMap->Zones().resize(255);
        StartDlg(hInst,hWnd,"ZONEDLG");
        return 0;
}

void CZoneEdDlg::UpdateDialog(HWND hWnd)
{
        curdat = pMap->Zones()[nCurzone];
        SetDlgItemText(hWnd,IDC_EDNAME,curdat.name.c_str());
        SetDlgItemText(hWnd,IDC_EDDESC,curdat.sDescription.c_str());
        SetDlgItemText(hWnd,IDC_EDSCRIPT,curdat.sActscript.c_str());
        SetDlgItemText(hWnd,IDC_EDENTSCRIPT,curdat.sEntactscript.c_str());

        SetDlgItemInt(hWnd,IDC_EDPERCENT,curdat.nActchance,false);
        SetDlgItemInt(hWnd,IDC_EDDELAY,curdat.nActdelay,false);

        if (curdat.bAdjacentactivation)
                CheckDlgButton(hWnd,IDC_AAA,BST_CHECKED);
        else
                CheckDlgButton(hWnd,IDC_AAA,BST_UNCHECKED);

        SetDlgItemText(hWnd,IDC_CURZONE, va("Zone: %d",nCurzone) );
}

void CZoneEdDlg::UpdateData(HWND hWnd)
{ 
        char c[255];
        GetDlgItemText(hWnd,IDC_EDNAME,c,255);                  curdat.name=c;
        GetDlgItemText(hWnd,IDC_EDDESC,c,255);                  curdat.sDescription=c;
        GetDlgItemText(hWnd,IDC_EDSCRIPT,c,255);                curdat.sActscript=c;
        GetDlgItemText(hWnd,IDC_EDENTSCRIPT,c,255);             curdat.sEntactscript=c;

        curdat.nActchance       =       GetDlgItemInt(hWnd,IDC_EDPERCENT  ,NULL,false);
        curdat.nActdelay        =       GetDlgItemInt(hWnd,IDC_EDDELAY    ,NULL,false);
        
        curdat.bAdjacentactivation=IsDlgButtonChecked(hWnd,IDC_AAA)==BST_CHECKED?1:0;
        
        pMap->Zones()[nCurzone] = curdat;
}

bool CZoneEdDlg::InitProc(HWND hWnd)
{
        UpdateDialog(hWnd);
        return true;
}

int CZoneEdDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
        switch(msg)
        {
        case WM_SHOWWINDOW:
                UpdateDialog(hWnd);
                return false;
        case WM_CLOSE:
                EndDialog(hWnd, IDOK);
                return true;
        case WM_COMMAND:
        switch (LOWORD(wParam))
                {
                case IDOK:
                        UpdateData(hWnd);
                        EndDialog(hWnd,IDOK);
                        break;
                case IDCANCEL:
                        EndDialog(hWnd,IDCANCEL);
                        break;
                case ID_NEXT:
                        UpdateData(hWnd);
                        if (nCurzone<pMap->Zones().size())
                                nCurzone++;
                        else
                                nCurzone=0;


                        curdat = pMap->Zones()[nCurzone];
                        UpdateDialog(hWnd);
                        break;
                case ID_PREV:
                        UpdateData(hWnd);
                        if (nCurzone>0 && pMap->Zones().size())
                                nCurzone--;
                        else
                                if (pMap->Zones().size()>0)
                                        nCurzone=pMap->Zones().size()-1;
                                else
                                        nCurzone=0;

                        curdat = pMap->Zones()[nCurzone];
                        UpdateDialog(hWnd);
                        break;
                case ID_AAA:
                        curdat.bAdjacentactivation^=1;
                        break;
                case ID_APPLY:
                        UpdateData(hWnd);
                        break;
                }
                break;
        }
        return false;
}
