/*
 * Font stuff.
 * You know the drill. --tSB
 */

#include "common/fontfile.h"
#include "common/fileio.h"
#include "common/rle.h"
#include "common/log.h"
#include "common/Canvas.h"
#include "video/Driver.h"
#include "video/Image.h"

#include "font.h"

#include <cassert>

static const char subsetmarker = '~';

Font::Font(const char* filename, Video::Driver* v)
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
}

Font::~Font()
{
    for (uint i = 0; i < _glyphs.size(); i++)
    {
        _video->FreeImage(_glyphs[i]);
    }

    _glyphs.clear();
}

int Font::GetGlyphIndex(char c, uint subset) const
{
    if (subset >= 0 && subset < _fontFile.NumSubSets())
        return _fontFile.GetSubSet(subset).glyphIndex[c];
    else
        return 0;
}

Video::Image* Font::GetGlyphImage(char c, uint subset)
{
    uint glyphIndex = GetGlyphIndex(c, subset);

    if (glyphIndex >= _glyphs.size())
        return 0;

    Video::Image* img = _glyphs[glyphIndex];

    if (!img)
    {
        img = _video->CreateImage(_fontFile.GetGlyph(glyphIndex));
        _glyphs[glyphIndex] = img;
    }

    return img;
}

Canvas& Font::GetGlyphCanvas(char c, uint subset) const
{
    return _fontFile.GetGlyph(GetGlyphIndex(c, subset));
}

void Font::PrintChar(int& x, int y, uint subset, char c)
{
    Video::Image* img = GetGlyphImage(c, subset);

    if (!img) return;

    _video->BlitImage(img, x, y);
    x += img->Width();
}

void Font::PrintChar(int& x, int y, uint subset, char c, Canvas& dest, Video::BlendMode blendMode)
{
    //if (c < 0 || c > 96)
    //    return;

    assert((uint)GetGlyphIndex(c, subset) < _fontFile.NumGlyphs()); // paranoia check

    Canvas& glyph = GetGlyphCanvas(c, subset);

    switch (blendMode)
    {
    default:
    case Video::None:       CBlitter<Opaque>::Blit(glyph, dest, x, y);      break;
    case Video::Add:        CBlitter<Additive>::Blit(glyph, dest, x, y);    break;
    case Video::Matte:      CBlitter<Matte>::Blit(glyph, dest, x, y);       break;
    case Video::Normal:     CBlitter<Alpha>::Blit(glyph, dest, x, y);       break;
    case Video::Subtract:   CBlitter<Subtractive>::Blit(glyph, dest, x, y); break;
    }

    x += glyph.Width();
}

template <class Printer>
void Font::PaintString(int startx, int starty, const char* s, Printer& print)
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
        inline void operator ()(int& x, int y, int subset, char c, Font* font)
        {
            font->PrintChar(x, y, subset, c);
        }
    };

    struct PrintToCanvas
    {
        Canvas& _dest;
        Video::BlendMode _blendMode;

        PrintToCanvas(Canvas& dest, Video::BlendMode blendMode)
            : _dest(dest)
            , _blendMode(blendMode)
        {}

        inline void operator ()(int& x, int y, int subset, char c, Font* font)
        {
            font->PrintChar(x, y, subset, c, _dest, _blendMode);
        }
    };
};

void Font::PrintString(int x, int y, const char* s)
{
    PrintToVideo printer;
    PaintString(x, y, s, printer);
}

void Font::PrintString(int x, int y, const char* s, Canvas& dest, Video::BlendMode blendMode)
{
    PrintToCanvas printer(dest, blendMode);
    PaintString(x, y, s, printer);
}

int Font::StringWidth(const char* s) const
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
