#ifndef FILEIO_H
#define FILEIO_H

#include "types.h"
#include <stdio.h>

class File
{
    enum    FileMode
    {
        closed,
        open_read,
        open_write
    };

    struct SDirectoryInfo
    {
        string sExtension;
        string sPath;
    };

    // Directory structure handling things
private:
    static vector<SDirectoryInfo> directoryinfo;
    string GetRealPath(const string& fname);
public:
    static void AddPath(string sExtension, string sPath);
    static void ClearPaths();

    // Misc handy file stuff
    static bool Exists(const std::string& fname);

    // Actual file junk
private:
    FILE *f;
    FileMode mode;
public:
    File(void);
    ~File(void);
    bool OpenRead(const char* fname, bool bBinary = true);
    bool OpenAppend(const char* fname, bool bBinary = true);
    bool OpenWrite(const char* fname, bool bBinary = true);
    void Close(void);
    
    void Read(void* dest, int numbytes);
    template <typename T>
        void Read(T& dest)
    {   Read(&dest, sizeof dest);    }
    void ReadString(char* dest);
    void ReadToken(char* dest);
    void ReadCompressed(void* dest, int numbytes);
    
    void Write(const void* source, int numbytes);
    void Write(const char* source);
    template <typename T>
        void Write(const T& dest)
    {   Write(&dest, sizeof dest);   }
    void WriteString(const char* source);
    void WriteCompressed(const void* source, int numbytes);
    
    void Seek(int position);
    int  Size(void);
    int  Pos(void);
    bool eof();
};

#endif
