// VSP.cpp
#include <cassert>
#include <stdio.h>
#include "vsp.h"
#include "common/utility.h"
#include "rle.h"
#include "fileio.h"
#include "common/log.h"
#include "zlib.h"

VSP::VSP()
    : _width(0)
    , _height(0)
    , vspAnim(_vspanim)
{
    _vspanim.resize(100);
    //New();
}

VSP::VSP(const std::string& fname)
    : vspAnim(_vspanim)
{
    Load(fname);
}

bool VSP::Load(const std::string& fileName) {
    File f;
    int numTiles = 0;

    Free(); // nuke any existing VSP data

    if (!f.OpenRead(fileName.c_str())) {
        Log::Write("Error opening %s", fileName.c_str());
        return false;
    }

    name = fileName;

    u16 ver;
    f.Read(&ver, 2);
    switch(ver) {
        case 2: {
            // 8bpp uncompressed VSP

            _width = 16;
            _height = 16;

            u8 pal[768];
            f.Read(&pal, 768);
            f.Read(&numTiles, 2);

            ScopedArray<u8> data8 = new u8[numTiles * 256];
            f.Read(data8.get(), numTiles * 256);

            CreateTilesFromBuffer(data8.get(), pal, numTiles, _width, _height);

            break;
        }

        case 3: {
            // 8bpp RLE compressed VSP
            _width = 16;
            _height = 16;

            u8 pal[768];
            u32 bufsize;

            f.Read(&pal, 768);
            f.Read(&numTiles, 2);
            f.Read(&bufsize, 4);

            ScopedArray<u8> buffer = new u8[bufsize];
            ScopedArray<u8> data8 = new u8[numTiles * 256];
            f.Read(buffer.get(), bufsize);
            ReadCompressedLayer1(data8.get(), numTiles * 256, buffer.get());

            CreateTilesFromBuffer(data8.get(), pal, numTiles, _width, _height);
            break;
        }

        case 4: {
            // 16bpp uncompressed
            _width = 16; _height = 16;

            f.Read(&numTiles, 2);

            ScopedArray<u16> data16 = new u16[numTiles * _width * _height];

            f.Read(data16.get(), numTiles * _width * _height * 2); 

            CreateTilesFromBuffer(data16.get(), numTiles, _width, _height);

            break;
        }

        case 5: {
            // 16bpp RLE compressed
            u32 bufsize;

            _width = 16; _height = 16;
            f.Read(&numTiles, 2);
            f.Read(&bufsize, 4);

            ScopedArray<u8> buffer = new u8[bufsize];
            ScopedArray<u16> data16 = new u16[numTiles * _width * _height];

            f.Read(buffer.get(), bufsize);
            ReadCompressedLayer2(data16.get(), numTiles * _width * _height, reinterpret_cast<u16*>(buffer.get()));

            CreateTilesFromBuffer(data16.get(), numTiles, _width, _height);

            break;
        }

        case 6: {
            // 8/32bpp zlib compressed.  (ika specific)
            // actually, 8bpp never was put to use
            z_stream stream;
            u8 maskColour;

            u8 bpp;
            u8 pal[768];

            f.Read(bpp);

            f.Read(&_width, 2);
            f.Read(&_height, 2);
            f.Read(&numTiles, 4);

            char stringBuffer[64];
            f.Read(stringBuffer, 64);
            stringBuffer[63] = 0;
            desc = stringBuffer;

            if (bpp == 1) {
                f.Read(pal, 768);
                f.Read(&maskColour, 1);
            }

            int dataSize = _width * _height * numTiles * bpp;
            int compressedBlockSize;
            f.Read(&compressedBlockSize, 4);

            ScopedArray<u8> buffer = new u8[compressedBlockSize];
            ScopedArray<u8> data = new u8[dataSize];

            f.Read(buffer.get(), compressedBlockSize);

            stream.next_in = (Bytef*)buffer.get();
            stream.avail_in = compressedBlockSize;
            stream.next_out = (Bytef*)data.get();
            stream.avail_out = dataSize;
            stream.data_type = Z_BINARY;

            stream.zalloc = NULL;
            stream.zfree = NULL;

            inflateInit(&stream);
            inflate(&stream, Z_SYNC_FLUSH);
            inflateEnd(&stream);

            if (bpp == 1) {
                CreateTilesFromBuffer(data.get(), pal, numTiles, _width, _height);
            } else {
                CreateTilesFromBuffer(reinterpret_cast<RGBA*>(data.get()), numTiles, _width, _height);
            }

            break;
        }
        default: {
            Log::Write("Fatal error: unknown VSP version (%d)", ver);
            return false;
        }
    }

    for (int j = 0; j < 100; j++) {
        f.Read(&_vspanim[j].start, 2);
        f.Read(&_vspanim[j].finish, 2);
        f.Read(&_vspanim[j].delay, 2);
        f.Read(&_vspanim[j].mode, 2);
    }

    f.Close();

    return true;
}

