#ifndef MISCDLG_H
#define MISCDLG_H

/*
	The CHR list and movescript list dialogs. (since they're small, I stuffed them both in this one file)
	Like all of my dialog classes, these ones are single-instance. (don't ask me why this is better than using
	globals, I'm not sure.  It just *feels* right)

	There's lots of other misc dialogs here too. (hence the filename)
*/

#include "types.h"

#include <windows.h>
#include "map.h"
#include "dialog.h"
#include "resource.h"
#include "winmaped.h"
#include "vsp.h"

// -----------------------------New map dialog---------------------------------

struct SNewMapInfo
{
	bool bLoadvsp;
	string sVspname;
	int tilex,tiley;
	int mapx,mapy;
};

class CNewMapDlg : public CDialog
{
private:
	virtual int MsgProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);
	void UpdateInfo(HWND hWnd);
	bool result;

public:
	bool Execute(HINSTANCE hInst,HWND hWnd);

	SNewMapInfo info;
};

// ----------------------------VSP animation dialog----------------------------

class CVSPAnimDlg : public CDialog
{
private:
	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);

	VSP*            pVsp;
        VSP::AnimState  curdata;
	int             nCurstrand;

	void UpdateDialog(HWND hWnd);
	void UpdateData(HWND hWnd);
public:
	void Execute(HINSTANCE hInst,HWND hwnd,VSP* pvsp,int curstrand=0);
};

// ---------------------------------Settings-----------------------------------

struct SMapEdConfig // not to be confused with user/game.cfg
// TODO: move this, and make a config file reader/writer
{
	bool	bSavev2vsps;

	SMapEdConfig()
	{
		bSavev2vsps=false;
	}
};

class CConfigDlg : public CDialog
{
private:	
	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);

	SMapEdConfig* config;

public:
	bool Execute(HINSTANCE hInst,HWND hWnd,SMapEdConfig& cfg);
};

#endif