#include "pix_edit.h"

//#define ALPHA_BLIT

static const int nMaxundos=10;
static const int nSwatchsize=128;

void CEdit::StartEdit(HINSTANCE hInst,HWND hWndParent,CPixelMatrix& src)
{
    hInstance=hInst;
    // Init state variables
    nMaxUndos = 10;
    nCurcolour[0]=RGBA(0,0,0,255);
    nCurcolour[1]=RGBA(0,0,0,255);
    nLumina=255;
    nAlpha =255;
    chroma=RGBA(0,0,0,255);
    lbutton=rbutton=false;
    bAltered=false;
    
    // Initialize objects
    PurgeUndo();
    PurgeRedo();
    
    curimage    =src;

    iCurrent    =new CDIB(src.Width(),src.Height(),4);
    iSwatch     =new CDIB(nSwatchsize,nSwatchsize,4);
    iBackbuffer =new CDIB(410,410,4);                       // gah, magic numbers

    iCurrent->CopyPixelData(curimage.GetPixelData(),curimage.Width(),curimage.Height(),4,0);
    // TODO: make this depend on the size of the client, then everything will scale all superhappy and stuff when the client window is resized
    rBigimage=Rect(20,20,430,430);
    rSmallimage=Rect(500-(curimage.Width()/2), 400-(curimage.Height()/2), 500+(curimage.Width()/2), 400+(curimage.Height()/2));
    rSwatch=Rect(640-128,148-128,640,148);
    
    MakeSwatchImage();

    HBITMAP hBmp=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_BACKGROUND));
    hBrush=CreatePatternBrush(hBmp);
//    DeleteObject(hBmp);

    StartDlg(hInst,hWndParent,"PixEdDlg");

    // Cleanup code
    delete iCurrent;
    delete iSwatch;
    delete iBackbuffer;
}

bool CEdit::InitProc(HWND hwnd)
{
    hWnd=hwnd;
    HMENU hMenu=LoadMenu(hInstance,MAKEINTRESOURCE(IDR_TILEEDMENU));
    SetMenu(hWnd,hMenu);
    DrawMenuBar(hWnd);
    return true;
}

void CEdit::ClipUndoList()
{
    while (UndoList.size()<=nMaxundos)
        UndoList.pop_back();
}

void CEdit::AddToUndo()
{
    if (UndoList.size())
    {	
        CPixelMatrix tmp = UndoList.front();
        if (curimage==tmp)
            return;
    }
    
    UndoList.push_front(curimage);
}

void CEdit::AddToRedo()
{
    if (RedoList.size())
    {
        CPixelMatrix tmp = RedoList.front();
        if (curimage==tmp)
            return;
    }
    
    RedoList.push_front(curimage);
}

void CEdit::PurgeUndo()
{
    UndoList.clear();
}

void CEdit::PurgeRedo()
{
    RedoList.clear();
}

void CEdit::Undo()
{
    if(!UndoList.size())
        return;
    
    AddToRedo();
    curimage = UndoList.front();
    UndoList.pop_front();
}

void CEdit::Redo()
{
    if (!RedoList.size())
        return;
    
    AddToUndo();
    curimage = RedoList.front();
    RedoList.pop_front();
}

void CEdit::DrawSwatch()
{
    StretchBlt(GetDC(hWnd),
        rSwatch.left,
        rSwatch.top,
        rSwatch.right-rSwatch.left,
        rSwatch.bottom-rSwatch.top,
        iSwatch->GetDC(),
        0,
        0,
        iSwatch->Width(),
        iSwatch->Height(),
        SRCCOPY);
    
    char sTemp[255];
    
    sprintf(sTemp,"(%i, %i, %i, %i)  |  (%i, %i, %i, %i)",
        nCurcolour[0].r,nCurcolour[0].g,nCurcolour[0].b,nCurcolour[0].a,
        nCurcolour[1].r,nCurcolour[1].g,nCurcolour[1].b,nCurcolour[1].a);
    SetDlgItemText(hWnd,IDC_CURRENTCOLOUR,sTemp);
}

