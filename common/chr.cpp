#include <cassert>

#include "chr.h"
#include "fileio.h"
#include "vergepal.h"
#include "rle.h"
#include "misc.h"

#ifdef USE_XML_GOODNESS
#   include "compression.h"
#   include "base64.h"
#   include "xmlutil.h"
#   include <cppdom/cppdom.h>
#   include <fstream>
using namespace cppdom;

#endif

CCHRfile::CCHRfile(int width, int height)
{
    nHotx = nHoty = 0;
    nHotw = nHoth = 16;
    nWidth = width;
    nHeight = height;
    frame.clear();
    sMovescript.resize(16); // egh
    AppendFrame();
}

Canvas& CCHRfile::GetFrame(int nFrame) const
{
    static Canvas dummy;
    
    if (nFrame < 0 || nFrame >= frame.size())
        return dummy;
    
    return (Canvas&)frame[nFrame];
}

void CCHRfile::UpdateFrame(const Canvas& newdata, int nFrame)
{
    if (nFrame < 0 || nFrame >= frame.size())
        return;

    frame[nFrame]=newdata;
}

void CCHRfile::AppendFrame()
{
    frame.push_back(Canvas(nWidth, nHeight));
}

void CCHRfile::InsertFrame(int idx)
{
    if (idx >= frame.size() || !frame.size())
    {
        AppendFrame();
        return;
    }
    
    if (idx < 0)
        idx = 0;
    
    Canvas p(frame[frame.size()-1]);        // copy the last frame
    
    for (int i = idx; i < frame.size(); i++)
        frame[i + 1]=frame[i];
    
    InsertFrame(frame.size(), p);                    // and tack it on the end.
}

void CCHRfile::InsertFrame(int idx, Canvas& p)
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

