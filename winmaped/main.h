/*

  WinMapEd version whatever.
  
  Main dealy
    
  I have a sinking suspicion that this is gonna get huge, since it's the one that has to throw all the objects around
  and make them interoperate correctly.
      
  ADDENUM:
  I was right.
        
*/

#ifndef MAIN_H
#define MAIN_H

#include "winmaped.h"
#include "graph.h"
#include "map.h"
#include "vsp.h"
#include "log.h"
// Dialogs
#include "mapview.h"
#include "zoneed.h"
#include "miscdlg.h"
#include "mapdlg.h"
#include "layerdlg.h"
#include "entityed.h"
#include "tilesel.h"
#include "coolstuff.h"
#include "chred.h"

class Engine
{
private:
    HWND	hWnd;				// window handle
    HWND	hToolbar;			// toolbar handle
    HWND	hStatbar;			// statbar handle
    HACCEL	hAccel;				// accelerator table handle
    HINSTANCE	hInst;			// instance handle
    
    SMapEdConfig	config;		// config options
    
    CTileSel	tilesel;		// tile selection window
    
    Map		map;				// the current map
    VSP		vsp;				// the current VSP
    CMapView*	pMapview;		// client window in which the map is rendered
    
    bool	bActive;			// don't do anything if this isn't set
    
    // UI variables
    string	sLastopenedmap;
    // filename of the last map that was opened.  For the SaveMap() function
    // helper funtion thingies
    HWND CreateMainWindow(HINSTANCE hInst,int nCmdShow);
    HWND CreateToolbar(HINSTANCE hInst);
    HWND CreateStatbar(HINSTANCE hInst);
    
public:
    Engine();
    ~Engine();
    int  Execute(HINSTANCE hInst,int nCmdShow);		
    
private:
    bool Init(HINSTANCE hInstance,int nCmdShow);
    void Shutdown();
    int  SizeWindow(int x,int y);
    
    // -------------- WndProc ----------------
    int  WMEProc(UINT msg,WPARAM wParam,LPARAM lParam);
    static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam);
    
public:
    // ------------ Logic stuff --------------
    void Sys_Error(const char *errmsg);														// bitches and quits the app
    void SetStatbarText(int part,const char *text);											// Sets the text in a given part of the status bar
    void UpdateToolbar();
    void UpdateStatbar(int cursorx,int cursory);											// Updates the status bar with the usual stuff
    
    void NewMap();																			// new map/vsp
    bool LoadMap();																			// prompts for a filename, and loads it if it can
    bool LoadVSP();																			// replaces the map's VSP with one of the user's choice
    bool SaveMap();																			// saves map/vsp, recompiles, blah blah
    bool SaveMapAs();																		// prompts for a filename, and saves the map
};

#endif