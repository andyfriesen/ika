
#include "mapview.h"
#include "draw.h"
#include "log.h"
#include <wx/dcmemory.h>

// CMapWnd's events
//BEGIN_EVENT_TABLE(CMapWnd,wxMDIParentFrame)
//END_EVENT_TABLE()

CMapWnd::CMapWnd(wxMDIParentFrame* parent,const wxString& title,const wxPoint& position,const wxSize& size,const long style,Map* m,VSP* v)
:wxMDIChildFrame(parent,-1,title,position,size,style)
{
    // create the map widget thingie
    int width,height;
    GetClientSize(&width,&height);
    
    pMapwidget=new CMapWidget(this,wxPoint(0,0),wxSize(width,height),0,m,v);
    pMapwidget->SetScrollbars(20,20,50,50);
    
    pMap=m;
    pVsp=v;
    
    m->Load("test.map");    
    v->Load("test.vsp");
}

CMapWnd::~CMapWnd()
{
}

//----------------------------------------------------------------------

// CMapWidget's events
BEGIN_EVENT_TABLE(CMapWidget,wxScrolledWindow)
    EVT_PAINT(CMapWidget::OnPaint)
END_EVENT_TABLE()

static const int nBlocksize=256;

CMapWidget::CMapWidget(wxWindow* parent,const wxPoint& position,const wxSize& size,const long style,Map* m,VSP* v)
: wxScrolledWindow(parent,-1,position,size,style)
{
    pMap=m;
    pVsp=v;
    
    bLayertoggle.resize(m->NumLayers());
    for (int i=0; i<m->NumLayers(); i++)
        bLayertoggle[i]=1;
    xwin=ywin=0;
    
    pBackbuffer=new wxImage(nBlocksize,nBlocksize);
}

CMapWidget::~CMapWidget()
{
    delete pBackbuffer;
}

void CMapWidget::OnPaint(wxPaintEvent& p)
{
    RenderAll();
/*    int width,height;

    GetClientSize(&width,&height);
    RenderAll();

    wxPaintDC dc(this);
    wxBitmap b(*pBackbuffer);
    wxMemoryDC tempdc;
    tempdc.SelectObject(b);

    dc.Blit(0,0,width,height,&tempdc,0,0);*/
}

// Renders the specified layer.  If transparent is false, the layer is drawn opaque.
void CMapWidget::RenderLayer(int lay,bool transparent,const wxRect& r)
{
    SMapLayerInfo linf;
    int xw  ,yw;				// x/ywin after being adjusted for parallax
    int xl  ,yl;				// the last tile on each axis to be drawn
    int xofs,yofs;				// sub-tile adjustment
    int xs  ,ys;				// First tile on each axis
    int tilex,tiley;
    u32 t;
    
    //	if (!pMap->IsLayerVisible(lay) || lay>=pMap->NumLayers())	return;
//    if (lay>=pMap->NumLayers() || !bLayertoggle[lay])
  //      return;
    
    pMap->GetLayerInfo(linf,lay);
    tilex=pVsp->TileX();
    tiley=pVsp->TileY();
    
    xw=(xwin*linf.pmulx)/linf.pdivx;
    yw=(ywin*linf.pmuly)/linf.pdivy;
    
    xw+=r.x;                yw+=r.y;
    xofs=-(xw%tilex);       yofs=-(yw%tiley);
    xs=xw/tilex;            ys=yw/tiley;
    
    yl=((r.height)/tiley)+2; xl=((r.width)/tilex)+2;
    
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
                gfx::BlitTile(*pBackbuffer,x*tilex+xofs,y*tiley+yofs,t,*pVsp,transparent);
            //                Blit(tileimages[t],x*tilex+xofs,y*tiley+yofs,transparent);
        }
        layptr+=xinc;
    }
}

// Draws the obstruction grid.
void CMapWidget::DrawObstructions(const wxRect& r)
{
    int		xt,yt,xofs,yofs;
    int		x,y;
    int		xe,ye;
    
    //	if (!pMap->IsLayerVisible(lay_obs)) return;
    if (!bObstoggle)	return;
    
    xe=r.width/pVsp->TileX()+2;  ye=r.height/pVsp->TileY()+2;
    
    if (xe>=pMap->Width()) { xe=pMap->Width(); }
    if (ye>=pMap->Height()) { ye=pMap->Height(); }
    
    int xw=xwin+r.x;	int yw=ywin+r.y;
    xt=xw/pVsp->TileX(); yt=yw/pVsp->TileY();
    xofs=-(xw%pVsp->TileX()); yofs=-(yw%pVsp->TileY());
    
    for (y=0; y<ye; y++)
        for (x=0; x<xe; x++)
        {
            if (pMap->IsObs(x+xt,y+yt))
                gfx::Stipple(*pBackbuffer,x*pVsp->TileX()+xofs,y*pVsp->TileY()+yofs,pVsp->TileX(),pVsp->TileY(),0);
            //                pGraph->Stipple(x*pVsp->TileX()+xofs,y*pVsp->TileY()+yofs,pVsp->TileX(),pVsp->TileY(),0);
            //				stipple.Blit(tilewidth*x+xofs,tileheight*y+yofs);
        }
}

