 /*
    .CHR files

    Note that the data structure allows for each frame to have its own dimensions.
    The file format does too, however we won't be actually implementing this yet.
*/

#ifndef CHR_H
#define CHR_H

#include "types.h"
#include "pixel_matrix.h"

class CCHRfile
{
    vector<CPixelMatrix>    frame;             // frame data

    int        nWidth,nHeight;
    int        nHotx,nHoty;                         // hotspot position
    int        nHotw,nHoth;                         // hotspot width/height

public:
    vector<string>    sMovescript;           // movement scripts
    string    sDescription;                       // authoring information?  I dunno, but it's here.
    
    CCHRfile();

    CPixelMatrix& GetFrame(int frame) const;
    void UpdateFrame(const CPixelMatrix& newdata,int nFrame);
    inline int Width(int nFrame=0)  const   {   return frame[nFrame].Width();   }
    inline int Height(int nFrame=0) const   {   return frame[nFrame].Height();  }
    inline int NumFrames()          const   {   return frame.size();            }
    
    inline int& HotX(int nFrame=0)          {   return nHotx;   }       // These are here so that each frame can have its own hotspot
    inline int& HotY(int nFrame=0)          {   return nHoty;   }       // later on.  That's why there's an optional frame
    inline int& HotW(int nFrame=0)          {   return nHotw;   }       // argument.
    inline int& HotH(int nFrame=0)          {   return nHoth;   }

    void AppendFrame();
    void InsertFrame(int i);
    void InsertFrame(int i,CPixelMatrix& p);
    void DeleteFrame(int i);

    void PackData(u8* data,int& size);
    void UnpackData(u8* data,int size);

    void New(int framex,int framey);
    bool Load(const char* fname);
    void Save(const char* fname);
    void SaveOld(const char* fname);
private:
    bool Loadv2CHR(class File& f);
    bool Loadv4CHR(class File& f);
};

#endif