
#include <algorithm>
#include <stdexcept>
#include <stdio.h>

#include "zlib.h"

#include "map.h"
#include "vsp.h"
#include "rle.h"
#include "misc.h"
#include "types.h"

static u16 fgetw(FILE* f)
{
    u16 s = 0;
    fread(&s, 1, 2, f);
    return s;
}

static u32 fgetq(FILE* f)
{
    u32 q = 0;
    fread(&q, 1, 4, f);
    return q;
}

static void fputw(u16 w, FILE* f)
{
    fwrite(&w, 1, sizeof(u16), f);
}

static void fputq(u32 q, FILE* f)
{
    fwrite(&q, 1, sizeof(u32), f);
}

Map* ImportVerge1Map(const std::string& fileName)
{
    FILE* f = fopen(fileName.c_str(), "rb");
    if (!f)
        throw std::runtime_error(va("Unable to open %s", fileName.c_str()));

    Map* map = new Map;

    try
    {
        u8 magic = fgetc(f);
        if (magic != 4)
            throw std::runtime_error(va(
                "Magic number was %i, not 4.  This is not a v1 map.",
                magic));

        char buffer[256];
        std::fill(buffer, buffer + 256, 0);

        fread(buffer, 1, 13, f);    map->tileSetName = buffer;
        fread(buffer, 1, 13, f);    map->metaData["music"] = buffer;

        int layerCount = fgetc(f);
        int pmulx = fgetc(f);
        int pmuly = pmulx;
        int pdivx = fgetc(f);
        int pdivy = pdivx;

        fread(buffer, 1, 30, f);    map->metaData["name"] = buffer;

        bool showName = fgetc(f) != 0;   map->metaData["showname"] = showName ? "true" : "false";
        bool saveFlag = fgetc(f) != 0;   map->metaData["saveflag"] = saveFlag ? "true" : "false";
        map->metaData["startx"] = ToString(fgetw(f));
        map->metaData["starty"] = ToString(fgetw(f));
        map->metaData["hide"] = ToString(fgetc(f));
        map->metaData["warp"] = ToString(fgetc(f));

        int xsize = fgetw(f);
        int ysize = fgetw(f);

        map->width = xsize * 16;
        map->height = ysize * 16;

        fgetc(f); // toss compression flag

        fseek(f, 27, SEEK_CUR); // "skip buffer" (beats me)
        ScopedArray<u16> lay0(new u16[xsize * ysize]);
        ScopedArray<u16> lay1(new u16[xsize * ysize]);
        ScopedArray<u8>  obs(new u8[xsize * ysize]);
        fread(lay0.get(), sizeof(u16), xsize * ysize, f);
        fread(lay1.get(), sizeof(u16), xsize * ysize, f);
        fread(obs.get(),  sizeof(u8),  xsize * ysize, f);

        // copy 16 bit indeces into a 32 bit buffer
        ScopedArray<uint> layBuffer(new uint[xsize * ysize]);
        std::copy(lay0.get(), lay0.get() + xsize * ysize, layBuffer.get());
        Map::Layer* layer0 = new Map::Layer("BG", xsize, ysize);
        layer0->tiles = Matrix<uint>(xsize, ysize, layBuffer.get());

        // FIXME: sometimes we want the obstructions on layer 1, not 0
        layer0->obstructions = Matrix<u8>(xsize, ysize, obs.get());
        
        std::copy(lay1.get(), lay1.get() + xsize * ysize, layBuffer.get());
        Map::Layer* layer1 = new Map::Layer("FG", xsize, ysize);
        layer1->tiles = Matrix<uint>(xsize, ysize, layBuffer.get());

        map->AddLayer(layer0);
        map->AddLayer(layer1);

        struct VergeZone
        {
            char name[15];
            u16  eventIndex;
            u8   chance;
            u8   delay;
            u8   adjacentActivate;
            char description[30];
        };

        // TODO: do something useful with the zone stuff
        fseek(f, sizeof(VergeZone) * 128, SEEK_CUR);

        std::fill(buffer, buffer + 256, 0);
        std::vector<std::string> chrlist;
        for (int i = 0; i < 100; i++)
        {
            fread(buffer, 1, 13, f);
            chrlist.push_back(std::string(buffer));
        }

        int numEntities = 0;
        fread(&numEntities, 1, 4, f);

        struct VergeEntity
        {
            u16 x;       
            u16 y;       
            u8 direction;
            u8 moveDir;  
            u8 moveCount;
            u8 frameCount;
            u8 specFrame;
            u8 spriteIndex;
            u8 moveCode;
            u8 adjacentActivate;
            u8 ghost;
            uint actScriptIndex;
            uint moveScriptIndex;
            u8 speed;
            u8 speedCount;

            u16 data[8]; // hell if I know
            u16 boundingBox[4];
            u8 curCommand;
            u8 commandArg;
            u8* scriptofs;
            u8 facePlayer;
            u8 chasePlayer;
            u8 chaseSpeed;
            u8 chaseDist;
            u16 tileX;
            u16 tileY;
            int omfg;
            char description[20];                 // Editing description
        };

        for (int i = 0; i < numEntities; i++)
        {
            VergeEntity vergeEnt;
            Map::Entity ikaEnt;

            fread(&vergeEnt, 1, 88, f); // don't ask where the 88 comes from :P
            ikaEnt.label = va("Ent%i", i);
            ikaEnt.x = vergeEnt.x * 16;
            ikaEnt.y = vergeEnt.y * 16;
            ikaEnt.direction = (Direction)vergeEnt.direction;
            ikaEnt.spriteName = chrlist[vergeEnt.spriteIndex];
            ikaEnt.obstructedByEntities =
                ikaEnt.obstructedByMap =
                ikaEnt.obstructsEntities = !vergeEnt.ghost;
            ikaEnt.activateScript = va("Event%i", vergeEnt.actScriptIndex);
            layer0->entities.push_back(ikaEnt);
        }

        u8 numMoveScripts = fgetc(f);
        int thingie = fgetq(f);
        fseek(f, numMoveScripts, SEEK_CUR); // skip movescripts
        fseek(f, thingie, SEEK_CUR);    // and other shit (who knows what the hell it is)

        fclose(f);
        return map;
    }
    catch (...)
    {
        fclose(f);
        throw;
    }
}

