#pragma once

#include <map>
#include "common/utility.h"
#include "Canvas.h"

struct File;

/**
 * Hardware independant representation of a sprite.
 *
 * Note that the data structure allows for each frame to have its own dimensions.
 * The file format does too, however we won't be actually implementing this yet.
 *
 * This is TERRIBLE.  TODO: unscrew. :P
 */
struct CCHRfile {
    typedef std::map<std::string, std::string> StringMap;
    StringMap moveScripts;
    StringMap metaData;  ///< Authoring information and the like.
    
    CCHRfile(int width = 16, int height = 16);

    ~CCHRfile() { ClearFrames(); }

    void ClearFrames();

    Canvas& GetFrame(uint frame) const;
    inline const std::vector<Canvas*>& GetAllFrames() { return _frame; }
    void UpdateFrame(const Canvas& newData, uint frame);
    inline int Width(int /*frame*/ = 0) const { return _width; }
    inline int Height(int /*frame*/ = 0) const { return _height; }
    inline uint NumFrames() const { return _frame.size(); }
    
    inline int& HotX(int /*frame*/ = 0) { return _hotspotX; }
    inline int& HotY(int /*frame*/ = 0) { return _hotspotY; }       ///< Hotspot position.
    inline int& HotW(int /*frame*/ = 0) { return _hotspotWidth; }
    inline int& HotH(int /*frame*/ = 0) { return _hotspotHeight; }  ///< Hotspot size.

    void AppendFrame();                         ///< Adds a new, empty frame.
    void AppendFrame(Canvas& c);
    void AppendFrame(Canvas* c);                ///< Append the frame.
    void InsertFrame(uint index);               ///< Inserts a new, empty frame at the specified position.
    void InsertFrame(uint index, Canvas& c);
    void InsertFrame(uint index, Canvas* c);    ///< Inserts the image as a new frame at the specified position.
    void DeleteFrame(uint index);               ///< Removes the specified frame.

    void Resize(int width, int height);         ///< Resize all the frames.

    void New(int frameX, int frameY);           ///< Creates a new sprite of the specified dimensions.
    void Load(const std::string& filename);     ///< Loads sprite data from a file.
    void Save(const std::string& filename);     ///< Writes the sprite data to a file.
    void SaveOld(const std::string& filename);  ///< Writes the sprite data to a file, in VERGE's CHR format.

    std::string GetStandingScript(Direction dir);
    std::string GetWalkingScript(Direction dir);

private:
    std::vector<Canvas*> _frame;       ///< Frame data.
    int _width, _height;
    int _hotspotX, _hotspotY;         ///< Hotspot position.
    int _hotspotWidth, _hotspotHeight; ///< Hotspot width and height.

    void LoadCHR(const std::string& filename);
    void Loadv2CHR(File& f);
    void Loadv4CHR(File& f);
    void Loadv5CHR(File& f);
};

