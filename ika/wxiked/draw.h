#ifndef DRAW_H
#define DRAW_H

// generic drawing stuff

#include <wx\image.h>
#include "types.h"
#include "vsp.h"

namespace gfx
{	
	void Blit(wxImage& dest,int x,int y,int w,int h,RGBA* src,bool trans);					// backend for all wxImage blits
	void BlitTile(wxImage& dest,int x,int y,int tileidx,class VSP& src,bool trans=true);	// draws tiles :P
};

#endif