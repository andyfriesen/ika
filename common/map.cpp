/* map.cpp
map stuff
written by tSB and all that poopoo 
Copyright me and stuff, so ph33r.
*/

#include "map.h"
#include "log.h"
#include "misc.h"

// ------------------============ map clip stuff =====================-------------------

MapClip::MapClip()
{
    nWidth=0;           nHeight=0;
    nLayers=0;
    pObstruct=NULL;     bObstruct=false;
    pZone=NULL;         bZone=false;
}

MapClip::~MapClip()
{
    Free();
}

void MapClip::Free()
{
    for (int i=0; i<nLayers; i++)
    {
        if (pData[i])
            delete[] pData[i];
        pData[i]=NULL;
        bUsed[i]=false;
    }
    pData.clear();
    bUsed.clear();
    
    if (pObstruct) delete[] pObstruct;
    if (pZone) delete[] pZone;
    pObstruct=pZone=NULL;
    bObstruct=false;
    bZone=false;
    bMegaclip=false;
}

void Map::Copy(MapClip &mc,Rect r,int layer)
// copies a single map layer to the clip
{
    int xl,yl;    // x,y length
    int xs,ys;    // x,y start
    
    xl=r.right-r.left;
    yl=r.bottom-r.top;
    xs=r.left;
    ys=r.top;
    
    // clipping
    if (xs+xl>=nWidth) xl=nWidth-xs;
    if (ys+yl>=nHeight) yl=nHeight-ys;
    
    if (xs<0 || ys<0) return;    // uh.... o_O
    if (xl<1 || yl<1) return;    // caca
    
    mc.Free();
    
    mc.bMegaclip=false;
    mc.pData.resize(1);
    mc.bUsed.resize(1);
    mc.bUsed[0]=true;    // dump it in layer 0 of the clip    
    mc.pData[0]=new u32[yl*xl];
    mc.bUsed[0]=true;
    mc.nWidth=xl;  mc.nHeight=yl;
    
    for (int ay=0; ay<yl; ay++)
        for (int ax=0; ax<xl; ax++)
            mc.pData[0][ay*xl+ax]=pData[layer][(ay+r.top)*nWidth+ax+r.left];    // buah... I'm not concerned with speed.  Not here.
}

void Map::Copy(MapClip &mc,Rect r)
// mass copier.  Copies all layers at once.
{
    int xl,yl;    // x/y length
    int xs,ys;    // x/y start
    
    xs=r.left;
    ys=r.top;
    xl=r.right-r.left;
    yl=r.bottom-r.top;
    
    // clipping
    if (xs+xl>=nWidth) xl=nWidth-xs;
    if (ys+yl>=nHeight) yl=nHeight-ys;
    
    if (xs<0 || ys<0) return;
    if (xl<1 || yl<1) return;
    
    mc.Free();
    
    mc.bMegaclip=true;
    mc.nWidth=xl;    mc.nHeight=yl;
    mc.pData.resize(NumLayers());
    mc.bUsed.resize(NumLayers());
    
    for (int curlayer=0; curlayer<NumLayers(); curlayer++)
    {
        mc.bUsed[curlayer]=true;
        mc.pData[curlayer]=new u32[yl*xl];
        
        for (int ay=0; ay<yl; ay++)
            for (int ax=0; ax<xl; ax++)
                mc.pData[curlayer][ay*xl+ax]=pData[curlayer][(ay+r.top)*nWidth+ax+r.left];    // buah... I'm not concerned with speed.  Not here.
    }
}

void Map::Paste(MapClip &mc,int xs,int ys,int sourcelayer,int destlayer,bool transparent)
// pastes one layer from the clip to a layer on the map (don't call this directly)
{
    int xl,yl;
    xl=mc.nWidth;  yl=mc.nHeight;
    
    if (xs+xl>nWidth)
        xl=nWidth-xs;
    if (ys+yl>nHeight)
        yl=nHeight-ys;
    
    for (int ay=0; ay<yl; ay++)
        for (int ax=0; ax<xl; ax++)
        {
            u32 t=mc.pData[sourcelayer][ay*mc.nWidth + ax];
            
            if (!(!t && transparent))
                pData[destlayer][(ay+ys)*nWidth + ax+xs ] = t;
        }
}

