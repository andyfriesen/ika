#include "layerdlg.h"

void CLayerDlg::UpdateDialog(HWND hWnd)
{
	SetDlgItemInt(hWnd,ID_EDMULTX,curdata.pmulx,false);
	SetDlgItemInt(hWnd,ID_EDDIVX,curdata.pdivx,false);
	SetDlgItemInt(hWnd,ID_EDMULTY,curdata.pmuly,false);
	SetDlgItemInt(hWnd,ID_EDDIVY,curdata.pdivy,false);
	
	switch(curdata.nTransmode)
	{
	case 0:	CheckRadioButton(hWnd,ID_LUCENT0,ID_LUCENT4,ID_LUCENT0);	break;
	case 1:	CheckRadioButton(hWnd,ID_LUCENT0,ID_LUCENT4,ID_LUCENT1);	break;
	case 3:	CheckRadioButton(hWnd,ID_LUCENT0,ID_LUCENT4,ID_LUCENT3);	break;
	case 4:	CheckRadioButton(hWnd,ID_LUCENT0,ID_LUCENT4,ID_LUCENT4);	break;
	}
	
//	SetDlgItemInt(hWnd,ID_EDHLINE,curdata.hline,false);
	
	SetDlgItemText(hWnd,ID_CURZONE,va("Layer %i",curlayer+1));
}

void CLayerDlg::UpdateData(HWND hWnd)
{
	curdata.pmulx=GetDlgItemInt(hWnd,ID_EDMULTX,NULL,false);
	curdata.pdivx=GetDlgItemInt(hWnd,ID_EDDIVX ,NULL,false);
	curdata.pmuly=GetDlgItemInt(hWnd,ID_EDMULTY,NULL,false);
	curdata.pdivy=GetDlgItemInt(hWnd,ID_EDDIVY ,NULL,false);
	
//	curdata.hline =GetDlgItemInt(hWnd,ID_EDHLINE,NULL,false);
}

void CLayerDlg::Execute(HINSTANCE hInst,HWND hWnd,Map* m,int lay)
{
	curlayer=lay>m->NumLayers()?lay:0;
	pMap=m;
	StartDlg(hInst,hWnd,"LayerDlg");
}

bool CLayerDlg::InitProc(HWND hWnd)
{
	pMap->GetLayerInfo(curdata,curlayer);
	UpdateDialog(hWnd);
	return true;
}

int CLayerDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
        switch(LOWORD(wParam))
		{
		case ID_NEXT:
			UpdateData(hWnd);
			pMap->SetLayerInfo(curdata,curlayer);
			if (curlayer<pMap->NumLayers()-1) curlayer++;
			pMap->GetLayerInfo(curdata,curlayer);
			UpdateDialog(hWnd);
			return true;
		case ID_PREV:
			UpdateData(hWnd);
			pMap->SetLayerInfo(curdata,curlayer);
			if (curlayer) curlayer--;
			pMap->GetLayerInfo(curdata,curlayer);
			UpdateDialog(hWnd);
			return true;
	
		case IDOK:
		case IDCANCEL:
			UpdateData(hWnd);
			pMap->SetLayerInfo(curdata,curlayer);
			EndDialog(hWnd,IDOK);
			return true;
		case ID_LUCENT0:	curdata.nTransmode=0;	break;
		case ID_LUCENT1:	curdata.nTransmode=1;	break;
		case ID_LUCENT3:	curdata.nTransmode=3;	break;
		case ID_LUCENT4:	curdata.nTransmode=4;	break;
		}
        return true;

	case WM_CLOSE:
		EndDialog(hWnd,IDOK);
		return true;
	}
	return false;
}