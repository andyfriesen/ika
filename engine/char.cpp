#include "char.h"
#include "chr.h"
//#include "rle.h"

#include "log.h"

bool CCharacter::LoadCHR(const char* fname)
{
	CCHRfile chr;
	bool bResult=chr.Load(fname);
	if (!bResult)
		return false;

	nFrames=chr.NumFrames();
	nFramex=chr.Width();
	nFramey=chr.Height();
	hotx=chr.HotX();
	hoty=chr.HotY();
	hotw=chr.HotW();
	hoth=chr.HotH();

	sScript.resize(chr.sMovescript.size());
	for (int s=0; s<chr.sMovescript.size(); s++)
		sScript[s]=chr.sMovescript[s];

	hFrames.resize(chr.NumFrames());
	for (int i=0; i<nFrames; i++)
	{
		hFrames[i]=gfxCreateImage(nFramex,nFramey);
		gfxCopyPixelData(hFrames[i],(u32*)chr.GetFrame(i).GetPixelData(),nFramex,nFramey);
	}

	sFilename=fname;

	return true;
}

// -----------------------------------  Charactercontroller methods ------------------------

int CCharacterController::Load(const char* filename,int x,int y,int numframes)
{
	// TODO:
	return -1;
}

int CCharacterController::Load(const char* fname)
{
	CDEBUG("ccharactercontroller::load");
	int i;

	// First, we see if this particular CHR has been loaded already
	for (i=0; i<nCount; i++)
		if (IsValid(i) && !stricmp(stuff[i].sFilename.c_str(),fname))
			return i;

	// Nope, load it.
	i=GetNew();

	if (stuff[i].LoadCHR(fname))
		return i;

	return -1;
}
