// blah!
#include <windows.h>
#include <vector>
#include "vsp.h"
#include "map.h"
#include "graph.h"
#include "tileed.h"
#include "resource.h"
#include "winmaped.h"

class CTileSel
{
private:
	
	HWND hWnd;						// Handle to our window
	HWND hWndParent;				// handle to the parent window
	HINSTANCE hInstance;			// App instance handle
	HMENU hMenu;					// handle to our right-click window
	bool active;					// true if we're allowed to do things

	CGraphView* pGraph;				// graphics handler for the window
	VSP* pVsp;						// pointer to the VSP we're working on.
	std::vector<CPixelMatrix>	tileimages;	// images that look a lot like the tileset itself
	
	int nYoffset;					// y position of the window (within the VSP list)
	int nZoomfactor;				// how much is this zoomed?
	bool bPad;						// true if we put one pixel of padding in between each tile
	BOOL MainProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	
	static BOOL CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void UpdateVSP(int tileidx=-1);	// updates the tileimages vector (or a specific tile, if one is specified)

	int  TileUnderCursor(int x,int y);
	void ScrollRel(int newoffset); 
	void Scroll(int newoffset);
	void Render(const RECT& r);

	static void RenderCallback(void* pThis,const RECT& r);

	// blaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaah!
	void SaveVSP();
	bool ExportPNG(const char* fname);
	// void ImportPNG();

public:
	CTileSel();
	~CTileSel();
	void Execute(HINSTANCE hInst,HWND hParentWnd,VSP& v);
	void Close();
};