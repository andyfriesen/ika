/*
	Zone editor class.
	coded by tSB, whenever.

	This sucker is a singleton.  There is but one of these.
*/

#include <windows.h>
#include "map.h"
#include "resource.h"
#include "winmaped.h"
#include "dialog.h"

class CZoneEdDlg : public CDialog
{
private:
	virtual int MsgProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	virtual bool InitProc(HWND hWnd);

	Map* pMap;
	int nCurzone;

	SMapZone curdat;

	void UpdateDialog(HWND hZoneEd);												// copies the stuff in the map to the dialog
	void UpdateData(HWND hZoneEd);												// copies the stuff on the dialog to the map
public:
	BOOL Execute(HINSTANCE hInst,HWND hWnd,int zone,Map* m);				// app instance handle, parent window's handle, the zone we're editing, and the map whose zones we are editing
};