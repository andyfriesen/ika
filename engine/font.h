#ifndef FONT_H
#define FONT_H

#include "video/Driver.h"
#include "common/types.h"
#include "common/fontfile.h"
#include "tileset.h"

class File;

struct FontException{};

/**
 * Encapsulates a hardware dependant copy of a bitmap font.
 *
 * I don't really like how fonts have to store a copy of the Video interface.
 * It's dirty.
 */
class Font
{
private:
    FontFile _fontFile;
    
    Video::Driver*  _video;
    std::vector<Video::Image*> _glyphs;
    
    int _width, _height;                                        ///< width/height of the widest/highest character in the whole font
    int _tabSize;                                               ///< tab granularity, in pixels

public:
    Font(const char* filename, Video::Driver* v);
    ~Font();

    int GetGlyphIndex(char c, uint subset) const;

    Video::Image* GetGlyphImage(char c, uint subset);
    Canvas& GetGlyphCanvas(char c, uint subset) const;

    void PrintChar(int& x, int y, uint subset, char c);
    void PrintChar(int& x, int y, uint subset, char c, Canvas& dest, Video::BlendMode blendMode);

    template <class Printer>
    void PaintString(int x, int y, const char* s, Printer& print);  ///< Draws the string somewhere.

    void PrintString(int x, int y, const char* s);                  ///< Draws the string to the screen
    void PrintString(int x, int y, const char* s, Canvas& dest, Video::BlendMode blendMode);    ///< Draws the string on a canvas.
    
    int StringWidth(const char* s) const;                       ///< Returns the width, in pixels, of the string, if printed in this font.
    
    int Width()   const {   return _width;      }               ///< Returns the width of the widest char in the font.
    int Height()  const {   return _height;     }               ///< Returns the height of the highest char in the font.
    int TabSize() const {   return _tabSize;     }              ///< Returns the tab granularity.
    void SetTabSize(int tabsize)    {   _tabSize = tabsize;   } ///< Sets the tab granularity.
};

#endif
