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

namespace Ika
{
    
    static const char subsetMarker = '~';
    static const char colourMarker = '#';
    
    Font::Font(const std::string& filename, Video::Driver* v)
        : _video(v)
        , _width(0)
        , _height(0)
        , _tabSize(30)
    {
        CDEBUG("cfont::loadfnt");
    
        if (!_fontFile.Load(filename.c_str()))
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
    
    void Font::PrintChar(int& x, int y, uint subset, char c, RGBA colour)
    {
        Video::Image* img = GetGlyphImage(c, subset);
    
        if (!img) return;
    
        //_video->BlitImage(img, x, y);
        _video->TintBlitImage(img, x, y, colour.i);
        x += img->Width();
    }
    
    void Font::PrintChar(int& x, int y, uint subset, char c, RGBA colour, Canvas& dest, Video::BlendMode blendMode)
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
    void Font::PaintString(int startx, int starty, const std::string& s, Printer& print)
    {
        int cursubset = 0;
        int x = startx;
        int y = starty;
        uint len = s.length();
        RGBA colour(255, 255, 255);
    
        _video->SetBlendMode(Video::Normal);
        for (uint i=0; i < len; i++)
        {
            switch (s[i])
            {
            case '\n':          // newline
                y += _height;
                x = startx;
                continue;
    
            case '\t':          // tab
                x += _tabSize - (x - startx) % _tabSize;
                continue;
    
            case subsetMarker:
                i++;
    
                if (i >= len)
                    break; // subset marker at end of string.  just print it
    
                if (i < len && s[i] >= '0' && s[i] <= '0' + static_cast<char>(_fontFile.NumSubSets()))
                {
                    // ~ followed by a digit is not printed.  the subset is instead changed.
                    cursubset=s[i] - '0';
                    continue;
                }
                else
                {
                    // ~ followed by anything else (or nothing at all) is printed like any other character.
                    i--;
                    break;
                }
    
            case colourMarker:
                {
                    i++;
                    int pos = s.find(']', i);
                    if (i >= len || s[i] != '[' || pos == std::string::npos)
                    {
                        i--;
                        break;
                    }
                    std::string t(s.substr(i + 1, pos - i - 1));
                    
                    // #[123] becomes #[112233]
                    // #[1234] becomes #[11223344]
                    // #[123456] becomes #[123456]
                    if (t.length() == 3)
                        t = std::string() + t[0] + t[0] + t[1] + t[1] + t[2] + t[2];
                    else if (t.length() == 4)
                        t = std::string() + t[0] + t[0] + t[1] + t[1] + t[2] + t[2] + t[3] + t[3];

                    colour = hexToInt(t);
                    i = pos + 1;
                }
            };
    
            // if execution gets here, we found no control character
            print(x, y, cursubset, s[i], colour, this);
        }
    }
    
    namespace
    {
        struct PrintToVideo
        {
            inline void operator ()(int& x, int y, int subset, char c, RGBA colour, Font* font)
            {
                font->PrintChar(x, y, subset, c, colour);
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
    
            inline void operator ()(int& x, int y, int subset, char c, RGBA colour, Font* font)
            {
                font->PrintChar(x, y, subset, c, colour, _dest, _blendMode);
            }
        };
    };
    
    void Font::PrintString(int x, int y, const std::string& s)
    {
        PrintToVideo printer;
        PaintString(x, y, s, printer);
    }
    
    void Font::PrintString(int x, int y, const std::string& s, Canvas& dest, Video::BlendMode blendMode)
    {
        PrintToCanvas printer(dest, blendMode);
        PaintString(x, y, s, printer);
    }
    
    int Font::StringWidth(const std::string& s) const
    {
        int _width = 0;
        int currentSubSet = 0;
        uint len = s.length();
    
        for (uint i = 0; i < len; i++)
        {
            u8 c = s[i];
    
            switch (c)
            {
            case '\n':
                //return _width;                                                          // um.. @_x
                return max(_width, StringWidth(s.substr(i + 1)));
    
            case '\t':                                                                  // tab
                _width += _tabSize - _width % _tabSize;
                continue;
    
            case subsetMarker:
                i++;
                if (s[i] == subsetMarker)
                    _width += GetGlyphCanvas(subsetMarker, currentSubSet).Width();
                else if (s[i] >= '0' && s[i] <= '0' + static_cast<char>(_fontFile.NumSubSets()))                       // valid subset number?
                    currentSubSet = s[i] - '0';
                continue;
    
            case colourMarker:
                {
                    if (i + 1 > len || s[i + 1] != '[')
                        break;
                    int pos = s.find(']', i + 1);
                    if (pos == std::string::npos)
                        break;
    
                    i = pos + 1;
                    continue;
                }
            };
    
            if (c < 32 || c > 32 + 96)
                continue;                                                           // invalid char, skip it.
            _width += GetGlyphCanvas(c, currentSubSet).Width();
        }
        return _width;
    }

}
