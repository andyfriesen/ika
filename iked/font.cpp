
#include <cassert>
#include "font.h"
#include "common/fontfile.h"

namespace iked {

    Font::Font() {
        font = new FontFile();
        invariant();
    }

    Font::Font(FontFile* c)
        : font(c)
    {
        invariant();
    }

    Font::~Font() {
        invariant();
        delete font;
    }

    void Font::save(const std::string& fileName) {
        invariant();
        font->Save(fileName.c_str());
    }

    int Font::count() {
        invariant();
        return font->NumGlyphs();
    }

    void Font::set(const Canvas& img, int idx) {
        font->GetGlyph(idx)=img;
        invariant();
    }

    const Canvas& Font::get(int idx) {
        invariant();
        return font->GetGlyph(idx);
    }

    void Font::insert(int i, Canvas& p) {
        invariant();
	throw std::runtime_error("Font::clone Not yet implemented");
    }

    void Font::remove(int i) {
        invariant();
        throw std::runtime_error("Font::remove Not yet implemented");
        //font->RemoveGlyph(i);
    }

    Font* Font::clone() {
	return 0;
    }

    int Font::width() {
        invariant();
        return font->Width();
    }

    int Font::height() {
        invariant();
        return font->Height();
    }

    DEBUG_BLOCK(
        void Font::invariant() {
            assert(font != 0);
        }
    )

}
