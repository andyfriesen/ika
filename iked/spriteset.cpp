
#include <cassert>
#include "spriteset.h"
#include "common/chr.h"
#include "common/log.h"

namespace iked {

    SpriteSet::SpriteSet(int width, int height, int numFrames) 
        : ImageArrayDocument("")
    {
        chr = new CCHRfile(width, height);
        flush();
        invariant();
    }

    SpriteSet::SpriteSet(CCHRfile* c, const std::string& fileName)
        : ImageArrayDocument(fileName)
        , chr(c)
    {
        flush();
        invariant();
    }

    SpriteSet::~SpriteSet() {
        invariant();
        delete chr;
    }

    void SpriteSet::save(const std::string& fileName) {
        invariant();
        int pos = fileName.rfind('.');
        
        // If the extension is CHR, then export to the old format.
        if (Path::equals(
            Path::getExtension(fileName), "chr"))
        {
            chr->SaveOld(fileName);
        } else {
            chr->Save(fileName);
        }
    }

    void SpriteSet::sendCommand(commands::Command* cmd) {
        delete cmd;
    }

    void SpriteSet::resize(int width, int height) {
        chr->Resize(width, height);
        invariant();
    }

    SpriteSet* SpriteSet::asSpriteSet() {
        return this;
    }

    const Canvas& SpriteSet::doGetCanvas(int idx) {
        invariant();
        return chr->GetFrame(idx);
    }

    void SpriteSet::doSetCanvas(const Canvas& img, int idx) {
        chr->GetFrame(idx)=img;
        invariant();
    }

    int SpriteSet::doGetCount() {
        invariant();
        return chr->NumFrames();
    }

    int SpriteSet::doGetWidth() {
        invariant();
        return chr->Width();
    }

    int SpriteSet::doGetHeight() {
        return chr->Height();
    }

    void SpriteSet::doInsert(const Canvas& canvas, int position) {
        chr->InsertFrame(position, const_cast<Canvas&>(canvas));
        invariant();
    }

    void SpriteSet::doRemove(int position) {
        chr->DeleteFrame(position);
        invariant();
    }

    DEBUG_BLOCK(
        void SpriteSet::invariant() {
            assert(chr != 0);
        }
    )
}
