#include <cassert>
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

#include <fstream>
#include <stdexcept>

using aries::NodeList;
using aries::DataNodeList;
using aries::DataNode;
using aries::newNode;

namespace
{
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
    const int numScripts = sizeof scriptNames / sizeof scriptNames[0];
}

CCHRfile::CCHRfile(int width, int height)
{
    nHotx = nHoty = 0;
    nHotw = nHoth = 16;
    nWidth = width;
    nHeight = height;
    frame.clear();
    //moveScripts.resize(16); // egh
    AppendFrame();
}

CCHRfile::~CCHRfile()
{
    ClearFrames();
}

void CCHRfile::ClearFrames()
{
    for (uint i = 0; i < frame.size(); i++)
        delete frame[i];
    frame.clear();
}

Canvas& CCHRfile::GetFrame(uint nFrame) const
{
    static Canvas dummy;
    
    if (nFrame < 0 || nFrame >= frame.size())
        return dummy;
    
    return *frame[nFrame];
}

void CCHRfile::UpdateFrame(const Canvas& newdata, uint nFrame)
{
    if (nFrame < 0 || nFrame >= frame.size())
        return;

    *frame[nFrame] = newdata;
}

void CCHRfile::AppendFrame()
{
    frame.push_back(new Canvas(nWidth, nHeight));
}

void CCHRfile::InsertFrame(uint idx)
{
    if (idx >= frame.size() || !frame.size())
    {
        AppendFrame();
        return;
    }
    
    if (idx < 0)
        idx = 0;
    
    frame.insert(frame.begin() + idx, new Canvas(nWidth, nHeight));
}

void CCHRfile::InsertFrame(uint idx, Canvas& p)
{
    if (idx < 0)
        idx = 0;
    if (idx >= frame.size())
    {
        frame.push_back(new Canvas(p));
        return;
    }
    
    frame.insert(frame.begin() + idx, new Canvas(p));
}

void CCHRfile::DeleteFrame(uint idx)
{
    if (idx < 0 || idx >= frame.size())
        return;

    // Nuke the frame
    delete frame[idx];
    
    // Move the other frames back one
    for (uint i = idx; i < frame.size(); i++)
        frame[i]=frame[i + 1];
    
    // Remove the last pointer in the list
    frame.pop_back();
}

void CCHRfile::Resize(int width, int height)
{
    if (width < 0 || height < 0)
        throw std::runtime_error(va("CHRfile::Resize: Invalid dimensions %i, %i", width, height));

    for (uint i = 0; i < frame.size(); i++)
        frame[i]->Resize(width, height);

    nWidth = width;
    nHeight = height;
}

void CCHRfile::New(int framex, int framey)
{
    nWidth = framex;
    nHeight = framey;
    nHotx = 0;        nHoty = 0;
    nHotw = framex;    nHoth = framey;
    ClearFrames();
}

