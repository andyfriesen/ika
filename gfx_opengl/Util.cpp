#include "gfx_opengl.h"

void InitGL()
{
    glClearColor(0,0,0,0);
    glClearDepth(1);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,xres,yres);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void SizeWindow(int xsize,int ysize)
{
    if (!ysize)
	ysize++;
    
    glViewport(0,0,xsize,ysize);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f,xres,yres,0.0f,-1.0f,1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MakeClientFit()
{
    RECT client,window,goal;
    int ox,oy; // how far off are we?
    
    if (bFullscreen) return;  // why?
    goal.left=goal.top=0;
    goal.right=xres; goal.bottom=yres;
    
    GetClientRect(hGLWnd,&client);
    GetWindowRect(hGLWnd,&window);
    
    // find out how much adjustment we need to do
    ox=xres-client.right;
    oy=yres-client.bottom;
    
    // do it!
    window.right+=ox;
    window.bottom+=oy;
    
    POINT pt;
    
    pt.x=pt.y=0;
    
    ClientToScreen(hGLWnd,&pt);
    
    MoveWindow(hGLWnd,window.left,window.top,window.right-window.left,window.bottom-window.top,true);
}