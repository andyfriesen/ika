
#include "graph.h"
#include "log.h"



//-------------------------------------------------------

BEGIN_EVENT_TABLE(CGraphFrame,wxGLCanvas)
    EVT_ERASE_BACKGROUND(CGraphFrame::OnErase)
    EVT_SIZE(CGraphFrame::OnSize)

    // hack -- GraphFrames Send all mouse events to their parent window.
    EVT_MOUSE_EVENTS(CGraphFrame::OnMouseEvent)
END_EVENT_TABLE()

std::set<CGraphFrame*> CGraphFrame::pInstances;

CGraphFrame::CGraphFrame(wxWindow* parent)
:   wxGLCanvas(parent,pInstances.empty() ? (wxGLCanvas*)0 : *pInstances.begin())
{
    int w,h;
    GetClientSize(&w,&h);

    Show();

    SetCurrent();
    glClearColor(0,0,0,0);
    glClearDepth(1);

    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,w,h);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f,w,h,0.0f,-1.0f,1.0f);
    
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
    int w=event.GetSize().GetWidth();
    int h=event.GetSize().GetHeight();

    SetSize(w,h);
    SetCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f,w,h,0.0f,-1.0f,1.0f);
/*
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
*/

    glViewport(0,0,w,h);

    glScissor(0, 0, w,h);
}

void CGraphFrame::OnMouseEvent(wxMouseEvent& event)
{
    wxPostEvent(GetParent(),event);
}

void CGraphFrame::Rect(int x,int y,int w,int h,RGBA colour)
{
    glColor4ub(colour.r,colour.g,colour.b,colour.a);
    glBindTexture(GL_TEXTURE_2D,0);

    glBegin(GL_LINE_LOOP);

    glVertex2i(x,y);
    glVertex2i(x+w,y);
    glVertex2i(x+w,y+h);
    glVertex2i(x,y+h);

    glEnd();
}

void CGraphFrame::RectFill(int x,int y,int w,int h,RGBA colour)
{
    glBindTexture(GL_TEXTURE_2D,0);
    glColor4ub(colour.r,colour.g,colour.b,colour.a);

    glBegin(GL_QUADS);

    glVertex2i(x,y);
    glVertex2i(x+w,y);
    glVertex2i(x+w,y+h);
    glVertex2i(x,y+h);

    glEnd();
}

void CGraphFrame::Blit(CImage& src,int x,int y,bool trans)
{
    ScaleBlit(src,x,y,src.nWidth,src.nHeight,trans);
}

void CGraphFrame::ScaleBlit(CImage& src,int x,int y,int w,int h,bool trans)
{
    GLfloat nTexendx=1.0f*src.nWidth/src.nTexwidth;
    GLfloat nTexendy=1.0f*src.nHeight/src.nTexheight;

    glBindTexture(GL_TEXTURE_2D,src.hTex);
    glColor4f(1,1,1,1); 

    glBegin(GL_QUADS);

    glTexCoord2f(       0,       0);      glVertex2i(x,y);
    glTexCoord2f(nTexendx,       0);      glVertex2i(x+w,y);
    glTexCoord2f(nTexendx,nTexendy);      glVertex2i(x+w,y+h);
    glTexCoord2f(       0,nTexendy);      glVertex2i(x,y+h);

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

CImage::CImage(const CPixelMatrix& src)
{
    glGenTextures(1,&hTex);
    Update(src);
}

CImage::~CImage()
{
    glDeleteTextures(1,&hTex);
}

void CImage::Update(const CPixelMatrix& src)
{
    nWidth=src.Width();
    nHeight=src.Height();

    nTexwidth=1;
    nTexheight=1;

    while (nTexwidth<nWidth) nTexwidth<<=1;
    while (nTexheight<nHeight) nTexheight<<=1;

    CPixelMatrix tmp(src);
    tmp.Resize(nTexwidth,nTexheight);

    glBindTexture(GL_TEXTURE_2D,hTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,nTexwidth,nTexheight,0,GL_RGBA,GL_UNSIGNED_BYTE,(u32*)tmp.GetPixelData());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}
