/* Video::Image
 * This class defines the base interface for all device-dependent images.
*/

#ifndef VIDEO_IMAGE_H
#define VIDEO_IMAGE_H

namespace Video	{
class Image	{
    public:
        virtual int Width(void) = 0;
        virtual int Height(void) = 0;
};
}
#endif
