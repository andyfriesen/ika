#ifndef DIALOG_H
#define DIALOG_H

#include "windows.h"

class CDialog
{
private:
	static int	CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
protected:
	virtual int	MsgProc(HWND hWnd, UINT msg, WPARAM wParam,LPARAM lParam)=0;
	virtual bool InitProc(HWND hWnd)=0;													// this'll be called in WM_INITDIALOG
	void StartDlg(HINSTANCE hInst,HWND hWndParent,const char* sDlgClassName);

public:
};

#endif