#include "chr.h"
#include "fileio.h"
#include "vergepal.h"
#include "rle.h"

CCHRfile::CCHRfile()
{
	nHotx=nHoty=0;
	nHotw=nHoth=16;
	nWidth=nHeight=16;
	frame.clear();
	AppendFrame();
}

CPixelMatrix& CCHRfile::GetFrame(int nFrame) const
{
	static CPixelMatrix dummy;

	if (nFrame<0 || nFrame>=frame.size())
		return dummy;

	return (CPixelMatrix&)frame[nFrame];
}

void CCHRfile::UpdateFrame(const CPixelMatrix& newdata,int nFrame)
{
	if (nFrame<0 || nFrame>=frame.size())
		return;

//	frame[nFrame].CopyPixelData(newdata.GetPixelData(),newdata.Width(),newdata.Height(),newdata.BPP(),newdata.Pal());
	frame[nFrame]=newdata;
}

void CCHRfile::AppendFrame()
{
	frame.push_back(CPixelMatrix(nWidth,nHeight));
}

void CCHRfile::InsertFrame(int idx)
{
	if (idx>=frame.size() || !frame.size())
	{
		AppendFrame();
		return;
	}

	if (idx<0)
		idx=0;

	CPixelMatrix p(frame[frame.size()-1]);		// copy the last frame

	for (int i=idx; i<frame.size(); i++)
		frame[i+1]=frame[i];

	InsertFrame(frame.size(),p);					// and tack it on the end.
}

void CCHRfile::InsertFrame(int idx,CPixelMatrix& p)
{
	if (idx<0)
		idx=0;
	if (idx>=frame.size())
	{
		AppendFrame();
		frame[frame.size()-1]=p;
		return;
	}

	InsertFrame(idx);
	frame[idx]=p;
}

void CCHRfile::DeleteFrame(int idx)
{
	if (idx<0 || idx>=frame.size())
		return;

	for (int i=idx; i<frame.size(); i++)
		frame[i]=frame[i+1];

	frame.resize(frame.size()-1);
}

void CCHRfile::PackData(u8* data,int& size)
{
}

void CCHRfile::UnpackData(u8* data,int size)
{
}

void CCHRfile::New(int framex,int framey)
{
	nWidth=framex;
	nHeight=framey;
	nHotx=0;		nHoty=0;
	nHotw=framex;	nHoth=framey;
	frame.clear();
}

bool CCHRfile::Load(const char* fname)
// eek @_@;
{
	File f;

	bool bResult=f.OpenRead(fname);
	if (!bResult)
		return false;

	char ver;
	f.Read(ver);

	switch (ver)
	{
	case 2:
		bResult=Loadv2CHR(f);
		f.Close();
		return bResult;
	case 4:
		bResult=Loadv4CHR(f);
		f.Close();
		return bResult;
	case 5:
		break;
	default:
		f.Close();
		return false;
	}

	// Load the new format

	char s[65];
	f.Read(s,64);
	s[64]=0;
	sDescription=s;

	int nScripts;

	f.Read(nScripts);

	int i;
	for (i=0; i<nScripts; i++)
	{
		int nLen;
		f.Read(nLen);

		char* s=new char[nLen+1];
		f.Read(s,nLen);
		s[nLen]=0;
		sMovescript.push_back(s);
		delete[] s;
	}

	// Get the frame data
	int nFrames;
	f.Read(nFrames);
	frame.clear();
	for (i=0; i<nFrames; i++)
	{
		int x,y;
		f.Read(x);
		f.Read(y);
		f.Read(nHotx);
		f.Read(nHoty);
		f.Read(nHotw);
		f.Read(nHoth);
		
		RGBA* pTemp=new RGBA[x*y];
		f.ReadCompressed(pTemp,x*y*sizeof(RGBA));
		
		frame.push_back(CPixelMatrix(pTemp,x,y));

		delete[] pTemp;
	}
	f.Close();
	return true;
}

void CCHRfile::Save(const char* fname)
{
	File f;

	bool bResult=f.OpenWrite(fname);
	if (!bResult)
		return;														// :(

	f.Write((char)5);												// version - u8

	f.Write(sDescription.c_str(),64);								// desc    - 64 u8 string

	int i;

	f.Write(sMovescript.size());									// write the number of scripts

	for (i=0; i<sMovescript.size(); i++)
	{
		f.Write(sMovescript[i].length());							// write the length
		f.Write(sMovescript[i].c_str(),sMovescript[i].length());	// write the actual script
	}

	// Write the frame data
	f.Write((int)frame.size());
	for (i=0; i<frame.size(); i++)
	{
		f.Write(frame[i].Width());
		f.Write(frame[i].Height());
		f.Write(nHotx);												// note that the current data structure does not support variable hotspots.  But the file format does. (potential expansion)
		f.Write(nHoty);
		f.Write(nHotw);
		f.Write(nHoth);

		f.WriteCompressed(
			frame[i].GetPixelData(),
			frame[i].Width()*frame[i].Height()*sizeof(RGBA));
	}

	f.Close();
}

