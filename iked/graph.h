
// TODO: Make a second implementation of this stuff, and make these three classes into interfaces, with their existing implementation
// moved to GLGraphFactory, GLGraphFrame, and GLImage, respectively.

#pragma once

#include "common/utility.h"
#include "common/Canvas.h"
#include <set>
#include "wx/wx.h"
#include "wx/glcanvas.h"
#if _WIN32
#   include "wx/msw/winundef.h"
#endif

namespace iked {

    struct Image;

    struct GraphicsFrame : public wxGLCanvas {
        GraphicsFrame(wxWindow* parent);
        ~GraphicsFrame();

        void Rect(int x, int y, int w, int h, RGBA colour);
        void RectFill(int x, int y, int w, int h, RGBA colour);

        void Blit(Image& src, int x, int y, bool trans);
        void ScaleBlit(Image& src, int x, int y, int w, int h, bool trans);

        void Clear();

        void ShowPage();

        void OnErase(wxEraseEvent&);
        void OnSize(wxSizeEvent& event);
        void OnMouseEvent(wxMouseEvent& event);
        void OnPaint(wxPaintEvent& event);

        int LogicalWidth() const;
        int LogicalHeight() const;

        int Zoom() const;
        void Zoom(int z);

        /// x and y are client window coords.
        void getLogicalPosition(int* x, int* y);

        /// x and y are logical.  make them client window coords
        void getClientPosition(int* x, int* y);

        DECLARE_EVENT_TABLE()
    private:
        /**
         * We keep a list of all open GraphFrame instances so that they can all have the same OpenGL context.
         * When creating a new instance, the constructor uses an (any, it doesn't matter which) element from
         * this set, if there is one.  If not, then it creates a new OpenGL context.

         * One odd point is that every Image is dependant on a GL context.  So if any CImages exist, while
         * there is no context, things might get icky.  However, this does not make sense, as Images exist
         * only so that they may be blitted on GraphFrames.  Just something to keep in mind.
         */
        static std::set<GraphicsFrame*>  allFrames;

        /// The GraphFrame does its own zooming.  Everything possible is done to make this invisible to the parent window.
        int zoomFactor;
    };

    struct Image {
        friend struct GraphicsFrame;

        Image(const Canvas& src);
        ~Image();

        void Update(const Canvas& src);

    private:
        GLuint hTex;
        int width, height;
        int texWidth, texHeight;
    };

}
