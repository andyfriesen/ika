/*
	A sprite is a bunch of images.  All these images are the same size.

	Tilesets are treated as sprites, except with some extra information on animating tiles.
*/

#ifndef GRAPHLIB_H
#define GRAPHLIB_H

#include "graph.h"
#include "vsp.h"
#include <vector>

class CSprite
{
protected:
	int		nFrames;
	short int		nFramex,nFramey;

	//handle*			hFrames;
	std::vector<handle>	hFrames;
public:
	CSprite();
	CSprite(const CSprite& s);
	virtual ~CSprite();

	void Free();

	virtual void BlitFrame(int x,int y,int frame);
	virtual void TBlitFrame(int x,int y,int frame);
	virtual bool LoadPNG(const char* fname,int x,int y,int numframes);

	inline int Width() const { return nFramex; }
	inline int Height() const { return nFramey; }
};

class CTileSet : public CSprite
{
	enum
	{
		linear,
		reverse,
		random,
		flip
	};

	// tile animation data
	vspanim_r	vspanim	[100];
	int			nAnimct	[100];
	
	std::vector<int>	nTileidx;					// What tile is displayed, as opposed to which is on the map
	std::vector<bool>	bFlip;						// for flipped mode

	void AnimateStrand(int nStrand);
public:
	void UpdateAnimation(int time);
	bool LoadVSP(const char* fname);
	virtual bool LoadPNG(const char* fname,int x,int y,int numframes);

	virtual void BlitFrame(int x,int y,int frame);
	virtual void TBlitFrame(int x,int y,int frame);
};

#endif