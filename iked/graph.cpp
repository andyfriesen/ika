
#include "graph.h"

CGraphFrame* CGraphFactory::CreateFrame(wxWindow* parent)
{
    return new CGraphFrame(parent);
}

CImage* CGraphFactory::CreateImage(const CPixelMatrix& src)
{
    return new CImage(src);
}

CGraphFrame::CGraphFrame(wxWindow* parent)
:   wxGLCanvas(parent,(wxGLCanvas*)0)
{
    int w,h;
    GetSize(&w,&h);

    Show();

    SetCurrent();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

 //   glEnable(GL_SCISSOR_TEST);
   // glScissor(0,0,w,h);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f,w,h,0.0f,-1.0f,1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

CGraphFrame::~CGraphFrame()
{
}

void CGraphFrame::Rect(int x,int y,int w,int h,RGBA colour)
{
    SetCurrent();

    glColor4ub(colour.r,colour.g,colour.b,colour.a);

    glBegin(GL_LINE_LOOP);

    glVertex2i(x,y);
    glVertex2i(x+w,y);
    glVertex2i(x+w,y+h);
    glVertex2i(x,y+h);

    glEnd();
}

void CGraphFrame::RectFill(int x,int y,int w,int h,RGBA colour)
{
    SetCurrent();

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
    SetCurrent();

    glBindTexture(GL_TEXTURE_2D,src.hTex);
    glBegin(GL_QUADS);

    glTexCoord2f(0,0);      glVertex2i(x,y);
    glTexCoord2f(1,0);      glVertex2i(x+src.nWidth,y);
    glTexCoord2f(1,1);      glVertex2i(x+src.nWidth,y+src.nHeight);
    glTexCoord2f(0,1);      glVertex2i(x,y+src.nHeight);

    glEnd();
}

void CGraphFrame::ScaleBlit(CImage& src,int x,int y,int w,int h,bool trans)
{
}

void CGraphFrame::ShowPage()
{
    SwapBuffers();
}

CImage::CImage(const CPixelMatrix& src)
{
    glGenTextures(1,&hTex);
    nWidth=src.Width();
    nHeight=src.Height();

    int nTexwidth=nWidth;
    int nTexheight=nHeight;

    glBindTexture(GL_TEXTURE_2D,hTex);
    glTexImage2D(GL_TEXTURE_2D,0,4,nWidth,nHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,(u32*)src.GetPixelData());
}

CImage::~CImage()
{
    glDeleteTextures(1,&hTex);
}