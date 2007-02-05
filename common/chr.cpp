#include <cassert>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

#include "aries.h"
#include "base64.h"
#include "chr.h"
#include "compression.h"
#include "fileio.h"
#include "common/log.h"
#include "oldbase64.h"
#include "rle.h"
#include "utility.h"
#include "vergepal.h"

using aries::NodeList;
using aries::DataNodeList;
using aries::DataNode;
using aries::newNode;

namespace {
    const char* scriptNames[] = {
        "idle_left",
        "idle_right",
        "idle_up",
        "idle_down",
        "idle_upleft",
        "idle_upright",
        "idle_downleft",
        "idle_downright"
    };
    const int scriptCount = sizeof scriptNames / sizeof scriptNames[0];
}

CCHRfile::CCHRfile(int width, int height)
    : _hotspotX(0), _hotspotY(0)
    , _hotspotWidth(16), _hotspotHeight(16)
    , _width(width), _height(height)
{
    _frame.clear();
    //moveScripts.resize(16); // egh
    AppendFrame();
}

void CCHRfile::ClearFrames() {
    for (uint i = 0; i < _frame.size(); i++) {
        delete _frame[i];
    }
    _frame.clear();
}

Canvas& CCHRfile::GetFrame(uint frame) const {
    static Canvas dummy;
    
    if (frame < 0 || frame >= _frame.size()) {
        return dummy;
    }
    
    return *_frame[frame];
}

void CCHRfile::UpdateFrame(const Canvas& newData, uint frame) {
    if (0 <= frame && frame < _frame.size()) {
        *_frame[frame] = newData;
    }
}

void CCHRfile::AppendFrame() {
    _frame.push_back(new Canvas(_width, _height));
}

void CCHRfile::AppendFrame(Canvas& canvas) {
    _frame.push_back(new Canvas(canvas));
}

void CCHRfile::AppendFrame(Canvas* canvas) {
    _frame.push_back(new Canvas(*canvas));
}

void CCHRfile::InsertFrame(uint index) {
    if (index >= _frame.size() || !_frame.size()) {
        AppendFrame();
        return;
    }

    if (index < 0) {
        index = 0;
    }

    _frame.insert(_frame.begin() + index, new Canvas(_width, _height));
}

void CCHRfile::InsertFrame(uint index, Canvas& canvas) {
    if (index < 0) {
        index = 0;
    }

    if (index >= _frame.size()) {
        _frame.push_back(new Canvas(canvas));
        return;
    }

    _frame.insert(_frame.begin() + index, new Canvas(canvas));
}

void CCHRfile::InsertFrame(uint index, Canvas* canvas) {
    if (index < 0) {
        index = 0;
    }

    if (index >= _frame.size()) {
        AppendFrame(canvas);
        return;
    }

    _frame.insert(_frame.begin() + index, new Canvas(*canvas));
}

void CCHRfile::DeleteFrame(uint index) {
    if (index < 0 || index >= _frame.size()) {
        return;
    }

    // Nuke the _frame
    delete _frame[index];

    // Move the other frames back one
    for (uint i = index; i < _frame.size(); i++) {
        _frame[i] = _frame[i + 1];
    }

    // Remove the last pointer in the list
    _frame.pop_back();
}

void CCHRfile::Resize(int width, int height) {
    if (width < 0 || height < 0) {
        throw std::runtime_error(va("CHRfile::Resize: Invalid dimensions %i, %i", width, height));
    }

    for (uint i = 0; i < _frame.size(); i++) {
        _frame[i]->Resize(width, height);
    }

    _width = width;
    _height = height;
}

void CCHRfile::New(int frameX, int frameY) {
    _width = frameX;
    _height = frameY;
    _hotspotX = 0;
    _hotspotY = 0;
    _hotspotWidth = frameX;
    _hotspotHeight = frameY;
    ClearFrames();
}

