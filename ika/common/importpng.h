// --------------- PNG stuff -----------------

#ifndef IMPORTPNG_H
#define IMPORTPNG_H

#include "types.h"

/*#pragma warning(disable : 4103)
#pragma pack(push, 1)
typedef struct {
  byte red;
  byte green;
  byte blue;
  byte alpha;
} RGBA;
#pragma pack(pop)

#pragma warning(disable : 4103)
#pragma pack(push, 1)
typedef struct {
  byte red;
  byte green;
  byte blue;
} RGB;
#pragma pack(pop)*/

struct png_image {
  int width;
  int height;
  RGBA *pixels;
/*
  png_image() : pixels(0) {}
  ~png_image()
  {
	  delete[] pixels;
  }*/
};

png_image* Import_PNG(const char *filename);
bool Export_PNG(png_image* src,const char* filename);

#endif
