
#include "mapview.h"
#include "main.h"
#include "main.h"

CMapView::CMapView(HWND hWndparent,HINSTANCE hinst,Engine* pengine,Map* pmap,VSP* pvsp)
{
    hInst=hinst;
    pEngine=pengine;
    pMap=pmap;
    pVsp=pvsp;
    
    nLefttile=0;	nRighttile=0;
    nCurzone=0;		nCurlayer=0;
    cursormode=mode_normal;
    xwin=ywin=0;
    
    hWnd=CreateWnd(hWndparent);						// create the window
    
    pGraph=new CGraphView(hWnd,RenderCallback,(void*)this);
    
    bLayertoggle.resize(10);						// TEMP HACK
}

CMapView::~CMapView()
{
    delete pGraph;
    pGraph=0;
}

HWND CMapView::CreateWnd(HWND hWndparent)
{
    const char sClassname[] = "WME map view";
    WNDCLASSEX wincl;
    
    ZeroMemory(&wincl,sizeof wincl);
    wincl.hInstance=hInst;
    wincl.lpszClassName=sClassname;
    wincl.cbSize=sizeof wincl;
    wincl.style = CS_HREDRAW | CS_VREDRAW;
    wincl.lpfnWndProc=&CMapView::WndProc;
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClassEx(&wincl);
    
    HWND hwnd = CreateWindowEx(
        0,
        sClassname,
        sClassname,
        WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE,
        0,0,
        100,100,
        hWndparent,
        NULL,
        hInst,
        0);
    
    SetWindowLong(hwnd,GWL_USERDATA,(long)this);
    
    return hwnd;
}

void CMapView::Resize(const RECT& r)
{
    clientrect=r;
    MoveWindow(hWnd,r.left,r.top,r.right-r.left,r.bottom-r.top,true);
    ScrollWin(xwin,ywin);
}

void CMapView::GetTileCoords(int& x,int& y,int layer)
{
    SMapLayerInfo l;
    if (layer<0 || layer>=pMap->NumLayers())
        l.pmulx=l.pdivx=l.pmuly=l.pdivy=1;
    else
        pMap->GetLayerInfo(l,layer);
    
    x=(x+(xwin*l.pmulx/l.pdivx))/pVsp->Width();
    y=(y+(ywin*l.pmuly/l.pdivy))/pVsp->Height();
}

void CMapView::HandleMouse(int x,int y,int b)
{
    if (!(b&MK_LBUTTON)) bMouseleft =false;
    if (!(b&MK_RBUTTON)) bMouseright=false;
    if (!(b&MK_MBUTTON)) bMousemid  =false;
    
    SMapLayerInfo l;
    l.pdivx=l.pdivy=1;
    l.pmulx=l.pmuly=1;							// if map::getlayerinfo is out of bounds, we end up with 1:1 parallax
    
    pMap->GetLayerInfo(l,nCurlayer);
    
    pEngine->UpdateStatbar(x,y);
    
    if (bMouseleft)		DoMouseLeftDown	(x,y,b);
    else				DoMouseLeftUp	(x,y,b);
    
    if (bMouseright)	DoMouseRightDown(x,y,b);
    else				DoMouseRightUp	(x,y,b);
}

