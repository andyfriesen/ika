// VSP.cpp
#include <stdio.h>
#include "VSP.h"
#include "types.h"
#include "rle.h"
#include "fileio.h"
#include "log.h"
#include "zlib.h"

TileBuffer::TileBuffer()
{
    width=0; height=0; bpp=0; data=0;
}

TileBuffer::~TileBuffer()
{
    if (data!=NULL)
    {
	delete[] data;
	data=NULL;
    }
    width=0; height=0; bpp=0; data=0;
}

TileBuffer::TileBuffer(const TileBuffer& tb)
{
    width=tb.width; height=tb.height;
    bpp=tb.bpp;
    
    if(bpp < 0 || bpp > 4)
	return;
    
    data=new unsigned char[width*height*bpp];
    memcpy(data,tb.data,width*height*bpp);
}

void TileBuffer::CopyTo(TileBuffer& tb) const
{
    if(!data || &tb == NULL)
	return;
    
    if(data == tb.data)
	return;
    
    if (tb.data!=NULL) 
	delete[] tb.data;
    
    tb.width=width; tb.height=height;
    tb.bpp=bpp;
    
    if(bpp < 0 || bpp > 4)
	return;
    
    tb.data=new u8[width*height*bpp];
    memcpy(tb.data,data,width*height*bpp);
    
    if (bpp==1)
	memcpy(tb.pal,pal,768);
}

bool TileBuffer::CompareBuffers(TileBuffer t)
{
    int c;
    
    if(&t == NULL)
	return false;
    
    c = memcmp(data, t.data, width*height*bpp);
    
    if(c)
	return false;
    else if (!c)
	return true;
    
    return false;
}

void TileBuffer::SetDot(int x,int y,u32 colour)
{
    if (x<0 || x>=width) return;
    if (y<0 || y>=height) return;
    if (!data) return;
    
    if (bpp==1)
	data[y*width+x]=(u8)(colour&255);
    else if (bpp==4)
	((u32*)data)[y*width+x]=colour;
}

u32 TileBuffer::Pixel(int x,int y)
{
    if (data==NULL) return 0;
    if (x<0 || x>=width) return 0;
    if (y<0 || y>=width) return 0;
    
    switch(bpp)
    {
    case 1: return data[y*width+x];
    case 4: return ((u32*)data)[y*width+x];
    }
    return 0;
}

void TileBuffer::Mirror()
// flips the tile left to right
{
    u8* tempdata;
    tempdata=new u8[width*height*bpp];
    memcpy(tempdata,data,width*height*bpp);
    
    for (int y=0; y<height; y++)
	for (int x=0; x<width; x++)
	{
	    if (bpp==1)
		SetDot(width-x-1,y,tempdata[y*width+x]);
	    else
		SetDot(width-x-1,y,((u32*)tempdata)[y*width+x]);
	}
	
	
	delete[] tempdata;
}

void TileBuffer::Flip()
// flips the tile top to bottom
{
    u8 *tempdata;
    tempdata=new u8[width*height*bpp];
    memcpy(tempdata,data,width*height*bpp);
    
    for (int y=0; y<height; y++)
	for (int x=0; x<width; x++)
	{
	    if (bpp==1)
		SetDot(x,height-y-1,tempdata[y*width+x]);
	    else
		SetDot(x,height-y-1,((u32*)tempdata)[y*width+x]);
	}
	
	delete[] tempdata;
}

void TileBuffer::Rotate()
// rotates the tile 90 degrees
{
    u8 *tempdata;
    tempdata=new u8[width*height*bpp];
    memcpy(tempdata,data,width*height*bpp);
    
    for (int y=0; y<height; y++)
	for (int x=0; x<width; x++)
	{
	    if (bpp==1)
		SetDot(y,x,tempdata[y*width+x]);
	    else
		SetDot(y,x,((u32*)tempdata)[y*width+x]);
	}
	
	delete[] tempdata;
}


void TileBuffer::FillRect(int t, int l, int b, int r, u32 color)
{
    int x, y;
    
    for(y = t;y < b;y++)
    {
	for(x = l;x < r;x++)
	{
	    SetDot(x, y, color);
	}
    }
}

/*
void TileBuffer::FloodFill(int x,int y,u32 colour)
{
Original code by tSB 
// Recursive solution
u32 c=Pixel(x,y);

  SetDot(x,y,colour);
  
    if (Pixel(x-1,y)==c)
    FloodFill(x-1,y,colour);
    if (Pixel(x+1,y)==c)
    FloodFill(x+1,y,colour);
    if (Pixel(x,y-1)==c)
    FloodFill(x,y-1,colour);
    if (Pixel(x,y+1)==c)
    FloodFill(x,y+1,colour);
    }
*/

VSP::VSP()
{
    data=0;
    numtiles=0;
    New();
}

