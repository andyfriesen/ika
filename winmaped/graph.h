/*
	This is a class that will allow a single window to... have graphics on it. :P

	It renders the window in arbitrary rectangular regions, and.. uh... stuff.

	32bpp ONLY, but since DIBs are device independant, it should display properly on all displays.

        TODO: See if we can generalize CDIB, Canvas, and this, so that everything uses the same blitting code.
*/

#ifndef GRAPH_H
#define GRAPH_H

#include <windows.h>
#include <set>
#include "Canvas.h"
#include "dib.h"
#include "types.h"

class CGraphView
{
private:
	// ----------types----------
	struct point                                                                                // A lil' struct to store what we need to store for a single block
	{
		int x,y;
		point(int initx,int inity) { x=initx; y=inity; }
        bool operator < (const point& p) const
        {
            return this < &p;   // ... heh
        }

        bool operator == (const point& p) const
        {
            return x == p.x && y == p.y;
        }
	};
	typedef void(*renderfunc)(void* pThis,const RECT& r);

	// ---------Data----------
	HWND hWnd;
	CDIB* pDib;
	renderfunc Render;                                                                          // called when we want to render something

	void* pThis;
		
    std::set<point> dirtyrects;                                                                 // a list of blocks that have been altered
	void AddBlock(int x,int y);                                                                 // just in case we want to get fancy and avoid redundantly setting a block later on
	void AlphaBlit(const Canvas& src,int x,int y);                                        // just so that we don't have both opaque and transparent crap being blitted in one long, ugly function :)
	void CGraphView::DoClipping(int& x,int& y,int& xstart,int& xlen,int& ystart,int& ylen);

public:
	CGraphView(HWND hwnd,renderfunc pRenderfunc,void* pthis);
	~CGraphView();

	// drawing functions
	void Blit(const Canvas& src,int x,int y,bool trans);
	// void scaleblit?
	void HLine(int x1,int x2,int y,u32 colour);
	void VLine(int x,int y1,int y2,u32 colour);
	void DrawRect(int x1,int y1,int x2,int y2,u32 colour);
	void Stipple(int x1,int y1,int x2,int y2,u32 colour);

	void Clear();

	// Rendering stuff
	void DirtyRect(int x1,int y1,int x2,int y2);                                                // dirties everything in the specified rect, so that the next ShowPage will redraw that section
	void ShowPage();                                                                            // renders dirty rects
    void ShowPage(const ::Rect& r);                                                             // renders the specified rect
	void ForceShowPage();                                                                       // rerenders the whole window
};

#endif