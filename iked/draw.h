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
    
    void Stipple(wxImage& dest,int x,int y,int w,int h,RGBA colour);
    
    // Primatives
    void HLine(wxImage& dest,int x1,int x2,int y,RGBA colour);
    void VLine(wxImage& dest,int x,int y1,int y2,RGBA colour);
    void Rect(wxImage& dest,int x1,int y1,int x2,int y2,RGBA colour);
};

#endif