Map* ImportVerge2Map(const std::string& fileName)
{
    FILE* f = fopen(fileName.c_str(), "rb");
    if (!f)
        throw std::runtime_error(va("Unable to open %s", fileName.c_str()));

    Map* map = new Map;

    try
    {
        const char* mapsig="MAPù5";
        char buffer[256];

        std::fill(buffer, buffer + 256, 0);

        fread(buffer, 1, 6, f);
        if (std::string(buffer) != mapsig)
            throw "Bogus map file";

        fseek(f, 4, SEEK_CUR);

        fread(buffer, 1, 60, f);    map->tileSetName = buffer;
        fread(buffer, 1, 60, f);    map->metaData["music"] = buffer;
        fread(buffer, 1, 20, f);    map->metaData["rstring"] = buffer;

        fseek(f, 55, SEEK_CUR);

        int numLayers = fgetc(f);
        int width = 0;
        int height = 0;

        std::vector<int> pmulx(numLayers), pmuly(numLayers);
        std::vector<int> pdivx(numLayers), pdivy(numLayers);

        for (int i = 0; i < numLayers; i++)
        {
            pmulx.push_back(fgetc(f));
            pdivx.push_back(fgetc(f));
            pmuly.push_back(fgetc(f));
            pdivy.push_back(fgetc(f));
            width = fgetw(f);
            height = fgetw(f);
            fgetc(f); // trans
            fgetc(f); // hline
            fgetw(f); // skip two
        }

        map->width = width * 16;
        map->height = height * 16;

        for (int i = 0; i < numLayers; i++)
        {
            u32 bufSize = fgetq(f);
            ScopedArray<u16> buffer(new u16[bufSize]);
            ScopedArray<u32> data(new u32[width * height]);
            fread(buffer.get(), 1, bufSize, f);
            ReadCompressedLayer2tou32(data.get(), width * height, buffer.get());
            Map::Layer* lay = new Map::Layer(va("Layer%i", i), width, height);

            lay->tiles = Matrix<uint>(width, height, data.get());
            lay->parallax.mulx = pmulx[i];
            lay->parallax.muly = pmuly[i];
            lay->parallax.divx = pdivx[i];
            lay->parallax.divy = pdivy[i];

            if (lay->parallax.divx == 0)
            {
                lay->parallax.mulx = 1;
                lay->parallax.divx = 1;
            }

            if (lay->parallax.divy == 0)
            {
                lay->parallax.muly = 1;
                lay->parallax.divy = 1;
            }

            map->AddLayer(lay);
        }

        // obstructions
        uint bufSize = fgetq(f);
        ScopedArray<u8> rleBuffer(new u8[bufSize]);
        ScopedArray<u8> obsData(new u8[width * height]);
        fread(rleBuffer.get(), 1, bufSize, f);
        ReadCompressedLayer1(obsData.get(), width * height, rleBuffer.get());
        if (map->NumLayers())
            map->GetLayer(0)->obstructions = Matrix<u8>(width, height, obsData.get());

        // zones
        bufSize = fgetq(f);
        fseek(f, bufSize, SEEK_CUR); // TODO: use this data

        struct Verge2Zone
        {
            char name[40];       
            u16 scriptIndex;          
            u16 chance;         
            u16 delay;           
            u16 adjacentActivate;             
            u16 entityScript;    
        };

        uint numZones = fgetq(f);
        fseek(f, numZones * sizeof(Verge2Zone), SEEK_CUR); // TODO: use this too

        std::fill(buffer, buffer + 255, 0);
        int numSprites = fgetc(f);
        std::vector<std::string> chrList(numSprites);
        for (int i = 0; i < numSprites; i++)
        {
            fread(buffer, 1, 60, f);
            chrList.push_back(buffer);
        }

        int numEntities = fgetc(f);
        for (int i = 0; i < numEntities; i++)
        {
            Map::Entity ent;

            ent.x = fgetq(f) * 16;
            ent.y = fgetq(f) * 16;
            fgetq(f); // toss tile x/y

            fgetc(f); // toss move direction
            ent.direction = (Direction)fgetc(f);
            fgetc(f); // movecount
            fgetc(f); // curframe
            fgetc(f); // specframe
            int chrIndex = fgetc(f);
            ent.spriteName = chrList[chrIndex];

            fgetc(f); // reset?

            ent.obstructedByEntities = ent.obstructedByMap = fgetc(f) != 0;
            ent.obstructsEntities = fgetc(f) != 0;

            const int speedxlat[] =
            { 100, 12, 25, 50, 100, 200, 400, 800, 800, 800, 800, 800 };
            ent.speed = speedxlat[fgetc(f)];

            fgetc(f); // speedcount
            fgetc(f); // animation frame delay?
            fgetq(f); // anim script index
            fgetq(f); // move script index
            fgetc(f); // autoface
            fgetc(f); // adjacent activation
            fgetc(f); // move code
            fgetc(f); // move script
            fgetc(f); // subtile move count? :P
            fgetc(f); // more runtime internal junk

            fgetw(f); // wander step count
            fgetw(f); // wander delay
            fgetw(f); // ???
            fgetw(f); // @_@;

            fseek(f, 14, SEEK_CUR); // skip wander rect and some other stuff

            ent.activateScript = va("Event%i", fgetw(f));

            fseek(f, 18, SEEK_CUR); // ???

            fread(buffer, 1, 20, f);
            ent.label = buffer; // what the hell

            map->GetLayer(0)->entities.push_back(ent);

        }

        // skip move scripts and "things" (which were never implemented anyway)

        fclose(f);
        return map;
    }
    catch (...)
    {
        fclose(f);
        throw;
    }
}