void CCHRfile::Load(const std::string& fname)
{
    moveScripts.clear();

    // first, a quick check for loading older file formats.
    if (Path::equals("chr", Path::getExtension(fname))) {
        LoadCHR(fname);
        return;
    }

    try {
        if (!fname.length()) {
            throw std::runtime_error("LoadCHR: No filename given.");
        }

        std::ifstream file;
        file.open(fname.c_str());
        if (!file.is_open()) {
            throw std::runtime_error(va("LoadCHR: %s does not exist.", fname.c_str()));
        }

        DataNode* document;
        file >> document;
        file.close();

        DataNode* rootNode = document->getChild("ika-sprite");

        std::string ver = rootNode->getChild("version")->getString();

        {
            DataNode* infoNode = rootNode->getChild("information");

            {
                DataNode* metaNode = infoNode->getChild("meta");

                NodeList nodes = metaNode->getChildren();
                for (NodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++) {
                    if ((*iter)->isString()) {
                        continue;
                    }

                    DataNode* n = (DataNode*)*iter;

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

            uint frameCount = (uint)atoi(frameNode->getChild("count")->getString().c_str());

            DataNode* dimNode = frameNode->getChild("dimensions");
            nWidth = atoi(dimNode->getChild("width")->getString().c_str());
            nHeight = atoi(dimNode->getChild("height")->getString().c_str());
            
            DataNode* hsNode = frameNode->getChild("hotspot");
            nHotx = atoi(hsNode->getChild("x")->getString().c_str());
            nHoty = atoi(hsNode->getChild("y")->getString().c_str());
            nHotw = atoi(hsNode->getChild("width")->getString().c_str());
            nHoth = atoi(hsNode->getChild("height")->getString().c_str());

            DataNode* dataNode = frameNode->getChild("data");
            if (dataNode->getChild("format")->getString() != "zlib")
                throw std::runtime_error("Unsupported data format");

            std::string d64 = dataNode->getString();
            ScopedArray<u8> compressed(new u8[d64.length()]); // way more than enough.
            int compressedSize;
            if (ver == "1.0") {
                compressedSize = oldBase64::decode(d64, compressed.get(), d64.length());
            } else if (ver == "1.1") {
                std::string un64 = base64::decode(d64);
                std::copy((u8*)(un64.c_str()), (u8*)(un64.c_str() + un64.length()), compressed.get());
                compressedSize = un64.length();
            } else {
                throw std::runtime_error(va("Sprite uses unrecognized version %s.  "
                    "This build of ika only knows about 1.0 and 1.1", ver.c_str())
                );
            }

            ScopedArray<u8> pixels(new u8[nWidth * nHeight * frameCount * sizeof(RGBA)]);
            Compression::decompress(compressed.get(), compressedSize, pixels.get(), nWidth * nHeight * frameCount * sizeof(RGBA));

            ClearFrames();
            frame.reserve(frameCount);
            RGBA* ptr = (RGBA*)pixels.get();
            for (uint i = 0; i < frameCount; i++)
            {
                frame.push_back(new Canvas(ptr, nWidth, nHeight));
                ptr += nWidth * nHeight;
            }
        }

        delete rootNode;
    }
    catch (std::runtime_error err)
    {
        Log::Write("LoadCHR(\"%s\"): %s", fname.c_str(), err.what());
        throw err;
     }
}

void CCHRfile::Save(const std::string& fname)
{
    if (Path::equals("chr", Path::getExtension(fname))) {
        SaveOld(fname);
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
            iter != metaData.end(); iter++)
        {
            metaNode->addChild(newNode(iter->first)->addChild(iter->second));
        }
    }

    rootNode->addChild(newNode("header")
        ->addChild(newNode("depth")->addChild("32"))
        );

    {
        DataNode* scriptNode = newNode("scripts");
        rootNode->addChild(scriptNode);

        for (StringMap::iterator 
            iter = moveScripts.begin();
            iter != moveScripts.end();
            iter++)
        {
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
            ->addChild(newNode("count")->addChild(frame.size()))
            ->addChild(newNode("dimensions")
                ->addChild(newNode("width")->addChild(nWidth))
                ->addChild(newNode("height")->addChild(nHeight))
                )
            ->addChild(newNode("hotspot")
                ->addChild(newNode("x")->addChild(nHotx))
                ->addChild(newNode("y")->addChild(nHoty))
                ->addChild(newNode("width")->addChild(nHotw))
                ->addChild(newNode("height")->addChild(nHoth))
                );

        const int uncompressedBlockSize = nWidth * nHeight * frame.size() * sizeof(RGBA);
        const int compressedBlockSize = uncompressedBlockSize * 4 / 3; // more than is needed.  Way more

        ScopedArray<u8> uncompressed(new u8[uncompressedBlockSize]);
        ScopedArray<u8> compressed(new u8[compressedBlockSize]);

        // pack the uncompressed data into one big block
        RGBA* dest = reinterpret_cast<RGBA*>(uncompressed.get());
        for (uint i = 0; i < frame.size(); i++)
        {
            RGBA* src = frame[i]->GetPixels();
            memcpy(dest, src, nWidth * nHeight * sizeof(RGBA));
            
            dest += nWidth * nHeight;
        }

        // compress
        int compressSize = Compression::compress(
            uncompressed.get(), uncompressedBlockSize, 
            compressed.get(), compressedBlockSize);

        // base64
        //std::string d64 = base64::encode(compressed.get(), compressSize);
        std::string d64 = base64::encode(std::string(compressed.get(), compressed.get() + compressSize));

        frameNode->addChild(newNode("data")
            ->addChild(newNode("format")->addChild("zlib"))
            ->addChild(d64));            
    }

    std::ofstream file(fname.c_str());
    file << rootNode;
    delete rootNode;

}

void CCHRfile::SaveOld(const std::string& fname)
{
    File f;
    
    bool bResult = f.OpenWrite(fname.c_str());
    if (!bResult)
        throw std::runtime_error(va("Failed to open %s for writing.", fname.c_str()));                                                     // :(
    
    f.Write((char)5);                                               // version - u8
    
    f.Write(metaData["description"].c_str(), 64);                   // desc    - 64 byte string
    
    f.Write(moveScripts.size());                                    // write the number of scripts
    
    for (uint i = 0; i < numScripts; i++)
    {
        const std::string& script = moveScripts[scriptNames[i]];
        f.Write(script.length());                                   // write the length
        f.Write(script.c_str(), script.length());                   // write the actual script
    }
    
    // Write the frame data
    f.Write((int)frame.size());
    for (uint i = 0; i < frame.size(); i++)
    {
        f.Write(frame[i]->Width());
        f.Write(frame[i]->Height());
        f.Write(nHotx);                                             // note that the current data structure does not support variable hotspots.  But the file format does. (potential expansion)
        f.Write(nHoty);
        f.Write(nHotw);
        f.Write(nHoth);
        
        f.WriteCompressed(
            frame[i]->GetPixels(),
            frame[i]->Width() * frame[i]->Height() * sizeof(RGBA));
    }
    
    f.Close();
}

std::string CCHRfile::GetStandingScript(Direction dir)
{
    return moveScripts[scriptNames[int(dir) + 8]];
}

std::string CCHRfile::GetWalkingScript(Direction dir)
{
    return moveScripts[scriptNames[int(dir)]];
}

void CCHRfile::LoadCHR(const std::string& fileName)
{
    File f;
    
    bool result = f.OpenRead(fileName.c_str());
    if (!result)
        throw std::runtime_error(va("Unable to load %s.", fileName.c_str()));
    
    char ver;
    f.Read(ver);
    
    switch (ver)
    {
    case 2:     return Loadv2CHR(f);
    case 4:     return Loadv4CHR(f);
    case 5:     return Loadv5CHR(f);
    default:    throw std::runtime_error(va("Bogus version number %i", (int)ver));        
    }
}

void CCHRfile::Loadv2CHR(File& f)
{
    nWidth = nHeight = 0;
    nHotx = nHoty = 0;
    nHotw = nHoth = 0;
    
    f.Read(&nWidth, 2);
    f.Read(&nHeight, 2);
    f.Read(&nHotx, 2);
    f.Read(&nHoty, 2);
    f.Read(&nHotw, 2);
    f.Read(&nHoth, 2);
    
    u16 nFrames;
    f.Read(nFrames);
    
    int nBufsize;
    f.Read(nBufsize);
    u8* pCompbuf = new u8[nBufsize];
    u8* pUncompbuf = new u8[nFrames * nWidth * nHeight];
    f.Read(pCompbuf, nBufsize);
    ReadCompressedLayer1(pUncompbuf, nFrames * nWidth * nHeight, pCompbuf);
    delete[] pCompbuf;
    
    ClearFrames();
    frame.resize(nFrames);
    u8* src = pUncompbuf;
    for (int nCurframe = 0; nCurframe < nFrames; nCurframe++)
    {
        frame[nCurframe] = new Canvas(src, nWidth, nHeight, cVergepal);
        src += nWidth * nHeight;
    }
    
    moveScripts.clear();
    //moveScripts.resize(16);
    
    // Get the idle frames
    int i;
    f.Read(i);        moveScripts["idle_left"]  = std::string("F") + toString(i) + "W10";
    f.Read(i);        moveScripts["idle_right"] = std::string("F") + toString(i) + "W10";
    f.Read(i);        moveScripts["idle_up"]    = std::string("F") + toString(i) + "W10";
    f.Read(i);        moveScripts["idle_down"]  = std::string("F") + toString(i) + "W10";
    
    for (int b = 0; b < 4; b++)
    {
        char ptr[255];
        int n;
        f.Read(n);
        
        if (n > 99 || n < 0)
            throw std::runtime_error(va("Bogus movescript length %i", n));
        f.Read(ptr, n);
        ptr[n]=0;                    // terminating null
        
        switch (b)
        {
        case 0: moveScripts["walk_left"]  = ptr;  break;
        case 1: moveScripts["walk_right"] = ptr;  break;
        case 2: moveScripts["walk_up"]    = ptr;  break;
        case 3: moveScripts["walk_down"]  = ptr;  break;
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

void CCHRfile::Loadv4CHR(File& f)
{
    // VERGE v2 chrs store two bytes for these, but ika has four.  Must nuke the high bits or anality will ensue.
    nWidth = nHeight = 0;
    nHotx = nHoty = 0;
    nHotw = nHoth = 0;
    
    f.Read(&nWidth, 2);
    f.Read(&nHeight, 2);
    f.Read(&nHotx, 2);
    f.Read(&nHoty, 2);
    f.Read(&nHotw, 2);
    f.Read(&nHoth, 2);
    
    //moveScripts.resize(16);
    moveScripts.clear();
    
    // Get the idle frames
    u16 i;
    f.Read(i);        moveScripts["idle_left"]  = std::string("F") + toString(i) + "W10";
    f.Read(i);        moveScripts["idle_right"] = std::string("F") + toString(i) + "W10";
    f.Read(i);        moveScripts["idle_up"]    = std::string("F") + toString(i) + "W10";
    f.Read(i);        moveScripts["idle_down"]  = std::string("F") + toString(i) + "W10";
    
    f.Read(i);
    int nFrames = i;                // frame count
    
    for (int b = 0; b < 4; b++)
    {
        char ptr[255];
        int n;
        f.Read(n);
        
        if (n > 99)
            throw std::runtime_error(va("Bogus movescript length %i", n));

        f.Read(ptr, n);
        ptr[n]=0;                    // terminating null
        
        switch (b)
        {
        case 0: moveScripts["walk_left"]  = ptr;  break;
        case 1: moveScripts["walk_right"] = ptr;  break;
        case 2: moveScripts["walk_up"]    = ptr;  break;
        case 3: moveScripts["walk_down"]  = ptr;  break;
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
    
    u16* pTemp = new u16[nWidth * nHeight * nFrames];
    ReadCompressedLayer2(pTemp, nWidth * nHeight * nFrames, (u16*)ptr);
    delete[] ptr;
    
    ClearFrames();
    frame.resize(nFrames);
    
    // adjust to 32bpp
    RGBA* p = new RGBA[nWidth * nHeight];
    for (int nCurframe = 0; nCurframe < nFrames; nCurframe++)
    {
        for (int n = 0; n < nWidth * nHeight; n++)
        {
            u16 u = pTemp[nCurframe * nWidth * nHeight + n];
            u8 r = ((u >> 11) & 31) << 3;
            u8 g = ((u >> 5) & 63) << 2;
            u8 b = (u & 31) << 3;
            u8 a = ((!r && !g && !b) || (r == 255 && !g && b == 255)) ? 0 : 255;
            p[n]=RGBA(r, g, b, a);
        }
        
        frame[nCurframe] = new Canvas(p, nWidth, nHeight);
    }
    
    delete[] pTemp;
    delete[] p;
}

void CCHRfile::Loadv5CHR(File& f)
{    
    // Load the new format
    
    char s[65];
    f.Read(s, 64);
    s[64]=0;
    metaData["description"] = s;
    
    int nScripts;
    
    f.Read(nScripts);

    moveScripts.clear();
    
    for (int i = 0; i < nScripts && i < numScripts; i++)
    {
        int nLen;
        f.Read(nLen);
        
        char* s = new char[nLen + 1];
        f.Read(s, nLen);
        s[nLen] = 0;
        moveScripts[scriptNames[i]] = s;
        delete[] s;
    }
    
    // Get the frame data
    int nFrames;
    f.Read(nFrames);
    ClearFrames();
    frame.reserve(nFrames);
    for (int i = 0; i < nFrames; i++)
    {
        int x, y;
        f.Read(x);
        f.Read(y);
        f.Read(nHotx);
        f.Read(nHoty);
        f.Read(nHotw);
        f.Read(nHoth);
        
        RGBA* pTemp = new RGBA[x * y];
        f.ReadCompressed(pTemp, x * y * sizeof(RGBA));
        
        frame.push_back(new Canvas(pTemp, x, y));
        
        delete[] pTemp;
    }

    f.Close();
}

