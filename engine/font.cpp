/*
Font stuff.
You know the drill. --tSB
*/

#include "common/fontfile.h"
#include "common/fileio.h"
#include "common/rle.h"
#include "common/log.h"
#include "common/canvas.h"
#include "video/Driver.h"
#include "video/Image.h"

#include "font.h"

#include <cassert>

static const char subsetmarker = '~';

CFont::CFont(const char* filename, Video::Driver* v)
    : _video(v)
    , _width(0)
    , _height(0)
    , _tabSize(30)
{
    CDEBUG("cfont::loadfnt");
    
    if (!_fontFile.Load(filename))
        throw FontException();

    _glyphs.resize(_fontFile.NumGlyphs());

    for (uint i = 0; i < _fontFile.NumGlyphs(); i++)
    {
        Canvas& glyph = _fontFile.GetGlyph(i);
        _width = max<uint>(_width, glyph.Width());
        _height = max<uint>(_height, glyph.Height());
    }
    
    /*set.resize(f.NumSubSets());
    
    _width = _height = 0;
    _tabSize = 30;
    
    // eep @_@
    for (int nSet = 0; nSet < f.NumSubSets(); nSet++)
    {
        for (int nGlyph = 0; nGlyph < 96; nGlyph++)
        {
            CFontFile::SSubSet& s = f.GetSubSet(nSet);
            
            int nGlyphidx = s.nGlyphtbl[nGlyph + 32];
            
            Canvas& glyph = f.GetGlyph(nGlyphidx);

            set[nSet].glyph[nGlyph] = _video->CreateImage(glyph);
            
            if (_width < glyph.Width())     _width = glyph.Width();
            if (_height < glyph.Height())   _height = glyph.Height();
        }
    }*/
}

CFont::~CFont()
{
    for (uint i = 0; i < _glyphs.size(); i++)
    {
        _video->FreeImage(_glyphs[i]);
    }
        
    _glyphs.clear();
}

int CFont::GetGlyphIndex(char c, uint subset) const
{
    assert(subset >= 0 && subset < _fontFile.NumSubSets());

    return _fontFile.GetSubSet(subset).glyphIndex[c];
}

Video::Image* CFont::GetGlyphImage(char c, uint subset)
{
    uint glyphIndex = GetGlyphIndex(c, subset);

    assert(glyphIndex <= _glyphs.size()); // :x
    
    Video::Image* img = _glyphs[glyphIndex];

    if (!img)
    {
        img = _video->CreateImage(_fontFile.GetGlyph(glyphIndex));
        _glyphs[glyphIndex] = img;
    }

    return img;
}

Canvas& CFont::GetGlyphCanvas(char c, uint subset) const
{
    return _fontFile.GetGlyph(GetGlyphIndex(c, subset));
}

void CFont::PrintChar(int& x, int y, uint subset, char c)
{
    Video::Image* img = GetGlyphImage(c, subset);

    if (!img) return;

    _video->BlitImage(img, x, y);
    x += img->Width();
}

void CFont::PrintChar(int& x, int y, uint subset, char c, Canvas& dest)
{
    //if (c < 0 || c > 96)
    //    return;

    assert((uint)GetGlyphIndex(c, subset) >= _fontFile.NumGlyphs()); // paranoia check

    Canvas& glyph = GetGlyphCanvas(c, subset);

    CBlitter<Alpha>::Blit(glyph, dest, x, y);
    x += glyph.Width();
}

template <class Printer>
void CFont::PaintString(int startx, int starty, const char* s, Printer& print)
{   
    int cursubset = 0;
    int x = startx;
    int y = starty;
    
    _video->SetBlendMode(Video::Normal);
    for (uint i=0; i < strlen(s); i++)
    {
        switch (s[i])
        {
        case '\n':          // newline
            y += _height;
            x = startx;
            break;
            
        case '\t':          // tab
            x += _tabSize - (x - startx) % _tabSize;
            break;
            
        case subsetmarker:
            i++;
            
            if (i >= strlen(s))
                return; // subset marker at end of string.  bjork.
            
            if (s[i] >= '0' && s[i] <= '0' + static_cast<char>(_fontFile.NumSubSets()))                    // number?  switch the subset. (also make sure that it's a valid subset index
                cursubset=s[i] - '0';
            else if (s[i] == subsetmarker)
                //PrintChar(x, y, cursubset, s[i]);
                print(x, y, cursubset, s[i], this);
            break;
            
        default:
            //PrintChar(x, y, cursubset, s[i]);
            print(x, y, cursubset, s[i], this);
        }
    }
}

namespace
{
    struct PrintToVideo
    {
        inline void operator ()(int& x, int y, int subset, char c, CFont* font)
        {
            font->PrintChar(x, y, subset, c);
        }
    };

    struct PrintToCanvas
    {
        Canvas& _dest;

        PrintToCanvas(Canvas& dest)
            : _dest(dest)
        {}

        inline void operator ()(int& x, int y, int subset, char c, CFont* font)
        {
            font->PrintChar(x, y, subset, c, _dest);
        }
    };
};

void CFont::PrintString(int x, int y, const char* s)
{
    PaintString(x, y, s, PrintToVideo());
}

void CFont::PrintString(int x, int y, const char* s, Canvas& dest)
{
    PaintString(x, y, s, PrintToCanvas(dest));
}

int CFont::StringWidth(const char* s) const
{
    int _width = 0;
    int currentSubSet = 0;
    uint len = strlen(s);
    
    for (uint i = 0; i < len; i++)
    {
        unsigned char c = s[i];
        
        switch (c)
        {
        case '\n':
            return _width;                                                          // um.. @_x
            
        case '\t':                                                                  // tab
            _width += _tabSize - _width % _tabSize;
            break;
            
        case subsetmarker:
            i++;
            if (s[i] == subsetmarker)
                _width += GetGlyphCanvas(subsetmarker, currentSubSet).Width();
            else if (s[i] >= '0' && s[i] <= '0' + static_cast<char>(_fontFile.NumSubSets()))                       // valid subset number?
                currentSubSet = s[i] - '0';
            break;
            
        default:
            if (c < 32 || c > 32 + 96)
                continue;                                                           // invalid char, skip it.
            _width += GetGlyphCanvas(c, currentSubSet).Width();
        }
    }
    return _width;
}