static void DecompressVerge3(void* dest, int outSize, FILE* f)
{
    int uncompressedSize = fgetq(f);
    int compressedSize = fgetq(f);

    if (uncompressedSize != outSize)
        throw std::runtime_error(va("Compression block sizes do not match. "
            "(%i vs %i)", compressedSize, outSize));

    uLong zsize = outSize;
    ScopedArray<u8> cdata(new u8[compressedSize]);

    fread(cdata.get(), 1, compressedSize, f);
    int result = uncompress(reinterpret_cast<Bytef*>(dest), &zsize, cdata.get(), compressedSize);
    if (result != Z_OK)
        throw std::runtime_error(va("DecompressVerge3 returned zlib error %i", result));
}

static void CompressVerge3(void* src, int size, FILE* f)
{
    uLongf bufferSize = size * 101 / 100 + 12;    // 0.1% larger than source, plus 12 bytes. (we splurge and give it a whole 1%)
    ScopedArray<u8> buffer = new u8[bufferSize];

    int result = compress((Bytef*)buffer.get(), &bufferSize, (Bytef*)src, size);
    if (result != Z_OK)
        throw std::runtime_error(va("CompressVerg3 returned zlib error %i", result));

    fputq(size, f);
    fputq(bufferSize, f);
    fwrite(buffer.get(), 1, bufferSize, f);
}


