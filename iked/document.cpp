
#include <stdexcept>

#include "document.h"
#include "documentresource.h"

namespace iked {

    Document::Document()
        : refCount(0)
    {}

    int Document::getRefCount() const {
        return refCount;
    }

    void Document::ref() {
        assert(refCount >= 0);
        refCount++;
    }

    void Document::unref() {
        assert(refCount > 0);
        refCount--;
        if (refCount == 0) {
            // Temporarily make refcount 1, call destroyed event, then 
            // decrement and retest against 0, just in case somebody 
            // revived the document.
            // Potential bug: an erronious unref() someplace else may cause scary infinite loops!
            refCount = 1;
            destroyed.fire(this);
            if (refCount == 1) {
                delete this;
            }
        }
    }

    Document::~Document() {
        destroyed.fire(this);
    }
}