void CMapView::DoMouseLeftDown(int x,int y,int b)		// b is the key state
{
    int e;	
    RECT r= { x,y,x+pVsp->Width(),y+pVsp->Height() };	// In a lot of the cases below, we'll be re-rendering
    // a specific tile.  We're calculating that rect here, for brevity.
    switch (nCurlayer)
    {
    case lay_obs:
        GetTileCoords(x,y);
        pMap->SetObs(x,y,true);							// set an obstruction
        pGraph->ShowPage(r);
        break;
        
    case lay_zone:
        GetTileCoords(x,y);
        pMap->SetZone(x,y,nCurzone);					// set a zone
        pGraph->ShowPage(r);
        break;
        
    case lay_ent:
        x+=xwin;		y+=ywin;
        e=pMap->EntityAt(x,y);							// Either create an entity, or edit an existing entity
        bMouseleft=0;
        if (e==-1)
        {
            e=MessageBox(hWnd, "Do you wish to create a new entity here?" , "New Entity?", MB_YESNO|MB_SYSTEMMODAL);
            if (e==IDNO) return;
            e=pMap->CreateEntity(x,y);
        }
        
        {
            CEntityDlg entitydlg;						// This looks a bit kooky, I just wanted to destroy the dialog object right away.
            entitydlg.Execute(hInst,hWnd,pMap,e);
        }
        
        pGraph->ShowPage(r);
        return;
        
    default:
        GetTileCoords(x,y,nCurlayer);
        switch (cursormode)
        {
        case mode_normal:			
            if (b&MK_SHIFT)				// shift click (tile grabber)
            {
                nLefttile=pMap->GetTile(x,y,nCurlayer);
                return;
            }
            else if (b&MK_CONTROL)
            {
                curselection.left=x;
                curselection.top=y;
                curselection.right=curselection.left;
                curselection.bottom=curselection.top;
                cursormode=mode_copylay;
                return;
            }
            // default.  Place a tile
            pMap->SetTile(x,y,nCurlayer,nLefttile);
            pGraph->ShowPage(r);
            break;
        case mode_copylay:
            pGraph->ShowPage();
            
            pGraph->DirtyRect(MakeRect(
                curselection.left*pVsp->Width()-xwin-1,
                curselection.top*pVsp->Height()-ywin-1,
                curselection.right*pVsp->Width()-xwin+1,
                curselection.bottom*pVsp->Height()-ywin+1
                ));																// erase whatever was under the selection rect (including the rect itself)
            
            curselection.right=x+1;
            curselection.bottom=y+1;
            
            pGraph->DirtyRect(MakeRect(
                curselection.left*pVsp->Width()-xwin-1,
                curselection.top*pVsp->Height()-ywin-1,
                curselection.right*pVsp->Width()-xwin+1,
                curselection.bottom*pVsp->Height()-ywin+1
                ));																// redraw the new rect
            
            pGraph->ShowPage();
            break;
        }
    }
}

void CMapView::DoMouseLeftUp(int x,int y,int b)
{
    switch (nCurlayer)
    {
    case lay_obs:
    case lay_zone:
    case lay_ent:
        break;
    default:
        if (cursormode==mode_copylay)
        {
            
            int i;
            if (curselection.right<curselection.left)
            {
                i=curselection.right;
                curselection.right=curselection.left;
                curselection.left=i;
                
                curselection.right++;	// makes it act a little more intuitively
                curselection.left--;												
            }
            if (curselection.bottom<curselection.top)
            {
                i=curselection.top;
                curselection.top=curselection.bottom;
                curselection.bottom=i;
                
                curselection.bottom++;
                curselection.top--;
            }
            // done selecting.  Grab the chunk and store it away
            
            pMap->Copy(clipboard,curselection,nCurlayer);
            cursormode=mode_normal;
            PostMessage(hWnd,WM_PAINT,0,0);
            return;
        }
    }
}

void CMapView::DoMouseRightDown(int x,int y,int b)
{
    RECT r= { x,y,x+pVsp->Width(),y+pVsp->Height() };
    
    switch(nCurlayer)
    {
    case lay_obs:
        GetTileCoords(x,y);
        pMap->SetObs(x,y,false);
        pGraph->ShowPage(r);
        break;
        
    case lay_zone:
        GetTileCoords(x,y);
        pMap->SetZone(x,y,0);
        pGraph->ShowPage(r);
        break;
        
    case lay_ent:
        break;	// nuthin yet.  Consider allowing the user to drag entities around with this one. :)
        
    default:
        GetTileCoords(x,y,nCurlayer);
        switch(cursormode)
        {
        case mode_normal:
            if (!bMouseright) return;
            if (b&MK_CONTROL)													// Select region
            {
                curselection.left=x;											// here's one corner.
                curselection.top=y;
                
                cursormode=mode_copyall;										// and remember that we aren't in normal tile-editing mode now.
                return;
            }
            
            pMap->Paste(clipboard,x,y,nCurlayer);								// temp code -- TODO: Make pasting tile sections more intuitive than this
            r.right=r.left+(clipboard.Width()*pVsp->Width());
            r.bottom=r.top+(clipboard.Height()*pVsp->Height());
            pGraph->ShowPage(r);
            break;
            
        case mode_copyall:														// at this point, the user is still dragging their selection rectangle
            pGraph->ShowPage(MakeRect(
                curselection.left*pVsp->Width()-xwin-1,
                curselection.top*pVsp->Height()-ywin-1,
                curselection.right*pVsp->Width()-xwin+1,
                curselection.bottom*pVsp->Height()-ywin+1
                ));																// erase the old rect
            
            curselection.right=x+1;
            curselection.bottom=y+1;											// calculate the new rect
            
            pGraph->ShowPage(MakeRect(
                curselection.left*pVsp->Width()-xwin-1,
                curselection.top*pVsp->Height()-ywin-1,
                curselection.right*pVsp->Width()-xwin+1,
                curselection.bottom*pVsp->Height()-ywin+1
                ));																// redraw the new rect
            break;
        }			
    }
}

