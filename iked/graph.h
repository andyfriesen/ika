
// TODO: Make a second implementation of this stuff, and make these three classes into interfaces, with their existing implementation
// moved to GLGraphFactory, GLGraphFrame, and GLImage, respectively.

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include "types.h"
#include "pixel_matrix.h"
#include <set>
#include <wx\wx.h>
#include <wx\glcanvas.h>

// win32 is retarded -- andy
#ifdef WIN32
#   undef FindText
#endif

class CGraphFrame;
class CImage;

class CGraphFrame : public wxGLCanvas
{
    /*    
        We keep a list of all open CGraphFrame instances so that they can all have the same OpenGL context.
        When creating a new instance, the constructor uses an (any, it doesn't matter which) element from
        this set, if there is one.  If not, then it creates a new OpenGL context.

        One odd point is that every CImage is dependant on a GL context.  So if any CImages exist, while
        there is no context, things might get icky.  However, this does not make sense, as CImages exist
        only so that they may be blitted on CGraphFrames.  Just something to keep in mind.
    */
    static std::set<CGraphFrame*>  pInstances;

    // The GraphFrame does its own zooming.  Everything possible is done to make this invisible to the parent window.
    int nZoom;
public:

    CGraphFrame(wxWindow* parent);
    ~CGraphFrame();

    void Rect(int x,int y,int w,int h,RGBA colour);
    void RectFill(int x,int y,int w,int h,RGBA colour);

    void Blit(CImage& src,int x,int y,bool trans);
    void ScaleBlit(CImage& src,int x,int y,int w,int h,bool trans);

    void Clear();

    void ShowPage();

    void OnErase(wxEraseEvent&) {}
    void OnSize(wxSizeEvent& event);
    void OnMouseEvent(wxMouseEvent& event);

    int LogicalWidth() const;
    int LogicalHeight() const;

    int Zoom() const;
    void Zoom(int z);

    DECLARE_EVENT_TABLE()
};

class CImage
{
    friend CGraphFrame; // :x
protected:
    GLuint hTex;
    int nWidth,nHeight;
    int nTexwidth,nTexheight;

public:
    CImage(const CPixelMatrix& src);
    ~CImage();

    void Update(const CPixelMatrix& src);
};

#endif