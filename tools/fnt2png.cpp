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
    int fntw = font.Width(), fnth = font.Height();
    uint numGlyphs = font.NumGlyphs();

    int numDown = (int)((double)numGlyphs / numAcross + 0.5);
    int imgw = (fntw + 1) * numAcross + 1;
    int imgh = (fnth + 1) * numDown + 1;
    Canvas image(imgw, imgh);
    image.Clear(RGBA(0,255,255,255));

    int x=1, y=1, w, h;
    for(int i=0; i < numGlyphs; i++) {
        Canvas &glyph = font.GetGlyph(i);
        w = glyph.Width();
        if(x+w >= imgw) {
            x = 1;
            y += fnth + 1;
        }
        CBlitter<Opaque>::Blit(glyph, image, x, y);
        x += w + 1;
    }
    image.Save(args[2]);

    std::cout << "Wrote " << args[2] << "." << std::endl;
    std::cout << "Max glyph size " << fntw << "x" << fnth << "." << std::endl;
    std::cout << font.NumSubSets() << " subset(s), " << numGlyphs << " glyphs." << std::endl;
    //CFontFile::SSubSet &s = font.GetSubSet(0);
    //for(int i = 0; i < 256; i++)
        //std::cout << s.glyphIndex[i] << std::endl;
    exit(0);
}
