#include "pix_edit.h"

static const int nMaxundos=10;
static const int nSwatchsize=128;

static inline int Alpha(u32 c) { return (c>>24)&255; }
static inline int Red(u32 c) { return (c>>16)&255; }
static inline int Green(u32 c) { return (c>>8)&255; }
static inline int Blue(u32 c) { return c&255; }

void CEdit::StartEdit(HINSTANCE hInst,HWND hWndParent,CPixelMatrix& src)
{
	hInstance=hInst;
	// Init state variables
	nMaxUndos = 10;
	nCurcolour[0]=src.BPP()==1?1:SetAlpha(255, 0);
	nCurcolour[1]=src.BPP()==1?0:SetAlpha(255, 0);
	nLumina=255;
	nAlpha =255;
	nChroma=0;
	lbutton=rbutton=false;
	bAltered=false;

	// Initialize objects
	PurgeUndo();
	PurgeRedo();

	curimage	=src;

	iCurrent	=new CDIB(src.Width(),src.Height(),src.BPP(),src.Pal());
	iSwatch		=new CDIB(nSwatchsize,nSwatchsize,src.BPP(),src.Pal());
	MakeSwatchImage();

	iCurrent->CopyPixelData(curimage.GetPixelData(),curimage.Width(),curimage.Height(),curimage.BPP(),curimage.Pal());
	// TODO: make this depend on the size of the client, then everything will scale all superhappy and stuff when the client window is resized
	rBigimage=MakeRect(20,20,430,430);
	rSmallimage=MakeRect(500-(curimage.Width()/2), 400-(curimage.Height()/2), 500+(curimage.Width()/2), 400+(curimage.Height()/2));
	rSwatch=MakeRect(640-128,148-128,640,148);
	
	StartDlg(hInst,hWndParent,"PixEdDlg");
	
	// Cleanup code
	delete iCurrent;
	delete iSwatch;
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

	if (curimage.BPP()==1)
	{
		int tempx=rSwatch.left+((nCurcolour[0]%16)*8)-1;
		int tempy=rSwatch.top +((nCurcolour[0]/16)*8)-1;

		Rectangle(GetDC(hWnd),tempx,tempy,tempx+9,tempy+9);
	}
/*	else
	{
		iBlock[0].Clear(nCurcolour[0]);
		iBlock[1].Clear(nCurcolour[1]);
	}*/
	
	// Note that one way or another, the block has been cleared with the colour we want. :)
//	iBlock[0].DirectBlit(rSwatch.right - 70,rSwatch.bottom+10,30,30,false);
//	iBlock[1].DirectBlit(rSwatch.right - 30,rSwatch.bottom+10,30,30,false);
    
	char sTemp[255];

	if(curimage.BPP()==1)
		sprintf(sTemp, "Index: %i  |  Index: %i", nCurcolour[0],nCurcolour[1]);
	else
		sprintf(sTemp,"(%i, %i, %i, %i)  |  (%i, %i, %i, %i)",
			Red(nCurcolour[0]),Green(nCurcolour[0]),Blue(nCurcolour[0]),Alpha(nCurcolour[0]),
			Red(nCurcolour[1]),Green(nCurcolour[1]),Blue(nCurcolour[1]),Alpha(nCurcolour[1]));
	SetDlgItemText(hWnd,IDC_CURRENTCOLOUR,sTemp);
}

// ugh
void CEdit::Redraw()
{
//	bool bUsealphablit=true;

	DrawSwatch();

	HDC hDC=GetDC(hWnd);

/*	// :(  won't work
	if (bUsealphablit)
	{
		BLENDFUNCTION blendfunc =
		{
			AC_SRC_OVER,
			0,
			255,
			0
		};

		AlphaBlend(hDC,
			rBigimage.left,
			rBigimage.top,
			rBigimage.right-rBigimage.left,
			rBigimage.bottom-rBigimage.top,
			iCurrent->GetDC(),
			0,
			0,
			iCurrent->Width(),
			iCurrent->Height(),
			blendfunc);

	}
	else*/
	StretchBlt(hDC,
		rBigimage.left,
		rBigimage.top,
		rBigimage.right-rBigimage.left,
		rBigimage.bottom-rBigimage.top,
		iCurrent->GetDC(),
		0,
		0,
		iCurrent->Width(),
		iCurrent->Height(),
		SRCCOPY);

	BitBlt(hDC,
		rSmallimage.left,
		rSmallimage.top,
		iCurrent->Width(),
		iCurrent->Height(),
		iCurrent->GetDC(),
		0,0,SRCCOPY);

	ReleaseDC(hWnd,hDC);
}

int  CEdit::CalcColour(int nLumina,int nChroma)
{
	int r,g,b;
	
	r=(nChroma>>16)&255;
	g=(nChroma>>8)&255;
	b=nChroma&255;
	
	r=r*nLumina/128;
	g=g*nLumina/128;
	b=b*nLumina/128;
	
	if (r>255) r=255;
	if (g>255) g=255;
	if (b>255) b=255;
	
	int i=(r<<16)+(g<<8)+b+(nAlpha<<24);
	
	return i;
}

int  CEdit::SetAlpha(int nAlpha, int nColor)
{
	return (nAlpha<<24)+(nColor&0x00FFFFFF);
}

