// VSP.cpp
#include <stdio.h>
#include "vsp.h"
#include "types.h"
#include "rle.h"
#include "fileio.h"
#include "log.h"
#include "zlib.h"

VSP::VSP() : _width(0), _height(0)
{
    vspanim.resize(100);
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
    int nTiles = 0;
    
    Free(); // nuke any existing VSP data
    
    if (!f.OpenRead(fname))
    {
        Log::Write("Error opening %s", fname);
        return false;
    }
    
    strcpy(name, fname);
    
    u16 ver;
    f.Read(&ver, 2);
    switch(ver)
    {
    case 2: 
        {
            _width = 16;
            _height = 16;
            
            u8 pal[768];
            f.Read(&pal, 768);
            f.Read(&nTiles, 2);
            
            u8* pData8 = new u8[nTiles * 256];
            f.Read(pData8, nTiles * 256);
            
            CreateTilesFromBuffer(pData8, pal, nTiles, _width, _height);
            
            delete[] pData8;
            break;
        }
        
    case 3: 
        {
            _width = 16;
            _height = 16;
            
            u8 pal[768];
            u32 bufsize;
            
            f.Read(&pal, 768);
            f.Read(&nTiles, 2);
            f.Read(&bufsize, 4);
            
            u8* pBuffer = new u8[bufsize];
            u8* pData8 = new u8[nTiles * 256];
            f.Read(pBuffer, bufsize);
            ReadCompressedLayer1(pData8, nTiles * 256, pBuffer);
            
            CreateTilesFromBuffer(pData8, pal, nTiles, _width, _height);
            
            delete[] pBuffer;
            delete[] pData8;
            break;
        }
        
    case 4:
        {
            _width = 16; _height = 16;
            
            f.Read(&nTiles, 2);
            
            u16* pData16 = new u16[nTiles * _width * _height];
            
            f.Read(pData16, nTiles * _width * _height * 2); // the VSP is 2bpp
            
            CreateTilesFromBuffer(pData16, nTiles, _width, _height);
            
            delete[] pData16;
            break;
        }
        
    case 5:
        {
            u32 bufsize;
            
            _width = 16; _height = 16;
            f.Read(&nTiles, 2);
            f.Read(&bufsize, 4);
            
            u8* pBuffer = new u8[bufsize];
            u16* pData16 = new u16[nTiles * _width * _height];
            
            f.Read(pBuffer, bufsize);
            ReadCompressedLayer2(pData16, nTiles * _width * _height, (u16*)pBuffer);
            
            CreateTilesFromBuffer(pData16, nTiles, _width, _height);
            
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
            
            f.Read(&_width, 2);
            f.Read(&_height, 2);
            f.Read(&nTiles, 4);
            
            f.Read(sDesc, 64);
            
            if (bpp==1)
            {
                f.Read(pal, 768);
                f.Read(&nMaskcolour, 1);
            }
            
            int nDatasize = _width * _height * nTiles * bpp;
            int nCompressedblocksize;
            f.Read(&nCompressedblocksize, 4);
            
            u8* pBuffer = new u8[nCompressedblocksize];
            u8* pData = new u8[nDatasize];
            
            f.Read(pBuffer, nCompressedblocksize);
            
            stream.next_in=(Bytef*)pBuffer;
            stream.avail_in = nCompressedblocksize;
            stream.next_out=(Bytef*)pData;
            stream.avail_out = nDatasize;
            stream.data_type = Z_BINARY;
            
            stream.zalloc = NULL;
            stream.zfree = NULL;
            
            inflateInit(&stream);
            inflate(&stream, Z_SYNC_FLUSH);
            inflateEnd(&stream);
            
            if (bpp==1)
                CreateTilesFromBuffer(pData, pal, nTiles, _width, _height);
            else
                CreateTilesFromBuffer((RGBA*)pData, nTiles, _width, _height);

            delete[] pBuffer;
            delete[] pData;
            break;
        }
    default: 
        Log::Write("Fatal error: unknown VSP version (%d)", ver);
        return false;
    }
    
    for (int j = 0; j < 100; j++)
    {
        f.Read(&vspanim[j].nStart, 2);
        f.Read(&vspanim[j].nFinish, 2);
        f.Read(&vspanim[j].nDelay, 2);
        f.Read(&vspanim[j].mode, 2);
    }
    
    f.Close();
    
    return true;
}

