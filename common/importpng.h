// --------------- PNG stuff -----------------

// The name is a bit of a misnomer, the PNG exporting stuff is here also

#ifndef IMPORTPNG_H
#define IMPORTPNG_H

#include "types.h"

class CPixelMatrix;

namespace PNG
{
    bool Load(CPixelMatrix& dest,const char* fname);
    bool Save(CPixelMatrix& src,const char* fname);
};

#endif