void Map::Paste(MapClip &mc,int x,int y,int destlayer)
{
    if (!mc.bMegaclip)    // only one layer here?
    {
        Paste(mc,x,y,0,destlayer,false);        // then paste it on the specified dest layer
        return;
    }
    
    for (int i=0; i<mc.nLayers; i++)    // else
        if (mc.bUsed[i])
            Paste(mc,x,y,i,i,true);        // copy whichever layer to wherever
}

// --------------------------======================================-----------------------------

static const char sOldmapsig[] = "MAPù5";
static const char sNewmapsig[] = "MAPù6";

// heh, con/destructors
Map::Map()
    : pObstruct(0), pZone(0), nWidth(0), nHeight(0)
{
    New();
}

Map::Map(const char* fname)
    : pObstruct(0), pZone(0), nWidth(0), nHeight(0)
{
    Load(fname);
}

Map::~Map()
{
    Free();
}

// Private member stuff

int Map::CountMoveScripts()
{
/*    int i;

  i=99;
  while (i)
  {
        if (strlen(movescript[i].t)) break;
                i--;
                }
    return i+1;*/
    return 0;
}

int Map::CountCHRs()
{
/*    int i;

  i=99;
  while (i)
  {
        if (strlen(chrlist[i].t)) break;
                i--;
                }
    return i+1;*/
    return 0;
}

int Map::CountUsedZones()
// basicly, it starts at the top, and stops as soon as it finds an unempty zone entry.
{
/*    int i;

  i=255;
  while (i)
  {
        if (zoneinfo[i].script)       break;
                if (zoneinfo[i].percent)      break;
                if (zoneinfo[i].delay)        break;
                if (zoneinfo[i].aaa)          break;
                if (zoneinfo[i].entityscript) break;
                if (strlen(zoneinfo[i].name)) break;
                i--;
                }
    return i+1;*/
    return 0;
}

// Interface

u32 Map::GetTile(int x,int y,int layer)
{
    if (layer>NumLayers()) return 0;
    if (x<0 || y<0) return 0;
    if (x>=nWidth) return 0;
    if (y>=nHeight) return 0;
    
    return pData[layer][y*nWidth+x];
}

void Map::SetTile(int x,int y,int layer,u32 tile)
{
    if (layer<0 || layer>NumLayers()) return;
    if (x<0 || y<0) return;
    if (x>=nWidth) return;
    if (y>=nHeight) return;
    
    pData[layer][y*nWidth+x]=tile;
}

u32* Map::GetDataPtr(int layer)
{
    if (layer<0 || layer>NumLayers()) return NULL;
    return pData[layer];
}

void Map::New()
{
    // TODO: remove redundant code from here, and simply call AddLayer
    Free();
    
    nWidth=nHeight=100;
    pData.resize(1);
    info.resize(1);
    pData[0]=new u32[nWidth*nHeight];                   // layer data
    memset(pData[0],0, nWidth*nHeight*sizeof(u32) );    // clear it
    
    pObstruct=new u8[nWidth*nHeight];
    pZone=new u32[nWidth*nHeight];
    
    memset(pObstruct,0, nWidth*nHeight*sizeof(u8) );
    memset(pZone,0, nWidth*nHeight*sizeof(u32) );
    
    info[0].pmulx = info[0].pdivx=1;
    info[0].pmuly = info[0].pdivy=1;    
    sMusicname="";
    sVSPname="untitled.vsp";
    SetRString("1ER");
}

void Map::Free()
{
    for (int i=0; i<NumLayers(); i++)
    {
        delete[] pData[i];
    }
    pData.clear();
    
    delete[] pObstruct;
    delete[] pZone;
    pObstruct=0;
    pZone=0;
}

