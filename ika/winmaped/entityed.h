#ifndef ENTITYED_H
#define ENTITYED_H

#include "dialog.h"
#include "map.h"
#include "winmaped.h"
#include "resource.h"

class CEntityDlg : public CDialog
{
private:
	Map*		pMap;
	SMapEntity	curdata;
	int			nCurent;
	HINSTANCE	hInst;							// needed to make more child dialogs

	void UpdateDialog(HWND hWnd);
	void UpdateData(HWND hWnd);
	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);

public:
	void Execute(HINSTANCE hInst,HWND hWnd,Map* m,int e);
};

#endif