static const char* verge3MapSig = "V3MAP";
static const int VERGE3_MAP_VERSION = 2;

Map* ImportVerge3Map(const std::string& fileName)
{
    FILE* f = fopen(fileName.c_str(), "rb");
    if (!f)
        return 0;

    try
    {
        Map* map = new Map;

        char buffer[256];

        fread(buffer, 1, 6, f);
        if (std::string(buffer) != verge3MapSig)
            throw std::runtime_error("Bogus map signature");

        int version = fgetq(f);
        if (version != VERGE3_MAP_VERSION)
            throw std::runtime_error(va("Invalid map version %i (looking for %i)", version, VERGE3_MAP_VERSION));

        fseek(f, 4, SEEK_CUR); // skip vc offset

        fread(buffer, 1, 256, f);   map->title = buffer;
        fread(buffer, 1, 256, f);   map->tileSetName = buffer;
        fread(buffer, 1, 256, f);   map->metaData["music"] = buffer;
        fread(buffer, 1, 256, f);   map->metaData["rstring"] = buffer;
        fread(buffer, 1, 256, f);   map->metaData["startupscript"] = buffer;

        map->metaData["startx"] = ToString(fgetw(f));
        map->metaData["starty"] = ToString(fgetw(f));

        int numLayers = fgetq(f);
        for (int i = 0; i < numLayers; i++)
        {
            Map::Layer* lay = new Map::Layer;
            fread(buffer, 1, 256, f);   lay->label = buffer;
            assert(sizeof(double) == 8);
            // TODO: convert to a fraction somehow. #_#
            double parallaxX;   fread(&parallaxX, 1, 8, f);
            double parallaxY;   fread(&parallaxY, 1, 8, f);
            int width = fgetw(f);
            int height = fgetw(f);
            int lucent = fgetc(f);

            ScopedArray<u16> data(new u16[width * height]);
            DecompressVerge3(data.get(), width * height * 2, f);

            ScopedArray<u32> data32(new u32[width * height]);
            std::copy(data.get(), data.get() + width * height, data32.get());

            lay->tiles = Matrix<u32>(width, height, data32.get());
            map->AddLayer(lay);
        }

        int width = map->GetLayer(0)->Width();
        int height = map->GetLayer(0)->Height();
        map->width = width * 16;
        map->height = height * 16;

        ScopedArray<u8> obsData(new u8[width * height]);
        DecompressVerge3(obsData.get(), width * height, f);
        map->GetLayer(0)->obstructions = Matrix<u8>(width, height, obsData.get());

        ScopedArray<u16> zoneData(new u16[width * height]);
        DecompressVerge3(zoneData.get(), width * height * 2, f);

        fclose(f);

        // Postprocessing -- figure out where the entities and obstructions belong.

        return map;
    }
    catch (...)
    {
        fclose(f);
        throw;
    }
}