// Good Lord, do not judge me by this code when I pass on!
bool Map::Importv2Map(File& f)
{
    int i;
    u8    cTemp;
    u16    wTemp;
    
    f.Read(i);
    
    char c[255];
    f.Read(c,60);   sVSPname=c;
    f.Read(c,60);   sMusicname=c;
    f.Read(c,20);   sRenderstring=c;
    f.Read(wTemp);  nStartx=wTemp;
    f.Read(wTemp);  nStarty=wTemp;
    f.Read(c,51);                        // don't ask me!
    
    f.Read(cTemp);
    int nLayers=cTemp;
    
    for (i=0; i<nLayers; i++)
    {
        layer_r templay;
       
        f.Read(&templay,12);            // ...... -_-;
        
        // If any maps exist which have different layer sizes, this could cause problems.
        nWidth=templay.sizex;   nHeight=templay.sizey;
        
        SMapLayerInfo l;
        l.pmulx=templay.pmultx; l.pdivx=templay.pdivx;
        l.pmuly=templay.pmulty; l.pdivy=templay.pdivy;
        l.nTransmode=templay.trans;
        
        info.push_back(l);
    }
    
    pData.resize(nLayers);
    
    for (i=0; i<nLayers; i++)
    {
        u32 nBufsize;
        f.Read(nBufsize);

        u32* pTemp=new u32[nWidth*nHeight];
        u8* pBuffer=new u8[nBufsize];
        
        f.Read(pBuffer,nBufsize);
        ReadCompressedLayer2tou32(pTemp,nWidth*nHeight,pBuffer);
        
        delete[] pBuffer;
        
        pData[i]=pTemp;
    }
    
    u32 nBufsize;
    u8* pBuffer;
    
    // Decompress obstruction crap (egad this is ugly code)
    pObstruct=new u8[nWidth*nHeight];
    f.Read(nBufsize);
    pBuffer=new u8[nBufsize];
    f.Read(pBuffer,nBufsize);
    ReadCompressedLayer1(pObstruct,nWidth*nHeight,pBuffer);
    delete[] pBuffer;
    
    // zone crap
    pZone=new u32[nWidth*nHeight];
    u8* pBuf2=new u8[nWidth*nHeight];
    
    f.Read(nBufsize);
    pBuffer=new u8[nBufsize];
    f.Read(pBuffer,nBufsize);
    ReadCompressedLayer1(pBuf2,nWidth*nHeight,pBuffer);
    delete[] pBuffer;
    for (i=0; i<nWidth*nHeight; i++)
        pZone[i]=pBuf2[i];
    delete[] pBuf2;
    
    // Zones
    int nZones;
    f.Read(nZones);
    
    zoneinfo.resize(nZones);
    
    for (i=0; i<nZones; i++)
    {
        zoneinfo_r z;
        
        f.Read(&z,sizeof(z));                                            // these magic numbers frighten me.
        
        zoneinfo[i].sName=va("Zone%i",i);
        zoneinfo[i].sDescription=z.name;
        
        if (z.script)       zoneinfo[i].sActscript=va("event%i",(int)z.script);
        else                zoneinfo[i].sActscript="";
        
        if (z.entityscript) zoneinfo[i].sEntactscript=va("event%i",(int)z.entityscript);
        else                zoneinfo[i].sEntactscript="";
        
        zoneinfo[i].nActchance=z.percent;
        zoneinfo[i].nActdelay=z.delay;
        zoneinfo[i].bAdjacentactivation=z.aaa?true:false;
    }
    
    char sChrlist[255][60];
    memset(sChrlist,0,255*60);
    
    f.Read(cTemp);
    for (i=0; i<cTemp; i++)
        f.Read(sChrlist[i],60);
    
    f.Read(cTemp);
    int nEnts=cTemp;
    
    // grargh
    std::vector<int>    nScriptidx;
    nScriptidx.resize(nEnts);
    entity.resize(nEnts);
    
    for (i=0; i<nEnts; i++)
    {
        Sv2entity e1;
        SMapEntity e2;
        
        f.Read(e1);
        
        e2.sName=va("Ent%i",i);
        e2.sDescription=e1.desc;
        
        e2.x=e1.x*16;        e2.y=e1.y*16;
        e2.direction=e1.facing;
        switch (e1.speed)
        {
        case 1:    e2.nSpeed=12;        break;
        case 2:    e2.nSpeed=25;        break;
        case 3:    e2.nSpeed=50;        break;
        case 4:    e2.nSpeed=100;        break;
        case 5:    e2.nSpeed=200;        break;
        case 6:    e2.nSpeed=400;        break;
        case 7:    e2.nSpeed=800;        break;
        case 8:    e2.nSpeed=0;        break;
        default:
            e2.nSpeed=100;
        }
        
        e2.bEntobs=e1.bIsobs!=0;
        e2.bMapobs=e1.bIsobs!=0;
        e2.bIsobs=e1.bIsobs!=0;
        e2.sCHRname=sChrlist[e1.chrindex];
        e2.bAdjacentactivation=e1.actm!=0;
        e2.sActscript=va("event%i",e1.actscript);
        e2.state=(MoveCode)e1.movecode;
        nScriptidx[i]=e1.movescript;    // blargh
        e2.nWanderdelay=e1.delay;
        e2.nWandersteps=e1.step*16;
        e2.wanderrect.left=e1.data1;
        e2.wanderrect.top=e1.data2;
        e2.wanderrect.right=e1.data3;
        e2.wanderrect.bottom=e1.data4;
        //e2.sZone=? :P
        //e2.sChasetarget="";
        //e2.nChasedist=0;
        
        entity[i]=e2;
    }
    
    std::vector<string>    sMovescripts;
    char nMovescripts;
    int nCount;
    f.Read(nMovescripts);
    u32* nOfstbl=new u32[nMovescripts*4];;

    f.Read(nCount);
    f.Read(nOfstbl,nMovescripts*4);
    nOfstbl[nMovescripts]=nCount;
    
    sMovescripts.resize(nMovescripts);
    
    for (i=0; i<nMovescripts-1; i++)
    {
        f.Read(c,nOfstbl[i+1]-nOfstbl[i]);
        sMovescripts[i]=c;
    }

    delete[] nOfstbl;
    
    for (i=0; i<nEnts; i++)
    {
        int j=nScriptidx[i];
        entity[i].sMovescript=sMovescripts[j];
    }
    
    return true;
}