// Note: This works just dandy, and that'd be great, except that it won't work on win95.  Only 98 and up.  It'll have to be replaced
// with the Hard Way. :(
void CEdit::DrawBlownUpImage()
{
    SelectObject(iBackbuffer->GetDC(),hBrush);

    Rectangle(iBackbuffer->GetDC(),0,0,rBigimage.right-rBigimage.left,rBigimage.bottom-rBigimage.top);
#ifdef ALPHA_BLIT

    const BLENDFUNCTION blendfunc =
    {
        AC_SRC_OVER,
        0,
        255,
        1
    };

    AlphaBlend(iBackbuffer->GetDC(),
        0,0,
        rBigimage.right-rBigimage.left,
        rBigimage.bottom-rBigimage.top,
        iCurrent->GetDC(),
        0,
        0,
        iCurrent->Width(),
        iCurrent->Height(),
        blendfunc);

#else

    StretchBlt(iBackbuffer->GetDC(),
        0,0,
        rBigimage.Width(),
        rBigimage.Height(),
        iCurrent->GetDC(),
        0,0,
        iCurrent->Width(),
        iCurrent->Height(),
        SRCCOPY);

#endif
}

// ugh
void CEdit::Redraw()
{   
    DrawSwatch();
    
    HDC hDC=GetDC(hWnd);

    DrawBlownUpImage();
 
    BitBlt(hDC,
        rBigimage.left,
        rBigimage.top,
        iBackbuffer->Width(),
        iBackbuffer->Height(),
        iBackbuffer->GetDC(),
        0,0,SRCCOPY);
    
    BitBlt(hDC,
        rSmallimage.left,
        rSmallimage.top,
        iCurrent->Width(),
        iCurrent->Height(),
        iCurrent->GetDC(),
        0,0,SRCCOPY);
    
    ReleaseDC(hWnd,hDC);
}

RGBA  CEdit::CalcColour(int nLumina,RGBA chroma)
{
    int r,g,b;
    
    r=chroma.r;
    g=chroma.g;
    b=chroma.b;
    
    r=r*nLumina/128;
    g=g*nLumina/128;
    b=b*nLumina/128;
    
    if (r>255) r=255;
    if (g>255) g=255;
    if (b>255) b=255;
    
    return RGBA(r,g,b,chroma.a);
}

RGBA CEdit::MergeColors(RGBA color1,RGBA color2)
{
    if(color1.a == 255)
        return color1;
    else if(color1.a == 0)
        return color2;
    
    int i, r, g, b, a;
    int alpha1 = color1.a;
    int alpha2 = 255-color1.a;
    
    r = ((color1.r * alpha1)+(color2.r * alpha2)) / 256;
    g = ((color1.g * alpha1)+(color2.g * alpha2)) / 256;
    b = ((color1.b * alpha1)+(color2.b * alpha2)) / 256;
    
    if(alpha2 == 255)
        a = 255;
    else
        a =((alpha2 + alpha1)>255)?(255):(alpha2 + alpha1);
    
    i = (a<<24)+(b<<16)+(g<<8)+r;
    
    return i;
}

void CEdit::UpdateLumina(HWND hBar)
{
    SCROLLINFO si;
    
    si.cbSize=sizeof si;
    si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nPage=1;
    si.nMin=0;
    si.nMax=255;
    si.nPos=nLumina;
    SetScrollInfo(hBar,SB_CTL,&si,true);

    nCurcolour[0]=CalcColour(nLumina,chroma);
    nCurcolour[0].a=nAlpha;
    
    // Also, the numeric thingie right next to it.
    char c[255];
    sprintf(c,"%i",nLumina);
    SetDlgItemText(hWnd,IDC_LUMINAINDICATOR,c);
    
    MakeSwatchImage();
}

