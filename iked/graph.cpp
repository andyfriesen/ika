
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
    Show();
    SwapBuffers();
}

CGraphFrame::~CGraphFrame()
{
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