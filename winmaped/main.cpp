// gah... code bloat

#include "main.h"

const char wintitle[]="WinMapEd";
const char mapfilter[]="Map files\0*.map\0\0";
const char vspfilter[]="Tilesets\0*.vsp\0\0";

Engine::Engine()
{
    hWnd=0;
    pMapview=NULL;
}

Engine::~Engine()
{
    Shutdown();
}

// ------------------------============ Setup/shutdown ==============---------------------------

HWND Engine::CreateStatbar(HINSTANCE hInst)
{
    HWND hTmp;
    
    hTmp=CreateStatusWindow(WS_CHILD | WS_VISIBLE,"",hWnd,IDD_STATBAR);
    
    int iStatWidths[] = { 50,300,-1 };
    SendMessage(hTmp,SB_SETPARTS,3,(LPARAM)iStatWidths);
    
    return hTmp;
}

HWND Engine::CreateToolbar(HINSTANCE hInst)
{
    TBBUTTON tbb[14];
    HWND hToolbar;
    
    ZeroMemory(tbb,sizeof tbb);
    tbb[0].idCommand= ID_FILE_NEWMAP;
    tbb[0].fsState  = TBSTATE_ENABLED;
    tbb[1].iBitmap  = 1;
    tbb[1].idCommand= ID_FILE_OPENMAP;
    tbb[1].fsState  = TBSTATE_ENABLED;
    tbb[2].iBitmap  = 2;
    tbb[2].idCommand= ID_FILE_SAVEMAP;
    tbb[2].fsState  = TBSTATE_ENABLED;
    
    tbb[3].fsStyle	= TBSTYLE_SEP;
    
    tbb[4].iBitmap  = 3;
    tbb[4].idCommand= ID_SHOWLAY1;
    tbb[4].fsState  = TBSTATE_ENABLED;
    tbb[5].iBitmap  = 4;
    tbb[5].idCommand= ID_SHOWLAY2;
    tbb[5].fsState  = TBSTATE_ENABLED;
    tbb[6].iBitmap  = 5;
    tbb[6].idCommand= ID_SHOWLAY3;
    tbb[6].fsState  = TBSTATE_ENABLED;
    tbb[7].iBitmap  = 6;
    tbb[7].idCommand= ID_SHOWLAY4;
    tbb[7].fsState  = TBSTATE_ENABLED;
    tbb[8].iBitmap  = 7;
    tbb[8].idCommand= ID_SHOWLAY5;
    tbb[8].fsState  = TBSTATE_ENABLED;
    tbb[9].iBitmap  = 8;
    tbb[9].idCommand= ID_SHOWLAY6;
    tbb[9].fsState  = TBSTATE_ENABLED;
    
    tbb[10].fsStyle = TBSTYLE_SEP;
    
    tbb[11].iBitmap  = 9;
    tbb[11].idCommand= ID_SHOWOBS;
    tbb[11].fsState  = TBSTATE_ENABLED;
    tbb[12].iBitmap  = 10;
    tbb[12].idCommand= ID_SHOWZONE;
    tbb[12].fsState  = TBSTATE_ENABLED;
    tbb[13].iBitmap  = 11;
    tbb[13].idCommand= ID_SHOWENT;
    tbb[13].fsState  = TBSTATE_ENABLED;
    
    // Note: look up the TB_SETSTATE message for changing the button states dynamicALly
    HBITMAP hbm=LoadBitmap(hInst,MAKEINTRESOURCE(IDB_TOOLBUTTONS));
    
    // Create the toolbar
    hToolbar=CreateToolbarEx(hWnd,WS_CHILD | WS_VISIBLE,IDD_TOOLBAR,1,NULL,(UINT) hbm,tbb,14,16,16,16,16,sizeof tbb[0] );
    return hToolbar;
}

