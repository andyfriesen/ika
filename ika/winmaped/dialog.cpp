#include "dialog.h"

void CDialog::StartDlg(HINSTANCE hInst,HWND hWndParent,const char* sDlgClassName)
{
	DialogBoxParam(hInst,sDlgClassName,hWndParent,CDialog::WndProc,(long)this);
}

/*bool CDialog::InitProc(HWND hWnd)
{
	return true;
}*/

int CALLBACK CDialog::WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	CDialog* pThis;

	switch (msg)
	{
	case WM_INITDIALOG:
		SetWindowLong(hWnd,DWL_USER,lParam);
		pThis=(CDialog*)lParam;
		return pThis->InitProc(hWnd);
	}

	pThis=(CDialog*)GetWindowLong(hWnd,DWL_USER);
	if (pThis)
		return pThis->MsgProc(hWnd,msg,wParam,lParam);

	return false;
}