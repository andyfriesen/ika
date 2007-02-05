#pragma once
#include "../../common/utility.h"

namespace Video {
    /**
     * This class defines the base interface for all device-dependent images.
     */
    struct Image {
        virtual int Width() = 0;         ///< Returns the width of the image
        virtual int Height() = 0;        ///< Returns the height of the image
        virtual uint GetTextureID() = 0; ///< Returns the texture ID of the image.

    protected:
        virtual ~Image(){}
    };
}