HWND Engine::CreateMainWindow(HINSTANCE hInst,int nCmdShow)
{
    const char szClassName[] = "WMEMainWnd";
    
    WNDCLASSEX	wincl;
    HWND hwnd;
    
    // The Window structure
    ZeroMemory(&wincl,sizeof wincl);
    wincl.hInstance = hInst;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = Engine::WndProc;								// This function is called by windows
    wincl.style = CS_HREDRAW | CS_VREDRAW;								// redraw whenever the window is resized
    wincl.cbSize = sizeof(WNDCLASSEX);
    
    wincl.hIcon = 0;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);						// Use default mouse-pointer
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);					// My menu
    wincl.cbClsExtra = 0;												// No extra bytes after the window class
    wincl.cbWndExtra = 0;												// structure or the window instance
    
    wincl.hbrBackground = NULL;											// IF I DO NOT DRAW IT, DO NOT DO ANYTHING
    
    RegisterClassEx(&wincl);											// if this fails, it's probably because the class was already registered
    
    hwnd = CreateWindowEx(												// The class is registered, let's create the program
	0,																// Extended possibilites for variation
	szClassName,													// Classname
	wintitle,														// Title Text
	WS_VISIBLE | WS_OVERLAPPEDWINDOW,								// window style
	CW_USEDEFAULT,													// Windows decides the position
	CW_USEDEFAULT,													// where the window ends up on the screen
	544,															// The programs width
	375,															// and height in pixels
	HWND_DESKTOP,													// The window is a child-window to desktop
	NULL,															// No menu
	hInst,															// Program Instance handler
	NULL															// No Window Creation data
	);
    SetWindowLong(hwnd,GWL_USERDATA,(long)this);						// Shackle the window to my engine class.  muahahahaha!  You are my prisoner, measly window!  AHAHAHAHAHAHAHAHAHA!
    
    return hwnd;
}

bool Engine::Init(HINSTANCE hInstance,int nCmdShow)
{    
    const int initxsize=600;									// these are arbitrary
    const int initysize=450;

    Log::Init("winmaped.log");
    Log::Write("WinMapEd Logging initialized");
    Log::Write("Built on %s",__DATE__);
    hInst=hInstance;
    
    hWnd=CreateMainWindow(hInstance,nCmdShow);
    hToolbar=CreateToolbar(hInstance);
    hStatbar=CreateStatbar(hInstance);
    hAccel=LoadAccelerators(hInstance,(LPCSTR)IDR_MAINMENU);
    
    pMapview=new CMapView(hWnd,hInstance,this,&map,&vsp);
    
    if (!hWnd) 
    {
	MessageBox(hWnd,"Couldn't create main window.\r\nAborting.","Something fucked up",0);
	return false;
    }
    Log::Write("Created main window");
    
    SizeWindow(initxsize,initysize);							// Inits correct clipping and whatnot
    vsp.New();
    map.New();
    //	SetActiveLayer(0);
    pMapview->SetActiveLayer(0);
    pMapview->ScrollWin(0,0);
    Log::Write("Map and VSP memory initialized");
    
    //	UpdateVSP();
    
    ShowWindow(hWnd, nCmdShow);									// Make the window visible on the screen
    PostMessage(hWnd,WM_PAINT,0,0);
    SetFocus(hWnd);												// In theory, this should send a WM_ACTIVATE message to our window, which should set bActive to true. (note that
    // it's important that the graphics are inited before this is called, as WM_PAINT uses the graphics stuff)

    Log::Write("Startup finished");
    return true;
}

void Engine::Shutdown()
{
    tilesel.Close();
    if (pMapview)
	delete pMapview;
    pMapview=0;
    
    map.Free();
    vsp.Free();
    
    DestroyWindow(hWnd);
    hWnd=0;
}

int Engine::SizeWindow(int x,int y)
{		
    RECT r,clientrect;
    
    PostMessage(hToolbar,WM_SIZE,SIZE_RESTORED,MAKELPARAM(x,y));
    PostMessage(hStatbar,WM_SIZE,SIZE_RESTORED,MAKELPARAM(x,y));		// relay to the toolbar and status bar
    
    GetClientRect(hWnd,&clientrect);							// adjust the clip rect for the new client region
    GetClientRect(hToolbar,&r);									// how much space does the toolbar take up?
    clientrect.top+=r.bottom+2;									// don't draw over the toolbar (plus two more pixels for good measure
    
    GetClientRect(hStatbar,&r);									// how much space does the stat bar take up?
    clientrect.bottom-=r.bottom;								// make room.
    
    pMapview->Resize(clientrect);
    
    bActive=true;
    return 0;				
}

