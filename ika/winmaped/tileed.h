#include "pix_edit.h"
#include "vsp.h"

class CTileEd : public CEdit
{
private:
	VSP* pVsp;
	int nCurtile;

	virtual void UpdateData();
	virtual void GoNext();
	virtual void GoPrev();
	virtual void Resize(int newwidth,int newheight);
	virtual int  HandleCommand(HWND hWnd,int wParam);

public:
	void Execute(HINSTANCE hInst,HWND hWnd,VSP* vsp,int tileidx);
};