VSP* ImportVerge3TileSet(const std::string& fileName)
{
    const char* VSP_SIGNATURE = "VSP";
    const int VSP_VERSION = 6;
    enum
    {
        BPP24 = 1,
        BPP32 = 2
    };

    enum
    {
        NO_COMPRESSION = 0,
        ZLIB_COMPRESSION = 1,
    };

    enum
    {
        ANIM_FORWARD = 0,
        ANIM_BACKWARD = 1,
        ANIM_RANDOM = 2,
        ANIM_PINGPONG = 3
    };

    FILE* f = fopen(fileName.c_str(), "rb");
    if (!f)
        throw std::runtime_error(va("Unable to load %s", fileName.c_str()));

    VSP* vsp = new VSP;

    char buffer[256];
    std::fill(buffer, buffer + 256, 0);

    fread(buffer, 1, 4, f);
    if (std::string(buffer) != VSP_SIGNATURE)
        throw std::runtime_error("VSP signature ain't there.  This isn't no map.");

    int version = fgetq(f);
    if (version != VSP_VERSION)
        throw std::runtime_error(va("Version should have been %i.  It was %i.  This not good.", VSP_VERSION, version));

    int tileWidth = fgetq(f);
    int tileHeight = tileWidth;
    vsp->SetSize(tileWidth, tileHeight);

    int format = fgetq(f);
    int numTiles = fgetq(f);
    int compression = fgetq(f);

    if (compression != ZLIB_COMPRESSION)
        throw std::runtime_error("I was unaware than Verge3 VSPs had any compression other than zlib.");

    ScopedArray<RGBA> pixels(new RGBA[numTiles * tileWidth * tileHeight]);

    if (format == BPP24)
    {
        ScopedArray<u8> rgb(new u8[numTiles * tileWidth * tileHeight * 3]);
        DecompressVerge3(rgb.get(), numTiles * tileWidth * tileHeight * 3, f);
        u8* p = rgb.get();
        for (int i = 0; i < numTiles * tileWidth * tileHeight; i++)
        {
            u8 r = *p++;
            u8 g = *p++;
            u8 b = *p++;
            u8 a = (r == 255 && g == 0 && b == 255) ? 0 : 255;
            pixels[i] = RGBA(r, g, b, a);
        }
    }
    else if (format == BPP32)
    {
        DecompressVerge3(pixels.get(), 
            numTiles * tileWidth * tileHeight * sizeof(u32), f);
    }
    else
        assert(false);

    RGBA* p = pixels.get();
    for (int i = 0; i < numTiles; i++)
    {
        Canvas c(p, tileWidth, tileHeight);
        
        vsp->AppendTile();
        vsp->PasteTile(c, vsp->NumTiles() - 1);

        p += tileWidth * tileHeight;
    }

    int numAnimStrands = fgetq(f);
    for (int i = 0; i < numAnimStrands; i++)
    {
        VSP::AnimState ikaAnim;

        fread(buffer, 1, 256, f); // discard strand name
        ikaAnim.start = fgetq(f);
        ikaAnim.finish = fgetq(f);
        ikaAnim.delay = fgetq(f);
        ikaAnim.mode = fgetq(f);

        vsp->vspAnim.push_back(ikaAnim);
    }

    int numObsTiles = fgetq(f);
    ScopedArray<u8> obsPix(new u8[numObsTiles * tileWidth * tileHeight]);
    DecompressVerge3(obsPix.get(), numObsTiles * tileWidth * tileHeight, f);

    // TODO: use this. :)

    fclose(f);
    return vsp;
}

void ExportVerge3Map(const std::string& fileName, Map* map)
{
    FILE* f = fopen(fileName.c_str(), "wb");
    if (!f)
        throw std::runtime_error(va("Unable to open %s for writing", fileName.c_str()));

    fwrite(verge3MapSig, 1, 6, f);
    fputq(VERGE3_MAP_VERSION, f);
    fputq(0, f); // vc offset

    std::string buffer;
    buffer.reserve(256);
    
    buffer = map->title;           fwrite(buffer.c_str(), 1, 256, f);
    buffer = map->tileSetName;     fwrite(buffer.c_str(), 1, 256, f);
    buffer = map->metaData.count("music") ? map->metaData["music"] : "";     
    fwrite(buffer.c_str(), 1, 256, f);
    if (map->metaData.count("rstring")) buffer = map->metaData["rstring"];
    else
    {
        buffer.clear();
        for (uint i = 0; i < buffer.size(); i++)
            buffer += '1' + i;
        buffer += "ER";
    }
    fwrite(buffer.c_str(), 1, 256, f);
    buffer = map->metaData.count("startupscript") ? map->metaData["startupscript"] : "";     
    fwrite(buffer.c_str(), 1, 256, f);

    fputw(0, f); // startx
    fputw(0, f); // starty

    fputq(map->NumLayers(), f);
    for (uint i = 0; i < map->NumLayers(); i++)
    {
        Map::Layer* lay = map->GetLayer(i);
        buffer = lay->label;
        fwrite(buffer.c_str(), 1, 256, f);

        double parallaxDummy = 1.0;
        fwrite(&parallaxDummy, 1, 8, f);
        fwrite(&parallaxDummy, 1, 8, f);

        fputw(lay->Width(), f);
        fputw(lay->Height(), f);
        fputc(0, f); // lucent

        ScopedArray<u16> data16 = new u16[lay->Width() * lay->Height()];
        std::copy(lay->tiles.GetPointer(0, 0), lay->tiles.GetPointer(0, 0) + lay->Width() * lay->Height(), data16.get());
        CompressVerge3(data16.get(), lay->Width() * lay->Height() * sizeof(u16), f);
    }

    // for now, don't write any obstruction or zone data
    int width = map->GetLayer(0)->Width();
    int height = map->GetLayer(0)->Height();
    ScopedArray<u8> obsData = new u8[width * height];
    std::fill(obsData.get(), obsData.get() + width * height, 0);
    CompressVerge3(obsData.get(), width * height, f);

    // zone data too
    CompressVerge3(obsData.get(), width * height, f);

    fclose(f);
}