void CMapView::DoMouseRightUp(int x,int y,int b)
{
    switch (nCurlayer)
    {
    case lay_obs:
    case lay_zone:
    case lay_ent:
        break;	// bla
    default:
        if (cursormode==mode_copyall)									// Done selecting a region?  Great, copy some tiles to the clipboard
        {
            int i;
            if (curselection.right<curselection.left)					// Adjust the selection rect so that it behaves more intuitively.
            {
                i=curselection.right;
                curselection.right=curselection.left;
                curselection.left=i;
                
                curselection.right++;									// This causes the tile that the user initially started dragging on part
                curselection.left--;									// of the selected region, regardless as to which direction they dragged in
            }
            if (curselection.bottom<curselection.top)
            {
                i=curselection.top;
                curselection.top=curselection.bottom;
                curselection.bottom=i;
                
                curselection.bottom++;
                curselection.top--;
            }
            
            pMap->Copy(clipboard,curselection);							// done selecting.  Grab the chunk and store it away
            cursormode=mode_normal;										// reset the cursor mode
            PostMessage(hWnd,WM_PAINT,0,0);								// redraw the window
            return;
        }
    }
}

void CMapView::Mouse_NextTile()
{
    switch (nCurlayer)
    {
    case lay_zone:
        if (nCurzone<=pMap->NumZones())
            nCurzone++;
        break;
    default:
        nLefttile++;
        nLefttile=nLefttile%pVsp->NumTiles();	// clippin' yo
        return;
    }
}

void CMapView::Mouse_PrevTile()
{
    switch (nCurlayer)
    {
    case lay_zone:
        if (nCurzone>0)
            nCurzone--;
        break;
    default:
        if (nLefttile>0)
            nLefttile--;
        else
            nLefttile=pVsp->NumTiles()-1;
    }
    return;
}

void CMapView::SetActiveLayer(int i)
{
    bObstoggle=bEnttoggle=bZonetoggle=false;
    
    switch (i)
    {
    case lay_obs:	bObstoggle=true;		break;
    case lay_zone:	bZonetoggle=true;		break;
    case lay_ent:	bEnttoggle=true;		break;
    default:
        if (i<0 || i>=pMap->NumLayers())
        {
            log("SetActiveLayer: layer index out of bounds!!!");
            return;
        }
        bLayertoggle[i]=true;
    }
    
    nCurlayer=i;
    pEngine->UpdateToolbar();
    
    pGraph->ForceShowPage();
}

// Hides the layer if it's visible, shows it if not.
void CMapView::FlipLayer(int i)
{
    switch (i)
    {
    case lay_obs:		bObstoggle^=true;	break;
    case lay_zone:		bZonetoggle^=true;	break;
    case lay_ent:		bEnttoggle^=true;	break;
    default:
        if (i<0 || i>=pMap->NumLayers())
        {
            log("FlipLayer: layer index out of bounds!!!");
            return;
        }
        bLayertoggle[i]=!bLayertoggle[i];
        break;
    }
    
    pEngine->UpdateToolbar();
    pGraph->ForceShowPage();
}

//-----------------Rendering---------------------

void CMapView::Redraw()
{
    pGraph->ForceShowPage();
}

