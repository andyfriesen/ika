/*
Font file
*/

#pragma once

#include "common/utility.h"
#include "Canvas.h"

struct File;

/**
 * Hardware independant representation of a bitmap font.
 *
 * FIXME: not even remotely complete.
 */
struct FontFile {
    /// glyph -> character table
    struct SSubSet {
        uint glyphIndex[256];
    };

    ~FontFile();
    
    bool Load(const char* fname);                           //!< Reads data from the specified filename.
    void Save(const char* fname);                           //!< Writes data to the specified filename, destroying any data that was there.

    /// Returns the number of subsets.
    uint NumSubSets() const;

    /// Returns the specified subset table.
    const SSubSet& GetSubSet(int subset) const;

    /// Returns the specified glyph.
    const Canvas& GetGlyph(int glyphidx) const;

    /// Returns the number of glyphs in the font.
    uint NumGlyphs() const;

    /// Returns the width of the widest character in the font
    int Width() const;

    /// Returns the height of the highest character in the font
    int Height() const;
    
private:
    bool Load8bppFont(File& f);
    bool Load16bppFont(File& f);
    bool Load32bppFont(File& f);

    std::vector<SSubSet>    set;                    ///< font subsets.
    std::vector<Canvas>     glyph;                  ///< Actual font glyphs.

    int width, height;                              ///< The width and height of the largest characters in the font.
};

