#ifndef WINMAPED_H
#define WINMAPED_H

#define _WIN32_WINNT 0X0400     // I want Mousewheel stuff, dammit!
#include <windows.h>
#include <commctrl.h>

#include "resource.h"

// constants
enum
{
	WMU_SETLTILE=WM_USER,			// yay4me
	WMU_SETRTILE,					// custom windows messages
	WMU_REBUILDVSP,					// Tell the main window to resync the VSP image
};

extern int sgn(int x);
extern char* va(char* format, ...);

struct SConfig
{
	bool bSaveoldvsps;
	// TODO: add more when necessary
};

#endif