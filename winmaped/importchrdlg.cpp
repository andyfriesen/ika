#include "importchrdlg.h"
#include "resource.h"
#include "fileio.h"
#include "pixel_matrix.h"
#include "importpng.h"

void CImportCHRDlg::Execute(HINSTANCE hinst,HWND hwndparent,CCHRfile* chr)
{
	pCHR=chr;
	StartDlg(hinst,hwndparent,"ImportCHRDlg");
}

bool CImportCHRDlg::InitProc(HWND hWnd)
{
	return false;
}

int CImportCHRDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BROWSE:
			{
				OPENFILENAME ofn;
				char sFilename[256];
				const char sTitle[] = "Import image as CHR";
				const char sFilter[]= "PNG Images\0*.PNG\0\0";

				ZeroMemory(&ofn,sizeof ofn);
				ofn.lStructSize=sizeof ofn;
				ofn.lpstrTitle=sTitle;
				ofn.lpstrFilter=sFilter;
				ofn.hwndOwner=hWnd;
				ofn.lpstrFileTitle=sFilename;
				ofn.nMaxFileTitle=255;

				if (!GetOpenFileName(&ofn))
					return 0;

				SetDlgItemText(hWnd,IDC_FILENAME,sFilename);
				return 0;
			}
		case IDOK:
			{
				int nFramex=GetDlgItemInt(hWnd,IDC_EDFRAMEWIDTH,NULL,false);
				int nFramey=GetDlgItemInt(hWnd,IDC_EDFRAMEHEIGHT,NULL,false);
				int nFrames=GetDlgItemInt(hWnd,IDC_EDNUMFRAMES,NULL,false);
				int nFramesperrow=GetDlgItemInt(hWnd,IDC_EDFRAMESPERROW,NULL,false);

				bool bPad=IsDlgButtonChecked(hWnd,IDC_PAD)!=0;
				bool bAppend=IsDlgButtonChecked(hWnd,IDC_APPEND)!=0;

				char sFilename[255];
				GetDlgItemText(hWnd,IDC_FILENAME,sFilename,255);

				ImportImage(nFrames,nFramex,nFramey,nFramesperrow,bPad,bAppend,sFilename);
			}
			// Note, there's no break here for a reason
		case IDCANCEL:
			EndDialog(hWnd,0);
		}
	}
	return 0;
}

void CImportCHRDlg::ImportImage(int nFrames,int nFramex,int nFramey,int nFramesperrow,bool pad,bool append,const char* fname)
{
	int nCurframe=0;
	const int bpp=4;

	CPixelMatrix bigimage;
        PNG::Load(bigimage,fname);

	if (append)
	{
		nFramex=pCHR->Width();
		nFramey=pCHR->Height();
		nCurframe=pCHR->NumFrames();
	}
	else
		pCHR->New(nFramex,nFramey);		

	int tx=nFramex+(pad?1:0);
	int ty=nFramey+(pad?1:0);
	
	if (nFramesperrow>bigimage.Width()/tx)
		nFramesperrow=bigimage.Width()/tx;

	CPixelMatrix lilimage;
	lilimage.Resize(nFramex,nFramey);
	
	int curx=pad?1:0;
	int cury=pad?1:0;
	
	for (int i=0,xcount=0; i<nFrames; i++,xcount++)
	{
		bigimage.Blit(lilimage, -curx,-cury );
		pCHR->InsertFrame(pCHR->NumFrames(),lilimage);

		curx+=tx;
		if (xcount>=nFramesperrow)
		{
			xcount=0;
			curx=pad?1:0;
			cury+=ty;
			if (cury+nFramey>bigimage.Height())
				break;
		}
	}
/*	for (int y=0; y<nTileheight; y++)
	{
		for (int x=0; x<nFramesperrow; x++)
		{
			bigimage.Blit(lilimage,-(x*tx),-(y*ty) );
			pCHR->InsertFrame(pCHR->NumFrames(),lilimage);
			nCurframe++;
		}
	}*/
}