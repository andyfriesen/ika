#ifndef HARDBLITS_H
#define HARDBLITS_H

void SynchTexture(IMAGE img);
void RenderTexture(IMAGE img,int x,int y,bool transparent);
void ScaleRenderTexture(IMAGE img,int x,int y,int width,int height,bool transparent);
void DistortRenderTexture(IMAGE img,int x[4],int y[4],bool transparent);
void HardLine(int x1,int y1,int x2,int y2,u32 colour);
void HardRect(int x1,int y1,int x2,int y2,u32 colour,bool filled);
void HardEllipse(int cx,int cy,int rx,int ry,u32 colour,bool filled);
void HardPoly(IMAGE img,int x[3],int y[3],u32 colour[3]);
void HardPoint(IMAGE img,int x,int y,u32 colour);

#endif