/*
win32.cpp

  All of the Win32 stuff is here.  (although it's used alittle in the graphics routines)
  
    There's also some handy lil' functions, that don't really fit in a class, but are uberhandy to have around.
*/

#pragma warning (disable:4786)

#include <windows.h>
#include "main.h"

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CEngine* e=(CEngine*)GetWindowLong(hwnd,GWL_USERDATA);
    if (!e)
	return DefWindowProc(hwnd,message,wParam,lParam);
    
    switch (message)					// handle the messages
    {
	// Do Not Remove - Begin Insert 3
	// Do Not Remove - End   Insert 3
    case WM_ACTIVATEAPP:
    case WM_ACTIVATE:
	if (LOWORD(wParam)==WA_INACTIVE)
	{
	    e->bActive=false;
	    WaitMessage();				// being deactivated?  Then we stop until something happens.
	}
	else
	    e->bActive=true;
	return 0;
	
    case WM_DESTROY:           
	PostQuitMessage(0);				// send a WM_QUIT to the message queue
	break;
	
    default:							// for messages that we don't deal with           
	return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrev,LPSTR lpCmdline,int nCmdShow)
{
    char szClassName[ ] = "IKACLIENT";
    WNDCLASSEX wincl;									// Data structure for the windowclass
    HWND hWnd;
    
    ZeroMemory(&wincl,sizeof wincl);
    
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;				// This function is called by windows
    wincl.style = 0;							// Catch double-clicks
    wincl.cbSize = sizeof(WNDCLASSEX);
    
    wincl.hIcon = 0;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;						// No menu
    wincl.cbClsExtra = 0;						// No extra bytes after the window class
    wincl.cbWndExtra = 0;						// structure or the window instance
    
    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);	// Use light-gray as the background of the window
    
    if(!RegisterClassEx(&wincl)) return 0;				// Register the window class, if fail quit the program
    
    hWnd = CreateWindowEx(						// The class is registered, let's create the program
	0,								// Extended possibilites for variation
	szClassName,							// Classname
	"ika",								// Title Text
	WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX &~WS_THICKFRAME,		// default window
	CW_USEDEFAULT,							// Windows decides the position
	CW_USEDEFAULT,							// where the window ends up on the screen
	544,								// The programs width
	375,								// and height in pixels
	HWND_DESKTOP,							// The window is a child-window to desktop
	NULL,								// No menu
	hThisInstance,							// Program Instance handler
	NULL								// No Window Creation data
	);
    
    CEngine engine;
    
    SetWindowLong(hWnd,GWL_USERDATA,(long)&engine);
    
    ShowWindow(hWnd, nCmdShow);						// Make the window visible on the screen
    if (!hWnd) 
    { 
	MessageBox(hWnd,"Couldn't create main window.\r\nAborting.","Something fucked up",0);
	return 0; 
    }
    
    SetFocus(hWnd);							// In theory, this should send a WM_ACTIVATE message to our window, which should set bActive to true.
    
    ShowWindow(hWnd,nCmdShow);
    
    engine.Startup(hWnd,hThisInstance);
    engine.MainLoop();
    engine.Shutdown();
    
    return 0;
}
