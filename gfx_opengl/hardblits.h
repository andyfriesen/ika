
void SynchTexture(handle img)
{
    int width=img->nWidth;
    int height=img->nHeight;
    int nTexwidth,nTexheight;
    
    if (!img->hTex)
    {
        nTexwidth=1<<(int(ceil(log(width)/log(2))));
        nTexheight=1<<(int(ceil(log(height)/log(2))));
        
        if (width>nMaxtexsize || height>nMaxtexsize)
        {
            if (img->hTex)
                glDeleteTextures(1,&img->hTex);
            img->hTex=0;
            return; // image is too big to have a texture
        }
        
        img->nTexwidth=nTexwidth;
        img->nTexheight=nTexheight;
        
        glGenTextures(1,&img->hTex);
    }
    else
    {
        nTexwidth=img->nTexwidth;
        nTexheight=img->nTexheight;
    }
    
    u32* pTemp;
    if (nTexwidth==width && nTexheight==height)
        pTemp=(u32*)img->pixels.GetPixelData();
    else
    {
        pTemp=new u32[nTexwidth*nTexheight];
        ZeroMemory(pTemp,nTexwidth*nTexheight*sizeof(u32));
        
        for (int y=0; y<height; y++)
            memcpy(pTemp+y*nTexwidth,img->pixels.GetPixelData()+y*width,width*sizeof(u32));
    }
    
    glBindTexture(GL_TEXTURE_2D,img->hTex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,nTexwidth,nTexheight,0,GL_RGBA,GL_UNSIGNED_BYTE,pTemp);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    if (nTexwidth!=width || nTexheight!=height)
        delete[] pTemp;
    
    glFlush();
}

void RenderTexture(handle img,int x,int y,bool transparent)
{
    if (img->nWidth==32 && img->nHeight==32)
    {
        int a=5;
        a++;
    }

    // texX/1 = width/img->width
    GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
    GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;
    
    if (transparent)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    
    glDisable(GL_COLOR_MATERIAL);            // no tinting
    
    glBindTexture(GL_TEXTURE_2D,img->hTex);
    glBegin(GL_QUADS);
    glColor4ub(255,255,255,255);
    glTexCoord2f(0.0f,0.0f);            glVertex2i(x            ,y             );
    glTexCoord2f(nTexendx,0.0f);        glVertex2i(x+img->nWidth,y             );
    glTexCoord2f(nTexendx,nTexendy);    glVertex2i(x+img->nWidth,y+img->nHeight);
    glTexCoord2f(0.0f,nTexendy);        glVertex2i(x            ,y+img->nHeight);
    glEnd();
}

void ScaleRenderTexture(handle img,int x,int y,int width,int height,bool transparent)
{
    GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
    GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;
    
    if (transparent)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    
    glDisable(GL_COLOR_MATERIAL);
    
    glBindTexture(GL_TEXTURE_2D,img->hTex);
    glBegin(GL_QUADS);
    glColor4ub(255,255,255,255);
    glTexCoord2f(0.0f,0.0f);            glVertex2i(x      ,y       );
    glTexCoord2f(nTexendx,0.0f);        glVertex2i(x+width,y       );
    glTexCoord2f(nTexendx,nTexendy);    glVertex2i(x+width,y+height);
    glTexCoord2f(0.0f,nTexendy);        glVertex2i(x      ,y+height);
    glEnd();
}

void RotScaleRenderTexture(handle img,int cx,int cy,float angle,int scale,bool transparent)
{
    GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
    GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;
    
    if (transparent)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    
    glDisable(GL_COLOR_MATERIAL);
    
    glPushMatrix();
    glRotatef(angle, GLfloat(img->nWidth/2), GLfloat(img->nHeight/2), 0.0f);
    
    glBindTexture(GL_TEXTURE_2D,img->hTex);
    glBegin(GL_QUADS);
    glColor4ub(255,255,255,255);
    glTexCoord2f(0.0f,0.0f);            glVertex2i(cx                  ,cy                   );
    glTexCoord2f(nTexendx,0.0f);        glVertex2i(cx+img->nWidth*scale,cy                   );
    glTexCoord2f(nTexendx,nTexendy);    glVertex2i(cx+img->nWidth*scale,cy+img->nHeight*scale);
    glTexCoord2f(0,nTexendy);           glVertex2i(cx                  ,cy+img->nHeight*scale);
    glEnd();
    
    glPopMatrix();
}

//----------------------------------- Primatives ----------------------------------

void HardLine(int x1,int y1,int x2,int y2,u32 colour)
{
    glBegin(GL_LINES);
    glColor4ub((colour>>16)&255,(colour>>8)&255,colour&255,(colour>>24)&255);
    glVertex2i(x1,y1);
    glVertex2i(x2,y2);
    glColor4ub(255,255,255,255);
    glEnd();
}

void HardRect(int x1,int y1,int x2,int y2,u32 colour,bool filled)
{
    RGBA col=colour;

    glBindTexture(GL_TEXTURE_2D,NULL);
    if (filled)
    {
        glBegin(GL_QUADS);
        glColor4ub(col.r & 255,col.g & 255,col.b & 255,col.a & 255);
        glVertex2i(x1,y1);
        glVertex2i(x2,y1);
        glVertex2i(x2,y2);
        glVertex2i(x1,y2);
        glColor4ub(255,255,255,255);
        glEnd();
    }
    else
    {
        glBegin(GL_LINES);
        glColor4ub(col.r & 255,col.g & 255,col.b & 255,col.a & 255);
        glVertex2i(x1,y1);
        glVertex2i(x2,y1);
        
        glVertex2i(x2,y1);
        glVertex2i(x2,y2);
        
        glVertex2i(x2,y2);
        glVertex2i(x1,y2);
        
        glVertex2i(x1,y2);
        glVertex2i(x1,y1);
        glColor4ub(255,255,255,255);
        glEnd();
    }
}

void HardPoly(handle img,int x[3],int y[3],u32 colour[3])
{
    glBegin(GL_TRIANGLES);
    for (int i=0; i<3; i++)
    {
        glColor4ub((colour[i]>>16)&255,(colour[i]>>8)&255,colour[i]&255,(colour[i]>>24)&255);
        glVertex2i(x[i],y[i]);
        glColor4ub(255,255,255,255);
    }
    glEnd();
}

void HardPoint(handle img,int x,int y,u32 colour)
{
    glBegin(GL_POINTS);
    glColor4ub((colour>>16)&255,(colour>>8)&255,colour&255,(colour>>24)&255);
    glVertex2i(x,y);
    glColor4ub(255,255,255,255);
    glEnd();
}