void CCHRfile::DeleteFrame(int idx)
{
    if (idx < 0 || idx >= frame.size())
        return;
    
    for (int i = idx; i < frame.size(); i++)
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
        throw std::runtime_error(va("CHRfile::Resize: Invalid dimensions %i,%i", width, height));

    for (int i = 0; i < frame.size(); i++)
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

bool CCHRfile::Load(const char* fname)
#ifdef USE_XML_GOODNESS
{
    XMLContextPtr context(new XMLContext);
    XMLDocument document;

    try
    {
        document.load(std::ifstream(fname), context);
        XMLNodePtr rootNode = document.getChild("ika-sprite");
        if (!rootNode.get())
            throw "No document root!";

        {
            XMLNodePtr infoNode = rootNode->getChild("information");
            if (!infoNode.get())
                throw "<information> tag not found.";

            // grab <information> elements that we care about
        }

        {
            XMLNodePtr headerNode = rootNode->getChild("header");
            if (!headerNode.get())
                throw "<header> tag not found.";

            // grab header stuff
        }

        {
            XMLNodePtr scriptNode = rootNode->getChild("scripts");
            if (!scriptNode.get())
                throw "<scripts> tag not found.";

            XMLNodeList nodes = scriptNode->getChildren("script");
            for (XMLNodeList::iterator iter = nodes.begin(); iter != nodes.end(); iter++)
            {
                std::string name((*iter)->getAttribute("label").getString());
                if (name.empty())
                    throw "<script> tag lacking label attribute.";
                
                XMLNodePtr n((*iter)->getChildren().front());
                if (!n.get() || n->getType() != xml_nt_cdata)
                    throw va("Script \"%s\" has no cdata", name.c_str());

                // TODO: deal with the names
                sMovescript.push_back(n->getCdata());
            }
        }

        {
            XMLNodePtr framesNode = rootNode->getChild("frames");
            if (!framesNode.get())
                throw "<frames> tag not found.";

            int frameCount = atoi(framesNode->getAttribute("count").getString().c_str());

            {
                XMLNodePtr dimNode = framesNode->getChild("dimensions");
                if (!dimNode.get())
                    throw "<dimensions> tag not found.";

                nWidth  = atoi(dimNode->getAttribute("width").getString().c_str());
                nHeight = atoi(dimNode->getAttribute("height").getString().c_str());
            }

            {
                XMLNodePtr hsNode = framesNode->getChild("hotspot");
                if (!hsNode.get())
                    throw "<hotspot> tag not found.";

                nHotx = atoi(hsNode->getAttribute("x").getString().c_str());
                nHoty = atoi(hsNode->getAttribute("y").getString().c_str());
                nHotw = atoi(hsNode->getAttribute("width").getString().c_str());
                nHoth = atoi(hsNode->getAttribute("height").getString().c_str());
            }

#ifdef DEBUG
            {
                // I assume that vector stores bytes consecutively
                // so as to avoid copying things around, or changing
                // the base64 interface.
                // Hopefully, this assertion will never be tested.
                std::vector<u8> v(2);
                assert((v.begin() + 1) - v.begin() == sizeof(u8));
            }
#endif
            
            {
                XMLNodePtr dataNode = framesNode->getChild("data");
                if (!dataNode.get())
                    throw "<data> tag not found.";

                XMLNodePtr n(dataNode->getChildren().front());
                if (!n.get() || n->getType() != xml_nt_cdata)
                    throw va("No pixel data!");

                std::string s = n->getCdata();
                // This line won't work if vector doesn't store the bytes consecutively.

                std::vector<u8> compressed;
                base64::decode(std::vector<u8>(&*s.begin(), &*s.end()), compressed); // compressed binary data

                ScopedPtr<u8> pixels(new u8[nWidth * nHeight * frameCount]);

                Compression::decompress(&*compressed.begin(), compressed.size(), pixels.get(), nWidth * nHeight * frameCount);

                frame.clear();
                frame.reserve(frameCount);
                RGBA* ptr = (RGBA*)pixels.get();
                for (int i = 0; i < frameCount; i++)
                {
                    frame.push_back(Canvas(ptr, nWidth, nHeight));
                    ptr += nWidth * nHeight;
                }
            }
        }

        return true;
    }
    catch (XMLError)
    {
        throw std::runtime_error(va("Unable to load %s.", fname));
    }
    catch (const char* s)
    {
        throw std::runtime_error(va("CHRFile::Load(%s): %s", fname, s));
    }

    // Execution only gets here if an exception of some sort was raised.
    // Clear the sprite.
    frame.clear();
    nWidth = nHeight = 0;
    return false;
}
#else
// eek @_@;
{
    File f;
    
    bool bResult = f.OpenRead(fname);
    if (!bResult)
        return false;
    
    char ver;
    f.Read(ver);
    
    switch (ver)
    {
    case 2:
        bResult = Loadv2CHR(f);
        f.Close();
        return bResult;
    case 4:
        bResult = Loadv4CHR(f);
        f.Close();
        return bResult;
    case 5:
        break;
    default:
        f.Close();
        return false;
    }
    
    // Load the new format
    
    char s[65];
    f.Read(s, 64);
    s[64]=0;
    sDescription = s;
    
    int nScripts;
    
    f.Read(nScripts);

    sMovescript.clear();
    sMovescript.reserve(nScripts);
    
    for (int i = 0; i < nScripts; i++)
    {
        int nLen;
        f.Read(nLen);
        
        char* s = new char[nLen + 1];
        f.Read(s, nLen);
        s[nLen]=0;
        sMovescript.push_back(s);
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
    return true;
}
#endif

void CCHRfile::Save(const char* fname)
#ifdef USE_XML_GOODNESS
{
    XMLContextPtr context(new XMLContext);
    XMLDocument document;
    XMLNodePtr rootNode(new XMLNode(context));
    rootNode->setName("ika-sprite");
    rootNode->setAttribute("version", "1.0");

    try
    {
        {
            XMLNodePtr infoNode(new XMLNode(context));
            infoNode->setName("information");
            
            {
                XMLNodePtr titleNode(new XMLNode(context));
                titleNode->setName("title");
                titleNode->addChild(CData(context, "Untitled")); // FIXME

                infoNode->addChild(titleNode);
            }

            // Fill this in later.
            infoNode->addChild(MetaNode(context, "author", "Andy"));
            infoNode->addChild(MetaNode(context, "date", "A longass time ago"));
            infoNode->addChild(MetaNode(context, "description", "Wouldn't you like to know."));
            infoNode->addChild(MetaNode(context, "url", "http://ikagames.com"));
            infoNode->addChild(MetaNode(context, "email", "theamazing@explodinguterus.com"));

            rootNode->addChild(infoNode);
        }

        {
            XMLNodePtr headerNode(new XMLNode(context));
            headerNode->setName("header");

            {
                XMLNodePtr depthNode(new XMLNode(context));
                depthNode->setName("depth");
                depthNode->addChild(CData(context, "32")); // Don't expect to do anything but 32bpp for awhile

                headerNode->addChild(depthNode);
            }

            rootNode->addChild(headerNode);
        }

        {
            XMLNodePtr scriptNode(new XMLNode(context));
            scriptNode->setName("scripts");

            for (int i = 0; i < sMovescript.size(); i++)
            {
                if (!sMovescript[i].empty())
                {
                    XMLNodePtr n(new XMLNode(context));
                    n->setName("script");
                    n->setAttribute("label", va("script%i", i));
                    
                    n->addChild(CData(context, sMovescript[i].c_str()));

                    scriptNode->addChild(n);
                }
            }

            rootNode->addChild(scriptNode);
        }

        {
            XMLNodePtr framesNode(new XMLNode(context));
            framesNode->setName("frames");
            framesNode->setAttribute("count", va("%i", frame.size()));

            {
                XMLNodePtr dimNode(new XMLNode(context));
                dimNode->setName("dimensions");
                dimNode->setAttribute("width", va("%i", nWidth));
                dimNode->setAttribute("height", va("%i", nHeight));

                framesNode->addChild(dimNode);
            }

            {
                XMLNodePtr hsNode(new XMLNode(context));
                hsNode->setName("hotspot");
                hsNode->setAttribute("x", va("%i", nHotx));
                hsNode->setAttribute("y", va("%i", nHoty));
                hsNode->setAttribute("width", va("%i", nHotw));
                hsNode->setAttribute("height", va("%i", nHoth));

                framesNode->addChild(hsNode);
            }

            {
                const int uncompressedBlockSize = nWidth * nHeight * frame.size() * sizeof(RGBA);
                const int compressedBlockSize = uncompressedBlockSize * 4 / 3; // more than is needed.  Way more

                ScopedPtr<u8> uncompressed(new u8[uncompressedBlockSize]);
                ScopedPtr<u8> compressed(new u8[compressedBlockSize]);
                std::vector<u8> d64; // base64 encoded data

                // pack the uncompressed data into one big block
                RGBA* p = (RGBA*)uncompressed.get();
                for (int i = 0; i < frame.size(); i++)
                {
                    std::copy(frame[i].GetPixels(), frame[i].GetPixels() + nWidth * nHeight, p);
                    p += nWidth * nHeight;
                }

                // compress
                int compressSize = Compression::compress(
                    uncompressed.get(), uncompressedBlockSize, 
                    compressed.get(), compressedBlockSize);

                // base64
                base64::encode(
                    std::vector<u8>(compressed.get(), compressed.get() + compressSize), // ugh
                    d64);
                d64.push_back(0); // null termination for ASCIIZ

                XMLNodePtr dataNode(new XMLNode(context));
                dataNode->setName("data");
                dataNode->addChild(CData(context, (const char*)&*d64.begin()));
                
                framesNode->addChild(dataNode);
            }

            rootNode->addChild(framesNode);
        }

        document.addChild(rootNode);
        document.save(std::ofstream(fname));
    }
    catch (XMLError)
    {
        throw std::runtime_error(va("Unable to write %s.", fname));
    }
    catch (const char* s)
    {
        throw std::runtime_error(va("CHRFile::Save(%s): %s", fname, s));
    }
}
#else
{
    File f;
    
    bool bResult = f.OpenWrite(fname);
    if (!bResult)
        return;                                                     // :(
    
    f.Write((char)5);                                               // version - u8
    
    f.Write(sDescription.c_str(), 64);                               // desc    - 64 byte string
    
    int i;
    
    f.Write(sMovescript.size());                                    // write the number of scripts
    
    for (i = 0; i < sMovescript.size(); i++)
    {
        f.Write(sMovescript[i].length());                           // write the length
        f.Write(sMovescript[i].c_str(), sMovescript[i].length());    // write the actual script
    }
    
    // Write the frame data
    f.Write((int)frame.size());
    for (i = 0; i < frame.size(); i++)
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
#endif

bool CCHRfile::Loadv2CHR(File& f)
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
    
    sMovescript.resize(16);
    
    // Get the idle frames
    int i;
    f.Read(i);        sMovescript[8 + face_left]  =string("F") + ToString(i) + "W10";
    f.Read(i);        sMovescript[8 + face_right] =string("F") + ToString(i) + "W10";
    f.Read(i);        sMovescript[8 + face_up]    =string("F") + ToString(i) + "W10";
    f.Read(i);        sMovescript[8 + face_down]  =string("F") + ToString(i) + "W10";
    
    for (int b = 0; b < 4; b++)
    {
        char ptr[255];
        int n;
        f.Read(n);
        
        if (n > 99)
            return false;                    // blargh
        f.Read(ptr, n);
        ptr[n]=0;                    // terminating null
        
        switch (b)
        {
        case 0: sMovescript[face_left]  = ptr;  break;
        case 1: sMovescript[face_right] = ptr;  break;
        case 2: sMovescript[face_up]    = ptr;  break;
        case 3: sMovescript[face_down]  = ptr;  break;
        }
    }
    sMovescript[face_upleft]    = sMovescript[face_left];
    sMovescript[face_downleft]  = sMovescript[face_left];
    sMovescript[face_upright]   = sMovescript[face_right];
    sMovescript[face_downright] = sMovescript[face_right];
    
    return true;
}

bool CCHRfile::Loadv4CHR(File& f)
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
    
    sMovescript.resize(16);
    
    // Get the idle frames
    u16 i;
    f.Read(i);        sMovescript[8 + face_left]    = string("F") + ToString(i) + "W10";
    f.Read(i);        sMovescript[8 + face_right]   = string("F") + ToString(i) + "W10";
    f.Read(i);        sMovescript[8 + face_up]      = string("F") + ToString(i) + "W10";
    f.Read(i);        sMovescript[8 + face_down]    = string("F") + ToString(i) + "W10";
    
    f.Read(i);
    int nFrames = i;                // frame count
    
    for (int b = 0; b < 4; b++)
    {
        char ptr[255];
        int n;
        f.Read(n);
        
        if (n > 99)
            return false;                    // blargh
        f.Read(ptr, n);
        ptr[n]=0;                    // terminating null
        
        switch (b)
        {
        case 0: sMovescript[face_left]  = ptr;  break;
        case 1: sMovescript[face_right] = ptr;  break;
        case 2: sMovescript[face_up]    = ptr;  break;
        case 3: sMovescript[face_down]  = ptr;  break;
        }
    }
    sMovescript[face_upleft]    = sMovescript[face_left];
    sMovescript[face_downleft]  = sMovescript[face_left];
    sMovescript[face_upright]   = sMovescript[face_right];
    sMovescript[face_downright] = sMovescript[face_right];
    
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
    
    return true;
}

void CCHRfile::SaveOld(const char* fname)
{
}
