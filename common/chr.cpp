#include <cassert>
#include <stdexcept>

#include "chr.h"
#include "fileio.h"
#include "vergepal.h"
#include "rle.h"
#include "misc.h"
#include "log.h"

#include "compression.h"
#include "base64.h"
#include "aries.h"
#include <fstream>
#include <stdexcept>

using aries::NodeList;
using aries::DataNodeList;
using aries::DataNode;
using aries::newNode;

CCHRfile::CCHRfile(int width, int height)
{
    nHotx = nHoty = 0;
    nHotw = nHoth = 16;
    nWidth = width;
    nHeight = height;
    frame.clear();
    moveScripts.resize(16); // egh
    AppendFrame();
}

Canvas& CCHRfile::GetFrame(uint nFrame) const
{
    static Canvas dummy;
    
    if (nFrame < 0 || nFrame >= frame.size())
        return dummy;
    
    return (Canvas&)frame[nFrame];
}

void CCHRfile::UpdateFrame(const Canvas& newdata, uint nFrame)
{
    if (nFrame < 0 || nFrame >= frame.size())
        return;

    frame[nFrame]=newdata;
}

void CCHRfile::AppendFrame()
{
    frame.push_back(Canvas(nWidth, nHeight));
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
    
    Canvas p(frame[frame.size()-1]);        // copy the last frame
    
    for (uint i = idx; i < frame.size(); i++)
        frame[i + 1]=frame[i];
    
    InsertFrame(frame.size(), p);                    // and tack it on the end.
}

void CCHRfile::InsertFrame(uint idx, Canvas& p)
{
    if (idx < 0)
        idx = 0;
    if (idx >= frame.size())
    {
        AppendFrame();
        frame[frame.size()-1]=p;
        return;
    }
    
    InsertFrame(idx);
    frame[idx]=p;
}

void CCHRfile::DeleteFrame(uint idx)
{
    if (idx < 0 || idx >= frame.size())
        return;
    
    for (uint i = idx; i < frame.size(); i++)
        frame[i]=frame[i + 1];
    
    frame.resize(frame.size()-1);
}

void CCHRfile::PackData(u8* data, int& size)
{
}

void CCHRfile::UnpackData(u8* data, int size)
{
}

void CCHRfile::Resize(int width, int height)
{
    if (width < 0 || height < 0)
        throw std::runtime_error(va("CHRfile::Resize: Invalid dimensions %i, %i", width, height));

    for (uint i = 0; i < frame.size(); i++)
        frame[i].Resize(width, height);

    nWidth = width;
    nHeight = height;
}

void CCHRfile::New(int framex, int framey)
{
    nWidth = framex;
    nHeight = framey;
    nHotx = 0;        nHoty = 0;
    nHotw = framex;    nHoth = framey;
    frame.clear();
}

