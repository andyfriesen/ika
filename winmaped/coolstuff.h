
#ifndef COOLSTUFF_H
#define COOLSTUFF_H

#include "dialog.h"
#include "map.h"

// wowie, so simple now. :D

class CObstructionThingieDlg : public CDialog
{
	enum	{		nAllLayers=1337	};			// This is a pretty gay way to define a constant, too bad MSVC won't let me do it the right way. ^_~
private:
	// The map that we're operating on.
	Map* pMap;

	// Options
	bool bSetobs;								// if false, then we unset obstructions, instead of setting them.
	bool bFromorexcept;							// if false, we alter all tiles EXCEPT the ones from nStarttile to nEndtile
	int nStarttile,nEndtile;					// first/last tiles in range to obstruct
	int nLayer;									// layer to check tiles, or nAllLayers to check all layers for the specified tiles

	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd) { return true; }
	void	MangleObstructions(Map& map,bool bSetObs,bool bFromorexcept,int nStarttile,int nEndtile,int nLayer);

public:
	void Execute(HINSTANCE hInst,HWND hWndParent,Map* pSrcMap);
};

#endif