#include "fontfile.h"
#include "fileio.h"
#include "rle.h"
#include "vergepal.h"
#include "utility.h"

FontFile::~FontFile()
{
    glyph.clear();
}

bool FontFile::Load8bppFont(File& f)
{
    short int width, height, subsetcount;
    f.Read(&width, 2);
    f.Read(&height, 2);
    f.Read(&subsetcount, 2);
    const int nGlyphs = subsetcount * 96;
    
    ScopedArray<u8> buffer8(new u8[width * height * nGlyphs]);
    ScopedArray<RGBA> pPixels(new RGBA[width * height * nGlyphs]);
    
    f.Read(buffer8.get(), width * height * nGlyphs * sizeof(u8));
    
    // convert 8bpp to 32bpp
    
    for (int i = 0; i < width * height * nGlyphs; i++)
    {
        char c = buffer8[i];
        
        pPixels[i] = RGBA(c, cVergepal);
    }
    
    glyph.resize(nGlyphs);
    RGBA* p = pPixels.get();
    
    for (int nGlyph = 0; nGlyph < nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p, width, height);
        p += width * height;
    }
    
    set.resize(subsetcount);                                            // not very readable :(  Allocates the needed number of subsets.        
    for (int nCurset = 0; nCurset < subsetcount; nCurset++)
    {
        SSubSet& ss = set[nCurset];

        for (int j = 0; j < 256; j++)
            ss.glyphIndex[j] = 0;

        for (int j = 0; j < 96; j++)
            ss.glyphIndex[j + 32] = j + (nCurset * 96);
    }

    // blegh
    this->width = width;
    this->height = height;
    
    return true;
}

bool FontFile::Load16bppFont(File& f)
{
    short int width, height, subsetcount;
    f.Read(&width, 2);
    f.Read(&height, 2);
    f.Read(&subsetcount, 2);
    const int nGlyphs = subsetcount * 96;                               // magic number, due to old font format restrictions.

    ScopedArray<u16> buffer16(new u16[width * height * nGlyphs]);
    ScopedArray<RGBA> pPixels(new RGBA[width * height * nGlyphs]);
    
    f.Read(buffer16.get(), width * height * nGlyphs * sizeof(u16));
    
    // now we have uncompressed 16 bit font stuff.  Convert it to 32 bits.
    
    for (int i = 0; i < width * height * nGlyphs; i++)
    {
        u16 c = buffer16[i];
        
        pPixels[i] = RGBA(
             (c & 31)  << 3,
            ((c >>  5) << 3) & 0xFF,
            ((c >> 10) << 3) & 0xFF,
            c ? 255 : 0);
    }
    
    glyph.resize(nGlyphs);
    
    RGBA* p = pPixels.get();
    
    for (int nGlyph = 0; nGlyph < nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p, width, height);
        p += width * height;
    }
    
    set.resize(subsetcount);                                            // not very readable :(  Allocates the needed number of subsets.        
    for (int nCurset = 0; nCurset < subsetcount; nCurset++)
    {
        SSubSet& ss = set[nCurset];

        for (int j = 0; j < 256; j++)
            ss.glyphIndex[j] = 0;

        for (int j = 0; j < 96; j++)
            ss.glyphIndex[j + 32] = j + (nCurset * 96);
    }

    this->width = width;
    this->height = height;
    
    return true;
}

bool FontFile::Load32bppFont(File& f)
{
    f.Seek(0);
    
    char sSig[6];
    f.Read(sSig, 6);
    if (memcmp(sSig, "FONT27", 6)!=0)
        return false;
    
    char nSubsets;
    u16  nGlyphs;
    
    f.Read(nSubsets);
    f.Read(nGlyphs);
    
    set.resize(nSubsets);
    for (int nSet = 0; nSet < nSubsets; nSet++)
    {
        u16 i[256];
        f.Read(i, 256 * sizeof(u16));
        
        for (int j = 0; j < 256; j++)
            set[nSet].glyphIndex[j]=i[j];
    }
    
    std::vector<int>    nGlyphwidth;
    std::vector<int>    nGlyphheight;
    int nGlyphdatasize = 0;

    width = height = 0;
    
    for (int i = 0; i < nGlyphs; i++)
    {
        u16 w, h;
        f.Read(w);      f.Read(h);
        nGlyphwidth.push_back(w);
        nGlyphheight.push_back(h);
        nGlyphdatasize += w * h;

        if (width < w) width = w;
        if (height < h) height = h;
    }
    
    ScopedArray<RGBA> pBuffer(new RGBA[nGlyphdatasize]);
    f.ReadCompressed(pBuffer.get(), nGlyphdatasize * sizeof(RGBA));
    
    glyph.resize(nGlyphs);
    RGBA* p = pBuffer.get();
    for (int nGlyph = 0; nGlyph < nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p, nGlyphwidth[nGlyph], nGlyphheight[nGlyph]);
        p += nGlyphwidth[nGlyph]*nGlyphheight[nGlyph];
    }
    
    f.Close();
    return true;
}

bool FontFile::Load(const char* fname)
{
    File f;
    bool bResult;
    
    bResult = f.OpenRead(fname);
    if (!bResult)
        return false;
    
    char magic;
    f.Read(magic);
    
    switch (magic)
    {
    case 1:     bResult = Load8bppFont(f);              break;
    case 2:     bResult = Load16bppFont(f);             break;
    case 'F':   bResult = Load32bppFont(f);             break;
    default:    bResult = false;
    };
    
    return bResult;     
}

void FontFile::Save(const char* fname)
{
}