VSP::VSP(const char* fname)
{
	data=0;
	numtiles=0;
	Load(fname);
}

VSP::~VSP()
{
    Free();
}

int VSP::Load(const char *fname)
{
    File f;
    u16	ver;
    u8	*cb;		// char buffer
    u16	*wb;		// word buffer
    int	bufsize;	// size of compression buffer
    int	r,g,b;		// for remixing colours
    int	i;			// loop counter 
    
    Free(); // nuke any existing VSP data
    
    log("Loading VSP, %s",fname);
    // TO DO: Stick a little dealy in here for loading up a V16, if there is one
    if (!f.OpenRead(fname))
    {
	log("Error opening %s",fname);
	return 0;
    }
    
    strcpy(sName,fname);
    
    f.Read(&ver,2);
    switch(ver)
    {
    case 2: 
	bpp=1;
	nMaskcolour=0;
	tilex=16; tiley=16;
	
	f.Read(&pal,768);
	f.Read(&numtiles,2);
	data=new u8[numtiles*256];
	f.Read(data,numtiles*256);
	break;
	
    case 3: 
	bpp=1;
	nMaskcolour=0;
	tilex=16; tiley=16;
	
	f.Read(&pal,768);
	f.Read(&numtiles,2);
	f.Read(&bufsize,4);
	
	cb=new u8[bufsize];
	data=new u8[numtiles*256];
	f.Read(cb,bufsize);
	ReadCompressedLayer1(data,numtiles*256,cb);
	delete[] cb;
	break;
	
    case 4: 
	bpp=4;
	tilex=16; tiley=16;
	
	f.Read(&numtiles,2);
	
	data=new u8[numtiles*tilex*tiley*bpp];
	wb=new u16[numtiles*tilex*tiley];
	
	f.Read(wb,numtiles*tilex*tiley*2); // the VSP is 2bpp
	
	u8* tmp;
	tmp=data;
	
	for (i=0; i<numtiles*tilex*tiley; i++)
	{
	    r=(wb[i]>>10)&31;
	    g=(wb[i]>>5)&31;
	    b=wb[i]&31;
	    
	    data[i*bpp  ]=r<<3;
	    data[i*bpp+1]=g<<3;
	    data[i*bpp+2]=b<<3;
	    data[i*bpp+3]=(r||g||b)?255:0; // alpha
	}
	delete[] wb;           
	break;
	
    case 5: 
	bpp=4;
	tilex=16; tiley=16;
	f.Read(&numtiles,2);
	f.Read(&bufsize,4);
	cb=new u8[bufsize];
	wb=new u16[numtiles*tilex*tiley];
	
	f.Read(cb,bufsize);
	ReadCompressedLayer2(wb,numtiles*tilex*tiley,(u16 *)cb);
	delete[] cb;
	
	data=new u8[numtiles*tilex*tiley*bpp];
	for (i=0; i<numtiles*tilex*tiley; i++)
	{
	    r=(wb[i]>>11)&31;
	    g=(wb[i]>>5)&63;
	    b=wb[i]&31;             
	    data[i*bpp  ]=r<<3;
	    data[i*bpp+1]=g<<2;
	    data[i*bpp+2]=b<<3;
	    data[i*bpp+3]=(r||g||b)?255:0; // alpha
	}
	delete[] wb;
	break;
	
    case 6:													// woo, the badass vsp format
	{
	    z_stream stream;
	    u8 nMaskcolour;
	    
	    bpp=0;
	    f.Read(&bpp,1);
	    
	    f.Read(&tilex,2);
	    f.Read(&tiley,2);
	    f.Read(&numtiles,4);
	    
	    f.Read(sDesc,64);
	    
	    if (bpp==1)
	    {
		f.Read(pal,768);
		f.Read(&nMaskcolour,1);
	    }
	    
	    int nDatasize=tilex*tiley*numtiles*bpp;
	    int nCompressedblocksize;
	    f.Read(&nCompressedblocksize,4);
	    
	    char* cb=new char[nCompressedblocksize];
	    data=new u8[nDatasize];
	    
	    f.Read(cb,nCompressedblocksize);
	    
	    stream.next_in=(Bytef*)cb;
	    stream.avail_in=nCompressedblocksize;
	    stream.next_out=(Bytef*)data;
	    stream.avail_out=nDatasize;
	    stream.data_type=Z_BINARY;
	    
	    stream.zalloc=NULL;
	    stream.zfree=NULL;
	    
	    inflateInit(&stream);
	    inflate(&stream,Z_SYNC_FLUSH);
	    inflateEnd(&stream);
	    
	    delete[] cb;
	}
	break;
    default: 
	log("Fatal error: unknown VSP version (%d)",ver);
	return 0;
	}
	
	for (int j=0; j<100; j++)
	{
	    f.Read(vspanim[j].start);
	    f.Read(vspanim[j].finish);
	    f.Read(vspanim[j].delay);
	    f.Read(vspanim[j].mode);
	}
	
	f.Close();
	
	logok();
	return 1;
}