/*void CMapView::UpdateVSP(int tileidx)																	// Updates the tileimages vector
{
    if (tileidx<0 || tileidx>=pVsp->NumTiles())
    {
        tileimages.resize(pVsp->NumTiles());
        
        for (int i=0; i<pVsp->NumTiles(); i++)
        {
            tileimages[i].CopyPixelData((RGBA*)pVsp->GetPixelData(i),pVsp->Width(),pVsp->Height());
        }
    }
    else
    {
        tileimages[tileidx].CopyPixelData((RGBA*)pVsp->GetPixelData(tileidx),pVsp->Width(),pVsp->Height());
    }
}*/

// Scrolls the viewport to (x,y)
void CMapView::ScrollWin(int x,int y)
{
    SCROLLINFO si;
    int maxx,maxy;
    maxx=pMap->Width()*pVsp->Width();// - xres/Zoom();
    maxy=pMap->Height()*pVsp->Height();// - yres/Zoom();
    
    if (x<0) x=0; 
    if (y<0) y=0;
    if (x>maxx) x=maxx;
    if (y>maxy) y=maxy;
    
    xwin=x;
    ywin=y;
    
    GetClientRect(hWnd,&clientrect);
    
    si.cbSize=sizeof si;
    si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nPage=clientrect.bottom-clientrect.top;///Zoom();
    si.nMin=0;
    si.nMax=maxy;///Zoom();
    si.nPos=y;
    SetScrollInfo(hWnd,SB_VERT,&si,true);
    
    si.nPage=clientrect.right-clientrect.left;///Zoom();
    si.nPos=x;
    si.nMax=maxx;///Zoom();
    SetScrollInfo(hWnd,SB_HORZ,&si,true);
    
    PostMessage(hWnd,WM_PAINT,0,0);
}

// Renders the specified layer.  If transparent is false, the layer is drawn opaque.
void CMapView::RenderLayer(int lay,bool transparent,const RECT& r)
{
    SMapLayerInfo linf;
    int xw  ,yw;				// x/ywin after being adjusted for parallax
    int xl  ,yl;				// the last tile on each axis to be drawn
    int xofs,yofs;				// sub-tile adjustment
    int xs  ,ys;				// First tile on each axis
    int tilex,tiley;
    u32 t;
    
    //	if (!pMap->IsLayerVisible(lay) || lay>=pMap->NumLayers())	return;
    if (lay>=pMap->NumLayers() || !bLayertoggle[lay])
        return;
    
    pMap->GetLayerInfo(linf,lay);
    tilex=pVsp->Width();
    tiley=pVsp->Height();
    
    xw=(xwin*linf.pmulx)/linf.pdivx;
    yw=(ywin*linf.pmuly)/linf.pdivy;
    
    xw+=r.left;			yw+=r.top;
    xofs=-(xw%tilex);	yofs=-(yw%tiley);
    xs=xw/tilex;		ys=yw/tiley;
    
    yl=((r.bottom-r.top)/tiley)+2; xl=((r.right-r.left)/tilex)+2;
    
    if (xl+xs>=pMap->Width()) { xl=pMap->Width()-xs; }
    if (yl+ys>=pMap->Height()) { yl=pMap->Height()-ys; }
    
    u32* layptr=pMap->GetDataPtr(lay)+(ys*pMap->Width())+xs;
    int  xinc=pMap->Width()-xl;
    
    int x,y;
    for (y=0; y<yl; y++)
    {
        for (x=0; x<xl; x++)
        {
            t=*layptr++;
            if (t<0 || t>pVsp->NumTiles())				
            {
                log("tile out of bounds! :o");
                continue;
            }
            
            if (!(transparent && !t))
                pGraph->Blit(pVsp->GetTile(t),x*tilex+xofs,y*tiley+yofs,transparent);
        }
        layptr+=xinc;
    }
}

