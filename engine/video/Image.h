
#ifndef VIDEO_IMAGE_H
#define VIDEO_IMAGE_H

namespace Video
{
    /**
     * This class defines the base interface for all device-dependent images.
     */
    struct Image
    {
    protected:
        virtual ~Image(){}
    public:
        virtual int Width() = 0;        ///< Returns the width of the image
        virtual int Height() = 0;       ///< Returns the height of the image
    };
}
#endif

