/*
Font file
*/

#ifndef FONTFILE_H
#define FONTFILE_H

#include "types.h"
#include "Canvas.h"

class File;

/*!
    Hardware - independant representation of a bitmap font.
*/
class CFontFile
{
public:
    // ------------------ Types --------------------
    struct SSubSet                                  ///< glyph -> character table
    {
        uint glyphIndex[256];
    };
    
private:
    // ------------------ Data ---------------------
    std::vector<SSubSet>    set;                    ///< font subsets.
    std::vector<Canvas>     glyph;                  ///< Actual font glyphs.

    int width, height;                            ///< The width and height of the largest characters in the font.
    
    // ---------- Implementation Details -----------
    bool Load8bppFont(File& f);
    bool Load16bppFont(File& f);
    bool Load32bppFont(File& f);
    
    // ----------------- Methods -------------------
public:
    ~CFontFile();
    
    bool Load(const char* fname);                           //!< Reads data from the specified filename.
    void Save(const char* fname);                           //!< Writes data to the specified filename, destroying any data that was there.
    
    // ---------------- Accessors ------------------
    uint          NumSubSets()           const { return set.size();                     }   //!< Returns the number of subsets.
    SSubSet&      GetSubSet(int subset)  const { return (SSubSet&)set[subset];          }   //!< Returns the specified subset table.
    Canvas& GetGlyph(int glyphidx) const { return (Canvas&)glyph[glyphidx]; }   //!< Returns the specified glyph.
    uint          NumGlyphs()            const { return glyph.size();                   }   //!< Returns the number of glyphs in the font.

    int           Width()                const { return width;                         }   //!< Returns the width of the widest character in the font
    int           Height()               const { return height;                        }   //!< Returns the height of the highest character in the font
};

#endif