// Draws the obstruction grid.
void CMapView::DrawObstructions(const RECT& r)
{
    int		xt,yt,xofs,yofs;
    int		x,y;
    int		xe,ye;
    
    //	if (!pMap->IsLayerVisible(lay_obs)) return;
    if (!bObstoggle)	return;
    
    xe=(r.right-r.left)/pVsp->Width()+2;  ye=(r.bottom-r.top)/pVsp->Height()+2;
    
    if (xe>=pMap->Width()) { xe=pMap->Width(); }
    if (ye>=pMap->Height()) { ye=pMap->Height(); }
    
    int xw=xwin+r.left;	int yw=ywin+r.top;
    xt=xw/pVsp->Width(); yt=yw/pVsp->Height();
    xofs=-(xw%pVsp->Width()); yofs=-(yw%pVsp->Height());
    
    for (y=0; y<ye; y++)
        for (x=0; x<xe; x++)
        {
            if (pMap->IsObs(x+xt,y+yt))
                pGraph->Stipple(x*pVsp->Width()+xofs,y*pVsp->Height()+yofs,pVsp->Width(),pVsp->Height(),0);
            //				stipple.Blit(tilewidth*x+xofs,tileheight*y+yofs);
        }
}

// Draws zone information
void CMapView::DrawZones(const RECT& r)
{
    //	if (!pMap->IsLayerVisible(lay_zone)) return;
    if (!bZonetoggle)	return;
    
    int xt,yt,xofs,yofs;
    int x,y;
    int xe,ye;
    SMapLayerInfo linf;
    
    pMap->GetLayerInfo(linf,0);
    
    xe=(r.right-r.left)/pVsp->Width()+2;  ye=(r.bottom-r.top)/pVsp->Height()+2;
    if (xe>=pMap->Width()) { xe=pMap->Width(); }
    if (ye>=pMap->Height()) { ye=pMap->Height(); }
    
    int xw=xwin+r.left;		int yw=ywin+r.top;
    xt=xw/pVsp->Width(); yt=yw/pVsp->Height();
    xofs=-(xw%pVsp->Width()); yofs=-(yw%pVsp->Height());
    
    for (y=0; y<ye; y++)
        for (x=0; x<xe; x++)
        {
            if (pMap->GetZone(x+xt,y+yt))//pMap->zone[(y+yt)*pMap->info[0].sizex+x+xt])
                pGraph->Stipple(x*pVsp->Width()+xofs,y*pVsp->Height()+yofs,pVsp->Width(),pVsp->Height(),0);
            //	stipple.Blit((x*pVsp->Width())+xofs,(y*pVsp->Height())+yofs);
        }
}

// Draws entities (why.. what else? ;)
void CMapView::DrawEntities(const RECT& r)
// TODO: Make this blit actual CHRs, not just stipples.
{
    const int hotx=16;		// todo: change this
    const int hoty=16;
    
    //	if (!pMap->IsLayerVisible(lay_ent)) return;
    if (!bEnttoggle)	return;
    
    int idx;
    int x,y;
    SMapEntity e;
    
    for (idx=0; idx<pMap->NumEnts(); idx++)
    {
        pMap->GetEntInfo(e,idx);
        x=e.x - xwin;
        y=e.y - ywin;
        
        if (x<r.right		&& y<r.bottom &&
            x>r.left-hotx	&& y>r.top-hoty) // if the entity is on-screen, draw it.
            pGraph->Stipple(x-r.left,y-r.top,hotx,hoty,0);
    }
}

// Renders a white rectangle around the current selection.
void CMapView::DrawSelection(const RECT& r)
{
    const int white=(255<<24)|(255<<16)|(255<<8)|(255);
    
    if (cursormode==mode_normal)	return;		// No visible selection.  Bye!
    
    int x1,y1,
        x2,y2;
    int i;
    x1=curselection.left;
    y1=curselection.top;
    x2=curselection.right;
    y2=curselection.bottom;
    
    if (x2<x1)
    {
        i=x1; x1=x2; x2=i;
        
        //		x2++;  x1--;
    }
    if (y2<y1)
    {
        i=y1; y1=y2; y2=i;
        
        //		y2++; y1--;
    }
    
    
    x1=x1*pVsp->Width()-xwin-r.left;
    y1=y1*pVsp->Height()-ywin-r.top;
    x2=x2*pVsp->Width()-xwin-r.left;
    y2=y2*pVsp->Height()-ywin-r.top;
    
    pGraph->Rect(x1,y1,x2,y2,white);
}

