
#include <stdexcept>

#include "document.h"
#include "documentresource.h"

namespace iked {

    Document::~Document() {
        destroyed.fire(this);
    }
}
