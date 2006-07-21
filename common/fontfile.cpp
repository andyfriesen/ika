#include <limits>

#include "fontfile.h"
#include "fileio.h"
#include "rle.h"
#include "vergepal.h"
#include "utility.h"

FontFile::~FontFile()
{
    glyph.clear();
}

uint FontFile::NumSubSets() const {
    return set.size();
}

const FontFile::SSubSet& FontFile::GetSubSet(int subset) const {
    return set[subset];
}

const Canvas& FontFile::GetGlyph(int glyphidx) const {
    return glyph[glyphidx];
}

uint FontFile::NumGlyphs() const {
    return glyph.size();
}

int FontFile::Width() const {
    return width;
}

int FontFile::Height() const {
    return height;
}


bool FontFile::Load8bppFont(File& f)
{
    short int width, height, subsetcount;
    f.Read(&width, 2);
    f.Read(&height, 2);
    f.Read(&subsetcount, 2);
    const int nGlyphs = subsetcount * 96;

    ScopedArray<u8> buffer8(new u8[width * height * nGlyphs]);
    ScopedArray<RGBA> pPixels(new RGBA[width * height * nGlyphs]);

    f.Read(buffer8.get(), width * height * nGlyphs * sizeof(u8));

    // convert 8bpp to 32bpp

    for (int i = 0; i < width * height * nGlyphs; i++)
    {
        char c = buffer8[i];

        pPixels[i] = RGBA(c, cVergepal);
    }

    glyph.resize(nGlyphs);
    RGBA* p = pPixels.get();

    for (int nGlyph = 0; nGlyph < nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p, width, height);
        p += width * height;
    }

    set.resize(subsetcount);                                            // not very readable :(  Allocates the needed number of subsets.
    for (int nCurset = 0; nCurset < subsetcount; nCurset++)
    {
        SSubSet& ss = set[nCurset];

        for (int j = 0; j < 256; j++)
            ss.glyphIndex[j] = 0;

        for (int j = 0; j < 96; j++)
            ss.glyphIndex[j + 32] = j + (nCurset * 96);
    }

    // blegh
    this->width = width;
    this->height = height;

    return true;
}

bool FontFile::Load16bppFont(File& f)
{
    short int width, height, subsetcount;
    f.Read(&width, 2);
    f.Read(&height, 2);
    f.Read(&subsetcount, 2);
    const int nGlyphs = subsetcount * 96;                               // magic number, due to old font format restrictions.

    ScopedArray<u16> buffer16(new u16[width * height * nGlyphs]);
    ScopedArray<RGBA> pPixels(new RGBA[width * height * nGlyphs]);

    f.Read(buffer16.get(), width * height * nGlyphs * sizeof(u16));

    // now we have uncompressed 16 bit font stuff.  Convert it to 32 bits.

    for (int i = 0; i < width * height * nGlyphs; i++)
    {
        u16 c = buffer16[i];

        pPixels[i] = RGBA(
             (c & 31)  << 3,
            ((c >>  5) << 3) & 0xFF,
            ((c >> 10) << 3) & 0xFF,
            c ? 255 : 0);
    }

    glyph.resize(nGlyphs);

    RGBA* p = pPixels.get();

    for (int nGlyph = 0; nGlyph < nGlyphs; nGlyph++)
    {
        glyph[nGlyph].CopyPixelData(p, width, height);
        p += width * height;
    }

    set.resize(subsetcount);                                            // not very readable :(  Allocates the needed number of subsets.
    for (int nCurset = 0; nCurset < subsetcount; nCurset++)
    {
        SSubSet& ss = set[nCurset];

        for (int j = 0; j < 256; j++)
            ss.glyphIndex[j] = 0;

        for (int j = 0; j < 96; j++)
            ss.glyphIndex[j + 32] = j + (nCurset * 96);
    }

    this->width = width;
    this->height = height;

    return true;
}

