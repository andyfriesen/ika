#include "fileio.h"
#include "zlib.h"
#include <stdio.h>

/* 
fileio.cpp
my own little custom class for file I / O der...
it's pretty simple, really
TODO: Pack files
*/

std::vector<File::SDirectoryInfo> File::directoryinfo;

std::string File::GetRealPath(const std::string& fname) {
    if (fname.substr(0, 2)==".\\" || fname.substr(0, 2)=="./") {
        // absolute path specified
        return fname;
    }

    std::string sExtension = fname;
    std::string sResult = fname;

    unsigned int i = sExtension.length()-1;
    do {
        if (sExtension[i]=='.') {
            sExtension.erase(0, i + 1);
            break;
        }
    } while (i-->0);


    if (!i) {                                                                           // no extension specified?
        sExtension='*';                                                                 // okay, then use the default path (if there is one)
    }

    int nSavepositionofdefaultdirectory=-1;                             // :D

    for (i = 0; i < directoryinfo.size(); i++) {                        // now, we know the extension of the file, so we search the directory list for a match
        if (directoryinfo[i].sExtension=="*") {
            nSavepositionofdefaultdirectory = i;
            continue;
        }
        if (sExtension==directoryinfo[i].sExtension) {
            nSavepositionofdefaultdirectory=-1;
            sResult = directoryinfo[i].sPath + fname;
            break;
        }
    }

    if (nSavepositionofdefaultdirectory!=-1) {                  // couldn't find an exact match, but we found a default
        sResult = directoryinfo[nSavepositionofdefaultdirectory].sPath + fname;
    }

    return sResult;
}

void File::AddPath(std::string sExtension, std::string sPath) {
    SDirectoryInfo di;

    if (sPath[sPath.length()-1]!='\\') {
        sPath+="\\";
    }

    for (unsigned int i = 0; i < directoryinfo.size(); i++) {
        if (directoryinfo[i].sExtension==sExtension) {
            directoryinfo[i].sPath = sPath;
            return;
        }
    }

    di.sExtension = sExtension;
    di.sPath = sPath;
    directoryinfo.push_back(di);
}

void File::ClearPaths() {
    directoryinfo.clear();
}

bool File::Exists(const std::string& fname) {
    File f;
    if (!f.OpenRead(fname.c_str())) {
        return false;
    }

    f.Close();
    return true;
}


File::File() {
    mode = closed;
}

File::~File() {
    if (mode!=closed) Close();
}

bool File::OpenRead(const char *fname, bool binary) {
    Close();

    if (fname == 0 || fname[0] == 0) {
        return false;
    }

    std::string fileName = GetRealPath(fname);

    f = fopen(fileName.c_str(),
        binary ? "rb" : "r"
    );

    if (f != 0) {
        mode = open_read;
        return true;
    } else {
        mode = closed;
        return false;
    }
}

bool File::OpenAppend(const char *fname, bool bBinary) {
    Close();

    if (!strlen(fname))
        return false;

    std::string sFilename = GetRealPath(fname);

    if (bBinary)
        f = fopen(sFilename.c_str(), "ab");
    else
        f = fopen(sFilename.c_str(), "a");
    if (f) {
        mode = open_write;
        return true;
    }
    mode = closed;
    return false;
}

bool File::OpenWrite(const char *fname, bool bBinary) {
    Close();

    if (bBinary)
        f = fopen(fname, "wb");
    else
        f = fopen(fname, "w");
    if (f) {
        mode = open_write;
        return true;
    }
    mode = closed;
    return false;
}

void File::Close() {
    if (mode==closed) return;
    fclose(f);
    mode = closed;
}

void File::Read(void* dest, int numbytes) {
    if (numbytes==0)            return;
    if (dest==NULL)                     return;
    if (mode!=open_read)        return;

    fread(dest, 1, numbytes, f);
}

void File::ReadString(char* dest) {
    int nLen;

    Read(nLen);
    Read(dest, nLen);
    dest[nLen]=0;
}

void File::ReadToken(std::string& dest) {
    if (mode != open_read)      return;

    char buffer[256];

    fscanf(f, "%255s", buffer);
    dest = buffer;
}

void File::ReadCompressed(void* dest, int numbytes) {
    z_stream stream;
    int nCompressedblocksize;

    Read(&nCompressedblocksize, 4);

    char* cb = new char[nCompressedblocksize];

    Read(cb, nCompressedblocksize);

    stream.next_in=(Bytef*)cb;
    stream.avail_in = nCompressedblocksize;
    stream.next_out=(Bytef*)dest;
    stream.avail_out = numbytes;
    stream.data_type = Z_BINARY;

    stream.zalloc = NULL;
    stream.zfree = NULL;

    inflateInit(&stream);
    inflate(&stream, Z_SYNC_FLUSH);
    inflateEnd(&stream);

    delete[] cb;
}

std::string File::ReadAll() {
    int size = Size();
    char* c = new char[size + 1];
    memset(c, 0, size + 1);
    //c[size] = 0;
    Seek(0);
    // Need the number of bytes actually read, since it may be different than 
    // the length of the file due to newline conversion
    int len = fread(c, 1, size, f);

    std::string s(c, len);
    delete[] c;
    return s;
}

void File::Write(const void* source, int numbytes) {
    if (numbytes==0)            return;
    if (source==NULL)           return;
    if (mode!=open_write)       return;

    fwrite(source, 1, numbytes, f);
    fflush(f);
}

void File::Write(const char* source) {
    if (mode!=open_write)       return;
    if (!source)                return;
    fprintf(f, "%s", source);
}

void File::WriteString(const char* source) {
    int nLen = strlen(source);

    Write(nLen);
    Write(source, nLen);
}

void File::WriteCompressed(const void* source, int numbytes) {
    z_stream stream;
    int nDatasize = (numbytes * 11) / 10 + 12;              // +10% and 12 bytes

    char* cb = new char[nDatasize];

    stream.next_in=(Bytef*)source;
    stream.avail_in = numbytes;
    stream.next_out=(Bytef*)cb;
    stream.avail_out = nDatasize;
    stream.data_type = Z_BINARY;

    stream.zalloc = NULL;
    stream.zfree = NULL;

    deflateInit(&stream, Z_DEFAULT_COMPRESSION);
    deflate(&stream, Z_SYNC_FLUSH);
    deflateEnd(&stream);

    Write(&stream.total_out, 4);
    Write(cb, stream.total_out);

    delete[] cb;
}

void File::Seek(int position) {
    if (mode==closed)           return;

    fseek(f, position, SEEK_SET);
}

int File::Size() {
    if (mode == closed) {
        return 0;
    } else {

        int i = ftell(f);
        fseek(f, 0, SEEK_END);

        int j = ftell(f);
        fseek(f, i, SEEK_SET);

        return j;
    }
}

int File::Pos() {
    if (mode) {
        return ftell(f);
    } else {
        return 0;
    }
}

bool File::eof() {
    if (mode == closed) {
        return true;
    } else {
        return feof(f) ? true : false;
    }
}