bool CCHRfile::Loadv2CHR(File& f)
{
	nWidth=nHeight=0;
	nHotx=nHoty=0;
	nHotw=nHoth=0;

	f.Read(&nWidth,2);
	f.Read(&nHeight,2);
	f.Read(&nHotx,2);
	f.Read(&nHoty,2);
	f.Read(&nHotw,2);
	f.Read(&nHoth,2);

	u16 nFrames;
	f.Read(nFrames);
	
	int nBufsize;
	f.Read(nBufsize);
	u8* pCompbuf=new u8[nBufsize];
	u8* pUncompbuf=new u8[nFrames*nWidth*nHeight];
	f.Read(pCompbuf,nBufsize);
	ReadCompressedLayer1(pUncompbuf,nFrames*nWidth*nHeight,pCompbuf);
	delete[] pCompbuf;

	frame.resize(nFrames);
//	RGBA* p32bpp=new u32[nWidth*nHeight];
	u8* src=pUncompbuf;
	for (int nCurframe=0; nCurframe<nFrames; nCurframe++)
	{
		frame[nCurframe].CopyPixelData(src,nWidth,nHeight,cVergepal);
		src+=nWidth*nHeight;
	}
//	delete[] p32bpp;

	sMovescript.resize(16);
	
	// Get the idle frames
	char c[10];
	int i;
	f.Read(i);		sMovescript[8+face_left]	=string_k("F")+itoa(i,c,10)+"W10";
	f.Read(i);		sMovescript[8+face_right]	=string_k("F")+itoa(i,c,10)+"W10";
	f.Read(i);		sMovescript[8+face_up]		=string_k("F")+itoa(i,c,10)+"W10";
	f.Read(i);		sMovescript[8+face_down]	=string_k("F")+itoa(i,c,10)+"W10";
	
	for (int b=0; b<4; b++)
	{
		char ptr[255];
		int n;
		f.Read(n);
		
		if (n>99)
			return false;					// blargh
		f.Read(ptr,n);
		ptr[n]=0;					// terminating null
		
		switch (b)
		{
		case 0: sMovescript[face_left]=ptr;		break;
		case 1: sMovescript[face_right]=ptr;	break;
		case 2: sMovescript[face_up]=ptr;		break;
		case 3: sMovescript[face_down]=ptr;		break;
		}
	}
	sMovescript[face_upleft]=	sMovescript[face_left];
	sMovescript[face_downleft]=	sMovescript[face_left];
	sMovescript[face_upright]=	sMovescript[face_right];
	sMovescript[face_downright]=sMovescript[face_right];
	
	return true;
}

bool CCHRfile::Loadv4CHR(File& f)
{
	// VERGE v2 chrs store two bytes for these, but ika has four.  Must nuke the high bits or anality will ensue.
	nWidth=nHeight=0;
	nHotx=nHoty=0;
	nHotw=nHoth=0;

	f.Read(&nWidth,2);
	f.Read(&nHeight,2);
	f.Read(&nHotx,2);
	f.Read(&nHoty,2);
	f.Read(&nHotw,2);
	f.Read(&nHoth,2);

	sMovescript.resize(16);
	
	// Get the idle frames
	char c[10];
	u16 i;
	f.Read(i);		sMovescript[8+face_left]	=string_k("F")+itoa(i,c,10)+"W10";
	f.Read(i);		sMovescript[8+face_right]	=string_k("F")+itoa(i,c,10)+"W10";
	f.Read(i);		sMovescript[8+face_up]		=string_k("F")+itoa(i,c,10)+"W10";
	f.Read(i);		sMovescript[8+face_down]	=string_k("F")+itoa(i,c,10)+"W10";
	
	f.Read(i);
	int nFrames=i;				// frame count
	
	for (int b=0; b<4; b++)
	{
		char ptr[255];
		int n;
		f.Read(n);
		
		if (n>99)
			return false;					// blargh
		f.Read(ptr,n);
		ptr[n]=0;					// terminating null
		
		switch (b)
		{
		case 0: sMovescript[face_left]=ptr;		break;
		case 1: sMovescript[face_right]=ptr;	break;
		case 2: sMovescript[face_up]=ptr;		break;
		case 3: sMovescript[face_down]=ptr;		break;
		}
	}
	sMovescript[face_upleft]=	sMovescript[face_left];
	sMovescript[face_downleft]=	sMovescript[face_left];
	sMovescript[face_upright]=	sMovescript[face_right];
	sMovescript[face_downright]=sMovescript[face_right];
	
	int n;
	f.Read(n);
	char* ptr=new char[n];
	
	f.Read(ptr,n);
	
	u16* pTemp=new u16[nWidth*nHeight*nFrames];
	ReadCompressedLayer2(pTemp,nWidth*nHeight*nFrames,(u16*)ptr);
	delete[] ptr;
	
	frame.resize(nFrames);
	
	// adjust to 32bpp
	RGBA* p=new RGBA[nWidth*nHeight];
	for (int nCurframe=0; nCurframe<nFrames; nCurframe++)
	{
		for (int n=0; n<nWidth*nHeight; n++)
			p[n]=RGBA( pTemp[ nCurframe*nWidth*nHeight + n ] );

		frame[nCurframe].CopyPixelData(p,nWidth,nHeight);
	}

	delete[] pTemp;
	delete[] p;
	
	return true;
}

void CCHRfile::SaveOld(const char* fname)
{
}
