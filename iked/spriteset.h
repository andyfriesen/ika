
#pragma once

#include "debug.h"
#include "imagebank.h"
#include "document.h"

struct CCHRfile;

namespace iked {

    struct SpriteSet : ImageArrayDocument {
        SpriteSet(const std::string& filename);
        SpriteSet(int width, int height, int numFrames);
        SpriteSet(CCHRfile* c, const std::string& fileName);
        ~SpriteSet();

        virtual void save(const std::string& fileName);

        virtual SpriteSet* asSpriteSet();

        void resize(int width, int height);

        CCHRfile& GetCHR() { return *chr; }

    protected:
        // ImageDocumentResource
        virtual int doGetCount();
        virtual int doGetWidth();
        virtual int doGetHeight();
        virtual const Canvas& doGetCanvas(int index);
        virtual void doSetCanvas(const Canvas& canvas, int index);
        virtual void doInsert(const Canvas& canvas, int position);
        virtual void doRemove(int position);
        // --

    private:
        DECLARE_INVARIANT();

        ScopedPtr<CCHRfile> chr;
    };

}
