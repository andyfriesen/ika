
#include "graph.h"
#include "common/log.h"

static const int nZoomscale = 16;

//-------------------------------------------------------

BEGIN_EVENT_TABLE(CGraphFrame, wxGLCanvas)
    EVT_ERASE_BACKGROUND(CGraphFrame::OnErase)
    //EVT_PAINT(CGraphFrame::OnPaint)
    EVT_SIZE(CGraphFrame::OnSize)

    // GraphFrames send all mouse events to their parent window, after adapting for zoom
    EVT_MOUSE_EVENTS(CGraphFrame::OnMouseEvent)
END_EVENT_TABLE()

static void SetTex(uint tex)
{
    static uint last = 0;

    if (tex==last)
        return;
    glBindTexture(GL_TEXTURE_2D, tex);
    last = tex;
}

std::set<CGraphFrame*> CGraphFrame::pInstances;

CGraphFrame::CGraphFrame(wxWindow* parent)
:   wxGLCanvas(parent, pInstances.empty() ? (wxGLCanvas*)0 : *pInstances.begin()), nZoom(16)
{
    int w, h;
    GetClientSize(&w, &h);

    Show();

    SetCurrent();
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);

    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, w, h);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, w, h, 0.0f, -1.0f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    pInstances.insert(this);
}

CGraphFrame::~CGraphFrame()
{
    pInstances.erase(this);
}

void CGraphFrame::OnSize(wxSizeEvent& event)
{
    int w = event.GetSize().GetWidth();
    int h = event.GetSize().GetHeight();

    SetSize(w, h);
    SetCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, w, h, 0.0f, -1.0f, 1.0f);

    glViewport(0, 0, w, h);

    glScissor(0, 0, w, h);
}

void CGraphFrame::OnMouseEvent(wxMouseEvent& event)
{
    // Tweak the mouse position, so that the parent doesn't have to compensate for interacting with the frame.
    event.m_x = event.m_x * nZoom / nZoomscale;
    event.m_y = event.m_y * nZoom / nZoomscale;

    // Relay to the parent.
    wxPostEvent(GetParent(), event);
}

void CGraphFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    wxPostEvent(GetParent(), event);
}

void CGraphFrame::Rect(int x, int y, int w, int h, RGBA colour)
{
    x = x * nZoomscale / nZoom;
    y = y * nZoomscale / nZoom;
    w = w * nZoomscale / nZoom;
    h = h * nZoomscale / nZoom;

    glColor4ub(colour.r, colour.g, colour.b, colour.a);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINE_LOOP);

    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);

    glEnd();

    glColor4f(1, 1, 1, 1);
    glEnable(GL_TEXTURE_2D);
}

void CGraphFrame::RectFill(int x, int y, int w, int h, RGBA colour)
{
    x = x * nZoomscale / nZoom;
    y = y * nZoomscale / nZoom;
    w = w * nZoomscale / nZoom;
    h = h * nZoomscale / nZoom;

    glDisable(GL_TEXTURE_2D);
    glColor4ub(colour.r, colour.g, colour.b, colour.a);

    glBegin(GL_QUADS);

    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);

    glEnd();

    glColor4f(1, 1, 1, 1);
    glEnable(GL_TEXTURE_2D);
}

void CGraphFrame::Blit(CImage& src, int x, int y, bool trans)
{
    ScaleBlit(src, x, y, src.nWidth, src.nHeight, trans);
}

void CGraphFrame::ScaleBlit(CImage& src, int x, int y, int w, int h, bool trans)
{
    x = x * nZoomscale / nZoom;
    y = y * nZoomscale / nZoom;
    w = w * nZoomscale / nZoom;
    h = h * nZoomscale / nZoom;
    
    if (nZoom!=nZoomscale)        w++, h++;

    GLfloat nTexendx = 1.0f * src.nWidth / src.nTexwidth;
    GLfloat nTexendy = 1.0f * src.nHeight / src.nTexheight;

    SetTex(src.hTex);
    if (trans)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    glBegin(GL_QUADS);

    glTexCoord2f(       0,       0);    glVertex2i(x, y);
    glTexCoord2f(nTexendx,       0);    glVertex2i(x + w, y);
    glTexCoord2f(nTexendx, nTexendy);   glVertex2i(x + w, y + h);
    glTexCoord2f(       0, nTexendy);   glVertex2i(x, y + h);

    glEnd();
}

void CGraphFrame::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void CGraphFrame::ShowPage()
{
    SwapBuffers();
}

int CGraphFrame::LogicalWidth() const
{
    return GetClientSize().GetWidth() * nZoom / nZoomscale;
}

int CGraphFrame::LogicalHeight() const
{
    return GetClientSize().GetHeight() * nZoom / nZoomscale;
}

int CGraphFrame::Zoom() const
{
    return nZoom;
}

void CGraphFrame::Zoom(int z)
{
    nZoom = z;
}

CImage::CImage(const Canvas& src)
{
    glGenTextures(1, &hTex);
    Update(src);
}

CImage::~CImage()
{
    SetTex(0);
    glDeleteTextures(1, &hTex);
}

void CImage::Update(const Canvas& src)
{
    nWidth = src.Width();
    nHeight = src.Height();

    nTexwidth = 1;
    nTexheight = 1;

    while (nTexwidth < nWidth) nTexwidth<<=1;
    while (nTexheight < nHeight) nTexheight<<=1;

    Canvas tmp(src);
    tmp.Resize(nTexwidth, nTexheight);

    SetTex(hTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nTexwidth, nTexheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (u32*)tmp.GetPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
