/*
win32.cpp

All of the Win32 stuff is here.  (although it's used alittle in the graphics routines)

There's also some handy lil' functions, that don't really fit in a class, but are uberhandy to have around.
*/

#ifdef MSVC
#   pragma warning (disable:4786)
#endif

#include <windows.h>
#include "main.h"
#include "benchmark.h"

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CEngine* e = (CEngine*)GetWindowLong(hwnd,GWL_USERDATA);
    if (!e)
        return DefWindowProc(hwnd,message,wParam,lParam);

    switch (message)                            // handle the messages
    {
    case WM_ACTIVATE:
        if (LOWORD(wParam)==WA_INACTIVE)
        {
            e->bActive = false;
            Log::Write("Deactivate!");
        }
        else
        {
            e->bActive = true;
            Log::Write("Activate!");
        }
        return 0;

    case WM_DESTROY:           
        PostQuitMessage(0);                     // send a WM_QUIT to the message queue
        return 0;

    case WM_LBUTTONDOWN:    e->input.mouseb |= 1;   return 0;
    case WM_LBUTTONUP:      e->input.mouseb &= ~1;  return 0;
    case WM_RBUTTONDOWN:    e->input.mouseb |= 2;   return 0;
    case WM_RBUTTONUP:      e->input.mouseb &= ~2;  return 0;

    default:                            // for messages that we don't deal with           
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hThisInstance,HINSTANCE hPrev,LPSTR lpCmdline,int nCmdShow)
{
    char szClassName[ ] = "IKACLIENT";
    WNDCLASSEX wincl;
    HWND hWnd;

    ZeroMemory(&wincl,sizeof wincl);

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = 0;
    wincl.cbSize = sizeof(WNDCLASSEX);

    wincl.hIcon = 0;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                      // No menu
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;

    wincl.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

    if(!RegisterClassEx(&wincl)) return 0;

    hWnd = CreateWindowEx(
        0,
        szClassName,
        "ika",                                                      // caption
        WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX &~WS_THICKFRAME,       // window style
        CW_USEDEFAULT, CW_USEDEFAULT,                               // position
        544, 375,                                                   // size
        HWND_DESKTOP,
        NULL,                                                       // No menu
        hThisInstance,                                              // Program Instance handler
        NULL
        );

    CEngine engine;

    SetWindowLong(hWnd,GWL_USERDATA,(long)&engine);

    ShowWindow(hWnd, SW_SHOW);
    if (!hWnd) 
    { 
        MessageBox(hWnd,"Couldn't create main window.\r\nAborting.","Something fucked up",0);
        return 0; 
    }

    SetFocus(hWnd);

    if (!stricmp(lpCmdline, "-benchmark"))
    {
        Benchmark(hWnd);
        return 0;
    }

    engine.Startup(hWnd,hThisInstance);
    engine.MainLoop();
    engine.Shutdown();

    return 0;
}