void CCHRfile::Load(const std::string& fname)
{
    moveScripts.clear();

    // first, a quick check for loading older file formats.
    if (Lower(Path::Extension(fname)) == "chr")
    {
        LoadCHR(fname);
        return;
    }

    try
    {
        if (!fname.length())
            throw std::runtime_error("LoadCHR: No filename given.");

        std::ifstream file;
        file.open(fname.c_str());
        if (file.bad())
            throw std::runtime_error(va("LoadCHR: %s does not exist.", fname.c_str()));

        DataNode* document;
        file >> document;
        file.close();

        DataNode* rootNode = document->getChild("ika-sprite");

        {
            DataNode* infoNode = rootNode->getChild("information");

            {
                DataNode* metaNode = infoNode->getChild("meta");

                NodeList nodes = metaNode->getChildren();
                for (NodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
                {
                    if ((*iter)->isString())
                        continue;

                    DataNode* n = (DataNode*)*iter;

                    if (n->getString().empty())
                        metaData[n->getName()] = n->getString();
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
            for (DataNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                std::string name = (*iter)->getChild("label")->getString();
                std::string script = (*iter)->getString();

                // FIXME: script name not actually used!
                moveScripts.push_back(script);
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

            std::string cdata = dataNode->getString();
            ScopedArray<u8> compressed(new u8[cdata.length()]); // way more than enough.
            int compressedSize = base64::decode(cdata, compressed.get(), cdata.length());
            ScopedArray<u8> pixels(new u8[nWidth * nHeight * frameCount * sizeof(RGBA)]);
            Compression::decompress(compressed.get(), compressedSize, pixels.get(), nWidth * nHeight * frameCount * sizeof(RGBA));

            frame.clear();
            frame.reserve(frameCount);
            RGBA* ptr = (RGBA*)pixels.get();
            for (uint i = 0; i < frameCount; i++)
            {
                frame.push_back(Canvas(ptr, nWidth, nHeight));
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
    if (Lower(Path::Extension(fname)) == "chr")
    {
        SaveOld(fname);
        return;
    }

    DataNode* rootNode = newNode("ika-sprite");
    rootNode->addChild(newNode("version")->addChild("1.0"));

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

        for (uint i = 0; i < moveScripts.size(); i++)
        {
            scriptNode->addChild(
                newNode("script")->addChild(
                    newNode("label")->addChild(va("script%i", i))
                    )
                    ->addChild(moveScripts[i])
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
            RGBA* src = frame[i].GetPixels();
            memcpy(dest, src, nWidth * nHeight * sizeof(RGBA));
            
            dest += nWidth * nHeight;
        }

        // compress
        int compressSize = Compression::compress(
            uncompressed.get(), uncompressedBlockSize, 
            compressed.get(), compressedBlockSize);

        // base64
        std::string d64 = base64::encode(compressed.get(), compressSize);

        frameNode->addChild(newNode("data")
            ->addChild(newNode("format")->addChild("zlib"))
            ->addChild(d64));            
    }

    std::ofstream file(fname.c_str());
    file << rootNode;
    delete rootNode;

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
    
    frame.resize(nFrames);
    u8* src = pUncompbuf;
    for (int nCurframe = 0; nCurframe < nFrames; nCurframe++)
    {
        frame[nCurframe].CopyPixelData(src, nWidth, nHeight, cVergepal);
        src += nWidth * nHeight;
    }
    
    moveScripts.resize(16);
    
    // Get the idle frames
    int i;
    f.Read(i);        moveScripts[8 + face_left]  =std::string("F") + ToString(i) + "W10";
    f.Read(i);        moveScripts[8 + face_right] =std::string("F") + ToString(i) + "W10";
    f.Read(i);        moveScripts[8 + face_up]    =std::string("F") + ToString(i) + "W10";
    f.Read(i);        moveScripts[8 + face_down]  =std::string("F") + ToString(i) + "W10";
    
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
        case 0: moveScripts[face_left]  = ptr;  break;
        case 1: moveScripts[face_right] = ptr;  break;
        case 2: moveScripts[face_up]    = ptr;  break;
        case 3: moveScripts[face_down]  = ptr;  break;
        }
    }
    moveScripts[face_upleft]    = moveScripts[face_left];
    moveScripts[face_downleft]  = moveScripts[face_left];
    moveScripts[face_upright]   = moveScripts[face_right];
    moveScripts[face_downright] = moveScripts[face_right];
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
    
    moveScripts.resize(16);
    
    // Get the idle frames
    u16 i;
    f.Read(i);        moveScripts[8 + face_left]    = std::string("F") + ToString(i) + "W10";
    f.Read(i);        moveScripts[8 + face_right]   = std::string("F") + ToString(i) + "W10";
    f.Read(i);        moveScripts[8 + face_up]      = std::string("F") + ToString(i) + "W10";
    f.Read(i);        moveScripts[8 + face_down]    = std::string("F") + ToString(i) + "W10";
    
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
        case 0: moveScripts[face_left]  = ptr;  break;
        case 1: moveScripts[face_right] = ptr;  break;
        case 2: moveScripts[face_up]    = ptr;  break;
        case 3: moveScripts[face_down]  = ptr;  break;
        }
    }
    moveScripts[face_upleft]    = moveScripts[face_left];
    moveScripts[face_downleft]  = moveScripts[face_left];
    moveScripts[face_upright]   = moveScripts[face_right];
    moveScripts[face_downright] = moveScripts[face_right];
    
    int n;
    f.Read(n);
    char* ptr = new char[n];
    
    f.Read(ptr, n);
    
    u16* pTemp = new u16[nWidth * nHeight * nFrames];
    ReadCompressedLayer2(pTemp, nWidth * nHeight * nFrames, (u16*)ptr);
    delete[] ptr;
    
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
        
        frame[nCurframe].CopyPixelData(p, nWidth, nHeight);
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
    moveScripts.reserve(nScripts);
    
    for (int i = 0; i < nScripts; i++)
    {
        int nLen;
        f.Read(nLen);
        
        char* s = new char[nLen + 1];
        f.Read(s, nLen);
        s[nLen]=0;
        moveScripts.push_back(s);
        delete[] s;
    }
    
    // Get the frame data
    int nFrames;
    f.Read(nFrames);
    frame.clear();
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
        
        frame.push_back(Canvas(pTemp, x, y));
        
        delete[] pTemp;
    }

    f.Close();
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
    
    for (uint i = 0; i < moveScripts.size(); i++)
    {
        f.Write(moveScripts[i].length());                           // write the length
        f.Write(moveScripts[i].c_str(), moveScripts[i].length());   // write the actual script
    }
    
    // Write the frame data
    f.Write((int)frame.size());
    for (uint i = 0; i < frame.size(); i++)
    {
        f.Write(frame[i].Width());
        f.Write(frame[i].Height());
        f.Write(nHotx);                                             // note that the current data structure does not support variable hotspots.  But the file format does. (potential expansion)
        f.Write(nHoty);
        f.Write(nHotw);
        f.Write(nHoth);
        
        f.WriteCompressed(
            frame[i].GetPixels(),
            frame[i].Width()*frame[i].Height()*sizeof(RGBA));
    }
    
    f.Close();
}