void CMapView::Render(const RECT& r)
{
    char *s;
    char renderstring[50];
    int  laycount=0;
    
//    if (tileimages.size()!=pVsp->NumTiles())
//        UpdateVSP();
    
    strcpy(renderstring,pMap->GetRString().c_str());
    
    s=renderstring;
    
    pGraph->Clear();
    
    if (xwin<0) xwin=0;
    if (ywin<0) ywin=0;
    
    while (*s!='\0')
    {
        switch (*s)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6': 
            RenderLayer(*s-'1',laycount!=0?true:false,r);
            laycount++;
            break;
        }
        s++;
    }
    
    DrawObstructions(r);
    DrawZones(r);
    DrawEntities(r);
    DrawSelection(r);
    
    //	gfxShowPage();
}

//-----------------Callbacks---------------------
LRESULT CMapView::MsgProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
    case WM_LBUTTONDOWN:	bMouseleft =true;	HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	return 0;
    case WM_LBUTTONUP:		bMouseleft =false;	HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	return 0;
    case WM_RBUTTONDOWN:	bMouseright=true;	HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	return 0;
    case WM_RBUTTONUP:		bMouseright=false;	HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	return 0;
    case WM_MBUTTONDOWN:	bMousemid  =true;	HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	return 0;
    case WM_MBUTTONUP:		bMousemid  =false;	HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);	return 0;
    case WM_MOUSEMOVE:					HandleMouse(LOWORD(lParam),HIWORD(lParam),wParam);  return 0;
    case WM_VSCROLL: 
        switch (LOWORD(wParam))
        {
        case SB_BOTTOM:        ScrollWin(xwin,pMap->Height()*pVsp->Height());		return 0;
        case SB_TOP:           ScrollWin(xwin,0);					return 0;                      
        case SB_LINEDOWN:      ScrollWin(xwin,ywin+1);					return 0;
        case SB_LINEUP:        ScrollWin(xwin,ywin-1);					return 0;
        case SB_PAGEDOWN:      ScrollWin(xwin,ywin+clientrect.bottom);	                return 0;
        case SB_PAGEUP:        ScrollWin(xwin,ywin-clientrect.bottom);	                return 0;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION: ScrollWin(xwin,HIWORD(wParam));				return 0;                    
        }
        break;
        
        case WM_HSCROLL: 
            switch (LOWORD(wParam))
            {
            case SB_BOTTOM:        ScrollWin(pMap->Width()*pVsp->Width(),ywin);		return 0;
            case SB_TOP:           ScrollWin(0,ywin);					return 0;                      
            case SB_LINEDOWN:      ScrollWin(xwin+1,ywin);				return 0;
            case SB_LINEUP:        ScrollWin(xwin-1,ywin);				return 0;
            case SB_PAGEDOWN:      ScrollWin(xwin+clientrect.right,ywin);	        return 0;
            case SB_PAGEUP:        ScrollWin(xwin-clientrect.right,ywin);	        return 0;
            case SB_THUMBTRACK:
            case SB_THUMBPOSITION: ScrollWin(HIWORD(wParam),ywin);			return 0;                    
            }
            break;
            
            case WM_MOUSEWHEEL:  
            case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hWnd,&ps);
                    pGraph->ForceShowPage();
                    EndPaint(hWnd,&ps);
                }
    }
    return DefWindowProc(hWnd,msg,wParam,lParam);
}
void CMapView::RenderCallback(void* pThis,const RECT& r)
{
    ((CMapView*)pThis)->Render(r);
}

LRESULT CALLBACK CMapView::WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    CMapView* e;
    
    if (msg==WM_QUIT || msg==WM_CLOSE) 
    {
        SetWindowLong(hwnd,GWL_USERDATA,0);
        PostQuitMessage(0);
        return 0;
    }
    
    e=(CMapView*)GetWindowLong(hwnd,GWL_USERDATA);
    if (!e) return DefWindowProc(hwnd,msg,wParam,lParam);
    
    return e->MsgProc(msg,wParam,lParam);								// so we aren't dragged down by this whole 'static member function' stuff :/
}