/* Video::Driver
 * This class defines the base interface for all ika
 * video drivers.
*/

#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include "Image.h"
#include "common/types.h"
#include "common/pixel_matrix.h"

namespace Video    {
class Driver    {
    public:
        // Returns the version of the video driver.
        virtual int GetVersion() = 0;
        
        // Initializes the video subsystem and prepares to draw.
        virtual bool Init(int x, int y, int bpp, bool fullscreen) = 0;
        // Shuts down the video subsystem and frees resources.
        virtual bool Shutdown() = 0;
        // Switches the driver to display fullscreen.
        virtual bool SwitchToFullScreen() = 0;
        // Switches the driver to display in a window.
        virtual bool SwitchToWindowed() = 0;
        // Changes the resolution of the output.
        virtual bool SwitchResolution(int x, int y) = 0;
        // Creates a new image of the requested size.
        virtual Video::Image* CreateImage(int x, int y) = 0;
        // Creates a new image from the provided pixel buffer.
        virtual Video::Image* CreateImage(CPixelMatrix &pm);
        // Frees the previously created image.
        virtual bool FreeImage(Video::Image* img) = 0;
        // Creates a copy of the provided image.
        virtual Video::Image* CopyImage(Video::Image* img) = 0;
        // Clips the image to the provided rectangle.
        virtual bool ClipImage(Video::Image* img, Rect& r) = 0;
        // Returns the image that is treates as the screen.
        virtual Video::Image* GetScreenImage() = 0;
        // Flips the buffers, displays the screen, whatever.
        virtual bool ShowPage() = 0;
        // Blits an image to the screen.
        virtual bool BlitImage(Video::Image* img, int x, int y, bool transparent) = 0;
        // Blits a scaled version of the provided image to the screen.
        virtual bool ScaleBlitImage(Video::Image* img, int x, int y, int w, int h, bool transparent) = 0;
        // Blits a distorted version of the image to the screen, given the provided corner points.
        virtual bool DistortBlitImage(Video::Image* img, int x[4], int y[4], bool transparent) = 0;
        // Sets a pixel to a color on the provided image.
        virtual bool SetPixel(Video::Image* img, int x, int y, u32 color) = 0;
        // Gets the value of a pixel in an image.
        virtual u32 GetPixel(Video::Image* img, int x, int y) = 0;
        // Draws a line on the image.
        virtual bool DrawLine(Video::Image* img, int x1, int y1, int x2, int y2, u32 color) = 0;
        // Draws a rectangle on the image.
        virtual bool DrawRect(Video::Image* img, int x1, int y1, int x2, int y2, bool filled) = 0;
        // Draws an ellipse on the image.
        virtual bool DrawEllipse(Video::Image* img, int cx, int cy, int rx, int ry, u32 color, bool filled) = 0;
        // Draws a polygon on the image.
        virtual bool DrawPoly(Video::Image* img, int x[3], int y[3], u32 color[3]) = 0;
};
}

#endif
