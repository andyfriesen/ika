#ifndef IMPORTCHRDLG_H
#define IMPORTCHRDLG_H

#include "dialog.h"
#include "graph.h"
#include "CHR.h"

class CImportCHRDlg : public CDialog
{
private:
	void ImportImage(int nFrames,int nFramex,int nFramey,int nFramesperrow,bool pad,bool append,const char* fname);

	CCHRfile* pCHR;
	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);
public:
	void Execute(HINSTANCE hinst,HWND hwndparent,CCHRfile* chr);
};

#endif