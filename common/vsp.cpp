// VSP.cpp
#include <stdio.h>
#include "VSP.h"
#include "types.h"
#include "rle.h"
#include "fileio.h"
#include "log.h"
#include "zlib.h"

VSP::VSP() : nTilex(0), nTiley(0)
{
    New();
}

VSP::VSP(const char* fname)
{
    Load(fname);
}

VSP::~VSP()
{
    Free();
}

bool VSP::Load(const char *fname)
{
    File f;
    int nTiles=0;
    
    Free(); // nuke any existing VSP data
    
    log("Loading VSP, %s",fname);
    // TO DO: Stick a little dealy in here for loading up a V16, if there is one
    if (!f.OpenRead(fname))
    {
        log("Error opening %s",fname);
        return false;
    }
    
    strcpy(sName,fname);
    
    u16 ver;
    f.Read(&ver,2);
    switch(ver)
    {
    case 2: 
        {
            nTilex=16;
            nTiley=16;
            
            u8 pal[768];
            f.Read(&pal,768);
            f.Read(&nTiles,2);
            
            u8* pData8=new u8[nTiles*256];
            f.Read(pData8,nTiles*256);
            
            CreateTilesFromBuffer(pData8,pal,nTiles,nTilex,nTiley);
            
            delete[] pData8;
            break;
        }
        
    case 3: 
        {
            nTilex=16;
            nTiley=16;
            
            u8 pal[768];
            u32 bufsize;
            
            f.Read(&pal,768);
            f.Read(&nTiles,2);
            f.Read(&bufsize,4);
            
            u8* pBuffer=new u8[bufsize];
            u8* pData8=new u8[nTiles*256];
            f.Read(pBuffer,bufsize);
            ReadCompressedLayer1(pData8,nTiles*256,pBuffer);
            
            CreateTilesFromBuffer(pData8,pal,nTiles,nTilex,nTiley);
            
            delete[] pBuffer;
            delete[] pData8;
            break;
        }
        
    case 4:
        {
            nTilex=16; nTiley=16;
            
            f.Read(&nTiles,2);
            
            u16* pData16=new u16[nTiles*nTilex*nTiley];
            
            f.Read(pData16,nTiles*nTilex*nTiley*2); // the VSP is 2bpp
            
            CreateTilesFromBuffer(pData16,nTiles,nTilex,nTiley);
            
            delete[] pData16;
            break;
        }
        
    case 5:
        {
            u32 bufsize;
            
            nTilex=16; nTiley=16;
            f.Read(&nTiles,2);
            f.Read(&bufsize,4);
            
            u8* pBuffer=new u8[bufsize];
            u16* pData16=new u16[nTiles*nTilex*nTiley];
            
            f.Read(pBuffer,bufsize);
            ReadCompressedLayer2(pData16,nTiles*nTilex*nTiley,(u16*)pBuffer);
            
            CreateTilesFromBuffer(pData16,nTiles,nTilex,nTiley);
            
            delete[] pBuffer;            
            delete[] pData16;
            break;
        }
    case 6:													// woo, the badass vsp format
        {
            z_stream stream;
            u8 nMaskcolour;
            
            u8 bpp;
            u8 pal[768];
            
            f.Read(bpp);
            
            f.Read(&nTilex,2);
            f.Read(&nTiley,2);
            f.Read(&nTiles,4);
            
            f.Read(sDesc,64);
            
            if (bpp==1)
            {
                f.Read(pal,768);
                f.Read(&nMaskcolour,1);
            }
            
            int nDatasize=nTilex*nTiley*nTiles*bpp;
            int nCompressedblocksize;
            f.Read(&nCompressedblocksize,4);
            
            u8* pBuffer=new u8[nCompressedblocksize];
            u8* pData=new u8[nDatasize];
            
            f.Read(pBuffer,nCompressedblocksize);
            
            stream.next_in=(Bytef*)pBuffer;
            stream.avail_in=nCompressedblocksize;
            stream.next_out=(Bytef*)pData;
            stream.avail_out=nDatasize;
            stream.data_type=Z_BINARY;
            
            stream.zalloc=NULL;
            stream.zfree=NULL;
            
            inflateInit(&stream);
            inflate(&stream,Z_SYNC_FLUSH);
            inflateEnd(&stream);
            
            if (bpp==1)
                CreateTilesFromBuffer(pData,pal,nTiles,nTilex,nTiley);
            else
                CreateTilesFromBuffer((RGBA*)pData,nTiles,nTilex,nTiley);
            
            delete[] pBuffer;
            delete[] pData;
            break;
        }
    default: 
        log("Fatal error: unknown VSP version (%d)",ver);
        return false;
    }
    
    vspanim.clear();
    vspanim.resize(100);    // ugh
    for (int j=0; j<100; j++)
    {
        f.Read(&vspanim[j].nStart,2);
        f.Read(&vspanim[j].nFinish,2);
        f.Read(&vspanim[j].nDelay,2);
        f.Read(&vspanim[j].mode,2);
    }
    
    f.Close();
    
    logok();
    
    return true;
}

