/*
Font stuff.
You know the drill. --tSB
*/

#include "font.h"
#include "fontfile.h"
#include "fileio.h"
#include "rle.h"
#include "log.h"

static const char subsetmarker='~';

void CFont::Free()
{
    int nSetsize = set.size();
    for (int nSet = 0; nSet < nSetsize; nSet++)
        for (int nGlyph = 0; nGlyph < 96; nGlyph++)
            gfxFreeImage(set[nSet].glyph[nGlyph]);
        
        set.clear();
}

bool CFont::LoadFNT(const char* filename)
{
    CDEBUG("cfont::loadfnt");
    
    CFontFile f;
    if (!f.Load(filename))
        return false;
    
    set.resize(f.NumSubSets());
    
    nWidth = nHeight = 0;
    nTabsize = 30;
    
    // eep @_@
    for (int nSet = 0; nSet < f.NumSubSets(); nSet++)
    {
        for (int nGlyph = 0; nGlyph < 96; nGlyph++)
        {
            CFontFile::SSubSet& s = f.GetSubSet(nSet);
            
            int nGlyphidx = s.nGlyphtbl[nGlyph + 32];
            
            set[nSet].glyph[nGlyph] = gfxCreateImage(1, 1);
            
            CPixelMatrix& glyph = f.GetGlyph(nGlyphidx);
            
            gfxCopyPixelData(
                set[nSet].glyph[nGlyph],
                (u32*) glyph.GetPixelData(),
                glyph.Width(),
                glyph.Height()
                );
            
            
            if (nWidth < glyph.Width())     nWidth = glyph.Width();
            if (nHeight < glyph.Height())   nHeight = glyph.Height();
        }
    }
    
    return true;
}

void CFont::PrintChar(int& x, int y, int cursubset, char c)
{
    c -= 32;
    
    if (c < 0 || c > 96)
        return;
    
    handle h = set[cursubset].glyph[c];
    gfxBlitImage(h, x, y, true);
    x += gfxImageWidth(h);
}

void CFont::PrintString(int startx, int starty, const char* s)
{   
    int cursubset=0;
    int x=startx;
    int y=starty;
    
    for (uint i=0; i<strlen(s); i++)
    {
        switch (s[i])
        {
        case '\n':          // newline
            y += nHeight;
            x = startx;
            break;
            
        case '\t':          // tab
            x += nTabsize - (x - startx) % nTabsize;
            break;
            
        case subsetmarker:
            i++;
            
            if (i >= strlen(s))
                return; // subset marker at end of string.  bjork.
            
            if (s[i] >= '0' && s[i] <= '0' + set.size())                    // number?  switch the subset. (also make sure that it's a valid subset index
                cursubset=s[i] - '0';
            else if (s[i] == subsetmarker)
                PrintChar(x, y, cursubset, s[i]);
            break;
            
        default:
            PrintChar(x, y, cursubset, s[i]);
        }
    }
}

int CFont::StringWidth(const char* s) const
{
    int nWidth = 0;
    int nCursubset = 0;
    uint len = strlen(s);
    
    for (uint i = 0; i < len; i++)
    {
        unsigned char c = s[i];
        
        switch (c)
        {
        case '\n':
            return nWidth;                                                          // um.. @_x
            
        case '\t':                                                                  // tab
            nWidth += nTabsize - nWidth % nTabsize;
            break;
            
        case subsetmarker:
            i++;
            if (s[i] == subsetmarker)
                nWidth += gfxImageWidth(set[nCursubset].glyph[subsetmarker - 32]);
            else if (s[i] >= '0' && s[i] <= '0' + set.size())                       // valid subset number?
                nCursubset = s[i] - '0';
            break;
            
        default:
            if (c < 32 || c > 32 + 96)
                continue;                                                           // invalid char, skip it.
            nWidth += gfxImageWidth(set[nCursubset].glyph[c - 32]);
        }
    }
    return nWidth;
}
