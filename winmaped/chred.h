
#ifndef CHRED_H
#define CHRED_H

#include "types.h"

#include "chr.h"
#include "pix_edit.h"
#include "dialog.h"

class CCHReditor : public CEdit
{
protected:
	string sLastfilename;

	CCHRfile* pChar;
	int nCurframe;

	virtual void UpdateData();
	virtual void GoNext();
	virtual void GoPrev();

	virtual bool InitProc(HWND hWnd);
	virtual int  HandleCommand(HWND hWnd,int wParam);

	void Open();
	void Save();
	void SaveAs();

public:
	void Execute(HINSTANCE hInst,HWND hWnd,CCHRfile* chr);
};

class CCHRPropDlg : public CDialog
{
	CCHRfile* pChar;

	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);

	void UpdateDlg(HWND hWnd);
	void UpdateData(HWND hWnd);
public:
	void Execute(HINSTANCE hInst,HWND hWnd,CCHRfile* chr);
};

class CCHRImportDlg : public CDialog
{
	CCHRfile* pChar;
};

#endif