bool Map::Exportv2Map(const char* fname)
{
/*    File f;
    int i, ofstbl[200],ct,t;
    char strbuf[255];
    
    int numzones,numchr,numms;
    
    f.OpenWrite(fname);
    f.Write("MAPù5",6);
    f.Write("    ",4);
    strcpy(strbuf,sVSPname.c_str());        f.Write(strbuf,60);
    strcpy(strbuf,sMusicname.c_str());      f.Write(strbuf,60);
    strcpy(strbuf,sRenderstring.c_str());   f.Write(strbuf,20);
    f.Write(&nStartx,2);
    f.Write(&nStarty,2);
    f.Write(&bWrap,1);
    f.Write(strbuf,50);
    f.Write(&nLayers,1);

    for (i=0; i<nLayers; i++)
    {
        layer_r l =
        { 
            info[i].pmulx,  info[i].pdivx,
            info[i].pmuly,  info[i].pdivy,
            nWidth,         nHeight,
            info[i].nTransmode
        };

        f.Write(&l,12);
    }

    for (i=0; i<nLayers; i++)
    {        
        u16* pBuffer=new u16[nWidth*nHeight];
        int nBuffersize;


//        WriteCompressedLayer2(pBuffer,nWidth*nHeight,nBuffersize,pData[i]);   
        
        f.Write(&nBuffersize,4);
        f.Write(pBuffer,bufsize);
        delete[] cb;
    }

    int nBuffersize;
    u8* cb=new byte[nWidth*nHeight];
    WriteCompressedLayer1(cb,nWidth*nHeight,nBuffersize,obstruct);
    f.Write(&nBuffersize,4);
    f.Write(cb,nBuffersize);
    
    WriteCompressedLayer1(cb,nWidth*nHeight,nBuffersize,zone);
    f.Write(&nBuffersize,4);
    f.Write(pBuffer,nBuffersize);
    delete[] pBuffer;
    
    numzones=CountUsedZones();
    f.Write(&numzones,4);
    f.Write(&zoneinfo,numzones*50);
    numchr=CountCHRs();
    f.Write(&numchr,1);
    f.Write(&chrlist,numchr*60);
    
    f.Write(&entities,1);
    for (i=0; i<entities; i++)
    {
        entity[i].x=entity[i].tx;
        entity[i].y=entity[i].ty;
    }
    f.Write(&entity,sizeof(entity)/256*entities);
    numms=CountMoveScripts();
    f.Write(&numms,1);
    ct=0;
    for (i=0; i<numms; i++)
    {
        ofstbl[i]=ct;
        t=strlen((char*)(&movescript[i].t))+1;
        ct+=t;
    }
    f.Write(&ct,4);
    f.Write(&ofstbl,numms*4);
    for (i=0; i<numms; i++)
        f.Write((char*)(&movescript[i].t),strlen((char*)(&movescript[i].t))+1);
    
    ct=0;
    f.Write(&ct,4);
    ct=f.Pos();
    
    i=1; f.Write(&i,4);
    i=0; f.Write(&i,4);
    i=1; f.Write(&i,4);
    i=9; f.Write(&i,1);
    
    f.Seek(6);
    f.Write(&ct,4);
    f.Close();
    */
    return false;
}

