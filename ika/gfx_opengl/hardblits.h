
void RenderTexture(handle img,int x,int y,bool transparent)
{
	// texX/1 = width/img->width
	GLfloat nTexendx=1.0f*img->nWidth/img->nTexwidth;
	GLfloat nTexendy=1.0f*img->nHeight/img->nTexheight;

	if (transparent)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glDisable(GL_COLOR_MATERIAL);			// no tinting

	glBindTexture(GL_TEXTURE_2D,img->hTex);
	glBegin(GL_QUADS);
	glColor4ub(255,255,255,255);
	glTexCoord2f(0.0f,0.0f);			glVertex2i(x,y);
	glTexCoord2f(nTexendx,0.0f);		glVertex2i(x+img->nWidth,y);
	glTexCoord2f(nTexendx,nTexendy);	glVertex2i(x+img->nWidth,y+img->nHeight);
	glTexCoord2f(0.0f,nTexendy);		glVertex2i(x,y+img->nHeight);
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
	glTexCoord2f(0.0f,0.0f);			glVertex2i(x,y);
	glTexCoord2f(nTexendx,0.0f);		glVertex2i(x+width,y);
	glTexCoord2f(nTexendx,nTexendy);	glVertex2i(x+width,y+height);
	glTexCoord2f(0,nTexendy);			glVertex2i(x,y+height);
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
	glTexCoord2f(0.0f,0.0f);			glVertex2i(cx,cy);
	glTexCoord2f(nTexendx,0.0f);		glVertex2i(cx+img->nWidth*scale,cy);
	glTexCoord2f(nTexendx,nTexendy);	glVertex2i(cx+img->nWidth*scale,cy+img->nHeight*scale);
	glTexCoord2f(0,nTexendy);			glVertex2i(cx,cy+img->nHeight*scale);
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
	glBindTexture(GL_TEXTURE_2D,NULL);
	if (filled)
	{
		glBegin(GL_QUADS);
		glColor4ub((colour>>16)&255,(colour>>8)&255,colour&255,(colour>>24)&255);
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
		glColor4ub((colour>>16)&255,(colour>>8)&255,colour&255,(colour>>24)&255);
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