int VSP::Save(const char* fname)
{
    File f;
    int i;
    u8 *cb;
    
    if (!f.OpenWrite(fname))
    {
        Log::Write("Error writing to %s", fname);
        return 0;
    }
    
    RGBA* pTemp = new RGBA[_width * _height * tiles.size()];
    
    // copy all the tile data into one big long buffer that we can write to disk
    for (unsigned int j = 0; j < tiles.size(); j++)
        memcpy(pTemp+(j * _width * _height), tiles[j].GetPixels(), _width * _height * sizeof(RGBA));
    
    const char bpp = 4;
    
    i = 6;
    f.Write(&i, 2);
    f.Write(&bpp, 1);
    f.Write(&_width, 2);
    f.Write(&_height, 2);
    f.Write((int)tiles.size());
    
    f.Write(sDesc, 64);			// description. (authoring info, whatever)
    
    z_stream stream;
    int nDatasize = tiles.size() * _width * _height * bpp;
    
    cb = new u8[(nDatasize * 11)/10 + 12];
    
    stream.next_in=(Bytef*)pTemp;
    stream.avail_in = nDatasize;
    stream.next_out=(Bytef*)cb;
    stream.avail_out=(nDatasize * 11)/10 + 12;	// +10% and 12 u8s
    stream.data_type = Z_BINARY;
    
    stream.zalloc = NULL;
    stream.zfree = NULL;
				
    deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    deflate(&stream, Z_SYNC_FLUSH);
    deflateEnd(&stream);
    
    f.Write(&stream.total_out, 4);
    f.Write(cb, stream.total_out);
    
    delete[] cb;
    delete[] pTemp;
    
    for (int k = 0; k < 100; k++)
    {
        f.Write(&vspanim[k].nStart, 2);
        f.Write(&vspanim[k].nFinish, 2);
        f.Write(&vspanim[k].nDelay, 2);
        f.Write(&vspanim[k].mode, 2);
    }

    f.Close();
    
    return true;
}

void VSP::Free()
{
    tiles.clear();
}

void VSP::New(int xsize, int ysize, int numtiles)     // creates a blank 32 bit VSP, of the specified size and number of tiles
{
    Free();
    _width = xsize > 0?xsize:1;
    _height = ysize > 0?ysize:1;
    
    tiles.resize(numtiles);
    
    for (int i = 0; i < numtiles; i++)
        tiles[i].Resize(_width, _height);
}

// vsp alteration routines

void VSP::InsertTile(uint pos)
{
    if (pos < 0 || pos >= tiles.size())
        return;

    tiles.insert(tiles.begin() + pos, Canvas(_width, _height));
}

void VSP::DeleteTile(uint pos)
{
    if (pos < 0 || pos >= tiles.size())
        return;
    
    tiles.erase(tiles.begin() + pos);
}

void VSP::AppendTiles(uint count)
{
    Canvas dummy(_width, _height);
    
    for (uint i = 0; i < count; i++)
        tiles.push_back(dummy);
}

void VSP::CopyTile(Canvas& tb, uint pos)
{
    if (pos < 0 || pos >= tiles.size())
        return;
    
    tb = tiles[pos];
}

void VSP::PasteTile(const Canvas& tb, uint pos)
{
    if (pos < 0 || pos >= tiles.size())
        return;
    
    tiles[pos]=tb;
}

void VSP::TPasteTile(Canvas& tb, uint pos)
{
    CBlitter < Alpha>::Blit(tb, tiles[pos], 0, 0);
    // NYI
}

VSP::AnimState& VSP::Anim(uint strand)
{
    static AnimState dummy;
    
    if (strand < 0 || strand > 99) return dummy;
    
    return vspanim[strand];
}

Canvas& VSP::GetTile(uint tileidx)
{
    if (tileidx < 0 || tileidx > tiles.size())
        tileidx = 0;
    
    return tiles[tileidx];
}

void VSP::CreateTilesFromBuffer(u8* data, u8* pal, uint numtiles, int tilex, int tiley)
{
    _width = tilex;
    _height = tiley;
    
    tiles.resize(numtiles);
    
    for (uint i = 0; i < numtiles; i++)
    {
        tiles[i].Resize(_width, _height);       
        tiles[i].CopyPixelData(data, _width, _height, pal);
        data += _width * _height;
    }
}

void VSP::CreateTilesFromBuffer(u16* data, uint numtiles, int tilex, int tiley)
{
    _width = tilex;
    _height = tiley;
    
    tiles.resize(numtiles);
    
    RGBA* pBuffer = new RGBA[_width * _height];
    u16* pSrc = data;
    
    for (uint i = 0; i < numtiles; i++)
    {
        tiles[i].Resize(_width, _height);
        
        for (int y = 0; y < _height; y++)
            for (int x = 0; x < _width; x++)
                pBuffer[y * _width + x]=RGBA(pSrc[y * _width + x]);
            
            pSrc += _width * _height;
            
            tiles[i].CopyPixelData(pBuffer, _width, _height);
    }
    
    delete[] pBuffer;
}

void VSP::CreateTilesFromBuffer(RGBA* data, uint numtiles, int tilex, int tiley)
{
    _width = tilex;
    _height = tiley;
    
    tiles.resize(numtiles);
    
    for (uint i = 0; i < numtiles; i++)
    {
        tiles[i].Resize(_width, _height);
        tiles[i].CopyPixelData(data + i * _width * _height, _width, _height);
    }
}
