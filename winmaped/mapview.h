#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "winmaped.h"
#include "graph.h"
#include "map.h"
#include "vsp.h"
#include "log.h"

/*
	Draws the pretty map
*/

enum
{
	lay_obs=-1000,				// this is arbitrary, but can't be positive, since there can conceivably be any positive number of layers
	lay_zone,
	lay_ent
};

class CMapView
{
	enum	csrMode
	{
		mode_normal,
		mode_copylay,			// Selecting a rectangle from the active layer
		mode_copyall,			// Selecting a rectangle from all visible layers
		mode_paste
	};

private:
	//------------------------Data----------------------
	HWND			hWnd;		// window handle
	HINSTANCE		hInst;		// instance handle
	CGraphView*		pGraph;		// draws the pretty tiles

	class Engine*	pEngine;	// parent window object
	Map*			pMap;		// current map
	VSP*			pVsp;		// current VSP

	MapClip			clipboard;	// copy/pasting buffer

public:
	// The engine class likes to mangle these
	int     xwin,ywin;
	float   nZoom;                          // current zoom level
	int     nCurlayer;                      // the currently active layer.  Mouse clicks will interact on this layer.
	csrMode cursormode;                     // what's the cursor doing now?  copying?  pasting?
	int     nLefttile,nRighttile;           // currently active tiles
	int     nCurzone;                       // currently active zone
	bool    bMouseleft,bMouseright,bMousemid;
								// mouse button flags	

	std::vector<bool> bLayertoggle;
								// each element is true if the specified layer is to be rendered.  False otherwise
	bool bZonetoggle,bEnttoggle,bObstoggle;
								// true if the zone, entities, and obstructions are to be rendered, respectively

	RECT	clientrect;
	Rect	curselection;		// the currently active selection rectangle.

	//---------------------Startup------------------------
	HWND CreateWnd(HWND hWndparent);

public:
	//-----------------------Interface--------------------
	CMapView(HWND hWndparent,HINSTANCE hinst,Engine* pengine,Map* pmap,VSP* pvsp);
	~CMapView();

	void Resize(const RECT& r);

	void GetTileCoords(int& x,int& y,int layer=-1);														// converts mouse coordinates into tile coordinates

	//------------------------UI--------------------------
	void HandleMouse(int x,int y,int b);													// Handles mouse movement, clicks, etc...
	void DoMouseLeftDown	(int x,int y,int b);
	void DoMouseLeftUp		(int x,int y,int b);
	void DoMouseRightDown	(int x,int y,int b);
	void DoMouseRightUp		(int x,int y,int b);
	void Mouse_NextTile();																	// advances to the next tile/zone/whatever
	void Mouse_PrevTile();																	// goes back to the previous tile/zone/whatever
	void SetActiveLayer(int i);																// sets i to the active editing layer
	void FlipLayer(int i);																	// makes i visible if it was hidden, shows it otherwise

	//---------------------Rendering----------------------
	void Redraw();
//	void UpdateVSP(int tileidx=-1);															// re-updates the specified tile, or all of them, if the parameter is omitted
	void RenderLayer(int lay,bool transparent,const RECT& r);
	void DrawObstructions(const RECT& r);
	void DrawZones(const RECT& r);
	void DrawEntities(const RECT& r);
	void DrawSelection(const RECT& r);
	void Render(const RECT& r);
	void ScrollWin(int x,int y);
	
	//-----------------------Callbacks--------------------
	LRESULT MsgProc(UINT msg,WPARAM wParam,LPARAM lParam);
	static void RenderCallback(void* pThis,const RECT& r);
	static LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
};

#endif