bool Map::Load(const char* fname)
{
    int i;
    
    File f;
    
    bool bResult=f.OpenRead(fname);
    if (!bResult)
        return false;
    
    char c[1024];
    
    f.Read(c,6);
    c[6]=0;    // just in case?
    if (!strcmp(c,sOldmapsig))
        return Importv2Map(f);
    else if (strcmp(c,sNewmapsig)!=0)
        return false;    // unrecognized map signature
    
    Free();
    
    f.ReadString(c);    sVSPname=c;
    f.ReadString(c);    sMusicname=c;
    f.ReadString(c);    sRenderstring=c;
    
    f.Read(nWidth);
    f.Read(nHeight);
    
    f.Read(nStartx);
    f.Read(nStarty);
    f.Read(bWrap);

    int nLayers;
    f.Read(nLayers);
    for (i=0; i<nLayers; i++)
    {
        SMapLayerInfo l;
        f.Read(l.pmulx);
        f.Read(l.pdivx);
        f.Read(l.pmuly);
        f.Read(l.pdivy);
        
        info.push_back(l);
    }
    
    pData.resize(nLayers);
    for (i=0; i<nLayers; i++)
    {
        pData[i]=new u32[nWidth*nHeight];
        f.ReadCompressed(pData[i],nWidth*nHeight*sizeof(u32));
    }
    
    pObstruct=new u8[nWidth*nHeight];
    f.ReadCompressed(pObstruct,nWidth*nHeight*sizeof(u8));
    
    pZone=new u32[nWidth*nHeight];
    f.ReadCompressed(pZone,nWidth*nHeight*sizeof(u32));
    
    int nZones;
    f.Read(nZones);
    
    zoneinfo.resize(nZones);
    
    for (i=0; i<nZones; i++)
    {
        char c[1024];
        f.ReadString(c);    zoneinfo[i].sName=c;
        f.ReadString(c);    zoneinfo[i].sDescription=c;
        f.ReadString(c);    zoneinfo[i].sActscript=c;
        f.ReadString(c);    zoneinfo[i].sEntactscript=c;
        f.Read(zoneinfo[i].nActchance);
        f.Read(zoneinfo[i].nActdelay);
        f.Read(zoneinfo[i].bAdjacentactivation);
    }
    
    int nEnts;
    f.Read(nEnts);
    entity.resize(nEnts);
    
    for (i=0; i<nEnts; i++)
    {
        SMapEntity& e=entity[i];
        
        char c[1024];
        f.ReadString(c);        e.sName=c;
        f.Read(e.x);
        f.Read(e.y);
        f.Read(e.direction);
        f.Read(e.nSpeed);
        f.Read(e.bMapobs);
        f.Read(e.bEntobs);
        f.Read(e.bIsobs);
        f.ReadString(c);        e.sCHRname=c;
        f.Read(e.bAdjacentactivation);
        f.ReadString(c);        e.sActscript=c;
        f.Read(e.state);
        f.ReadString(c);        e.sMovescript=c;
        f.Read(e.nWandersteps);
        f.Read(e.nWanderdelay);

        f.Read(e.wanderrect.left);
        f.Read(e.wanderrect.top);
        f.Read(e.wanderrect.right);
        f.Read(e.wanderrect.bottom);
        
        f.ReadString(c);        e.sZone=c;
        f.ReadString(c);        e.sChasetarget=c;
        f.Read(e.nChasedist);
    }
    
    f.Close();
    
    return true;
}

