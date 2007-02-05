#pragma once

#include "video/Driver.h"
#include "common/utility.h"
#include "common/fontfile.h"
#include "tileset.h"

struct File;

// fix X11 header conflict
namespace Ika {
    struct FontException{};

    /**
    * Encapsulates a hardware dependant copy of a bitmap font.
    *
    * I don't really like how fonts have to store a copy of the Video interface.
    * It's dirty.
    */
    struct Font {
        Font(const std::string& filename, Video::Driver* v);
        ~Font();

        uint GetGlyphIndex(char c, uint subset) const;

        Video::Image* GetGlyphImage(char c, uint subset);
        const Canvas& GetGlyphCanvas(char c, uint subset) const;

        void PrintChar(int& x, int y, uint subset, char c, RGBA colour);
        void PrintChar(int& x, int y, uint subset, char c, RGBA colour, Canvas& dest, Video::BlendMode blendMode);

        template <typename Printer>
        void PaintString(int x, int y, const std::string& s, Printer& print);  ///< Draws the string somewhere.

        void PrintString(int x, int y, const std::string& s);                  ///< Draws the string to the screen
        void PrintString(int x, int y, const std::string& s, Canvas& dest, Video::BlendMode blendMode);  ///< Draws the string on a canvas.

        int StringWidth(const std::string& s);                            ///< Returns the width, in pixels, of the string, if printed in this font.

        uint Width()  const { return _width; }                            ///< Returns the width of the widest char in the font.
        uint Height() const { return _height; }                           ///< Returns the height of the highest char in the font.
        int TabSize() const { return _tabSize; }                          ///< Returns the tab granularity.
        int LetterSpacing() const { return _letterSpacing; }              ///< Returns the letter spacing.
        void SetTabSize(int tabsize) { _tabSize = tabsize; }              ///< Sets the tab granularity.
        void SetLetterSpacing(int spacing) { _letterSpacing = spacing; }  ///< Sets the letter spacing, in pixels.

    private:
        FontFile _fontFile;

        Video::Driver*  _video;
        std::vector<Video::Image*> _glyphs;

        uint _width, _height;  ///< Width/height of the widest/highest character in the whole font.
        int _tabSize;          ///< Tab granularity, in pixels.
        int _letterSpacing;    ///< Spacing between letters, in pixels.
    };

}
