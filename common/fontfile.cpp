#include "fontfile.h"
#include "fileio.h"
#include "rle.h"
#include "vergepal.h"

CFontFile::~CFontFile()
{
    glyph.clear();
}

bool CFontFile::Load8bppFont(File& f)
{
    short int nWidth,nHeight,subsetcount;
    f.Read(&nWidth,2);
    f.Read(&nHeight,2);
    f.Read(&subsetcount,2);
    const int nGlyphs=subsetcount*96;
    
    u8* buffer8=new u8[nWidth*nHeight*nGlyphs];
    RGBA* pPixels=new RGBA[nWidth*nHeight*nGlyphs];
    
    f.Read(buffer8,nWidth*nHeight*nGlyphs*sizeof(u8));
    
    // convert 8bpp to 32bpp
    
    for (int i=0; i<nWidth*nHeight*nGlyphs; i++)
    {
        char c=buffer8[i];
        
        pPixels[i]=RGBA(c,cVergepal);
    }
    
    glyph.resize(nGlyphs);
    RGBA* p=pPixels;
    
    for (int nGlyph=0; nGlyph<nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p,nWidth,nHeight);
        p+=nWidth*nHeight;
    }
    
    set.resize(subsetcount);						// not very readable :(  Allocates the needed number of subsets.	
    for (int nCurset=0; nCurset<subsetcount; nCurset++)
    {
        memset(set[nCurset].nGlyphtbl,0,256*sizeof(int));
        for (int j=0; j<96; j++)
            set[nCurset].nGlyphtbl[j+32]=j+(nCurset*96);
    }

    // blegh
    this->nWidth=nWidth;
    this->nHeight=nHeight;
    
    delete[] buffer8;
    delete[] pPixels;
    
    return true;
}

bool CFontFile::Load16bppFont(File& f)
{
    short int nWidth,nHeight,subsetcount;
    f.Read(&nWidth,2);
    f.Read(&nHeight,2);
    f.Read(&subsetcount,2);
    const int nGlyphs=subsetcount*96;				// magic number, due to old font format restrictions.
    
    u16* buffer16=new u16[nWidth*nHeight*nGlyphs];
    RGBA* pPixels=new RGBA[nWidth*nHeight*nGlyphs];
    
    f.Read(buffer16,nWidth*nHeight*nGlyphs*sizeof(u16));
    
    // now we have uncompressed 16 bit font stuff.  Convert it to 32 bits.
    
    for (int i=0; i<nWidth*nHeight*nGlyphs; i++)
    {
        u16 c=buffer16[i];
        
        pPixels[i]=RGBA(
            (c&31)<<3,
            ((c>>5)<<3)&0xFF,
            ((c>>10)<<3)&0xFF,
            c?255:0);
    }
    
    glyph.resize(nGlyphs);
    
    RGBA* p=pPixels;
    
    for (int nGlyph=0; nGlyph<nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p,nWidth,nHeight);
        p+=nWidth*nHeight;
    }
    
    set.resize(subsetcount);						// not very readable :(  Allocates the needed number of subsets.	
    for (int nCurset=0; nCurset<subsetcount; nCurset++)
    {
        memset(set[nCurset].nGlyphtbl,0,256*sizeof(int));
        for (int j=0; j<96; j++)
            set[nCurset].nGlyphtbl[j+32]=j+(nCurset*96);
    }

    this->nWidth=nWidth;
    this->nHeight=nHeight;
    
    delete[] buffer16;
    delete[] pPixels;
    
    return true;
}

bool CFontFile::Load32bppFont(File& f)
{
    f.Seek(0);
    
    char sSig[6];
    f.Read(sSig,6);
    if (memcmp(sSig,"FONT27",6)!=0)
        return false;
    
    char nSubsets;
    u16  nGlyphs;
    
    f.Read(nSubsets);
    f.Read(nGlyphs);
    
    set.resize(nSubsets);
    for (int nSet=0; nSet<nSubsets; nSet++)
    {
        u16 i[256];
        f.Read(i,256*sizeof(u16));
        
        for (int j=0; j<256; j++)
            set[nSet].nGlyphtbl[j]=i[j];
    }
    
    std::vector<int>	nGlyphwidth;
    std::vector<int>	nGlyphheight;
    int nGlyphdatasize=0;

    nWidth=nHeight=0;
    
    for (int i=0; i<nGlyphs; i++)
    {
        u16 w,h;
        f.Read(w);	f.Read(h);
        nGlyphwidth.push_back(w);
        nGlyphheight.push_back(h);
        nGlyphdatasize+=w*h;

        if (nWidth<w) nWidth=w;
        if (nHeight<h) nHeight=h;
    }
    
    RGBA* pBuffer=new RGBA[nGlyphdatasize];
    f.ReadCompressed(pBuffer,nGlyphdatasize*sizeof(RGBA));
    
    glyph.resize(nGlyphs);
    RGBA* p=pBuffer;
    for (int nGlyph=0; nGlyph<nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p,nGlyphwidth[nGlyph],nGlyphheight[nGlyph]);
        p+=nGlyphwidth[nGlyph]*nGlyphheight[nGlyph];
    }
    
    f.Close();
    return true;
}

bool CFontFile::Load(const char* fname)
{
    File f;
    bool bResult;
    
    bResult=f.OpenRead(fname);
    if (!bResult)
        return false;
    
    char magic;
    f.Read(magic);
    
    switch (magic)
    {
    case 1:		bResult=Load8bppFont(f);		break;
    case 2:		bResult=Load16bppFont(f);		break;
    case 'F':	bResult=Load32bppFont(f);		break;
    default:	bResult=false;
    };
    
    return bResult;	
}

void CFontFile::Save(const char* fname)
{
}
