#ifndef BLITS_H
#define BLITS_H

extern void ScanImage(handle img);

extern bool NullBlit(handle img,int x,int y);
extern bool OpaqueBlit(handle img,int x,int y);
extern bool SpriteBlit(handle img,int x,int y);
extern bool AlphaBlit(handle img,int x,int y);

extern bool NullScaleBlit(handle img,int x,int y,int w,int h);
extern bool OpaqueScaleBlit(handle img,int x,int y,int w,int h);
extern bool SpriteScaleBlit(handle img,int x,int y,int w,int h);
extern bool AlphaScaleBlit(handle img,int x,int y,int w,int h);

#endif