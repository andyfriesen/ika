/*
uh... it's stuff! :D

  added ExtensionsSupported, which checks if an OGL extension is supported. o_O surprising, isn't it?
  (intended for use with pBuffers and possibly texture compression :))
*/

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

bool ExtensionSupported(const char *extension)
{
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;
    
    where = (GLubyte *) strchr(extension, ' ');
    if (where || *extension == '\0')
	return 0;
    extensions = glGetString(GL_EXTENSIONS);
    start = extensions;
    for (;;) {
	where = (GLubyte *) strstr((const char *) start, extension);
	if (!where)
	    break;
	terminator = where + strlen(extension);
	if (where == start || *(where - 1) == ' ')
	    if (*terminator == ' ' || *terminator == '\0')
		return true;
	    start = terminator;
    }
    return false;
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

