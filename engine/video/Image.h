/** Video::Image
 * This class defines the base interface for all device-dependent images.
 */

#ifndef VIDEO_IMAGE_H
#define VIDEO_IMAGE_H

namespace Video
{
    class Image
    {
    public:
        virtual int Width() = 0;
        virtual int Height() = 0;
    };
}
#endif
