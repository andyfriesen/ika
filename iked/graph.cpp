
#include "graph.h"
#include "common/log.h"

static const int nZoomscale = 16;

//-------------------------------------------------------

namespace iked {

    BEGIN_EVENT_TABLE(GraphicsFrame, wxGLCanvas)
	EVT_ERASE_BACKGROUND(GraphicsFrame::OnErase)
	//EVT_PAINT(GraphicsFrame::OnPaint)
	EVT_SIZE(GraphicsFrame::OnSize)

	// GraphFrames send all mouse events to their parent window, after adapting for zoom
	EVT_MOUSE_EVENTS(GraphicsFrame::OnMouseEvent)
    END_EVENT_TABLE()

    static void SetTex(uint tex) {
	static uint last = 0;

	if (tex==last)
	    return;
	glBindTexture(GL_TEXTURE_2D, tex);
	last = tex;
    }

    std::set<GraphicsFrame*> GraphicsFrame::allFrames;

    GraphicsFrame::GraphicsFrame(wxWindow* parent)
	// Borrow a context from an existing frame, if there is one
	: wxGLCanvas(parent, 
	    allFrames.empty() 
		? 0//(wxGLCanvas*)0 
		: *allFrames.begin())
	, zoomFactor(16) 
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

	allFrames.insert(this);
    }

    GraphicsFrame::~GraphicsFrame() {
	allFrames.erase(this);
    }

    void GraphicsFrame::OnErase(wxEraseEvent&) { 
        // Do nothing.
    }

    void GraphicsFrame::OnSize(wxSizeEvent& event) {
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

    void GraphicsFrame::OnMouseEvent(wxMouseEvent& event) {
	// Tweak the mouse position, so that the parent doesn't have to compensate for interacting with the frame.
	event.m_x = event.m_x * zoomFactor / nZoomscale;
	event.m_y = event.m_y * zoomFactor / nZoomscale;
    }

    void GraphicsFrame::Rect(int x, int y, int w, int h, RGBA colour) {
	x = x * nZoomscale / zoomFactor;
	y = y * nZoomscale / zoomFactor;
	w = w * nZoomscale / zoomFactor;
	h = h * nZoomscale / zoomFactor;

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

    void GraphicsFrame::RectFill(int x, int y, int w, int h, RGBA colour) {
	x = x * nZoomscale / zoomFactor;
	y = y * nZoomscale / zoomFactor;
	w = w * nZoomscale / zoomFactor;
	h = h * nZoomscale / zoomFactor;

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

    void GraphicsFrame::Blit(Image& src, int x, int y, bool trans) {
	ScaleBlit(src, x, y, src.width, src.height, trans);
    }

    void GraphicsFrame::ScaleBlit(Image& src, int x, int y, int w, int h, bool trans) {
	x = x * nZoomscale / zoomFactor;
	y = y * nZoomscale / zoomFactor;
	w = w * nZoomscale / zoomFactor;
	h = h * nZoomscale / zoomFactor;
        
	if (zoomFactor!=nZoomscale)        w++, h++;

	GLfloat nTexendx = 1.0f * src.width / src.texWidth;
	GLfloat nTexendy = 1.0f * src.height / src.texHeight;

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

    void GraphicsFrame::Clear() {
	glClear(GL_COLOR_BUFFER_BIT);
    }

    void GraphicsFrame::ShowPage() {
	SwapBuffers();
    }

    int GraphicsFrame::LogicalWidth() const {
	return GetClientSize().GetWidth() * zoomFactor / nZoomscale;
    }

    int GraphicsFrame::LogicalHeight() const {
	return GetClientSize().GetHeight() * zoomFactor / nZoomscale;
    }

    int GraphicsFrame::Zoom() const {
	return zoomFactor;
    }

    void GraphicsFrame::Zoom(int z) {
	zoomFactor = z;
    }

    Image::Image(const Canvas& src) {
	glGenTextures(1, &hTex);
	Update(src);
    }

    Image::~Image() {
	SetTex(0);
	glDeleteTextures(1, &hTex);
    }

    void Image::Update(const Canvas& src) {
	width = src.Width();
	height = src.Height();

	texWidth = nextPowerOf2(width);
	texHeight = nextPowerOf2(height);

	Canvas tmp(src);
	tmp.Resize(texWidth, texHeight);

	SetTex(hTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (u32*)tmp.GetPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

}
