
// TODO: Make a second implementation of this stuff, and make these three classes into interfaces, with their existing implementation
// moved to GLGraphFactory, GLVideoFrame, and GLImage, respectively.

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "common/utility.h"
#include "common/Canvas.h"
#include <set>
#include "wxinc.h"
#include <wx/glcanvas.h>

struct Image;

struct VideoFrame : public wxGLCanvas
{
    /*    
        We keep a list of all open VideoFrame instances so that they can all have the same OpenGL context.
        When creating a new instance, the constructor uses an (any, it doesn't matter which) element from
        this set, if there is one.  If not, then it creates a new OpenGL context.

        One odd point is that every Image is dependant on a GL context.  So if any Images exist, while
        there is no context, things might get icky.  However, this does not make sense, as Images exist
        only so that they may be blitted on VideoFrames.  Just something to keep in mind.
    */
    static std::set<VideoFrame*>  _instances;

    // The VideoFrame does its own zooming.  Everything possible is done to make this invisible to the parent window.
    int _curZoom;
public:

    VideoFrame(wxWindow* parent);
    virtual ~VideoFrame();

    void Rect(int x, int y, int w, int h, RGBA colour);
    void RectFill(int x, int y, int w, int h, RGBA colour);

    void Blit(Image& src, int x, int y, bool trans = true);
    void ScaleBlit(Image& src, int x, int y, int w, int h, bool trans = true);

    void Clear();

    void SetClearColour(RGBA colour = RGBA(0, 0, 0));

    void ShowPage();

    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    //void OnPaint(wxPaintEvent& event);

    int LogicalWidth() const;
    int LogicalHeight() const;

    int GetZoom() const;
    void SetZoom(int z);
    void IncZoom(int amt);

    DECLARE_EVENT_TABLE()
};

struct Image
{
    friend struct VideoFrame;
protected:
    GLuint hTex;
    int nWidth, nHeight;
    int nTexwidth, nTexheight;

public:
    Image(const Canvas& src);
    ~Image();

    void Update(const Canvas& src);
};

#endif