int Engine::Execute(HINSTANCE hInst,int nCmdShow)
{
    MSG msg;
   
    if (!Init(hInst,nCmdShow))
    {
	Shutdown();
	return 0;
    }
    
    while (GetMessage(&msg,NULL,0,0))
    {
	if (!TranslateAccelerator(msg.hwnd,hAccel,&msg))
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }
    
    Shutdown();
    return 1;
}

// ---------------------=============================== WndProc... the meat of the app ===================-----------------------------

int Engine::WMEProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch (msg)
    {
	
    case WM_SIZE:
	switch(wParam)
	{
	case SIZE_MINIMIZED:
	case SIZE_MAXHIDE: 
	    bActive=false;
	    return 0;
	case SIZE_MAXIMIZED:
	case SIZE_RESTORED:
	case SIZE_MAXSHOW: 
	    SizeWindow(HIWORD(lParam),LOWORD(lParam));								
	    break;
	}
	break;
	
	case WM_PAINT:
	    {
		PAINTSTRUCT ps;
		BeginPaint(hWnd,&ps);
		pMapview->Redraw();
		EndPaint(hWnd,&ps);
	    }
	    break;
	    
	case WM_COMMAND:
	    switch (LOWORD(wParam))
	    {
		// File menu
	    case ID_FILE_NEWMAP:	NewMap();			break;
	    case ID_FILE_OPENMAP:	LoadMap();			break;
	    case ID_FILE_LOADVSP:	LoadVSP();			break;
	    case ID_FILE_SAVEMAP:	SaveMap();			break;
	    case ID_FILE_SAVEMAPAS:	SaveMapAs();		break;
	    case ID_FILE_EXIT:		PostQuitMessage(0);	break;
		// Edit menu
	    case ID_EDIT_ZONEPROPERTIES:	{	CZoneEdDlg zoneed;				zoneed.Execute(hInst,hWnd,pMapview->nCurzone,&map);		}	break;
	    case ID_EDIT_MAPPROPERTIES:		{	CMapDlg mapdlg;					mapdlg.Execute(hInst,hWnd,&map);						}	break;
	    case ID_EDIT_LAYERPROPERTIES:	{   CLayerDlg layerdlg;				layerdlg.Execute(hInst,hWnd,&map,pMapview->nCurlayer);	}	break;
	    case ID_EDIT_ADDLAYER:
		{
		    map.AddLayer(10);
		    pMapview->bLayertoggle.resize(map.NumLayers());;
		    MessageBox(hWnd,"Don't forget to update the render string!","",0);				
		}
		break;
	    case ID_EDIT_VSP:	tilesel.Execute(hInst,hWnd,vsp);	break;
		// Cool stuff
	    case ID_EDIT_COOLSTUFF_OBSTRUCTIONTHINGIE:
		{
		    CObstructionThingieDlg otd;
		    otd.Execute(hInst,hWnd,&map);
		}
		break;
	    case ID_EDIT_COOLSTUFF_CHREDITOR:
		{
		    CCHReditor chredit;
		    CCHRfile c;
		    chredit.Execute(hInst,hWnd,&c);
		}
		break;
	    case ID_EDIT_OPTIONS:
		{
		    CConfigDlg configdlg;
		    configdlg.Execute(hInst,hWnd,config);
		}
		break;
		// View menu
		/*		case ID_VIEW_ZOOM1:	Zoom(1);		PostMessage(hWnd,WM_PAINT,0,0);	break;
		case ID_VIEW_ZOOM2:	Zoom(2);		PostMessage(hWnd,WM_PAINT,0,0);	break;
		case ID_VIEW_ZOOM3:	Zoom(3);		PostMessage(hWnd,WM_PAINT,0,0);	break;
	    case ID_VIEW_ZOOM4:	Zoom(4);		PostMessage(hWnd,WM_PAINT,0,0);	break;*/
	    case ID_EDLAY1:		pMapview->SetActiveLayer(0);	break;
	    case ID_EDLAY2:		pMapview->SetActiveLayer(1);	break;
	    case ID_EDLAY3:		pMapview->SetActiveLayer(2);	break;
	    case ID_EDLAY4:		pMapview->SetActiveLayer(3);	break;
	    case ID_EDLAY5:		pMapview->SetActiveLayer(4);	break;
	    case ID_EDLAY6:		pMapview->SetActiveLayer(5);	break;
	    case ID_EDOBS:		pMapview->SetActiveLayer(lay_obs);	break;
	    case ID_EDZONE:		pMapview->SetActiveLayer(lay_zone);	break;
	    case ID_EDENT:		pMapview->SetActiveLayer(lay_ent);	break;
	    case ID_SHOWLAY1:	pMapview->FlipLayer(0);			break;
	    case ID_SHOWLAY2:	pMapview->FlipLayer(1);			break;
	    case ID_SHOWLAY3:	pMapview->FlipLayer(2);			break;
	    case ID_SHOWLAY4:	pMapview->FlipLayer(3);			break;
	    case ID_SHOWLAY5:	pMapview->FlipLayer(4);			break;
	    case ID_SHOWLAY6:	pMapview->FlipLayer(5);			break;
	    case ID_SHOWOBS:	pMapview->FlipLayer(lay_obs);	break;
	    case ID_SHOWZONE:	pMapview->FlipLayer(lay_zone);	break;
	    case ID_SHOWENT:	pMapview->FlipLayer(lay_ent);	break;
	    case ID_NEXTTILE:	pMapview->Mouse_NextTile();		break;
	    case ID_PREVTILE:	pMapview->Mouse_PrevTile();		break;
	    } 
	    break;
	    
	    case WM_MOUSEWHEEL:
		signed short int j;
		j=HIWORD(wParam);
		j/=WHEEL_DELTA;
		if (j>0) pMapview->Mouse_NextTile();
		if (j<0) pMapview->Mouse_PrevTile();
		pMapview->HandleMouse(LOWORD(lParam),HIWORD(lParam),0);
		return 0;
		
	    case WMU_SETLTILE:	pMapview->nLefttile=wParam;		break;
	    case WMU_SETRTILE:	pMapview->nRighttile=wParam;	break;
//	    case WMU_REBUILDVSP:
//		pMapview->UpdateVSP(wParam);
//		break;
		
	    default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

LRESULT CALLBACK Engine::WndProc(HWND hWnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
    Engine *e;
    
    if (msg==WM_QUIT || msg==WM_CLOSE) 
    {
	SetWindowLong(hWnd,GWL_USERDATA,0);
	PostQuitMessage(0);
	return 0;
    }
    
    e=(Engine*)GetWindowLong(hWnd,GWL_USERDATA);
    if (!e) return DefWindowProc(hWnd,msg,wParam,lParam);
    
    return e->WMEProc(msg,wParam,lParam);								// so we aren't dragged down by this whole 'static member function' stuff :/
}

// -----------------------------===================== Logic ==================----------------------

void Engine::Sys_Error(const char *errmsg)
{
    MessageBox(hWnd,errmsg,"Fatal Error",0);		// bitch ...
    PostQuitMessage(0);								//           ... quit
}

void Engine::SetStatbarText(int part,const char *text)
{
    if (part<0 || part>2) return;
    SendMessage(hStatbar,SB_SETTEXT,part,LPARAM(text));
}

// HACK HACK HACK
void Engine::UpdateToolbar()
{
    for (int i=0; i<6; i++)																						// GAK!  MAGIC NUMBERS BAD
	if (i>=map.NumLayers())
	    PostMessage(hToolbar,TB_SETSTATE,ID_SHOWLAY1+i,0);													// deactivate the button, 'cuz there is no layer there
	else
	    PostMessage(hToolbar,TB_SETSTATE,ID_SHOWLAY1+i,MAKELONG(TBSTATE_ENABLED | (pMapview->bLayertoggle[i]?TBSTATE_PRESSED:0),0));
	    /*		else if (bLayertoggle[i])
	    PostMessage(hToolbar,TB_SETSTATE,ID_SHOWLAY1+i,MAKELONG(TBSTATE_ENABLED | TBSTATE_PRESSED,0));		// push the button if the layer is visible
	    else
	PostMessage(hToolbar,TB_SETSTATE,ID_SHOWLAY1+i,MAKELONG(TBSTATE_ENABLED,0));						// enable the button if there is a layer there*/
	
	PostMessage(hToolbar,TB_SETSTATE,ID_SHOWOBS ,MAKELONG(TBSTATE_ENABLED |
	    (pMapview->bObstoggle?TBSTATE_PRESSED:0)							// push the button if the layer is visible
	    ,0));
	PostMessage(hToolbar,TB_SETSTATE,ID_SHOWZONE,MAKELONG(TBSTATE_ENABLED |
	    (pMapview->bZonetoggle?TBSTATE_PRESSED:0)
	    ,0));
	PostMessage(hToolbar,TB_SETSTATE,ID_SHOWENT ,MAKELONG(TBSTATE_ENABLED |
	    (pMapview->bEnttoggle ?TBSTATE_PRESSED:0)
	    ,0));
	
	int& nCurlayer=pMapview->nCurlayer;
	// Now, to highlight the button which corresponds with the active layer
	for (i=0; i<map.NumLayers(); i++)
	    PostMessage(hToolbar,TB_CHANGEBITMAP,ID_SHOWLAY1+i,MAKELPARAM(3+i+(nCurlayer==i?9:0),0));
	
	PostMessage(hToolbar,TB_CHANGEBITMAP,ID_SHOWOBS ,MAKELPARAM(9 +(nCurlayer==lay_obs ?9:0),0));
	PostMessage(hToolbar,TB_CHANGEBITMAP,ID_SHOWZONE,MAKELPARAM(10+(nCurlayer==lay_zone?9:0),0));
	PostMessage(hToolbar,TB_CHANGEBITMAP,ID_SHOWENT ,MAKELPARAM(11+(nCurlayer==lay_ent ?9:0),0));
}


void Engine::UpdateStatbar(int cursorx,int cursory)
// Also updates the toolbar buttons
{ 
    int i;
    
    int& nCurlayer=pMapview->nCurlayer;
    
    if (nCurlayer!=lay_ent)
	pMapview->GetTileCoords(cursorx,cursory);
    
    UpdateToolbar();
    
    SetStatbarText(0,va("(%i,%i)",cursorx,cursory));
    
    switch(nCurlayer)
    {
    case lay_obs:  SetStatbarText(1,"Obstruction layer"); SetStatbarText(2,""); break; // nuthin special
    case lay_zone: 
	{
	    SMapZone tempz;
	    
	    map.GetZoneInfo(tempz,map.GetZone(cursorx,cursory));                  
	    SetStatbarText(1, va("Zone under cursor #%i: %s",map.GetZone(cursorx,cursory),tempz.sName.c_str()) );
	    
	    map.GetZoneInfo(tempz,pMapview->nCurzone);
	    SetStatbarText(2, va("#%i: %s",pMapview->nCurzone,tempz.sName.c_str()) );
	}
	break;
	
    case lay_ent:
	{
	    SMapEntity tempe;
	    i=map.EntityAt(cursorx,cursory);
	    if (i!=-1)
	    {
		map.GetEntInfo(tempe,i);
		SetStatbarText(1, va("Entity #%i: %s",i,tempe.sName.c_str()) );
		SetStatbarText(2, tempe.sDescription.c_str() );
	    }
	    else
	    {
		SetStatbarText(1,"");
		SetStatbarText(2,"");
	    }
	}
	break;
	
    default: 
	SetStatbarText(1,va("Left tile: %i  Right tile: %i",pMapview->nLefttile,pMapview->nRighttile)); // we're editing a tile layer
	SetStatbarText(2,va("Tile under cursor: %i",map.GetTile(cursorx,cursory,nCurlayer)));
	break;
    }
}

// Makes the layer active, and visible.  Also hides the obstruction, entity, and zone layers, if they aren't being activated.

// Nukes everything and creates a fresh new map.
void Engine::NewMap()
{
    CNewMapDlg newmapdlg;
    
    if (!newmapdlg.Execute(hInst,hWnd))
	return;
    
    map.New();
    map.Resize(newmapdlg.info.mapx,newmapdlg.info.mapy);
    pMapview->bLayertoggle.resize(map.NumLayers());
    
    if (newmapdlg.info.bLoadvsp)
	vsp.Load(newmapdlg.info.sVspname.c_str());
    else
	vsp.New(newmapdlg.info.tilex,newmapdlg.info.tiley);
    
    //	vspimage.CopyVSPData(&vsp);
    
    pMapview->SetActiveLayer(0);
    //	tilesel.Close();
    pMapview->ScrollWin(0,0);
    PostMessage(hWnd,WM_PAINT,0,0);
}

// Prompts the user for a map file, and loads it if possible.
bool Engine::LoadMap()
{
    OPENFILENAME ofn;
    char filename[256];
    
    ZeroMemory(&ofn,sizeof ofn);
    ofn.hwndOwner=hWnd;
    ofn.lStructSize=sizeof ofn;
    ofn.hInstance=hInst;
    ofn.lpstrFilter=mapfilter;
    ofn.lpstrFileTitle=filename;
    ofn.nMaxFileTitle=255; // length of the filename array
    ofn.lpstrTitle="Load Map";
    ofn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
    
    if (!GetOpenFileName(&ofn)) return false;
    
    map.Load(filename);
    pMapview->bLayertoggle.resize(map.NumLayers());
    for (int i=0; i<map.NumLayers(); i++)
	pMapview->bLayertoggle[i]=true;
    
    if (!vsp.Load(map.GetVSPName().c_str())) 
    {
	Sys_Error("Error loading vsp");
	return false;
    }
    
    sLastopenedmap=filename;
    SetWindowText(hWnd,va("%s - [%s]",wintitle,filename));
    tilesel.Close();
    pMapview->SetActiveLayer(0);
//    pMapview->UpdateVSP();
    pMapview->Redraw();
    return true;
}

bool Engine::LoadVSP()
{
    OPENFILENAME ofn;
    char filename[256];
    
    ZeroMemory(&ofn,sizeof ofn);
    ofn.hwndOwner=hWnd;
    ofn.lStructSize=sizeof ofn;
    ofn.hInstance=hInst;
    ofn.lpstrFilter=vspfilter;
    ofn.lpstrFileTitle=filename;
    ofn.nMaxFileTitle=255; // length of the filename array
    ofn.lpstrTitle="Load VSP";
    ofn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_PATHMUSTEXIST;
    
    if (!GetOpenFileName(&ofn)) return false;
    
    try
    {
	int result=vsp.Load(filename);
	
	pMapview->SetActiveLayer(0);
//	pMapview->UpdateVSP();
	pMapview->Redraw();
	pMapview->ScrollWin(pMapview->xwin,pMapview->ywin);
	
	tilesel.Close();
    }
    catch (...)
    {
	Sys_Error("Error loading VSP");
    }
    
    return true;
}

// Saves the map
bool Engine::SaveMap()
{
    if (!sLastopenedmap.length())
	return SaveMapAs();								// no filename chosen?  Well!  Choose one now!
    
    map.Save(sLastopenedmap.c_str());
//    if (config.bSavev2vsps && vsp.Width()==16 && vsp.Width()==16)			// if we can, and the user wishes to...
//	vsp.SaveOld(map.GetVSPName().c_str());						// save a v2-style VSP instead of an ika VSP.
  //  else
	vsp.Save(map.GetVSPName().c_str());
    return true;
}

// Prompts the user for a filename, and saves the map.
bool Engine::SaveMapAs()
{
    OPENFILENAME ofn;
    char filename[256];
    
    ZeroMemory(&ofn,sizeof ofn);
    ofn.hwndOwner=hWnd;
    ofn.lStructSize=sizeof ofn;
    ofn.hInstance=hInst;
    ofn.lpstrFilter=mapfilter;
    ofn.lpstrFileTitle=filename;
    ofn.nMaxFileTitle=255;
    ofn.lpstrTitle="Save Map As...";
    ofn.Flags=OFN_OVERWRITEPROMPT;
    
    if (!GetSaveFileName(&ofn)) return false;
    
    if(filename[(strlen(filename)-4)] != '.')
	strcat(filename, ".map");
    
    map.Save(filename);
    
    if (!vsp.Save(map.GetVSPName().c_str())) 
    {
	Sys_Error("Error writing VSP");
	return false;
    }
    
    sLastopenedmap=filename;
    SetWindowText(hWnd,va("%s - [%s]",wintitle,filename));
    return true;
}

// -----------------------------=================== Rendering ================----------------------