int  CEdit::MergeColors(int nColor1, int nColor2)
{
	if(Alpha(nColor1) == 255)
		return nColor1;
	else if(Alpha(nColor1) == 0)
		return nColor2;
	
	int i, r, g, b, a;
	int alpha1 = Alpha(nColor1);
	int alpha2 = 255-Alpha(nColor1);
	
	r = ((Red  (nColor1) * alpha1)+(Red  (nColor2) * alpha2)) / 256;
	g = ((Green(nColor1) * alpha1)+(Green(nColor2) * alpha2)) / 256;
	b = ((Blue (nColor1) * alpha1)+(Blue (nColor2) * alpha2)) / 256;
	
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
	
	if(curimage.BPP() == 1)
	{
		SetDlgItemText(hWnd,IDC_LUMINAINDICATOR,"");
		return;
	}
	
	si.cbSize=sizeof si;
	si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nPage=1;
	si.nMin=0;
	si.nMax=255;
	si.nPos=nLumina;
	SetScrollInfo(hBar,SB_CTL,&si,true);
	
	// Also, the numeric thingie right next to it.
	char c[255];
	sprintf(c,"%i",nLumina);
	SetDlgItemText(hWnd,IDC_LUMINAINDICATOR,c);
	
	MakeSwatchImage();
}

void CEdit::UpdateAlpha(HWND hBar)
{
	SCROLLINFO si;
	
	if(curimage.BPP() == 1)
	{
		SetDlgItemText(hWnd,IDC_ALPHAINDICATOR,"");
		return;
	}
	
	si.cbSize=sizeof si;
	si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nPage=1;
	si.nMin=0;
	si.nMax=255;
	si.nPos=nAlpha;
	SetScrollInfo(hBar,SB_CTL,&si,true);
	
	// Also, the numeric thingie right next to it.
	char c[255];
	sprintf(c,"%i",nAlpha);
	SetDlgItemText(hWnd,IDC_ALPHAINDICATOR,c);
	
	//nCurcolour[0]=SetAlpha(nAlpha,nCurcolour[0]);
	
	MakeSwatchImage();
}

void CEdit::MakeSwatchImage()
{	
	if (curimage.BPP()==1)
	{		
		u8* pData=(u8*)iSwatch->GetPixelData();

		for (int y=0; y<nSwatchsize; y++)
			for (int x=0; x<nSwatchsize; x++)
			{
				int ty=y*256/nSwatchsize;
				int tx=x*256/nSwatchsize;
				int c=(ty&0xFFF0)+tx/16;	// teehee, I'm evil.
				
				pData[y*nSwatchsize+x]=c;
			}
	}
	else
	{
		u32* pData=(u32*)iSwatch->GetPixelData();
		for (int y=0; y<nSwatchsize; y++)
			for (int x=0; x<nSwatchsize; x++)
			{
				int ty=y*256/nSwatchsize;
				int tx=x*256/nSwatchsize;
				u32 nColour=
					(tx)			|		// blue
					(ty<<8)			|		// green
					((255-tx)<<16)	|		// red
					(nAlpha<<24);			// alpha
				
				pData[y*nSwatchsize+x] = CalcColour(nLumina,nColour);
			}
	}
}

bool CEdit::PointIsInRect(int x,int y,RECT r)
{
	if (x<r.left)	return false;
	if (y<r.top)	return false;
	if (x>=r.right) return false;
	if (y>=r.bottom)return false;
	return true;
}

void CEdit::DoLeftDownOnBigTile(int x,int y,int b,int& nCurcolour)
{
	x=(x*curimage.Width())/(rBigimage.right-rBigimage.left);
	y=(y*curimage.Height())/(rBigimage.bottom-rBigimage.top);
	
	
	if (b&MK_SHIFT)	// colour grabber
	{
		nCurcolour=curimage.GetPixel(x,y);
		if (curimage.BPP()==1)
			nCurcolour&=255;	// just to be paranoid
		return;
	}
	
	if (x==nOldx && y==nOldy)
		return;
	
	nOldx=x; nOldy=y;
	
	// Default action -- set a dot
	curimage.SetPixel(x, y, nCurcolour);
	iCurrent->SetPixel(x,y,nCurcolour);				// synch the DIB image
	bAltered=true;
	Redraw();

//	curimage.CopyToImage(iCurrent);
//	iCurrent.CopyPixelData(curimage.GetPixelData(),curimage.Pal(),curimage.Width(),curimage.Height());*/
}

void CEdit::DoLeftUpOnBigTile(int x,int y,int b)
{
}

void CEdit::DoLeftDownOnSwatch(int x,int y,int b,int& nCurcolour)
{
	if (curimage.BPP()==1)
	{
		x=(x*256)/(rSwatch.right-rSwatch.left);
		y=(y*256)/(rSwatch.bottom-rSwatch.top);
		x>>=4; y>>=4;
		nCurcolour=(y*16)+x;
		nCurcolour&=255;	// just to be safe
		// wee
	}
	else
	{
		int r,b,g;
		b=(x*256)/(rSwatch.right-rSwatch.left);
		r=255-b;
		g=(y*256)/(rSwatch.bottom-rSwatch.top);
		
		nChroma=(255<<24)+(r<<16)+(g<<8)+b;
		nCurcolour=CalcColour(nLumina,nChroma);
		nCurcolour=SetAlpha(nAlpha,nCurcolour);
	}
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
	
	// ... I dislike this
	// immensely
/*	if (PointIsInRect(x,y,MakeRect(rSwatch.right-70, rSwatch.bottom+10, rSwatch.right-40, rSwatch.bottom+40)))
	{
		if(lbutton)
			nCurcolour[0]=SetAlpha(nAlpha,nCurcolour[0]);
	} 
	else if (PointIsInRect(x,y,MakeRect(rSwatch.right-30, rSwatch.bottom+10, rSwatch.right, rSwatch.bottom+40)))
	{
		if(rbutton)
			nCurcolour[1]=SetAlpha(nAlpha,nCurcolour[1]);
	}*/
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