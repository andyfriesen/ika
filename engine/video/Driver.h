#ifndef VIDEO_DRIVER_H
#define VIDEO_DRIVER_H

#include "Image.h"
#include "ColourHandler.h"

#include "common/types.h"
#include "common/Canvas.h"

// linux hack.
#ifdef None
#   undef None
#endif

/// The video driver interface lives here.
namespace Video
{
    /// Thrown when something in the video system screws up. ;)
    class Exception{};

    /// Different methods for alpha blending
    enum BlendMode
    {
        None,
        Matte,
        Normal,
        Add,
        Subtract
    };

    /// Base interface for all ika video drivers.
    class Driver : public ColourHandler
    {
    public:
        virtual ~Driver(){}

        /// Switches the driver to display fullscreen.
        virtual void SwitchToFullScreen() = 0;

        /// Switches the driver to display in a window.
        virtual void SwitchToWindowed() = 0;

        /// Changes the resolution of the output.  Returns true on success.
        virtual bool SwitchResolution(int x, int y) = 0;

        /// Creates a new image from the provided pixel buffer.
        virtual Image* CreateImage(Canvas &pm) = 0;

        /// Frees the previously created image.
        virtual void FreeImage(Image* img) = 0;

        /// Clips the image to the provided rectangle.
        virtual void ClipScreen(int left, int top, int right, int bottom) = 0;

        /// Flips the buffers, displays the screen, whatever.
        virtual void ShowPage() = 0;

        /// Clears the screen!  With blackness!
        virtual void ClearScreen() = 0;

        /// Sets the blending mode.
        /// @returns The old blending mode.
        virtual BlendMode SetBlendMode(BlendMode bm) = 0;

        /// Blits an image to the screen.
        virtual void BlitImage(Image* img, int x, int y) = 0;
        
        /// Blits a rectangular piece of an image to the screen.
        virtual void ClipBlitImage(Video::Image* i, int x, int y, int ix, int iy, int iw, int ih) = 0;

        /// Blits a scaled version of the provided image to the screen.
        virtual void ScaleBlitImage(Image* img, int x, int y, int w, int h) = 0;

        /// Blits a distorted version of the image to the screen, given the provided corner points.
        virtual void DistortBlitImage(Image* img, int x[4], int y[4]) = 0;

        /// "tile" blits an image to fill the rect specified.
        virtual void TileBlitImage(Image* img, int x, int y, int w, int h, float scalex, float scaley) = 0;

        /// Blits the image, using tint as a colour mask thingie.
        virtual void TintBlitImage(Image* img, int x, int y, u32 tint) = 0;

        /// DistortBlits an image, using the colour array to tint each corner of the image.  Colours are interpolated
        /// like OpenGL usually does when rendering textured, distorted quads.
        virtual void TintDistortBlitImage(Image* img, int x[4], int y[4], u32 colour[4]) = 0;

        /// Draws a single pixel on the screen.
        virtual void DrawPixel(int x, int y, u32 colour) = 0;

        /// Gets the value of a pixel from the screen.
        virtual u32 GetPixel(int x, int y) = 0;

        /// Draws a line on the screen.
        virtual void DrawLine(int x1, int y1, int x2, int y2, u32 colour) = 0;

        /// Draws a rectangle on the screen.
        virtual void DrawRect(int x1, int y1, int x2, int y2, u32 colour, bool filled) = 0;

        /// Draws an ellipse on the screen.
        virtual void DrawEllipse(int cx, int cy, int rx, int ry, u32 colour, bool filled) = 0;

        /// Draws a triangle on the screen.
        virtual void DrawTriangle(int x[3], int y[3], u32 colour[3]) = 0;

        /// Grabs a rect from the screen, constructs an image from it, and returns it
        virtual Image* GrabImage(int x1, int y1, int x2, int y2) = 0;

        /// Like GrabImage, but stores the contents on a canvas, not an image
        virtual Canvas* GrabCanvas(int x1, int y1, int x2, int y2) = 0;

        /// Returns the size of the viewport, in pixels.
        virtual Point GetResolution() const = 0;

        /// Returns the number of times ShowPage() has been called in the past second.
        virtual int GetFrameRate() const = 0;

    };
}

#endif

