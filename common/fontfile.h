/*
Font file
*/

#ifndef FONTFILE_H
#define FONTFILE_H

#include <vector>
#include "pixel_matrix.h"

class File;

class CFontFile
{
public:
    // ------------------ Types --------------------
    struct SSubSet
    {
        int	nGlyphtbl[256];						// glyph -> character table		
    };
    
private:
    // ------------------ Data ---------------------
    std::vector<SSubSet>    set;                            // font subsets
    std::vector<CPixelMatrix>       glyph;
    
    // ---------- Implementation Details -----------
    bool Load8bppFont(File& f);
    bool Load16bppFont(File& f);
    bool Load32bppFont(File& f);
    
    // ----------------- Methods -------------------
public:
    ~CFontFile();
    
    bool Load(const char* fname);
    void Save(const char* fname);
    
    // ---------------- Accessors ------------------
    int                     NumSubSets()    const                   {       return set.size();                              }
    SSubSet&        GetSubSet(int subset)   const   {       return (SSubSet&)set[subset];   }
    CPixelMatrix& GetGlyph(int glyphidx) const      {       return (CPixelMatrix&)glyph[glyphidx];                  }
};

#endif