bool FontFile::Load32bppFont(File& f) {
    f.Seek(0);

    char signature[6];
    f.Read(signature, 6);
    if (memcmp(signature, "FONT27", 6) != 0) {
        return false;
    }

    u8   subsetCount;
    u16  glyphCount;

    f.Read(subsetCount);
    f.Read(glyphCount);

    set.resize(subsetCount);
    for (int nSet = 0; nSet < subsetCount; nSet++) {
        u16 i[256];
        f.Read(i, 256 * sizeof(u16));

        for (int j = 0; j < 256; j++) {
            set[nSet].glyphIndex[j] = i[j];
        }
    }

    std::vector<int>    glyphWidths;
    std::vector<int>    glyphHeights;
    int dataSize = 0;

    width = height = 0;

    for (int i = 0; i < glyphCount; i++) {
        u16 w, h;
        f.Read(w);      f.Read(h);
        glyphWidths.push_back(w);
        glyphHeights.push_back(h);
        dataSize += w * h;

        if (width < w) width = w;
        if (height < h) height = h;
    }

    ScopedArray<RGBA> pBuffer(new RGBA[dataSize]);
    f.ReadCompressed(pBuffer.get(), dataSize * sizeof(RGBA));

    glyph.resize(glyphCount);
    RGBA* p = pBuffer.get();
    for (int nGlyph = 0; nGlyph < glyphCount; nGlyph++) {
        glyph[nGlyph].CopyPixelData(p, glyphWidths[nGlyph], glyphHeights[nGlyph]);

        p += glyphWidths[nGlyph] * glyphHeights[nGlyph];
    }

    f.Close();
    return true;
}

bool FontFile::Load(const char* fname) {
    File f;
    bool result;

    result = f.OpenRead(fname);
    if (!result)
        return false;

    char magic;
    f.Read(magic);

    switch (magic) {
        case 1: {
            result = Load8bppFont(f);
            break;
        }
        case 2: {
            result = Load16bppFont(f);
            break;
        }
        case 'F': {
            result = Load32bppFont(f);
            break;
        }
        default: {
            result = false;
        }
    };

    return result;
}

void FontFile::Save(const char* fname) {
    File f;
    f.OpenWrite(fname, 1);

	assert(set.size() <= (std::numeric_limits<unsigned char>::max)());
    unsigned char numSubsets = set.size();

	assert(glyph.size() <= (std::numeric_limits<unsigned short>::max)());
	unsigned short numGlyphs = glyph.size();

    f.Write("FONT27");
    f.Write(numSubsets);
    f.Write(numGlyphs);

    for(std::vector<SSubSet>::iterator s = set.begin(); s != set.end(); s++) {
        u16 blah[256];
        std::copy(&s->glyphIndex[0], &s->glyphIndex[256], &blah[0]);
        f.Write(&blah[0], sizeof(blah));
    }

    for (unsigned int i = 0; i < glyph.size(); i++) {
		assert(glyph[i].Width() <= (std::numeric_limits<unsigned short>::max)());
        unsigned short w = glyph[i].Width();
		assert(glyph[i].Height() <= (std::numeric_limits<unsigned short>::max)());
        unsigned short h = glyph[i].Height();

        f.Write(w);
        f.Write(h);
    }

    std::vector<RGBA> pixels;
    for (uint i = 0; i < glyph.size(); i++) {
        Canvas* c = &glyph[i];
        RGBA* p = c->GetPixels();
        int len = c->Width() * c->Height();

        for (int j = 0; j < len; j++) {
            pixels.push_back(p[j]);
        }
    }

    ScopedArray<RGBA> pBuffer( new RGBA[pixels.size()] );

    std::copy(pixels.begin(), pixels.end(),
        std::raw_storage_iterator<RGBA*, int>(pBuffer.get()));

    f.WriteCompressed(pBuffer.get(), pixels.size() * sizeof(RGBA));
    f.Close();
}
