/* 
  the Speed Bump's Spiffy VSP class object thingie (tm)
  Copyright (c) 2000 the Speed Bump
  Based on some crap that vecna made way back when. (c) 199x

  Ihis is my first real attempt to make anything object-oriented, so it may
  look icky, but I'm kinda proud of it right now.

  This has got to be the oldest code chunk I have that I still
  use. ^_~
  --tSB May 2001
*/

// vsp.h

#ifndef vsp_h
#define vsp_h

#include "types.h"

enum VSPAnimMode				// tile animation modes
{
	linear,
	reverse,
	random,
	flip,
};

typedef struct
{
	unsigned short start;				// strand start
	unsigned short finish;				// strand end
	unsigned short delay;				// tile-switch delay
	unsigned short mode;				// tile animation mode
} vspanim_r;

// gasp, a tile outside of the vsp class
// this is used both as a copy/paste buffer, and as TileEd's workspace.
class TileBuffer
{
	friend class VSP;  
public:
	int width;                    // width of the tile
	int height;                   // height
	int bpp;                      // bytes per pixel (1 or 3)
	TileBuffer();
	~TileBuffer();
	TileBuffer(const TileBuffer& tb);
	
	u32 Pixel(int x,int y);
	void SetDot(int x,int y,u32 colour);
	
	void CopyTo(TileBuffer& dest) const;
	bool CompareBuffers(TileBuffer t);
    
	void Mirror();
	void Flip();
	void Rotate();
	
	//void FloodFill(int x,int y,u32 colour);
	void FillRect(int t, int l, int b, int r, u32 color);
	
	inline u8* GetPixelData() const { return data; }
	inline u8* Pal() const { return bpp==1?(u8*)pal:0; }
	
	TileBuffer& operator = (const TileBuffer &nt)
	{ 
		if(&nt == this)
			return *this;
		
		nt.CopyTo(*this);
		return *this;
	}
	
protected:
	u8* data;
	u8 pal[768];                // palette for the tile section (only applicable in 8bit vsps)
};

class VSP
{
	friend class TileBuffer;

private:
	u8 *data;    // actual pixel data (either 8bit palettized, or 32bit RGBA)
	u8 bpp;      // u8s per pixel (either 1 or 4, currrently)
	int  nMaskcolour;	// 8 bit tiles of this colour index are transparent
	char sDesc[64];
	char sName[64];		// the VSPs filename
	
	vspanim_r vspanim[100];
	
	u8 pal[768]; // palette for the 8bit VSP
	int tilex,tiley; // tile dimensions
	int numtiles;
	
public:
	VSP();
	VSP(const char* fname);
	~VSP();
	
	
	int  Load(const char* fname);
	int  Save(const char* fname);
	int  SaveOld(const char* fname);
	int  ColourDepth();
	void Free();
	void New();
	void New(int xsize,int ysize);
	int  NumTiles();
	
	void InsertTile(int pos);
	void DeleteTile(int pos);
	void AppendTiles(int count=1);
	void CopyTile(TileBuffer& tb,int pos); // Copies the tile into a buffer.
	void PasteTile(const TileBuffer& tb,int pos); // pastes the tile from a buffer
	void TPasteTile(const TileBuffer& tb,int pos); // transparently pastes the tile from a buffer
	void To32bpp();
	
	void SetPixel(int x,int y,int tileidx,int c);
	int  GetPixel(int x,int y,int tileidx);
	
	void GetAnim(vspanim_r& anim,int strand);
	void SetAnim(const vspanim_r& anim,int strand);
	
	inline int TileX() const { return tilex; }
	inline int TileY() const { return tiley; }
	inline int NumTiles() const { return numtiles; }
	inline u8* Pal() const { return bpp==1?(u8*)pal:NULL; }
	
	inline u8* GetPixelData(int tileidx=0) const
	{ 
		if (tileidx<0 || tileidx>=numtiles)
			tileidx=0;
		return data+(tileidx*tilex*tiley*bpp); 
	}

	inline char* Name() const { return (char*)sName; }
};

#endif
