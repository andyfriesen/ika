#ifndef LAYERDLG_H
#define LAYERDLG_H

/*
	More boring dialog classes. >:P

	Gotta love that CDialog class.  I should expand upon it sometime.
*/

#include <windows.h>
#include "map.h"
#include "winmaped.h"
#include "dialog.h"
#include "resource.h"

class CLayerDlg : public CDialog
{
private:
	virtual int	MsgProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam);

	Map* pMap;
	SMapLayerInfo curdata;
	int curlayer;
	void UpdateDialog(HWND hWnd);
	void UpdateData(HWND hWnd);
	bool InitProc(HWND hWnd);
public:
	void Execute(HINSTANCE hInst,HWND hWnd,Map* m,int lay);
};

#endif