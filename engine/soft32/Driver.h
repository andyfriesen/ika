#if 0
#ifndef SOFT32_DRIVER_H
#define SOFT32_DRIVER_H

#include "SDL/SDL.h"

#include "video/Driver.h"
#include "common/Canvas.h"

#include "FPSCounter.h"

/// Software 32-bit video driver implementation
namespace Soft32
{
    struct NotYetImplementedException{};
#define NYI { throw NotYetImplementedException(); }

    class Image;

    /// The driver itself.
    class Driver : public Video::Driver
    {
    public:
        Driver(int xres, int yres, int bpp, bool fullscreen);
        ~Driver();

        /// Switches the driver to display fullscreen.
        virtual void SwitchToFullScreen();

        /// Switches the driver to display in a window.
        virtual void SwitchToWindowed();

        /// Changes the resolution of the output.
        virtual void SwitchResolution(int x, int y);

        /// Creates a new image from the provided pixel buffer.
        virtual Video::Image* CreateImage(Canvas &pm);

        /// Frees the previously created image.
        /// @note As of yet, this isn't really necessary.
        /// deleting the image is safe.
        virtual void FreeImage(Video::Image* img);

        /// Clips the image to the provided rectangle.
        virtual void ClipScreen(int left, int top, int right, int bottom) NYI

        /// Flips the buffers, displays the screen, whatever.
        virtual void ShowPage();

        /// Sets the current blend mode.
        virtual Video::BlendMode SetBlendMode(Video::BlendMode bm);

        /// Blits an image to the screen.
        virtual void BlitImage(Video::Image* img, int x, int y);

        /// Blits a scaled version of the provided image to the screen.
        virtual void ScaleBlitImage(Video::Image* img, int x, int y, int w, int h);

        /// Blits a distorted version of the image to the screen, given the provided corner points.
        virtual void DistortBlitImage(Video::Image* img, int x[4], int y[4]) NYI

        /// "tile" blits an image to fill the rect specified.
        virtual void TileBlitImage(Video::Image* img, int x, int y, int w, int h, float scalex, float scaley) NYI

        /// Draws a single pixel on the screen.
        virtual void DrawPixel(int x, int y, u32 colour);

        /// Gets the value of a pixel on the screen.
        virtual u32 GetPixel(int x, int y);

        /// Draws a line on the screen.
        virtual void DrawLine(int x1, int y1, int x2, int y2, u32 colour);

        /// Draws a rectangle on the screen.
        virtual void DrawRect(int x1, int y1, int x2, int y2, u32 colour, bool filled);

        /// Draws an ellipse on the screen.
        virtual void DrawEllipse(int cx, int cy, int rx, int ry, u32 colour, bool filled);

        /// Draws a triangle on the screen.
        virtual void DrawTriangle(int x[3], int y[3], u32 colour[3]) NYI

        /// Grabs a rect from the screen, constructs an image from it, and returns it
        virtual ::Video::Image* GrabImage(int x1, int y1, int x2, int y2) NYI

        /// Like GrabImage, but stores the contents on a canvas, not an image
        virtual Canvas* GrabCanvas(int x1, int y1, int x2, int y2) NYI

        /// Returns the size of the viewport, in pixels.
        virtual Point GetResolution() const;

        /// Returns the number of times ShowPage() has been called in the past second.
        virtual int GetFrameRate() const;
    private:
        FPSCounter fps;
        SDL_Surface* _screen;
        int _xres;
        int _yres;
        int _bpp;
        bool _fullscreen;
        u32 _rmask, _gmask, _bmask, _amask;

        Video::BlendMode _blendMode;

        void Driver::HLine(int x1, int x2, int y, u32 colour);
        void Driver::VLine(int x, int y1, int y2, u32 colour);

    };
#undef NYI
};

#endif
#endif