int VSP::Save(const char* fname)
{
    File f;
    int i;
    u8 *cb;
    
    if (!f.OpenWrite(fname))
    {
        log("Error writing to %s",fname);
        return 0;
    }
    
    RGBA* pTemp=new RGBA[nTilex*nTiley*tiles.size()];
    
    // copy all the tile data into one big long buffer that we can write to disk
    for (int j=0; j<tiles.size(); j++)
        memcpy(pTemp+(j*nTilex*nTiley),tiles[j].GetPixelData(),nTilex*nTiley*sizeof(RGBA));
    
    const char bpp=4;
    
    i=6;
    f.Write(&i,2);
    f.Write(&bpp,1);
    f.Write(&nTilex,2);
    f.Write(&nTiley,2);
    f.Write((int)tiles.size());
    
    f.Write(sDesc,64);			// description. (authoring info, whatever)
    
    z_stream stream;
    int nDatasize=tiles.size()*nTilex*nTiley*bpp;
    
    cb=new u8[(nDatasize*11)/10+12];
    
    stream.next_in=(Bytef*)pTemp;
    stream.avail_in=nDatasize;
    stream.next_out=(Bytef*)cb;
    stream.avail_out=(nDatasize*11)/10+12;	// +10% and 12 u8s
    stream.data_type=Z_BINARY;
    
    stream.zalloc=NULL;
    stream.zfree=NULL;
				
    deflateInit(&stream,Z_DEFAULT_COMPRESSION);
    deflate(&stream,Z_SYNC_FLUSH);
    deflateEnd(&stream);
    
    f.Write(&stream.total_out,4);
    f.Write(cb,stream.total_out);
    
    delete[] cb;
    delete[] pTemp;
    
    f.Write(&vspanim,sizeof vspanim);
    f.Close();
    
    return true;
}

void VSP::Free()
{
    tiles.clear();
}

void VSP::New(int xsize,int ysize)	// creates a blank 100 tile, 32 bit VSP, of the specified size
{
    Free();
    nTilex=xsize>0?xsize:1;
    nTiley=ysize>0?ysize:1;
    
    tiles.resize(100);
    
    for (int i=0; i<100; i++)
        tiles[i].Resize(nTilex,nTiley);
}

// vsp alteration routines

void VSP::InsertTile(int pos)
{
    if (pos<0 || pos>=tiles.size())
        return;
    
    tiles.push_back(tiles[tiles.size()-1]); // tack the last tile on the end
    
    for (int i=pos; i<tiles.size()-1; i++)
        tiles[i+1]=tiles[i];                // bump 'em all over
}

void VSP::DeleteTile(int pos)
{
    if (pos<0 || pos>=tiles.size())
        return;
    
    for (int i=pos; i<tiles.size()-1; i++)
        tiles[i]=tiles[i+1];                // shuffle them all down one
    
    tiles.pop_back();
}

void VSP::AppendTiles(int count)
{
    CPixelMatrix dummy(nTilex,nTiley);
    
    for (int i=0; i<count; i++)
        tiles.push_back(dummy);
}

void VSP::CopyTile(CPixelMatrix& tb,int pos)
{
    if (pos<0 || pos>=tiles.size())
        return;
    
    tb=tiles[pos];
}

void VSP::PasteTile(const CPixelMatrix& tb,int pos)
{
    if (pos<0 || pos>=tiles.size())
        return;
    
    tiles[pos]=tb;
}

void VSP::TPasteTile(CPixelMatrix& tb,int pos)
{
    CBlitter<Alpha>::Blit(tb,tiles[pos],0,0);
    // NYI
}

VSP::AnimState& VSP::Anim(int strand)
{
    static AnimState dummy;
    
    if (strand<0 || strand>99) return dummy;
    
    return vspanim[strand];
}

CPixelMatrix& VSP::GetTile(int tileidx)
{
    static CPixelMatrix dummy;
    
    if (tileidx<0 || tileidx>tiles.size())
    {
        return dummy;
    }
    
    return tiles[tileidx];
}

void VSP::CreateTilesFromBuffer(u8* data,u8* pal,int numtiles,int tilex,int tiley)
{
    nTilex=tilex;
    nTiley=tiley;
    
    tiles.resize(numtiles);
    
    for (int i=0; i<numtiles; i++)
    {
        tiles[i].Resize(nTilex,nTiley);       
        tiles[i].CopyPixelData(data,nTilex,nTiley,pal);
        data+=nTilex*nTiley;
    }
}

void VSP::CreateTilesFromBuffer(u16* data,int numtiles,int tilex,int tiley)
{
    nTilex=tilex;
    nTiley=tiley;
    
    tiles.resize(numtiles);
    
    RGBA* pBuffer=new RGBA[nTilex*nTiley];
    u16* pSrc=data;
    
    for (int i=0; i<numtiles; i++)
    {
        tiles[i].Resize(nTilex,nTiley);
        
        for (int y=0; y<nTiley; y++)
            for (int x=0; x<nTilex; x++)
                pBuffer[y*nTilex+x]=RGBA(pSrc[y*nTilex+x]);
            
            pSrc+=nTilex*nTiley;
            
            tiles[i].CopyPixelData(pBuffer,nTilex,nTiley);
    }
    
    delete[] pBuffer;
}

void VSP::CreateTilesFromBuffer(RGBA* data,int numtiles,int tilex,int tiley)
{
    nTilex=tilex;
    nTiley=tiley;
    
    tiles.resize(numtiles);
    
    for (int i=0; i<numtiles; i++)
    {
        tiles[i].Resize(nTilex,nTiley);
        tiles[i].CopyPixelData(data+i*nTilex*nTiley,nTilex,nTiley);
    }
}