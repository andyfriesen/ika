#include "mapdlg.h"

bool CMapDlg::InitProc(HWND hWnd)
{
	SetDlgItemText(hWnd,ID_EDMUSIC,pMap->GetMusic().c_str());
	SetDlgItemText(hWnd,ID_EDRSTRING,pMap->GetRString().c_str());
	SetDlgItemText(hWnd,ID_EDVSP,pMap->GetVSPName().c_str());
	SetDlgItemInt(hWnd,ID_EDMAPX,pMap->Width(),false);
	SetDlgItemInt(hWnd,ID_EDMAPY,pMap->Height(),false);
	return true;
}

int CMapDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
        switch(LOWORD(wParam))
		{
		case IDOK:
			{
				char c[255];
				GetDlgItemText(hWnd,ID_EDMUSIC,c,255);    pMap->SetMusic(c);
				GetDlgItemText(hWnd,ID_EDRSTRING,c,255);  pMap->SetRString(c);
				GetDlgItemText(hWnd,ID_EDVSP,c,255);      pMap->SetVSPName(c);
			}
			// no break here on purpose!
		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		case ID_RESIZEMAP:
			int newx,newy;
			newx=GetDlgItemInt(hWnd,ID_EDMAPX,NULL,false);
			newy=GetDlgItemInt(hWnd,ID_EDMAPY,NULL,false);
			if (newx<1 || newy<1)
			{
				MessageBox (NULL, "Your maps must be at least one tile wide/high!" , " ", MB_ICONHAND);
				return false; // invalid map dimensions
			}
			if ((newx<pMap->Width() || newy<pMap->Height()) &&
				MessageBox (NULL,
				"You sure you want to resize the map?  Anything outside of the new dimensions will be GONE." , "Careful!",
				MB_YESNO + MB_DEFBUTTON2)!=IDYES)
                break;
			pMap->Resize(newx,newy);
			break;
		}
        break;
		case WM_CLOSE:
			EndDialog(hWnd,0);
			return true;
	}
	return false;
}

void CMapDlg::Execute(HINSTANCE hInst,HWND hWnd,Map* m)
{
	pMap=m;
	StartDlg(hInst,hWnd,"MapDlg");
}