void CCHRfile::Load(const std::string& filename) {
    moveScripts.clear();

    // first, a quick check for loading older file formats.
    if (Path::equals("chr", Path::getExtension(filename))) {
        LoadCHR(filename);
        return;
    }

    try {
        if (!filename.length()) {
            throw std::runtime_error("LoadCHR: No filename given.");
        }

        std::ifstream file;
        file.open(filename.c_str());
        if (!file.is_open()) {
            throw std::runtime_error(va("LoadCHR: %s does not exist.", filename.c_str()));
        }

        DataNode* document;
        file >> document;
        file.close();

        DataNode* rootNode = document->getChild("ika-sprite");

        std::string ver = rootNode->getChild("version")->getString();

        if (ver == "1.0" || ver == "1.1") {
            Log::Write(
                "Warning: v%s sprite being loaded.\n"
                "         It should work, but you should load and resave \n"
                "         %s in iked to update it to 1.2!\n",
                ver.c_str(),
                filename.c_str()
            );

        } else if (ver != "1.2") {
            Log::Write(
                "Warning: Unknown ika-sprite version %s in %s!  Expecting\n"
                "         1.0 through 1.2.  Trying to load it anyway.",
                ver.c_str(), filename.c_str()
            );
        }

        {
            DataNode* infoNode = rootNode->getChild("information");

            {
                DataNode* metaNode = infoNode->getChild("meta");

                NodeList nodes = metaNode->getChildren();
                for (NodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                    if ((*iter)->isString()) {
                        continue;
                    }

                    DataNode* n = static_cast<DataNode*>(*iter);

                    if (n->getString().empty()) {
                        metaData[n->getName()] = n->getString();
                    }
                }
            }
        }

        {
            // Header stuff?  All there is now is depth, which is always 32 anyway.
            //DataNode* headerNode = rootNode->getChild("header");
        }

        {
            DataNode* scriptNode = rootNode->getChild("scripts");

            DataNodeList nodes = scriptNode->getChildren("script");
            for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                std::string name = (*iter)->getChild("label")->getString();
                std::string script = (*iter)->getString();

                // FIXME: script name not actually used!
                moveScripts[name] = script;
                //moveScripts.push_back(script);
            }
        }

        {
            DataNode* frameNode = rootNode->getChild("frames");

            uint frameCount = (uint)std::atoi(frameNode->getChild("count")->getString().c_str());

            DataNode* dimNode = frameNode->getChild("dimensions");
            _width = std::atoi(dimNode->getChild("width")->getString().c_str());
            _height = std::atoi(dimNode->getChild("height")->getString().c_str());
            
            DataNode* hsNode = frameNode->getChild("hotspot");
            _hotspotX = std::atoi(hsNode->getChild("x")->getString().c_str());
            _hotspotY = std::atoi(hsNode->getChild("y")->getString().c_str());
            _hotspotWidth = std::atoi(hsNode->getChild("width")->getString().c_str());
            _hotspotHeight = std::atoi(hsNode->getChild("height")->getString().c_str());

            DataNode* dataNode = frameNode->getChild("data");
            if (dataNode->getChild("format")->getString() != "zlib") {
                throw std::runtime_error("Unsupported data format");
            }

            std::string d64 = dataNode->getString();
            ScopedArray<u8> compressed(new u8[d64.length()]); // way more than enough.
            uint compressedSize;
            if (ver == "1.0") {
                compressedSize = oldBase64::decode(d64, compressed.get(), d64.length());
            } else {
                std::string un64 = base64::decode(d64);
                std::copy((u8*)(un64.c_str()), (u8*)(un64.c_str() + un64.length()), compressed.get());
                compressedSize = un64.length();
            }

            ScopedArray<u8> pixels(new u8[_width * _height * frameCount * sizeof(RGBA)]);
            Compression::decompress(compressed.get(), compressedSize, pixels.get(), _width * _height * frameCount * sizeof(RGBA));

            ClearFrames();
            _frame.reserve(frameCount);
            RGBA* ptr = (RGBA*)pixels.get();
            for (uint i = 0; i < frameCount; i++) {
                _frame.push_back(new Canvas(ptr, _width, _height));
                ptr += _width * _height;
            }
        }

        delete rootNode;
    }
    catch (std::runtime_error error) {
        Log::Write("LoadCHR(\"%s\"): %s", filename.c_str(), error.what());
        throw error;
     }
}

