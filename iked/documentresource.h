
#pragma once

namespace iked {
    
    /**
     * Base class of all document resources.
     * Handles cloning, not much else at the moment.
     */
    struct DocumentResource {
        virtual ~DocumentResource(){}
    };

}