#include <math.h>
#include "gfx_opengl.h"
#include "SImage.h"

namespace
{
    inline void SwitchTexture(GLuint tex)
    {
        static GLuint hLasttex=-1;
        if (tex==hLasttex) return;

        glBindTexture(GL_TEXTURE_2D,tex);
        hLasttex=tex;
    }

}

void SynchTexture(IMAGE img)
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

void RenderTexture(IMAGE img,int x,int y,bool transparent)
{
    // texX/1 = width/img->width
    GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
    GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;
    
    if (transparent)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    
    SwitchTexture(img->hTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);            glVertex2i(x            ,y             );
    glTexCoord2f(nTexendx,0.0f);        glVertex2i(x+img->nWidth,y             );
    glTexCoord2f(nTexendx,nTexendy);    glVertex2i(x+img->nWidth,y+img->nHeight);
    glTexCoord2f(0.0f,nTexendy);        glVertex2i(x            ,y+img->nHeight);
    glEnd();
}

void ScaleRenderTexture(IMAGE img,int x,int y,int width,int height,bool transparent)
{
    GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
    GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;
    
    if (transparent)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
    
    SwitchTexture(img->hTex);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);            glVertex2i(x      ,y       );
    glTexCoord2f(nTexendx,0.0f);        glVertex2i(x+width,y       );
    glTexCoord2f(nTexendx,nTexendy);    glVertex2i(x+width,y+height);
    glTexCoord2f(0.0f,nTexendy);        glVertex2i(x      ,y+height);
    glEnd();
}

void DistortRenderTexture(IMAGE img,int x[4],int y[4],bool transparent)
{
    GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
    GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;
    
    if (transparent)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);

    SwitchTexture(img->hTex);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f);            glVertex2i(x[0],y[0]);
    glTexCoord2f(nTexendx,0.0f);        glVertex2i(x[1],y[1]);
    glTexCoord2f(nTexendx,nTexendy);    glVertex2i(x[2],y[2]);
    glTexCoord2f(0.0f,nTexendy);        glVertex2i(x[3],y[3]);
    glEnd();
}

//----------------------------------- Primatives ----------------------------------

void HardLine(int x1,int y1,int x2,int y2,u32 colour)
{
    const RGBA& c = *(RGBA*)&colour;
    SwitchTexture(0);

    glBegin(GL_LINES);
    glColor4ub(c.b, c.g, c.r, c.a);
    glVertex2i(x1,y1);
    glVertex2i(x2,y2);
    glEnd();

    glColor4ub(255,255,255,255);
}

void HardRect(int x1,int y1,int x2,int y2,u32 colour,bool filled)
{
    const RGBA& col = *(RGBA*)&colour;

    SwitchTexture(0);
    if (filled)
    {
        glBegin(GL_QUADS);
        glColor4ub(col.b,col.g,col.r,col.a);
        glVertex2i(x1,y1);
        glVertex2i(x2,y1);
        glVertex2i(x2,y2);
        glVertex2i(x1,y2);
        glEnd();
        glColor4ub(255,255,255,255);
    }
    else
    {
        glBegin(GL_LINES);
        glColor4ub(col.b,col.g,col.r,col.a);
        glVertex2i(x1,y1);
        glVertex2i(x2,y1);
        
        glVertex2i(x2,y1);
        glVertex2i(x2,y2);
        
        glVertex2i(x2,y2);
        glVertex2i(x1,y2);
        
        glVertex2i(x1,y2);
        glVertex2i(x1,y1);
        glEnd();
        glColor4ub(255,255,255,255);
    }
}