bool Map::Save(const char* fname)
{
    File f;
    
    bool bResult=f.OpenWrite(fname);
    if (!bResult)
        return false;
    
    f.Write(sNewmapsig,6);
    
    f.WriteString(sVSPname.c_str());
    f.WriteString(sMusicname.c_str());
    f.WriteString(sRenderstring.c_str());
    
    f.Write(nWidth);
    f.Write(nHeight);
    
    f.Write(nStartx);
    f.Write(nStarty);
    f.Write(bWrap);

    f.Write(NumLayers());
    for (int i=0; i<NumLayers(); i++)
    {
        f.Write(info[i].pmulx);
        f.Write(info[i].pdivx);
        f.Write(info[i].pmuly);
        f.Write(info[i].pdivy);
    }
    
    for (int i=0; i<NumLayers(); i++)
        f.WriteCompressed(pData[i],nWidth*nHeight*sizeof(u32));
    
    f.WriteCompressed(pObstruct,nWidth*nHeight*sizeof(u8));
    f.WriteCompressed(pZone,nWidth*nHeight*sizeof(u32));
    
    f.Write(NumZones());
    
    for (int i=0; i<NumZones(); i++)
    {
        f.WriteString(zoneinfo[i].sName.c_str());
        f.WriteString(zoneinfo[i].sDescription.c_str());
        f.WriteString(zoneinfo[i].sActscript.c_str());
        f.WriteString(zoneinfo[i].sEntactscript.c_str());
        f.Write(zoneinfo[i].nActchance);
        f.Write(zoneinfo[i].nActdelay);
        f.Write(zoneinfo[i].bAdjacentactivation);
    }
    
    f.Write(entity.size());
    
    for (u32 i=0; i<entity.size(); i++)
    {
        
        f.WriteString(entity[i].sName.c_str());
        f.Write(entity[i].x);
        f.Write(entity[i].y);
        f.Write(entity[i].direction);
        f.Write(entity[i].nSpeed);
        f.Write(entity[i].bMapobs);
        f.Write(entity[i].bEntobs);
        f.Write(entity[i].bIsobs);
        f.WriteString(entity[i].sCHRname.c_str());
        f.Write(entity[i].bAdjacentactivation);
        f.WriteString(entity[i].sActscript.c_str());
        f.Write(entity[i].state);
        f.WriteString(entity[i].sMovescript.c_str());
        f.Write(entity[i].nWandersteps);
        f.Write(entity[i].nWanderdelay);

        f.Write(entity[i].wanderrect.left);
        f.Write(entity[i].wanderrect.top);
        f.Write(entity[i].wanderrect.right);
        f.Write(entity[i].wanderrect.bottom);

        f.WriteString(entity[i].sZone.c_str());
        f.WriteString(entity[i].sChasetarget.c_str());
        f.Write(entity[i].nChasedist);
    }
    
    f.Close();
    
    return true;
}