// This saves old-style VSPs if the tiles are exactly 16x16 in size, so that winmaped is still useful for editing v2 maps.
int VSP::SaveOld(const char *fname)
{
    File f;
    int i;
    int bufsize;
    u8 *cb;
    u16 *buf,*buf2;
    
    if (!f.OpenWrite(fname))
    {
	log("Error writing to %s",fname);
	return 0;
    }
    
    if (bpp==1) // maybe I should split this up into two separate functions... nah!
    {
	if (tilex!=16 || tiley!=16)	return 0;
	// this file format can only save 16x16x8bpp tiles
	
	i=3;
	f.Write(&i,2);
	f.Write(&pal,768);
	f.Write(&numtiles,2);
	cb=new u8[numtiles*tilex*tiley];
	WriteCompressedLayer1(cb,numtiles*tilex*tiley,bufsize,data);
	f.Write(&bufsize,4);
	f.Write(cb,bufsize);
	delete[] cb;
	f.Write(&vspanim,sizeof vspanim);
	f.Close();
	return 1;
    }
    else
    {
	if (tilex!=16 || tiley!=16) return 0;
	// this file format can only save 16x16x16bpp tiles
	
	i=5;
	f.Write(&i,2);
	f.Write(&numtiles,2);
	buf=new u16[numtiles*tilex*tiley];
	// compress the 32bit data into 16 bit 565
	for (i=0; i<numtiles*tilex*tiley; i++)
	{
	    buf[i] = data[i*bpp+2]>>3;                // Blue
	    buf[i]|=(data[i*bpp+1]>>2)<<5;            // Green
	    buf[i]|=(data[i*bpp  ]>>3)<<11;           // red
	}
	buf2=new u16[numtiles*tilex*tiley];
	WriteCompressedLayer2(buf2,numtiles*tilex*tiley,bufsize,buf);
	
	f.Write(&bufsize,4);
	f.Write(buf2,bufsize);
	
	delete[] buf;
	delete[] buf2;
	f.Write(&vspanim,sizeof vspanim);
	f.Close();
	return 1;
    }
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
    
    i=6;
    f.Write(&i,2);
    f.Write(&bpp,1);
    f.Write(&tilex,2);
    f.Write(&tiley,2);
    f.Write(&numtiles,4);
    
    f.Write(sDesc,64);			// description. (authoring info, whatever)
    
    if (bpp==1)
    {
	f.Write(pal,768);
	i=0;
	f.Write(&i,1);
    }
    
    z_stream stream;
    int nDatasize=numtiles*tilex*tiley*bpp;
    
    cb=new u8[nDatasize];
    
    stream.next_in=(Bytef*)data;
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
    
    f.Write(&vspanim,sizeof vspanim);
    f.Close();
    return 1;
}

int VSP::ColourDepth()
{
    return bpp;
}

void VSP::Free()
{
    delete[] data;
    data=NULL;
}

void VSP::New() // creates a blank, 100 tile, 32bpp VSP
{
    Free();
    numtiles=100;
    bpp=4;
    nMaskcolour=0;
    tilex=16; tiley=16;
    data=new u8[numtiles*bpp*tilex*tiley];
    ZeroMemory(data,numtiles*bpp*tilex*tiley);
    
    ZeroMemory(&vspanim,sizeof vspanim);
    ZeroMemory(pal,768);
}

void VSP::New(int xsize,int ysize)	// creates a blank 100 tile, 32 bit VSP, of the specified size
{
    Free();
    numtiles=100;
    bpp=4;
    nMaskcolour=0;
    tilex=xsize>0?xsize:1;
    tiley=ysize>0?ysize:1;
    data=new u8[numtiles*bpp*tilex*tiley];
    ZeroMemory(data,numtiles*bpp*tilex*tiley);
    
    ZeroMemory(&vspanim,sizeof vspanim);
    ZeroMemory(pal,768);
}

int VSP::NumTiles()
{
    return numtiles;
}

// vsp alteration routines

void VSP::InsertTile(int pos)
{
    u8 *src;             // uh... vestigal, I think
    u8 *tmp;             // temp holder for new VSP data
    int  u8spertile=tilex*tiley*bpp;
    src=data;
    
    if (pos<0)
	pos=0;
    if (pos>=numtiles)
	pos=numtiles-1;
    
    // set aside some memory for the expanded VSP
    tmp=new u8[(numtiles+1)*u8spertile];
    ZeroMemory(tmp,(numtiles+1)*u8spertile);
    
    // copy it, bitch!
    memcpy(tmp,src,pos*u8spertile);															// copy the tiles before the insertion point
    if (pos<numtiles-1)
	memcpy(tmp+((pos+1)*u8spertile),src+(pos*u8spertile),(numtiles-pos)*u8spertile);	// copy the tiles after the insertion point (if applicable)
    numtiles++;
    
    // discard the old data
    delete[] src;
    
    // and set the pointer to point to the new (expanded) VSP
    data=tmp;
}

