#ifndef FONT_H
#define FONT_H

#include "common/types.h"
#include "tileset.h"

class File;

namespace Video
{
    class Driver;
    class Image;
}

struct FontException{};

/*!
    Encapsulates a hardware dependant copy of a bitmap font.
*/
class CFont
{
private:
    struct SubSet
    {
        Video::Image* glyph[96];
    };
    
    Video::Driver*  video;
    vector <SubSet> set;
    
    int nWidth, nHeight;                                         ///< width/height of the widest/highest character in the whole font
    int nTabsize;                                               ///< tab granularity, in pixels
    
    void PrintChar(int& x, int y, int cursubset, char c);
    
public:
    CFont(const char* filename, Video::Driver* v);
    ~CFont();

    void PrintString(int x, int y, const char* s);                ///< Draws the string
    
    int StringWidth(const char* s) const;                       ///< Returns the width, in pixels, of the string, if printed in this font.
    
    int Width() const   {   return nWidth;  }                   ///< Returns the width of the widest char in the font.
    int Height() const  {   return nHeight; }                   ///< Returns the height of the highest char in the font.
    int TabSize() const {   return nTabsize;    }               ///< Returns the tab granularity.
    void SetTabSize(int tabsize)    {   nTabsize=tabsize;   }   ///< Sets the tab granularity.
};

#endif
