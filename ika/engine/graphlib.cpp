#include "graphlib.h"
#include "log.h"
#include "importpng.h"

CSprite::CSprite()
{
	nFrames=0;
//	hFrames=NULL;
}

CSprite::CSprite(const CSprite& s)
{
	nFrames=s.nFrames;
	nFramex=s.nFramex;
	nFramey=s.nFramey;

	hFrames.resize(s.nFrames);

	handle hOldrenderdest=gfxGetRenderDest();

	for (int i=0; i<nFrames; i++)
	{
		hFrames[i]=gfxCreateImage(nFramex,nFramey);
		gfxSetRenderDest(hFrames[i]);
		gfxBlitImage(s.hFrames[i],0,0,false);
	}

	gfxSetRenderDest(hOldrenderdest);
}

CSprite::~CSprite()
{
	Free();
//	if (hFrames)
//		delete[] hFrames;
}

void CSprite::Free()
{
	for (int i=0; i<nFrames; i++)
	{
		gfxFreeImage(hFrames[i]);
		hFrames[i]=0;
	}
	nFrames=0;
}

void CSprite::BlitFrame(int x,int y,int frame)
{
	if (frame<0 || frame>=nFrames) return;
	
	gfxBlitImage(hFrames[frame],x,y,false);
}

void CSprite::TBlitFrame(int x,int y,int frame)
{
	if (frame<0 || frame>=nFrames) return;
	
	gfxBlitImage(hFrames[frame],x,y,true);
}

