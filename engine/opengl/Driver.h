#ifndef OPENGL_DRIVER_H
#define OPENGL_DRIVER_H

#include "SDL/SDL.h"

#include "video/Driver.h"
#include "common/Canvas.h"

#include "FPSCounter.h"

/// OpenGL video driver implementation
namespace OpenGL
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
        virtual void SwitchToFullScreen() NYI

        /// Switches the driver to display in a window.
        virtual void SwitchToWindowed() NYI

        /// Changes the resolution of the output.
        virtual void SwitchResolution(int x, int y) NYI

        /// Creates a new image from the provided pixel buffer.
        virtual Video::Image* CreateImage(Canvas &pm);

        /// Frees the previously created image.
        /// @note As of yet, this isn't really necessary.
        /// deleting the image is safe.
        virtual void FreeImage(Video::Image* img);

        /// Clips the image to the provided rectangle.
        virtual void ClipImage(Video::Image* img, Rect& r) NYI

        /// Flips the buffers, displays the screen, whatever.
        virtual void ShowPage();

        /// Blits an image to the screen.
        virtual void BlitImage(Video::Image* img, int x, int y, bool transparent);

        /// Blits a scaled version of the provided image to the screen.
        virtual void ScaleBlitImage(Video::Image* img, int x, int y, int w, int h, bool transparent);

        /// Blits a distorted version of the image to the screen, given the provided corner points.
        virtual void DistortBlitImage(Video::Image* img, int x[4], int y[4], bool transparent) NYI

        /// Draws a single pixel on the screen.
        virtual void DrawPixel(int x, int y, u32 color) NYI

        /// Gets the value of a pixel in an image.
        virtual u32 GetPixel(Video::Image* img, int x, int y) NYI

        /// Draws a line on the screen.
        virtual void DrawLine(int x1, int y1, int x2, int y2, u32 color) NYI

        /// Draws a rectangle on the screen.
        virtual void DrawRect(int x1, int y1, int x2, int y2, u32 color, bool filled);

        /// Draws an ellipse on the screen.
        virtual void DrawEllipse(int cx, int cy, int rx, int ry, u32 color, bool filled) NYI

        /// Draws a polygon on the screen.
        virtual void DrawPoly(int x[3], int y[3], u32 color[3]) NYI

        /// Returns the size of the viewport, in pixels.
        virtual Point GetResolution() const;

        /// Returns the number of times ShowPage() has been called in the past second.
        virtual int GetFrameRate() const;
    private:
        FPSCounter fps;
        SDL_Surface* _screen;
        int _xres;
        int _yres;

        uint _lasttex;
        void SwitchTexture(uint tex);
    };
};

#endif