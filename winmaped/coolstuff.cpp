
#include "coolstuff.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////////////
// Obstruction Thingie
/////////////////////////////////////////////////////////////////////////////////////

int CObstructionThingieDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch LOWORD(wParam)
		{
		case IDOK:
			bSetobs=IsDlgButtonChecked(hWnd,IDC_TOT_OBSTRUCT)?true:false;
			bFromorexcept=IsDlgButtonChecked(hWnd,IDC_TOT_FROM)?true:false;
			nStarttile	=GetDlgItemInt(hWnd,IDC_TOT_STARTVAL,NULL,false);
			nEndtile	=GetDlgItemInt(hWnd,IDC_TOT_ENDVAL,NULL,false);
			// Get information and poop from dialog controls
			MangleObstructions(*pMap,bSetobs,bFromorexcept,nStarttile,nEndtile,nLayer);
			EndDialog(hWnd,IDOK);
			return false;

		case IDC_TOT_LAY1:		nLayer=0;	break;
		case IDC_TOT_LAY2:		nLayer=1;	break;
		case IDC_TOT_LAY3:		nLayer=2;	break;
		case IDC_TOT_LAY4:		nLayer=3;	break;
		case IDC_TOT_LAY5:		nLayer=4;	break;
		case IDC_TOT_LAY6:		nLayer=5;	break;
		case IDC_TOT_ALLLAY:	nLayer=nAllLayers;	MessageBox(hWnd,"This can seriously mess up your map!\r\nBe careful!","Watch it, dude",0); break;

		case IDCANCEL:
			EndDialog(hWnd,IDCANCEL);
			return false;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd,IDOK);
		break;
	}
	return false;
}

void CObstructionThingieDlg::MangleObstructions(Map& map,bool bSetobs,bool bFromorexcept,int nStarttile,int nEndtile,int nLayer)
{
	// If we're mangling based on all layers
	if (nLayer==nAllLayers)
	{
		for (int i=0; i<map.NumLayers(); i++)
			MangleObstructions(map,bSetobs,bFromorexcept,nStarttile,nEndtile,i);		// I'm such a loser
		return;
	}
	if (nLayer>=map.NumLayers())
		return;																			// POOOOOOOOOOOOOOOOOOOOOOOO

	for (int y=0; y<map.Height(); y++)
		for (int x=0; x<map.Width(); x++)
		{
			int t=map.GetTile(x,y,nLayer);
			if (t>=nStarttile && t<=nEndtile)
				map.SetObs(x,y,bSetobs);
		}
}

void CObstructionThingieDlg::Execute(HINSTANCE hInst,HWND hWndParent,Map* pSrcMap)
{
	pMap=pSrcMap;																		// hold on to this
	StartDlg(hInst,hWndParent,"ObstructionThingieDlg");
}

///////////////////////////////////////////////////////////////////////////////////////
