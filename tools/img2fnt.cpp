#include "fileio.h"
#include "fontfile.h"
#include "Canvas.h"
#include "types.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
// #include <memory>

int main(int c, char **args) {
    if(c < 3) {
        std::cout << "Not enough arguments. (need image name, font name)" << std::endl;
        exit(1);
    }

    Canvas *image;
    try {
        image = new Canvas(args[1]);
    } catch(std::runtime_error &e) {
        std::cout << "Could not load image." << std::endl;
        exit(1);
    }

    char numSubsets = 1; // read me from command line or file
    int imgw = image->Width(), imgh = image->Height();

    RGBA *pixels = image->GetPixels();
    u32 surround = (u32)pixels[0];
    int x, y = 1, w, h;
    RGBA *pixel = pixels + imgw + 1, *pixel2;
    std::vector<u16> glyphW;
    std::vector<u16> glyphH;
    std::vector<RGBA> glyphData;
    int width = 0, height = 0, glyphX, rowHeight;
    u16 numGlyphs = 0;

    // find all rows of glyphs
    while(y < imgh) {
        // locate next row of glyphs
        x = 1;
        rowHeight = 0;
        while(x < imgw) {
            while(x < imgw && (u32)*pixel == surround) {
                ++pixel; ++x;
            }
            if(x == imgw)
                break; // nothing on this pixel row
            glyphX = x;
            pixel2 = pixel;
            while(x < imgw && (u32)*pixel != surround) {
                ++pixel; ++x;
            }
            glyphW.push_back(w = x - glyphX);
            // find height
            h = 0;
            while((u32)*pixel2 != surround) {
                //std::raw_storage_iterator<RGBA*, int> mem(pixel2), mem2(pixel2 + glyphW.back());
                //glyphData.insert(glyphData.size(), mem, mem2);  ???
                RGBA* p = pixel2;
                for(int i = 0; i < w; i++, p++)
                    glyphData.push_back(*p);
                pixel2 += imgw;
                ++h;
            }
            glyphH.push_back(h);
            ++numGlyphs;
            if(h > rowHeight) rowHeight = h;
            if(w > width) width = w;
            //std::cout << "("<< glyphX << ", " << y << ") size " << w << "x" << h << std::endl;
        }
        if(rowHeight)
        {   pixel += (rowHeight - 1) * imgw + 1;
            y += rowHeight;
            if(rowHeight > height) height = rowHeight;
        } else
        {   ++y;
            ++pixel;
        }
    }

    u16 *stdSubset = new u16[256];
    memset(stdSubset, 0, 256 * sizeof(u16));
    for (int j = 0; j < 96; j++)
        stdSubset[j + 32] = j; // + (nCurset * 96);

    File f;
    f.OpenWrite(args[2], 1);
    f.Write("FONT27");
    f.Write(numSubsets);
    f.Write(numGlyphs);
    f.Write(stdSubset, 256 * sizeof(u16));
    for(int i=0; i < numGlyphs; i++) {
        f.Write(glyphW[i]);
        f.Write(glyphH[i]);
    }
    RGBA* pBuffer = new RGBA[glyphData.size()];
    int i = 0;
    /*for(std::vector<RGBA>::iterator p = glyphData.begin();
      p != glyphData.end(); p++, i++)
        pBuffer[i] = *p;*/
    std::copy(glyphData.begin(), glyphData.end(),
        std::raw_storage_iterator<RGBA*, int>(pBuffer));
    f.WriteCompressed(pBuffer, glyphData.size() * sizeof(RGBA));
    f.Close();

    std::cout << "Wrote " << (int)numSubsets << " subset(s)." << std::endl;
    std::cout << numGlyphs << " glyphs." << std::endl;
}