void CCHRfile::Save(const std::string& filename) {
    if (Path::equals("chr", Path::getExtension(filename))) {
        SaveOld(filename);
        return;
    }

    DataNode* rootNode = newNode("ika-sprite");
    rootNode->addChild(newNode("version")->addChild("1.1"));

    DataNode* infoNode = newNode("information");
    rootNode->addChild(infoNode);
    infoNode->addChild(newNode("title")->addChild("Untitled")); // FIXME
    
    {
        DataNode* metaNode = newNode("meta");
        infoNode->addChild(metaNode);

        for (std::map<std::string, std::string>::iterator iter = metaData.begin();
             iter != metaData.end(); iter++
        ) {
            metaNode->addChild(newNode(iter->first)->addChild(iter->second));
        }
    }

    rootNode->addChild(newNode("header")
        ->addChild(newNode("depth")->addChild("32"))
        );

    {
        DataNode* scriptNode = newNode("scripts");
        rootNode->addChild(scriptNode);

        for (StringMap::iterator iter = moveScripts.begin();
             iter != moveScripts.end(); iter++
        ) {
            scriptNode->addChild(
                newNode("script")->addChild(
                    newNode("label")->addChild(iter->first)
                    )
                    ->addChild(iter->second)
                );
        }
    }

    {
        DataNode* frameNode = newNode("frames");
        rootNode->addChild(frameNode);

        frameNode
            ->addChild(newNode("count")->addChild(_frame.size()))
            ->addChild(newNode("dimensions")
                ->addChild(newNode("width")->addChild(_width))
                ->addChild(newNode("height")->addChild(_height))
                )
            ->addChild(newNode("hotspot")
                ->addChild(newNode("x")->addChild(_hotspotX))
                ->addChild(newNode("y")->addChild(_hotspotY))
                ->addChild(newNode("width")->addChild(_hotspotWidth))
                ->addChild(newNode("height")->addChild(_hotspotHeight))
                );

        const uint uncompressedBlockSize = _width * _height * _frame.size() * sizeof(RGBA);
        // More than is needed.  Way more.
        const uint compressedBlockSize = uncompressedBlockSize * 4 / 3;

        ScopedArray<u8> uncompressed(new u8[uncompressedBlockSize]);
        ScopedArray<u8> compressed(new u8[compressedBlockSize]);

        // Pack the uncompressed data into one big block.
        RGBA* dest = reinterpret_cast<RGBA*>(uncompressed.get());
        for (uint i = 0; i < _frame.size(); i++) {
            RGBA* src = _frame[i]->GetPixels();
            memcpy(dest, src, _width * _height * sizeof(RGBA));
            
            dest += _width * _height;
        }

        // Compress.
        uint compressSize = Compression::compress(uncompressed.get(), uncompressedBlockSize,
                                                  compressed.get(), compressedBlockSize);

        // base64
        //std::string d64 = base64::encode(compressed.get(), compressSize);
        std::string d64 = base64::encode(std::string(compressed.get(),
                                                     compressed.get() + compressSize));

        frameNode->addChild(newNode("data")
            ->addChild(newNode("format")->addChild("zlib"))
            ->addChild(d64));            
    }

    std::ofstream file(filename.c_str());
    file << rootNode;
    delete rootNode;
}

void CCHRfile::SaveOld(const std::string& filename) {
    File f;
    
    bool result = f.OpenWrite(filename.c_str());
    if (!result) {
        throw std::runtime_error(va("Failed to open %s for writing.", filename.c_str()));
    }
    
    f.Write(u8(5));                                              // version - u8
    
    f.Write(metaData["description"].c_str(), 64);                // desc    - 64 byte string
    
    f.Write(moveScripts.size());                                 // write the number of scripts
    
    for (uint i = 0; i < scriptCount; i++) {
        const std::string& script = moveScripts[scriptNames[i]];
        f.Write(script.length());                                // write the length
        f.Write(script.c_str(), script.length());                // write the actual script
    }
    
    // Write the _frame data
    f.Write((int)_frame.size());
    for (uint i = 0; i < _frame.size(); i++) {
        f.Write(_frame[i]->Width());
        f.Write(_frame[i]->Height());
        f.Write(_hotspotX);                                      // note that the current data structure does not support variable hotspots.  But the file format does. (potential expansion)
        f.Write(_hotspotY);
        f.Write(_hotspotWidth);
        f.Write(_hotspotHeight);
        
        f.WriteCompressed(
            _frame[i]->GetPixels(),
            _frame[i]->Width() * _frame[i]->Height() * sizeof(RGBA));
    }
    
    f.Close();
}