void Map::Resize(int newx,int newy)
/*
    note to self:  This routine really messes with the memory.  Make damn
    sure it works right, because if it doesn't, all hell can break loose.
    This is directly ported from MapEd 2+i

    Coming back to this after howevermany months of not looking at it, I think
    "dude... this is ugly", and yet I can't think of any way to clean it up.
    *shrugs*
*/
{
    
    int maxx,maxy;                                                  // how many tiles in each axis to be copied
    int oldx,oldy;
    int x,y;                                                        // loop counters
    
    oldx=nWidth; oldy=nHeight;
    
    maxx=(oldx<newx)?oldx:newx;
    maxy=(oldy<newy)?oldy:newy;                                     // whichever is smaller
    
    u32* pTemp=new u32[newx*newy];                                  // first the zone layer
    memset(pTemp,0,newx*newy*sizeof(u32));
    for (y=0; y<maxy; y++)
    {
        for (x=0; x<maxx; x++)
            pTemp[y*newx+x] = pZone[y*oldx+x];
    }
    delete[] pZone;                                                 // dump the old data
    pZone=pTemp;                                                    // and use the new data instead
    
    
    u8* pcTemp=new u8[newx*newy];                                   // now the obstructions
    memset(pcTemp,0,newx*newy);
    for (y=0; y<maxy; y++)
    {
        for (x=0; x<maxx; x++)
            pcTemp[y*newx+x] = pObstruct[y*oldx+x];
    }
    delete[] pObstruct;
    pObstruct=pcTemp;
    
    
    for (int curlayer=0; curlayer<NumLayers(); curlayer++)          // and finally, the tile layers
    {
        
        u32* pTemp=new u32[newx*newy];
        memset(pTemp,0,newx*newy*sizeof(u32));
        for (y=0; y<maxy; y++)
            for (x=0; x<maxx; x++)
                pTemp[y*newx+x]=pData[curlayer][y*oldx+x];
            
            delete[] pData[curlayer];
            pData[curlayer]=pTemp;
            
    }
    this->nWidth=newx; this->nHeight=newy;                          // whoops, one must be careful to not clog one's namespace up. >_<
}

int Map::NumLayers()
{
    return pData.size();
}

void Map::AddLayer(int pos)
{
    // TODO: make this actually use the pos variable. :P
    //    if (nLayerss) return;                                     // TODO: Remove this
    
    SMapLayerInfo newlay;
    
    newlay.pmulx = newlay.pdivx = 1;
    newlay.pmuly = newlay.pdivy = 1;
    u32* pTemp=new u32[nWidth*nHeight];
    memset(pTemp,0, nWidth*nHeight*sizeof(u32) );                   // make sure the new layer is empty
    
    info.push_back(newlay);
    pData.push_back(pTemp);
}

void Map::DeleteLayer(int pos)
{
}

void Map::GetLayerInfo(SMapLayerInfo& nfo,int layidx)
{
    if (layidx>=0 && layidx<NumLayers())
        nfo=info[layidx];
}

void Map::SetLayerInfo(const SMapLayerInfo& nfo,int layidx)
{
    if (layidx>=0 && layidx<NumLayers())
    {
        info[layidx]=nfo;
        if (!info[layidx].pdivx) {    info[layidx].pdivx=1;    info[layidx].pmulx=0;    }
        if (!info[layidx].pdivy) {    info[layidx].pdivy=1;    info[layidx].pmuly=0;    }
    }
}

const string& Map::GetRString()
{ return sRenderstring; }

void Map::SetRString(const string& s)
{ sRenderstring=s;}//.upper(); }

const string& Map::GetMusic()
{ return sMusicname; }

void Map::SetMusic(const string& s)
{ sMusicname=s; }

const string& Map::GetVSPName()
{ return sVSPname; }

void Map::SetVSPName(const string& s)
{ sVSPname=s; }

const char* Map::GetCHREntry(int idx)
{
/*    if (idx>=0 && idx<100)
return chrlist[idx].t;
    else*/
    return NULL;
}

