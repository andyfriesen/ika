
#ifndef CHR_H
#define CHR_H

#include <map>
#include "types.h"
#include "Canvas.h"

/**
 *  Hardware independant representation of a sprite.
 *
 *  Note that the data structure allows for each frame to have its own dimensions.
 *  The file format does too, however we won't be actually implementing this yet.
 */
class CCHRfile
{
    std::vector<Canvas>    frame;                    ///< frame data

    int        nWidth, nHeight;
    int        nHotx, nHoty;                    ///< hotspot position
    int        nHotw, nHoth;                    ///< hotspot width / height

    void PackData(u8* data, int& size);
    void UnpackData(u8* data, int size);

public:
    std::vector<std::string>  moveScripts;          ///< movement scripts
    std::map<std::string, std::string> metaData;    ///< Authoring information and the like.
    
    CCHRfile(int width = 16, int height = 16);

    Canvas& GetFrame(uint frame) const;
    void UpdateFrame(const Canvas& newdata, uint nFrame);
    inline int Width(int nFrame = 0)  const   {   return nWidth;  }
    inline int Height(int nFrame = 0) const   {   return nHeight; }
    inline uint NumFrames()           const   {   return frame.size();            }
    
    inline int& HotX(int frame = 0)           {   return nHotx;   }       
    inline int& HotY(int frame = 0)           {   return nHoty;   }     ///< Hotspot position
    inline int& HotW(int frame = 0)           {   return nHotw;   }
    inline int& HotH(int frame = 0)           {   return nHoth;   }     ///< Hotspot size

    void AppendFrame();                                                 ///< Adds a new, empty frame.
    void InsertFrame(uint i);                                           ///< Inserts a new, empty frame at the specified position.
    void InsertFrame(uint i, Canvas& p);                                ///< Inserts the image as a new frame at the specified position.
    void DeleteFrame(uint i);                                           ///< Removes the specified frame.

    void Resize(int width, int height);                                 ///< Resize all the frames.

    void New(int framex, int framey);                                   ///< Creates a new sprite of the specified dimensions.
    void Load(const std::string& fname);                                ///< Loads sprite data from a file
    void Save(const std::string& fname);                                ///< Writes the sprite data to a file.
    void SaveOld(const std::string& fname);                             ///< Writes the sprite data to a file, in VERGE's CHR format.

private:
    void LoadCHR(const std::string& filename);
    void Loadv2CHR(class File& f);
    void Loadv4CHR(class File& f);
    void Loadv5CHR(class File& f);
};

#endif