std::string CCHRfile::GetStandingScript(Direction dir) {
    return moveScripts[scriptNames[int(dir) + 8]];
}

std::string CCHRfile::GetWalkingScript(Direction dir) {
    return moveScripts[scriptNames[int(dir)]];
}

void CCHRfile::LoadCHR(const std::string& fileName) {
    File f;
    
    bool result = f.OpenRead(fileName.c_str());
    if (!result) {
        throw std::runtime_error(va("Unable to load %s.", fileName.c_str()));
    }
    
    char ver;
    f.Read(ver);
    
    switch (ver) {
        case 2:  return Loadv2CHR(f);
        case 4:  return Loadv4CHR(f);
        case 5:  return Loadv5CHR(f);
        default: throw std::runtime_error(va("Bogus version number %i", (int)ver));
    }
}

void CCHRfile::Loadv2CHR(File& f) {
    _width = _height = 0;
    _hotspotX = _hotspotY = 0;
    _hotspotWidth = _hotspotHeight = 0;
    
    f.Read(&_width, 2);
    f.Read(&_height, 2);
    f.Read(&_hotspotX, 2);
    f.Read(&_hotspotY, 2);
    f.Read(&_hotspotWidth, 2);
    f.Read(&_hotspotHeight, 2);
    
    u16 frameCount;
    f.Read(frameCount);
    
    int nBufsize;
    f.Read(nBufsize);
    u8* pCompbuf = new u8[nBufsize];
    u8* pUncompbuf = new u8[frameCount * _width * _height];
    f.Read(pCompbuf, nBufsize);
    ReadCompressedLayer1(pUncompbuf, frameCount * _width * _height, pCompbuf);
    delete[] pCompbuf;
    
    ClearFrames();
    _frame.resize(frameCount);
    u8* src = pUncompbuf;
    for (int nCurframe = 0; nCurframe < frameCount; nCurframe++) {
        _frame[nCurframe] = new Canvas(src, _width, _height, cVergepal);
        src += _width * _height;
    }
    
    moveScripts.clear();
    //moveScripts.resize(16);
    
    // Get the idle frames
    int i;
    f.Read(i); moveScripts["idle_left"]  = std::string("F") + toString(i) + "W10";
    f.Read(i); moveScripts["idle_right"] = std::string("F") + toString(i) + "W10";
    f.Read(i); moveScripts["idle_up"]    = std::string("F") + toString(i) + "W10";
    f.Read(i); moveScripts["idle_down"]  = std::string("F") + toString(i) + "W10";
    
    for (int b = 0; b < 4; b++) {
        char ptr[255];
        int n;
        f.Read(n);
        
        if (n > 99 || n < 0) {
            throw std::runtime_error(va("Bogus movescript length %i", n));
        }
        f.Read(ptr, n);
        ptr[n] = 0;  // terminating null
        
        switch (b) {
            case 0: moveScripts["walk_left"]  = ptr; break;
            case 1: moveScripts["walk_right"] = ptr; break;
            case 2: moveScripts["walk_up"]    = ptr; break;
            case 3: moveScripts["walk_down"]  = ptr; break;
        }
    }

    moveScripts["walk_upleft"]    = moveScripts["walk_left"];
    moveScripts["walk_downleft"]  = moveScripts["walk_left"];
    moveScripts["walk_upright"]   = moveScripts["walk_right"];
    moveScripts["walk_downright"] = moveScripts["walk_right"];

    moveScripts["idle_upleft"]    = moveScripts["idle_left"];
    moveScripts["idle_downleft"]  = moveScripts["idle_left"];
    moveScripts["idle_upright"]   = moveScripts["idle_right"];
    moveScripts["idle_downright"] = moveScripts["idle_right"];
}

