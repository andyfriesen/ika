#include "freetype/freetype.h"
#include "fileio.h"
#include "fontfile.h"
#include "Canvas.h"
#include <iostream>
#include <sstream>

int main(int c, char **args) {
    if(c < 3) {
        std::cout << "Not enough arguments. (need font name, image name, height in pixels?)" << std::endl;
        exit(1);
    }
    int numAcross = 32;
    int pixelSize = 20;
    if(c > 3) {
        std::stringstream ss("");
        ss << args[3];
        ss >> pixelSize;
    }

    int error;
    FT_Library library;
    FT_Face face;

    if(error = FT_Init_FreeType(&library)) {
        std::cout << "An error occurred initializing the FreeType library." << std::endl;
        exit(1);
    }
    error = FT_New_Face(library, args[1], 0, &face);
    if(error == FT_Err_Unknown_File_Format) {
        std::cout << "Unsupported file format." << std::endl;
        exit(1);
    }
    else if(error) {
        std::cout << "Error loading font file." << std::endl;
        exit(1);
    }
    if(!face->face_flags & FT_FACE_FLAG_SCALABLE) {
        std::cout << "Only scalable (TTF) fonts are currently supported." << std::endl;
        exit(1);
    }
    if(error = FT_Set_Pixel_Sizes(face, 0, pixelSize)) {
        std::cout << "Error setting font pixel size." << std::endl;
    }

    /*FT_BBox bbox;
    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
    pen_x + glyph->bitmap_left, pen_y - glyph->bitmap_top
    pen_x += slot->advance.x >> 6
    width = bbox.xMax - bbox.xMin;
    height = bbox.yMax - bbox.yMin;
    */

    FT_Size_Metrics *metrics = &face->size->metrics;

    //double d = (double)face->bbox.xMax - face->bbox.xMin;
    int fntw = 0;  //(int)(d * metrics->x_ppem / face->units_per_EM + 0.5);
    //d = (double)face->bbox.yMax - face->bbox.yMin;
    int fnth = 0; //(int)(d * metrics->y_ppem / face->units_per_EM + 0.5);
    uint numGlyphs = 96; // font.NumGlyphs();
    int numSubsets = 1; // font.NumSubSets();
    int perSubset = (int)((double)numGlyphs / numSubsets + 0.5);
    int numDown = (int)((double)perSubset / numAcross + 0.5) * numSubsets;
    int glyph_index;
    FT_Bitmap* bitmap;
    unsigned char colour, *buffer;
    int ix=0, y=1, w, h, pitch, x2, y2;
    FT_Glyph_Metrics *gmetrics;

    double xRatio = (double)metrics->x_ppem / face->units_per_EM;
    double yRatio = (double)metrics->y_ppem / face->units_per_EM;
    for(int charcode=32; charcode < 128; charcode++) {
        FT_Load_Char(face, charcode, FT_LOAD_RENDER);
        gmetrics = &face->glyph->metrics;
        fntw = max((int)((gmetrics->width + face->glyph->advance.x) * xRatio + 0.5), fntw);
    }
    fnth = (int)((face->ascender - face->descender) * yRatio);

    int imgw = (fntw + 1) * numAcross + 1;
    int imgh = (fnth + 1) * numDown + numSubsets;
    if(!imgw && !imgh) {
        std::cout << "Error calculating image dimensions." << std::endl;
        exit(1);
    }
    Canvas image(imgw, imgh);
    image.Clear(RGBA(0,255,255,255));

    for(int charcode=32; charcode < 128; charcode++, ix++) {
        if(ix >= numAcross) {
            ix = 0;
            y += fnth + 1;
        }

        glyph_index = FT_Get_Char_Index(face, charcode);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);  // returns error code
        bitmap = &face->glyph->bitmap;
        buffer = bitmap->buffer;
        w = bitmap->width; h = bitmap->rows;
        pitch = bitmap->pitch;
        y2 = (int)(y + fnth - face->glyph->bitmap_top + face->descender * yRatio);
        x2 = ix*(fntw+1) + 1;
        // min two-pixel width (for missing glyphs)
        int w2 = w + face->glyph->bitmap_left;
        if(!w2) w2 = max((int)(face->glyph->advance.x * xRatio), pixelSize / 2);  // guess space width
        CBlitter<Opaque>::DrawRect(image, x2, y, x2+w2, y+fnth, (RGBA)0, 1);
        x2 += face->glyph->bitmap_left;

        for(int j=0; j < h; j++, y2++) {
            x2 = ix*(fntw+1) + face->glyph->bitmap_left + 1;
            for(int i=0; i < w; i++, x2++) {
                colour = buffer[i];
                image.SetPixel(x2, y2, RGBA(0,0,0,colour));
            }
            buffer += pitch;
        }
    }
    image.Save(args[2]);

    std::cout << "Wrote " << args[2] << "." << std::endl;
    std::cout << "Max glyph size " << fntw << "x" << fnth << "." << std::endl;
    std::cout << numSubsets << " subset(s), " << numGlyphs << " glyphs." << std::endl;
    exit(0);
}