// Draws zone information
void CMapWidget::DrawZones(const wxRect& r)
{
    //	if (!pMap->IsLayerVisible(lay_zone)) return;
    if (!bZonetoggle)	return;
    
    int xt,yt,xofs,yofs;
    int x,y;
    int xe,ye;
    SMapLayerInfo linf;
    
    pMap->GetLayerInfo(linf,0);
    
    xe=(r.width)/pVsp->TileX()+2;  ye=(r.height)/pVsp->TileY()+2;
    if (xe>=pMap->Width()) { xe=pMap->Width(); }
    if (ye>=pMap->Height()) { ye=pMap->Height(); }
    
    int xw=xwin+r.x;        int yw=ywin+r.y;
    xt=xw/pVsp->TileX(); yt=yw/pVsp->TileY();
    xofs=-(xw%pVsp->TileX()); yofs=-(yw%pVsp->TileY());
    
    for (y=0; y<ye; y++)
        for (x=0; x<xe; x++)
        {
            if (pMap->GetZone(x+xt,y+yt))//pMap->zone[(y+yt)*pMap->info[0].sizex+x+xt])
                gfx::Stipple(*pBackbuffer,x*pVsp->TileX()+xofs,y*pVsp->TileY()+yofs,pVsp->TileX(),pVsp->TileY(),0);
            //                pGraph->Stipple(x*pVsp->TileX()+xofs,y*pVsp->TileY()+yofs,pVsp->TileX(),pVsp->TileY(),0);
            //	stipple.Blit((x*pVsp->TileX())+xofs,(y*pVsp->TileY())+yofs);
        }
}

// Draws entities (why.. what else? ;)
void CMapWidget::DrawEntities(const wxRect& r)
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
        
        if (x<r.GetRight()      && y<r.GetBottom() &&
            x>r.x-hotx          && y>r.y-hoty) // if the entity is on-screen, draw it.
            gfx::Stipple(*pBackbuffer,x-r.x,y-r.y,hotx,hoty,0);
    }
}

// Renders a white rectangle around the current selection.
void CMapWidget::DrawSelection(const wxRect& r)
{
    const int white=(255<<24)|(255<<16)|(255<<8)|(255);
    
    if (cursormode==mode_normal)	return;		// No visible selection.  Bye!
    
    int x1,y1,
        x2,y2;
    int i;
    x1=curselection.x;
    y1=curselection.y;
    x2=curselection.GetRight();
    y2=curselection.GetBottom();
    
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
    
    
    x1=x1*pVsp->TileX()-xwin-r.x;
    y1=y1*pVsp->TileY()-ywin-r.y;
    x2=x2*pVsp->TileX()-xwin-r.x;
    y2=y2*pVsp->TileY()-ywin-r.y;
    
    //    pGraph->Rect(x1,y1,x2,y2,white);
    gfx::Rect(*pBackbuffer,x1,y1,x2,y2,0);
}

void CMapWidget::Render(const wxRect& r)
{
    char *s;
    char renderstring[50];
    int  laycount=0;
    
    //    if (tileimages.size()!=pVsp->NumTiles())
    //       UpdateVSP();
    
    strcpy(renderstring,pMap->GetRString().c_str());
    
    s=renderstring;
    
    //pGraph->Clear();
    
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

void CMapWidget::RenderAll()
{
    int width,height;
    GetClientSize(&width,&height);

    wxPaintDC dc(this);

    for (int y=0; y<height/nBlocksize+1; y++)
        for (int x=0; x<width/nBlocksize+1; x++)
        {
            Render(wxRect(x*nBlocksize,y*nBlocksize,nBlocksize,nBlocksize));
            
            wxBitmap b(*pBackbuffer);
            wxMemoryDC tempdc;
            tempdc.SelectObject(b);
            
            dc.Blit(x*nBlocksize,y*nBlocksize,width,height,&tempdc,0,0);
        }
}