void CCHRfile::Loadv4CHR(File& f) {
    // VERGE v2 chrs store two bytes for these, but ika has four.
    // Must nuke the high bits or anality will ensue.
    _width = _height = 0;
    _hotspotX = _hotspotY = 0;
    _hotspotWidth = _hotspotHeight = 0;
    
    f.Read(&_width, 2);
    f.Read(&_height, 2);
    f.Read(&_hotspotX, 2);
    f.Read(&_hotspotY, 2);
    f.Read(&_hotspotWidth, 2);
    f.Read(&_hotspotHeight, 2);
    
    //moveScripts.resize(16);
    moveScripts.clear();
    
    // Get the idle frames
    u16 i;
    f.Read(i); moveScripts["idle_left"]  = std::string("F") + toString(i) + "W10";
    f.Read(i); moveScripts["idle_right"] = std::string("F") + toString(i) + "W10";
    f.Read(i); moveScripts["idle_up"]    = std::string("F") + toString(i) + "W10";
    f.Read(i); moveScripts["idle_down"]  = std::string("F") + toString(i) + "W10";
    
    f.Read(i);
    int frameCount = i;                // Frame count.
    
    for (int b = 0; b < 4; b++) {
        char ptr[255];
        int n;
        f.Read(n);
        
        if (n > 99) {
            throw std::runtime_error(va("Bogus movescript length %i", n));
        }

        f.Read(ptr, n);
        ptr[n] = 0;                    // Terminating null
        
        switch (b) {
            case 0: moveScripts["walk_left"]  = ptr; break;
            case 1: moveScripts["walk_right"] = ptr; break;
            case 2: moveScripts["walk_up"]    = ptr; break;
            case 3: moveScripts["walk_down"]  = ptr; break;
        }
    }

    moveScripts["walk_upleft"]    = moveScripts["walk_left"];
    moveScripts["walk_downleft"]  = moveScripts["walk_left"];
    moveScripts["walk_upright"]   = moveScripts["walk_right"];
    moveScripts["walk_downright"] = moveScripts["walk_right"];

    moveScripts["idle_upleft"]    = moveScripts["idle_left"];
    moveScripts["idle_downleft"]  = moveScripts["idle_left"];
    moveScripts["idle_upright"]   = moveScripts["idle_right"];
    moveScripts["idle_downright"] = moveScripts["idle_right"];
    
    int n;
    f.Read(n);
    char* ptr = new char[n];
    
    f.Read(ptr, n);

    u16* pTemp = new u16[_width * _height * frameCount];
    ReadCompressedLayer2(pTemp, _width * _height * frameCount, (u16*)ptr);
    delete[] ptr;
    
    ClearFrames();
    _frame.resize(frameCount);
    
    // adjust to 32bpp
    RGBA* p = new RGBA[_width * _height];
    for (int nCurframe = 0; nCurframe < frameCount; nCurframe++) {
        for (int n = 0; n < _width * _height; n++) {
            u16 u = pTemp[nCurframe * _width * _height + n];
            u8 r = ((u >> 11) & 31) << 3;
            u8 g = ((u >> 5) & 63) << 2;
            u8 b = (u & 31) << 3;
            u8 a = ((!r && !g && !b) || (r == 255 && !g && b == 255)) ? 0 : 255;
            p[n]=RGBA(r, g, b, a);
        }
        
        _frame[nCurframe] = new Canvas(p, _width, _height);
    }
    
    delete[] pTemp;
    delete[] p;
}

void CCHRfile::Loadv5CHR(File& f) {
    // Load the new format
    moveScripts.clear();
    
    char s[65];
    f.Read(s, 64);
    s[64] = 0;
    metaData["description"] = s;
    
    int scriptCount;
    f.Read(scriptCount);

    for (int i = 0; i < scriptCount && i < scriptCount; i++) {
        int length;
        f.Read(length);

        ScopedArray<char> s(new char[length + 1]);
        f.Read(s.get(), length);
        s[length] = 0;
        moveScripts[scriptNames[i]] = s.get();
    }
    
    // Get the _frame data
    int frameCount;
    f.Read(frameCount);
    ClearFrames();
    _frame.reserve(frameCount);
    for (int i = 0; i < frameCount; i++) {
        int x, y;
        f.Read(x);
        f.Read(y);
        f.Read(_hotspotX);
        f.Read(_hotspotY);
        f.Read(_hotspotWidth);
        f.Read(_hotspotHeight);

        ScopedArray<RGBA> pixels(new RGBA[x * y]);
        f.ReadCompressed(pixels.get(), x * y * sizeof(RGBA));
        
        _frame.push_back(new Canvas(pixels.get(), x, y));
    }

    f.Close();
}