void VSP::DeleteTile(int pos)
{
    u8 *src;
    u8 *tmp;
    int u8spertile=tilex*tiley*bpp;
    src=data;
    
    tmp=new u8[(numtiles-1)*u8spertile];
    
    // Copy it, bitch!
    // First, all the tiles up until the one being deleted.
    memcpy(tmp,src,pos*u8spertile);
    // Then all the ones AFTER the one being deleted.
    memcpy(tmp+((pos)*u8spertile),src+((pos+1)*u8spertile),(numtiles-pos-1)*u8spertile);
    delete[] src;
    numtiles--;
    
    data=tmp;
}

void VSP::AppendTiles(int count)
{
    if (count<1)
	return;
    
    u8* temp=new u8[(numtiles+count)*tilex*tiley*bpp];

    memcpy(temp,data,numtiles*tilex*tiley*bpp);
    memset(temp+(numtiles*tilex*tiley*bpp),0,count*tilex*tiley*bpp);
    numtiles+=count;

    delete[] data;
    data=temp;
}

void VSP::CopyTile(TileBuffer& tb,int pos)
{
    if (bpp==1)
	memcpy(tb.pal,pal,768); // copy the vsp
    
    if (tb.data!=NULL) delete[] tb.data;
    
    tb.width=tilex; tb.height=tiley;
    tb.bpp=bpp;
    tb.data=new u8[tilex*tiley*bpp];
    memcpy(tb.data,data+(tilex*tiley*bpp*pos),tilex*tiley*bpp);
}

void VSP::PasteTile(const TileBuffer& tb,int pos)
{
    if (tb.data==NULL) return;
    
    memcpy(data+(tilex*tiley*bpp*pos),tb.data,tilex*tiley*bpp);
    // ooh, complicated
}

void VSP::TPasteTile(const TileBuffer& tb,int pos)
{
    int i;
    u8 *src;
    u8 *dest;
    u8 c;
    u32 d;
    
    if (tb.data==NULL) return;
    
    src=(u8*)tb.data;
    dest=data+(tilex*tiley*pos*bpp);
    
    if (bpp==1)
    { 
	for (i=0; i<tilex*tiley; i++)
	{
	    c=*src++;
	    if (c)
		*dest=c;
	    dest++;
	}
    }
    else // if (bpp==4)
    {
	for (i=0; i<tilex*tiley; i++)
	{
	    d=*(u32*)src;
	    src+=bpp;
	    if (d)
		*((u32*)dest)=d;
	    
	    dest+=bpp;
	}
    }
}

void VSP::To32bpp()
{
    if (bpp==4)
	return;													// :P
    u32* pTemp=new u32[tilex*tiley*numtiles];
    
    for (int i=0; i<tilex*tiley*numtiles; i++)
    {
	u8 c=data[i];
	if (!c)
	    pTemp[i]=0;
	else
	    pTemp[i]=
	    (255       <<24) |
	    (pal[c*3  ]<<2 ) |
	    (pal[c*3+1]<<10) |
	    (pal[c*3+2]<<18);
    }
    
    delete[] data;
    data=(u8*)pTemp;
    bpp=4;
}

void VSP::SetPixel(int x,int y,int tileidx,int c)
{
    if (x<0 || x>=tilex) return;
    if (y<0 || y>=tiley) return;
    if (tileidx<0 || tileidx>=numtiles) return;
    
    if (bpp==1)
	data[tileidx*tilex*tiley + y*tilex + x]=c;
    else
	((u32*)data)[tileidx*tilex*tiley + y*tilex + x]=c; // :o
}

int  VSP::GetPixel(int x,int y,int tileidx)
{
    if (x<0 || x>=tilex) return 0;
    if (y<0 || y>=tiley) return 0;
    if (tileidx<0 || tileidx>=numtiles) return 0;
    
    if (bpp==1)
	return data[tileidx*tilex*tiley + y*tilex + x]&255;
    else
	return ((u32*)data)[tileidx*tilex*tiley + y*tilex + x];
}

void VSP::GetAnim(vspanim_r& anim,int strand)
{
    if (strand<0 || strand>99) return;
    
    anim=vspanim[strand];
}

void VSP::SetAnim(const vspanim_r& anim,int strand)
{
    if (strand<0 || strand>99) return;
    
    vspanim[strand]=anim;
}