#ifndef MAPDLG_H
#define MAPDLG_H

/*
yay
*/
#include "dialog.h"
#include "map.h"
#include "resource.h"

class CMapDlg : public CDialog
{
private:
	int		MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	bool	InitProc(HWND hWnd);

	Map*	pMap;
public:
	void Execute(HINSTANCE hInst,HWND hWnd,Map* m);
};

#endif