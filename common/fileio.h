#pragma once

#include "common/utility.h"
#include <stdio.h>

struct File {
    enum FileMode {
        closed,
        open_read,
        open_write
    };

    struct SDirectoryInfo {
        std::string sExtension;
        std::string sPath;
    };

    // Directory structure handling things
private:
    static std::vector<SDirectoryInfo> directoryinfo;
    std::string GetRealPath(const std::string& fname);
public:
    static void AddPath(std::string sExtension, std::string sPath);
    static void ClearPaths();

    // Misc handy file stuff
    static bool Exists(const std::string& fname);

    // Actual file junk
private:
    FILE *f;
    FileMode mode;
public:
    File();
    ~File();
    bool OpenRead(const char* fname, bool bBinary = true);
    bool OpenAppend(const char* fname, bool bBinary = true);
    bool OpenWrite(const char* fname, bool bBinary = true);
    void Close();
    
    void Read(void* dest, int numbytes);
    template <typename T>
        void Read(T& dest)
    {   Read(&dest, sizeof dest);    }
    void ReadString(char* dest);
	void ReadToken(std::string& dest);
    void ReadCompressed(void* dest, int numbytes);
    std::string ReadAll();
    
    void Write(const void* source, int numbytes);
    void Write(const char* source);
    template <typename T>
        void Write(const T& dest)
    {   Write(&dest, sizeof dest);   }
    void WriteString(const char* source);
    void WriteCompressed(const void* source, int numbytes);
    
    void Seek(int position);
    int  Size();
    int  Pos();
    bool eof();
};

