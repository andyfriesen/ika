
#include "dib.h"

void CDIB::CreateDIB(int width,int height,int bpp,u8* pal)
{
	if (hDC)		DeleteDC(hDC);
	if (hBitmap)	DeleteObject(hBitmap);

	hDC=CreateCompatibleDC(NULL);

	BITMAPINFO bmi;
	ZeroMemory(&bmi,sizeof bmi);
	bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth       = width;
	bmi.bmiHeader.biHeight      = -height;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biBitCount    = bpp*8;											// bytes per pixel --> bits per pixel
	bmi.bmiHeader.biCompression = BI_RGB;

	hBitmap=CreateDIBSection(hDC,&bmi,DIB_RGB_COLORS,(void**)&pPixels,NULL,0);

	SelectObject(hDC,hBitmap);

	nWidth=width;
	nHeight=height;
	this->bpp=bpp;

	if (pal)
		SetPalette(pal);
}

CDIB::CDIB(int width,int height,int bpp,u8* pal)
{
	hDC=0;
	hBitmap=0;
	CreateDIB(width,height,bpp,pal);
}

CDIB::CDIB(const CDIB& s)
{
	hDC=0;
	hBitmap=0;

	CreateDIB(s.nWidth,s.nHeight,4,0);

	memcpy(pPixels,s.pPixels,nWidth*nHeight*bpp);
}

CDIB::CDIB(const CPixelMatrix& s)
{
	hDC=0;
	hBitmap=0;

	CreateDIB(s.Width(),s.Height(),4,0);

	memcpy(pPixels,s.GetPixelData(),nWidth*nHeight*bpp);
}

CDIB::~CDIB()
{
	if (hDC)		DeleteDC(hDC);
	if (hBitmap)	DeleteObject(hBitmap);
}

CDIB::operator = (const CPixelMatrix& s)
{
	CreateDIB(s.Width(),s.Height(),4,0);

	if (bpp==1)
		memcpy(pPixels,s.GetPixelData(),nWidth*nHeight*bpp);
	else
		for (int i=0; i<nWidth*nHeight; i++)
			((u32*)pPixels)[i]=SwapBR(((u32*)s.GetPixelData())[i]);
}

void CDIB::SetPalette(u8* newpal)
{
	memcpy(pPal,newpal,768);

	RGBQUAD pal[256];
	for (int i=0; i<256; i++)
	{
		pal[i].rgbRed	=	newpal[i*3+0]*4;
		pal[i].rgbGreen	=	newpal[i*3+1]*4;
		pal[i].rgbBlue	=	newpal[i*3+2]*4;
		pal[i].rgbReserved=0;
	}

	SetDIBColorTable(hDC,0,256,pal);
}

void CDIB::SetPixel(int x,int y,u32 colour)
{
	if (x<0 || y<0) return;
	if (x>=nWidth || y>=nHeight) return;

	if (bpp==1)
		((u8*)pPixels)[y*nWidth+x]=(u8)colour;
	else
		((u32*)pPixels)[y*nWidth+x]=colour;//SwapBR(colour);
}

void CDIB::CopyPixelData(void* pixels,int width,int height,int bpp,u8* pal)
{
	CreateDIB(width,height,bpp,pal);

	if (bpp==1)
		memcpy(pPixels,pixels,width*height*bpp);
	else
	{
		for (int i=0; i<width*height; i++)
			pPixels[i]=SwapBR(((u32*)pixels)[i]);
	}
}