void CEdit::UpdateAlpha(HWND hBar)
{
    SCROLLINFO si;
    
    si.cbSize=sizeof si;
    si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nPage=1;
    si.nMin=0;
    si.nMax=255;
    si.nPos=nAlpha;
    SetScrollInfo(hBar,SB_CTL,&si,true);

    nCurcolour[0].a=nAlpha;
    
    // Also, the numeric thingie right next to it.
    char c[255];
    sprintf(c,"%i",nAlpha);
    SetDlgItemText(hWnd,IDC_ALPHAINDICATOR,c);
    
    //nCurcolour[0]=SetAlpha(nAlpha,nCurcolour[0]);
    
    MakeSwatchImage();
}

void CEdit::MakeSwatchImage()
{	
    RGBA* pData=(RGBA*)iSwatch->GetPixelData();
    for (int y=0; y<nSwatchsize; y++)
        for (int x=0; x<nSwatchsize; x++)
        {
            int ty=y*256/nSwatchsize;
            int tx=x*256/nSwatchsize;
            RGBA colour=RGBA(tx,ty,255-tx,nAlpha);
            
            pData[y*nSwatchsize+x] = CalcColour(nLumina,colour);
        }
}

bool CEdit::PointIsInRect(int x,int y,Rect r)
{
    if (x<r.left)   return false;
    if (y<r.top)    return false;
    if (x>=r.right) return false;
    if (y>=r.bottom)return false;
    return true;
}

void CEdit::DoLeftDownOnBigTile(int x,int y,int b,RGBA& curcolour)
{
    x=(x*curimage.Width())/(rBigimage.right-rBigimage.left);
    y=(y*curimage.Height())/(rBigimage.bottom-rBigimage.top);
    
    
    if (b&MK_SHIFT)	// colour grabber
    {
        curcolour=curimage.GetPixel(x,y);
        return;
    }
    
    if (x==nOldx && y==nOldy)
        return;
    
    nOldx=x; nOldy=y;
    
    // Default action -- set a dot
    curimage.SetPixel(x, y, curcolour);
    iCurrent->SetPixel(x, y, curcolour);                                // synch the DIB image
    bAltered=true;
    Redraw();
    
    //	curimage.CopyToImage(iCurrent);
    //	iCurrent.CopyPixelData(curimage.GetPixelData(),curimage.Pal(),curimage.Width(),curimage.Height());*/
}

void CEdit::DoLeftUpOnBigTile(int x,int y,int b)
{
}

void CEdit::DoLeftDownOnSwatch(int x,int y,int,RGBA& curcolour)
{
    int r,g,b;
    b=(x*256)/(rSwatch.right-rSwatch.left);
    r=255-b;
    g=(y*256)/(rSwatch.bottom-rSwatch.top);
    
    chroma=RGBA(r,g,b,255);
    curcolour=CalcColour(nLumina,chroma);
    curcolour.a=nAlpha;
}

void CEdit::DoLeftUpOnSwatch(int x,int y,int b)
{
}

void CEdit::HandleMouse(int x,int y,int b)
{
    static bool bOldleft=false;
    static bool bOldright=false;
    
    if(!(b&MK_LBUTTON))		lbutton = false;
    else					lbutton = true;
    
    if(!(b&MK_RBUTTON))		rbutton = false;
    else					rbutton = true;
    
    
    if (lbutton && !bOldleft)		AddToUndo();							// if the mouse button was *just* pressed now
    if (rbutton && !bOldright)		AddToUndo();
    
    bOldleft=lbutton;
    bOldright=rbutton;
    
    if (PointIsInRect(x,y,rBigimage))
    {
        if (lbutton && !rbutton)
            DoLeftDownOnBigTile(
            x-rBigimage.left,
            y-rBigimage.top,
            b,nCurcolour[0]);
        else
            DoLeftUpOnBigTile(
            x-rBigimage.left,
            y-rBigimage.top,
            b);
        
        if (rbutton && !lbutton)
            DoLeftDownOnBigTile(
            x-rBigimage.left,
            y-rBigimage.top,
            b,nCurcolour[1]);
    }
    
    if (PointIsInRect(x,y,rSwatch))
    {
        if (lbutton)
            DoLeftDownOnSwatch(
            x-rSwatch.left,
            y-rSwatch.top,
            b,nCurcolour[0]);
        else
            DoLeftUpOnSwatch(
            x-rSwatch.left,
            y-rSwatch.top,
            b);
        
        if (rbutton)
            DoLeftDownOnSwatch(
            x-rSwatch.left,
            y-rSwatch.top,
            b,nCurcolour[1]);
    }
    }