void Map::SetCHREntry(const char* newchr,int idx)
{
    //    if (idx>=0 && idx<100)
    //        strcpy(chrlist[idx].t,newchr);
    return;
}

const char* Map::GetMScript(int idx)
{
/*    if (idx>=0 && idx<100)
return movescript[idx].t;
    else*/
    return NULL;
}

void Map::SetMScript(const char* newscript,int idx)
{
    //    if (idx>=0 && idx<100)
    //        strcpy(movescript[idx].t,newscript);
    return;
}

void Map::SetObs(int x,int y,bool mode)
{
    if (x<0 || x>=nWidth)  return;
    if (y<0 || y>=nHeight) return; // bounds checking
    
    pObstruct[y*nWidth+x]=(mode==true?1:0);
}

bool Map::IsObs(int x,int y)
{
    if (x<0 || x>=nWidth) return true;
    if (y<0 || y>=nHeight) return true;    // outside the map?  HELL YES ITS OBSTRUCTED!  DON'T GO THERE!
    
    if (pObstruct[y*nWidth+x]) return true;
    return false;
}

void Map::SetZone(int x,int y,int z)
{
    if (x<0 || x>=nWidth) return;
    if (y<0 || y>=nHeight) return; // bounds checking
    
    pZone[y*nWidth+x]=z;
}

int  Map::GetZone(int x,int y)
{
    if (x<0 || x>=nWidth) return false;
    if (y<0 || y>=nHeight) return false; // bounds checking
    
    return pZone[y*nWidth+x];
}

void Map::GetZoneInfo(SMapZone& info,int zonenum)
{
    if (zonenum>=0 && zonenum<zoneinfo.size())
        info=zoneinfo[zonenum];
    else
    {
        SMapZone dummy;
        info=dummy;
    }
}

void Map::SetZoneInfo(const SMapZone& info,int zonenum)
{
    if (zonenum>=0 && zonenum<zoneinfo.size())
        zoneinfo[zonenum]=info;
    if (zonenum>=zoneinfo.size() && 
            (
                info.sName!="" ||
                info.sActscript!="" ||
                info.sDescription!="" ||
                info.sEntactscript!=""
            )
        )
        zoneinfo.push_back(info);
}

int Map::EntityAt(int x,int y)
{
    // Temp, make this handle hotspots later?
    const int nHotx=16;
    const int nHoty=16;
    int i;
    
    for (i=0; i<entity.size(); i++)
        if (x>=entity[i].x && y>=entity[i].y &&
            x<=entity[i].x+nHotx && y<=entity[i].y+nHoty)
            return i;
    return -1;
}

/*void Map::GetEntInfo(SMapEntity& info,int entidx)
{
    if (entidx>=0 && entidx<entity.size())
        info=entity[entidx];
}

void Map::SetEntInfo(const SMapEntity& info,int entidx)
{
    if (entidx>=0 && entidx<entity.size())
        entity[entidx]=info;
}*/

SMapEntity& Map::GetEntity(int idx) const
{
    static SMapEntity dummy;

    if (idx<0 || idx>=entity.size())
        return dummy;

    return const_cast<SMapEntity&>(entity[idx]);
}

void Map::SetEntity(SMapEntity& e,int idx)
{
    if (idx<0 || idx>=entity.size())
        return;

    entity[idx]=e;
}

int Map::CreateEntity(int x,int y)
{
    SMapEntity ent;
    ent.x=x; ent.y=y;
    ent.nSpeed=100;
    entity.push_back(ent);
    
    return entity.size()-1;
}

void Map::DestroyEntity(int entidx)
{
    for (int i=entidx; i<entity.size()-1; i++)
        entity[i]=entity[i+1];
    
    entity.pop_back();
}

int Map::NumEnts() const
{
    return entity.size();
}

/*void Map::SetWin(int xw,int yw)
{
xwin=xw; ywin=yw;
//ywin++;
}

  void Map::SetWinRel(int x,int y)
  {
  SetWin(xwin+x,ywin+y);
  }
*/