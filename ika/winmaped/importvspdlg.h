#ifndef IMPORTVSPDLG_H
#define IMPORTVSPDLG_H

#include "dialog.h"
#include "graph.h"
#include "vsp.h"

class CImportVSPDlg : public CDialog
{
private:
	void ImportImage(int tilex,int tiley,bool pad,bool append,const char* fname);

	VSP* pVsp;
	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);
public:
	void Execute(HINSTANCE hinst,HWND hwndparent,VSP* vsp);
};

#endif