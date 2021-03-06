#include "corona.h"

#include "importvspdlg.h"
#include "resource.h"
#include "fileio.h"
#include "Canvas.h"
#include "log.h"

void CImportVSPDlg::Execute(HINSTANCE hinst,HWND hwndparent,VSP* vsp)
{
    pVsp=vsp;
    StartDlg(hinst,hwndparent,"IMPORTIMAGEDLG");
}

bool CImportVSPDlg::InitProc(HWND hWnd)
{
    SetDlgItemInt(hWnd,IDC_VSPWIDTH,16,false);
    SetDlgItemInt(hWnd,IDC_VSPHEIGHT,16,false);
    CheckDlgButton(hWnd,IDC_PIXELPADDING,BST_CHECKED);
    CheckDlgButton(hWnd,IDC_APPEND,BST_CHECKED);
    return false;
}

int CImportVSPDlg::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDOK:
            // DO IT!
            char sFilename[256];
            int nTilex,nTiley;
            bool bPad,bAppend;

            GetDlgItemText(hWnd,IDC_FILENAME,sFilename,255);
            nTilex=GetDlgItemInt(hWnd,IDC_VSPWIDTH,NULL,false);
            nTiley=GetDlgItemInt(hWnd,IDC_VSPHEIGHT,NULL,false);
            bPad=IsDlgButtonChecked(hWnd,IDC_PIXELPADDING)==BST_CHECKED;
            bAppend=IsDlgButtonChecked(hWnd,IDC_APPEND)==BST_CHECKED;

            if (!File::Exists(sFilename))
            {
                MessageBox(hWnd,"The file you specified could not be found.","gah",0);
                return 0;
            }

            ImportImage(nTilex,nTiley,bPad,bAppend,sFilename);

            EndDialog(hWnd,IDOK);			
            return 0;
        case IDCANCEL:
            EndDialog(hWnd,IDCANCEL);
            return 0;
        case IDC_BROWSE:
            {
                OPENFILENAME ofn;
                char sFilename[256];
                const char sTitle[] = "Import image as tileset";
                const char sFilter[]= "Images (*.png;*.jpeg;*.jpg;*.pcx;*.bmp;*.tga;*.gif\0*.PNG;*.JPEG;*.JPG;*.PCX;*.BMP;*.TGA;*.GIF\0All Files (*.*)\0*.*\0\0";

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
        }
    }
    return 0;
}

void CImportVSPDlg::ImportImage(int tilex,int tiley,bool pad,bool append,const char* fname)
{
    int nCurtile=0;
    const int bpp=4;

    corona::Image* img = corona::OpenImage(fname,corona::FF_AUTODETECT,corona::PF_R8G8B8A8);
    if (!img)
    {
        MessageBox(0,"Unable to open the image","Import error",0);
        return;
    }

    Canvas bigimage((RGBA*)img->getPixels(),img->getWidth(),img->getHeight());

    delete img;

    int tx=tilex+(pad?1:0);
    int ty=tiley+(pad?1:0);

    int nTilewidth=bigimage.Width()/tx;
    int nTileheight=bigimage.Height()/ty;
    int nTiles=nTilewidth*nTileheight;

    if (append)
    {
        tilex=pVsp->Width();
        tiley=pVsp->Height();
        nCurtile=pVsp->NumTiles();
        pVsp->AppendTiles(nTiles);
    }
    else
    {
        pVsp->New(tilex,tiley,nTiles);
    }


    Canvas lilimage;
    lilimage.Resize(tilex,tiley);

    for (int y=0; y<nTileheight; y++)
    {
        for (int x=0; x<nTilewidth; x++)
        {
            // nasty hack to make it work like I want it to.
            int Tx=x*tx;
            int Ty=y*ty;
            if (pad)
                Tx++, Ty++;

            CBlitter<Opaque>::Blit(bigimage, lilimage, -Tx,-Ty);
            pVsp->GetTile(nCurtile)=lilimage;
            nCurtile++;
        }
    }
}