int CEdit::MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd,&ps);
            Redraw();
            EndPaint(hWnd,&ps);
        }
        break;
    case WM_LBUTTONUP:
        lbutton=false;
        HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	
        PostMessage(hWnd,WM_PAINT,0,0);
        break;
    case WM_LBUTTONDOWN:
        lbutton=true;
        HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);
        PostMessage(hWnd,WM_PAINT,0,0);
        break;
    case WM_RBUTTONUP:
        rbutton=false;
        HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);
        PostMessage(hWnd,WM_PAINT,0,0);
        break;
    case WM_RBUTTONDOWN:
        rbutton=true;
        HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);
        PostMessage(hWnd,WM_PAINT,0,0);
        break;  
    case WM_MOUSEMOVE:
        HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);
        break;
    case WM_VSCROLL:
        if((HWND)lParam == GetDlgItem(hWnd, IDC_LUMINABAR))
        {
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                nLumina=0;
                break;
            case SB_BOTTOM:
                nLumina=255;
                break;
            case SB_LINEUP:
                if (nLumina)
                    nLumina--;
                break;
            case SB_LINEDOWN:
                if (nLumina<255)
                    nLumina++;
                break;
            case SB_PAGEUP:
                nLumina-=10;
                if (nLumina<0)		nLumina=0;
                break;
            case SB_PAGEDOWN:
                nLumina+=10;
                if (nLumina>255)	nLumina=255;
                break;
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                nLumina=HIWORD(wParam);
                break;
            }
            UpdateLumina((HWND)lParam);
        } 
        else if ((HWND)lParam == GetDlgItem(hWnd, IDC_ALPHABAR))
        {
            switch(LOWORD(wParam))
            {
            case SB_TOP:
                nAlpha=0;
                break;
            case SB_BOTTOM:
                nAlpha=255;
                break;
            case SB_LINEUP:
                if (nAlpha)
                    nAlpha--;
                break;
            case SB_LINEDOWN:
                if (nAlpha<255)
                    nAlpha++;
                break;
            case SB_PAGEUP:
                nAlpha-=10;
                if (nAlpha<0)		nAlpha=0;
                break;
            case SB_PAGEDOWN:
                nAlpha+=10;
                if (nAlpha>255)	nAlpha=255;
                break;
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                nAlpha=HIWORD(wParam);
                break;
            }
            UpdateAlpha((HWND)lParam);
        }
        PostMessage(hWnd,WM_PAINT,0,0);
        break;
        
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_TILEED_NEXT: 
            GoNext();
            break;
            
        case ID_TILEED_PREV: 
            GoPrev();
            break;
            
        case ID_TILEED_UNDO:  Undo(); break;
        case ID_TILEED_REDO:  Redo(); break;
            
        case ID_TILEED_CLEAR:	curimage.Clear(nCurcolour[1]);	break;
        case ID_TILEED_ROTATE:	curimage.Rotate();				break;
        case ID_TILEED_FLIP:	curimage.Flip();				break;
        case ID_TILEED_MIRROR:	curimage.Mirror();				break;
        case ID_TILEED_EXIT: PostMessage(hWnd, WM_CLOSE, 0, 0); break;
        default:
            return HandleCommand(hWnd,wParam);										// unknown message?  Pass it down to the derived class
        }
        
        //				curimage.CopyToImage(iCurrent);
        //		iCurrent.CopyPixelData(curimage.GetPixelData(),curimage.Pal(),curimage.Width(),curimage.Height());
        PostMessage(hWnd,WM_PAINT,0,0);
        break;
        case WM_CLOSE:
            EndDialog(hWnd,0);
            break;
        }
        return 0;
}