#if 0

#pragma once

#include "debug.h"
#include "imagebank.h"
#include "document.h"

struct FontFile;

namespace iked {

    struct Font : CanvasArray, Resource {
        Font();
        Font(FontFile* c);
        ~Font();

        // CanvasArray
        virtual int count();
        virtual const Canvas& get(int idx);
        virtual void set(const Canvas& canvas, int index);
        virtual void insert(int pos, Canvas& c);
        // --

	// Resource
	virtual Font* clone();
	// --

        void save(const std::string& fileName);
        int width();
        int height();

        FontFile& GetFont() { return *font; }

    private:
        DECLARE_INVARIANT();

        FontFile* font;
    };

}

#endif
