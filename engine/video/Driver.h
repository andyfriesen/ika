/** Video::Driver
* This class defines the base interface for all ika
* video drivers.
*/

#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include "Image.h"

#include "common/types.h"
#include "common/Canvas.h"

namespace Video
{
    class Exception{};

    class Driver
    {
    public:
        virtual ~Driver(){}

        /// Returns the version of the video driver.
        //virtual int GetVersion() = 0;

        /// Initializes the video subsystem and prepares to draw.
        //virtual void Init(int x, int y, int bpp, bool fullscreen) = 0;

        /// Shuts down the video subsystem and frees resources.
        //virtual void Shutdown() = 0;

        /// Switches the driver to display fullscreen.
        virtual void SwitchToFullScreen() = 0;

        /// Switches the driver to display in a window.
        virtual void SwitchToWindowed() = 0;

        /// Changes the resolution of the output.
        virtual void SwitchResolution(int x, int y) = 0;

        /// Creates a new image from the provided pixel buffer.
        virtual Image* CreateImage(Canvas &pm) = 0;

        /// Frees the previously created image.
        virtual void FreeImage(Image* img) = 0;

        /// Creates a copy of the provided image.
        //virtual Image* CopyImage(Image* img) = 0;

        /// Clips the image to the provided rectangle.
        virtual void ClipImage(Image* img, Rect& r) = 0;

        /// Flips the buffers, displays the screen, whatever.
        virtual void ShowPage() = 0;

        /// Blits an image to the screen.
        virtual void BlitImage(Image* img, int x, int y, bool transparent) = 0;

        /// Blits a scaled version of the provided image to the screen.
        virtual void ScaleBlitImage(Image* img, int x, int y, int w, int h, bool transparent) = 0;

        /// Blits a distorted version of the image to the screen, given the provided corner points.
        virtual void DistortBlitImage(Image* img, int x[4], int y[4], bool transparent) = 0;

        /// Draws a single pixel on the screen.
        virtual void DrawPixel(int x, int y, u32 color) = 0;

        /// Gets the value of a pixel in an image.
        virtual u32 GetPixel(Video::Image* img, int x, int y) = 0;

        /// Draws a line on the screen.
        virtual void DrawLine(int x1, int y1, int x2, int y2, u32 color) = 0;

        /// Draws a rectangle on the screen.
        virtual void DrawRect(int x1, int y1, int x2, int y2, u32 color, bool filled) = 0;

        /// Draws an ellipse on the screen.
        virtual void DrawEllipse(int cx, int cy, int rx, int ry, u32 color, bool filled) = 0;

        /// Draws a polygon on the screen.
        virtual void DrawPoly(int x[3], int y[3], u32 color[3]) = 0;

        /// Returns the size of the viewport, in pixels.
        virtual Point GetResolution() const = 0;
    };
}

#endif