bool CSprite::LoadPNG(const char* fname,int x,int y,int numframes)
{
	// TODO: write this... for something or other, I'm sure.
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////

void CTileSet::UpdateAnimation(int time)
{
	static int lasttime=0;					// what time it was the last time this function was called

	int i=time-lasttime;					// how many ticks have elapsed?
	lasttime=time;
	if (i<1) return;						// not very much, wait a little longer

	while (i--)
	{
		for (int j=0; j<100; j++)
			if (vspanim[j].start!=vspanim[j].finish)	// save some processor time
				AnimateStrand(j);
	}
}

extern int Random(int min,int max);			// in win32.cpp

void CTileSet::AnimateStrand(int nStrand)
{
	int i;

	nAnimct[nStrand]--;

	if (nAnimct[nStrand]>0)	return;

	switch(vspanim[nStrand].mode)
	{
	case linear:
		for (i=vspanim[nStrand].start; i<vspanim[nStrand].finish; i++)
		{
			nTileidx[i]++;
			if (nTileidx[i]>vspanim[nStrand].finish)
				nTileidx[i]=vspanim[nStrand].start;
		}
		break;
	case reverse:
		for (i=vspanim[nStrand].start; i<vspanim[nStrand].finish; i++)
		{
			nTileidx[i]--;
			if (nTileidx[i]<vspanim[nStrand].start)
				nTileidx[i]=vspanim[nStrand].finish;
		}
		break;
	case random:
		for (i=vspanim[nStrand].start; i<vspanim[nStrand].finish; i++)
			nTileidx[i]=Random(vspanim[nStrand].start,vspanim[nStrand].finish);
		break;
	case flip:
		for (i=vspanim[nStrand].start; i<vspanim[nStrand].finish; i++)
		{
			if (bFlip[i])
			{
				nTileidx[i]++;
				if (nTileidx[i]>vspanim[nStrand].finish)
				{
					nTileidx[i]=vspanim[nStrand].start;
					bFlip[i]=!bFlip[i];
				}
			}
			else
			{
				nTileidx[i]--;
				if (nTileidx[i]<vspanim[nStrand].start)
				{
					nTileidx[i]=vspanim[nStrand].finish;
					bFlip[i]=!bFlip[i];
				}
			}
		}
		break;
	}

	nAnimct[nStrand]=vspanim[nStrand].delay;
}

void CTileSet::BlitFrame(int x,int y,int frame)
{
#ifdef _DEBUG
	if (frame<0 || frame>nFrames) return;
#endif
	frame=nTileidx[frame];
	
	gfxBlitImage(hFrames[frame],x,y,false);
}

void CTileSet::TBlitFrame(int x,int y,int frame)
{
	frame=nTileidx[frame];
#ifdef _DEBUG
	if (frame<0 || frame>nFrames) return;
#endif
	
	gfxBlitImage(hFrames[frame],x,y,true);
}

bool CTileSet::LoadVSP(const char* fname)
{
	CDEBUG("ctileset::loadvsp");
	VSP vsp;

	if (!vsp.Load(fname))
		return false;

	Free();
	nFrames=vsp.NumTiles();
	nFramex=vsp.TileX();
	nFramey=vsp.TileY();

	try
	{
		u32* data32;

		if (vsp.ColourDepth()==1)
		{
			u8* data8;
			u8* pal;
			int r,g,b,a;
			int c;

			pal=vsp.Pal();
			data8=vsp.GetPixelData();
			data32=new u32[nFramex*nFramey*nFrames];
			for (int y=0; y<nFramey*nFrames; y++)
				for (int x=0; x<nFramex; x++)
				{
					c=data8[y*nFramex+x];
					r=pal[c*3  ]<<2;
					g=pal[c*3+1]<<2;
					b=pal[c*3+2]<<2;
					a=c?255:0;	// colour index 0 is transparent, all others are opaque

					c=(a<<24) | (b<<16) | (g<<8) | r;
					data32[y*nFramex+x]=c;
				}
		}
		else
			data32=(u32*)vsp.GetPixelData();

//		if (hFrames)
//			delete[] hFrames;
//		hFrames=new handle[nFrames];
		hFrames.resize(nFrames);
		for (int i=0; i<nFrames; i++)
		{
			hFrames[i]=gfxCreateImage(nFramex,nFramey);
			gfxCopyPixelData(hFrames[i],data32+(nFramex*nFramey*i),nFramex,nFramey);
		}

		if (vsp.ColourDepth()==1)
			delete[] data32;
	}
	catch(...)
	{	
		vsp.Free();
		return false;
	}

	// Next up, set up the tile animation stuff

	nTileidx.resize(nFrames);			// Make the vectors fit
	bFlip.resize(nFrames);
	for (int i=0; i<nFrames; i++)
	{
		nTileidx[i]=i;											// set initial values for the vectors
		bFlip[i]=false;
	}

	for (i=0; i<100; i++)
	{
		vsp.GetAnim(vspanim[i],i);								// copy the animation data
		nAnimct[i]=vspanim[i].delay;							// init the counter
	}
	
	vsp.Free();
	return true;
}

bool CTileSet::LoadPNG(const char* fname,int x,int y,int numframes)
{
	png_image*	png;				// pointer to the source PNG
	u32*		temptile;			// temp holder for a single tile's pixel data (copied from the above PNG)
	int			nCurframe=0;		// the frame we're working on right now
	int			nRows,nCols;		// number of rows/columns (or parts thereof) that we need to process

	png=Import_PNG(fname);
	if (!png) return false;

	nFrames=numframes;	nFramex=x;	nFramey=y;
	temptile=new u32[x*y];
	nCols=png->width/(x+1);
	nRows=numframes/nCols+1;

//	if (hFrames)
//		delete[] hFrames;
//	hFrames=new handle[nFrames];
	hFrames.resize(nFrames);

	// Now, we have the image loaded up, we have to re-arrange it into individual tiles.
	// Assume that the tiles are padded, arranged such that there is as many tiles as possible on each line.
	for (int currow=0; currow<nRows; currow++)
		for (int curcol=0; curcol<nCols; curcol++)
		{
			if (nCurframe>=numframes)		break;
			int tx,ty;
			tx=curcol*(x+1)+1;	// pixel coords of the tile we're grabbing right now
			ty=currow*(y+1)+1;

			for (int cury=0; cury<y; cury++)
				memcpy(temptile+(cury*x),png->pixels+((ty+cury)*png->width)+tx,x*sizeof(u32));	// copy the tile into the temptile buffer*/

			hFrames[nCurframe]=gfxCreateImage(1,1);
			gfxCopyPixelData(hFrames[nCurframe],temptile,x,y);
			nCurframe++;
		}

	nTileidx.resize(nFrames);									// Make the vectors fit
	bFlip.resize(nFrames);
	for (int i=0; i<nFrames; i++)
	{
		nTileidx[i]=i;											// set initial values for the vectors
		bFlip[i]=false;
	}

	delete[] temptile;
	delete png;
	return true;
}