void HardEllipse(int cx,int cy,int rx,int ry,u32 colour,bool filled)
{
	if(rx==0 || ry==0) return;

	const RGBA& col = *(RGBA*)&colour;
	
    SwitchTexture(0);
	glColor4ub(col.b,col.g,col.r,col.a);
	rx=abs(rx);	ry=abs(ry);
	if(filled)
	{
		if(rx>ry)
		{
			float rx2=rx*rx*1.0f, ry2=ry*ry*1.0f;
			float ry2rx2=ry2/rx2*1.0f, rxry=rx/ry*1.0f;
			float sfac=rxry*0.2f*1.0f;
			float i=0.0f,curr=0.0f;
			glBegin(GL_TRIANGLE_STRIP); // left part
			glVertex2i(cx-rx,cy);
			for(i=-rx+1*1.0f; i<=-1; i+=(rx-fabsf(i))*sfac+1)
			{
				curr=sqrtf(ry2-i*i*ry2rx2);
				glVertex2f(cx+i,cy-curr); glVertex2f(cx+i,cy+curr);
			}
			glVertex2i(cx,cy-ry); glVertex2i(cx,cy+ry);
			glEnd();
			glBegin(GL_TRIANGLE_STRIP); // right part
			glVertex2i(cx+rx,cy);
			for(i=rx-1*1.0f; i>=1; i-=(rx-fabsf(i))*sfac+1)
			{
				curr=sqrtf(ry2-i*i*ry2rx2);
				glVertex2f(cx+i,cy-curr); glVertex2f(cx+i,cy+curr);
			}
			glVertex2i(cx,cy-ry); glVertex2i(cx,cy+ry);
			glEnd();
		}
		else // ry>rx
		{
			float rx2=rx*rx*1.0f, ry2=ry*ry*1.0f;
			float rx2ry2=rx2/ry2*1.0f, ryrx=ry/rx*1.0f;
			float sfac=ryrx*0.2f*1.0f;
			float i=0.0f,curr=0.0f;
			glBegin(GL_TRIANGLE_STRIP); // upper part
			glVertex2i(cx,cy-ry);
			for(i=-ry+1*1.0f; i<=-1; i+=(ry-fabsf(i))*sfac+1)
			{
				curr=sqrtf(rx2-i*i*rx2ry2);
				glVertex2f(cx-curr,cy+i); glVertex2f(cx+curr,cy+i);
			}
			glVertex2i(cx-rx,cy); glVertex2i(cx+rx,cy);
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);  //lower part
			glVertex2i(cx,cy+ry);
			for(i=ry-1*1.0f; i>=1; i-=(ry-fabsf(i))*sfac+1)
			{
				curr=sqrtf(rx2-i*i*rx2ry2);
				glVertex2f(cx-curr,cy+i); glVertex2f(cx+curr,cy+i);
			}
			glVertex2i(cx-rx,cy); glVertex2i(cx+rx,cy);
			glEnd();
		}
	}
	else //outlined
	{
		if(rx>ry)
		{
			double rx2=rx*rx*1.0, ry2=ry*ry*1.0;
			double ry2rx2=ry2/rx2*1.0, rxry=rx/(ry*1.0);
			double sfac=rxry*0.2;
			double i=rx*1.0,curr=0.0;
			double lastcurr=curr,lasti=i;
			glBegin(GL_LINES);
			for(; i>=0; i-=(rx-i)*sfac+1)
			{
				curr=sqrt(ry2-i*i*ry2rx2);
				glVertex2d(cx+lasti,cy+lastcurr); glVertex2d(cx+i,cy+curr);
				glVertex2d(cx+lasti,cy-lastcurr); glVertex2d(cx+i,cy-curr);
				glVertex2d(cx-lasti,cy+lastcurr); glVertex2d(cx-i,cy+curr);
				glVertex2d(cx-lasti,cy-lastcurr); glVertex2d(cx-i,cy-curr);
				lastcurr=curr;
				lasti=i;
			}
			glVertex2d(cx+lasti,cy+lastcurr); glVertex2i(cx,cy+ry);
			glVertex2d(cx+lasti,cy-lastcurr); glVertex2i(cx,cy-ry);
			glVertex2d(cx-lasti,cy+lastcurr); glVertex2i(cx,cy+ry);
			glVertex2d(cx-lasti,cy-lastcurr); glVertex2i(cx,cy-ry);
			glEnd();
		}
		else // ry>rx
		{
			double rx2=rx*rx*1.0, ry2=ry*ry*1.0;
			double rx2ry2=rx2/ry2*1.0, ryrx=ry/(rx*1.0);
			double sfac=ryrx*0.2;
			double i=ry*1.0,curr=0.0;
			double lastcurr=curr,lasti=i;
			glBegin(GL_LINES);
			for(; i>=0; i-=(ry-i)*sfac+1)
			{
				curr=sqrt(rx2-i*i*rx2ry2);
				glVertex2d(cx+lastcurr,cy+lasti); glVertex2d(cx+curr,cy+i);
				glVertex2d(cx-lastcurr,cy+lasti); glVertex2d(cx-curr,cy+i);
				glVertex2d(cx+lastcurr,cy-lasti); glVertex2d(cx+curr,cy-i);
				glVertex2d(cx-lastcurr,cy-lasti); glVertex2d(cx-curr,cy-i);
				lastcurr=curr;
				lasti=i;
			}
			glVertex2d(cx+lastcurr,cy+lasti); glVertex2i(cx+rx,cy);
			glVertex2d(cx-lastcurr,cy+lasti); glVertex2i(cx-rx,cy);
			glVertex2d(cx+lastcurr,cy-lasti); glVertex2i(cx+rx,cy);
			glVertex2d(cx-lastcurr,cy-lasti); glVertex2i(cx-rx,cy);
			glEnd();
		}
	}
	glColor4ub(255,255,255,255);
}

void HardPoly(IMAGE img,int x[3],int y[3],u32 colour[3])
{
    SwitchTexture(0);
    glBegin(GL_TRIANGLES);
    for (int i=0; i<3; i++)
    {
        const RGBA& col = *(RGBA*)&colour[i];
        glColor4ub(col.b, col.g, col.r, col.a); //colour[i]&255,(colour[i]>>8)&255,(colour[i]>>16)&255,(colour[i]>>24)&255);
        glVertex2i(x[i],y[i]);
    }
    glEnd();

    glColor4ub(255,255,255,255);
}

void HardPoint(IMAGE img,int x,int y,u32 colour)
{
    SwitchTexture(0);
    glBegin(GL_POINTS);
    glColor4ub(colour&255,(colour>>8)&255,(colour>>16)&255,(colour>>24)&255);
    glVertex2i(x,y);
    glColor4ub(255,255,255,255);
    glEnd();
}