#include "fileio.h"
#include "fontfile.h"
#include "Canvas.h"
#include <iostream>
#include <sstream>

int main(int c, char **args) {
    if(c < 3) {
        std::cout << "Not enough arguments. (need font name, image name)" << std::endl;
        exit(1);
    }
    int numAcross = 32;
    if(c > 3) {
        std::stringstream ss("");
        ss << args[3];
        ss >> numAcross;
    }

    CFontFile font;
    font.Load(args[1]);
    int fntw = font.Width() + 1, fnth = font.Height() + 1; // adjusted for spacer
    uint numGlyphs = font.NumGlyphs();
    int numSubsets = font.NumSubSets();

    // debug: check font subset data
    /*int ns = font.NumSubSets();
    for(int j = 0; j < ns; j++) {
        CFontFile::SSubSet &s = font.GetSubSet(j);
        for(int i = 0; i < 256; i++)
            std::cout << s.glyphIndex[i] << ", ";
        std::cout << std::endl << std::endl;
    }*/

    // assumes all subsets have the same size (likely 96).
    // not perfect but it'll have to do for now.
    int perSubset = (int)((double)numGlyphs / numSubsets + 0.5);
    int numDown = (int)((double)perSubset / numAcross + 0.5) * numSubsets;
    int imgw = fntw * numAcross + 1;
    int imgh = fnth * numDown + numSubsets;
    Canvas image(imgw, imgh);
    image.Clear(RGBA(0,255,255,255));

    int x=0, y=1, w, h;
    for(int i=0, j=0; i < numGlyphs; i++, j++, x++) {
        if(j >= perSubset) {
            j = 0;
            if(x > 0) {
                x = 0;
                y += fnth;
            }
            ++y; // 2 pixel spacer between subsets
        }
        if(x >= numAcross) {
            x = 0;
            y += fnth;
        }
        Canvas &glyph = font.GetGlyph(i);
        CBlitter<Opaque>::Blit(glyph, image, x*fntw+1, y);
    }
    image.Save(args[2]);

    std::cout << "Wrote " << args[2] << "." << std::endl;
    std::cout << "Max glyph size " << (fntw-1) << "x" << (fnth-1) << "." << std::endl;
    std::cout << font.NumSubSets() << " subset(s), " << numGlyphs << " glyphs." << std::endl;
    exit(0);
}