int VSP::Save(const std::string& fname) {
    File f;
    int i;

    if (!f.OpenWrite(fname.c_str())) {
        Log::Write("Error writing to %s", fname.c_str());
        return 0;
    }

    ScopedArray<RGBA> tileBuffer = new RGBA[_width * _height * tiles.size()];

    // copy all the tile data into one big long buffer that we can write to disk
    for (uint j = 0; j < tiles.size(); j++) {
        memcpy(
            tileBuffer.get() + (j * _width * _height), 
            tiles[j].GetPixels(), 
            _width * _height * sizeof(RGBA)
        );
    }

    const char bpp = 4;

    i = 6;
    f.Write(&i, 2);
    f.Write(&bpp, 1);
    f.Write(&_width, 2);
    f.Write(&_height, 2);
    f.Write((int)tiles.size());

    char buffer[64];
    strncpy(buffer, desc.c_str(), 63);
    f.Write(buffer, 64);			// description. (authoring info, whatever)

    z_stream stream;
    size_t nDatasize = tiles.size() * _width * _height * bpp;

    // Compression buffer.  11/10ths the size of the original, plus 12 bytes.
    ScopedArray<u8> cb = new u8[(nDatasize * 11) / 10 + 12];

    stream.next_in = reinterpret_cast<Bytef*>(tileBuffer.get());
    stream.avail_in = nDatasize;
    stream.next_out = reinterpret_cast<Bytef*>(cb.get());
    stream.avail_out = (nDatasize * 11) / 10 + 12;    // +10% and 12 bytes
    stream.data_type = Z_BINARY;

    stream.zalloc = NULL;
    stream.zfree = NULL;

    deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    deflate(&stream, Z_SYNC_FLUSH);
    deflateEnd(&stream);

    f.Write(&stream.total_out, 4);
    f.Write(cb.get(), stream.total_out);

    for (int k = 0; k < 100; k++) {
        f.Write(&_vspanim[k].start, 2);
        f.Write(&_vspanim[k].finish, 2);
        f.Write(&_vspanim[k].delay, 2);
        f.Write(&_vspanim[k].mode, 2);
    }

    f.Close();

    return true;
}

void VSP::Free() {
    tiles.clear();
}

/// creates a blank 32 bit VSP, of the specified size and number of tiles
void VSP::New(int xsize, int ysize, int numtiles) {
    Free();
    _width = xsize > 0 ? xsize : 1;
    _height = ysize > 0 ? ysize : 1;

    tiles.resize(numtiles);

    for (int i = 0; i < numtiles; i++) {
        tiles[i].Resize(_width, _height);
    }
}

// vsp alteration routines

void VSP::InsertTile(uint pos) {
    if (pos < 0 || pos > tiles.size()) {
        return;
    } else if (pos == tiles.size()) {
        AppendTile();
    } else {
        tiles.insert(tiles.begin() + pos, Canvas(_width, _height));
    }
}

void VSP::DeleteTile(uint pos) {
    if (pos < tiles.size()) {
        tiles.erase(tiles.begin() + pos);
    }
}

void VSP::AppendTiles(uint count) {
    Canvas dummy(_width, _height);

    for (uint i = 0; i < count; i++) {
        tiles.push_back(dummy);
    }
}

void VSP::CopyTile(Canvas& tb, uint pos) {
    if (pos < tiles.size()) {
        tb = tiles[pos];
    }
}

void VSP::PasteTile(const Canvas& tb, uint pos) {
    if (pos < tiles.size()) {
        tiles[pos]=tb;
    }
}

void VSP::TPasteTile(Canvas& tb, uint pos) {
    Blitter::Blit(tb, tiles[pos], 0, 0, Blitter::AlphaBlend());
    // NYI
}

void VSP::SetSize(int w, int h) {
    // temp:
    assert(tiles.empty());
    _width = w;
    _height = h;
}

Canvas& VSP::GetTile(uint tileidx) {
    if (tileidx < tiles.size()) {
        return tiles[tileidx];
    } else {
        return tiles[0];
    }
}

void VSP::CreateTilesFromBuffer(u8* data, u8* pal, uint numtiles, int tilex, int tiley) {
    _width = tilex;
    _height = tiley;

    tiles.resize(numtiles);

    for (uint i = 0; i < numtiles; i++) {
        tiles[i].Resize(_width, _height);
        tiles[i].CopyPixelData(data, _width, _height, pal);
        data += _width * _height;
    }
}

void VSP::CreateTilesFromBuffer(u16* data, uint numTiles, int tilex, int tiley) {
    _width = tilex;
    _height = tiley;

    tiles.resize(numTiles);

    ScopedArray<RGBA> buffer = new RGBA[_width * _height];
    u16* source = data;

    for (uint i = 0; i < numTiles; i++) {
        tiles[i].Resize(_width, _height);

        RGBA* dest = buffer.get();
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                *dest++ = RGBA(*source++);
            }
        }

        tiles[i].CopyPixelData(buffer.get(), _width, _height);
    }
}

void VSP::CreateTilesFromBuffer(RGBA* data, uint numtiles, int tilex, int tiley) {
    _width = tilex;
    _height = tiley;

    tiles.resize(numtiles);

    for (uint i = 0; i < numtiles; i++) {
        tiles[i].Resize(_width, _height);
        tiles[i].CopyPixelData(data, _width, _height);
        data += tilex * tiley;
    }
}
