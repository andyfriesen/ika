#ifndef FILEIO_H
#define FILEIO_H
#include <stdio.h>

#include <vector>
#include "strk.h"


class File
{
	enum	filemode
	{
		closed,
		open_read,
		open_write
	};
	
	struct SDirectoryInfo
	{
		string_k sExtension;
		string_k sPath;
	};

	// Directory structure handling things
private:
	static std::vector<SDirectoryInfo> directoryinfo;
	string_k GetRealPath(string_k fname);
public:
	static void AddPath(string_k sExtension,string_k sPath);
	static void ClearPaths();

	// Misc handy file stuff
	static bool Exists(const char* fname);

	// Actual file junk
private:
	FILE *f;
	filemode mode;
public:
	File(void);
	~File(void);
	bool OpenRead(const char* fname,bool bBinary=true);
	bool OpenAppend(const char* fname,bool bBinary=true);
	bool OpenWrite(const char* fname,bool bBinary=true);
	void Close(void);
	
	void Read(void* dest,int numbytes);
	template <typename T>
		void Read(T& dest)
		{	Read(&dest,sizeof dest);	}
	void ReadString(char* dest);
	void ReadToken(char* dest);
	void ReadCompressed(void* dest,int numbytes);

	void Write(const void* source,int numbytes);
//	void Write(const char* source);
	template <typename T>
		void Write(const T& dest)
		{	Write(&dest,sizeof dest);	}
	void WriteString(const char* source);
	void WriteCompressed(const void* source,int numbytes);
	
	void Seek(int position);
	int  Size(void);
	int  Pos(void);
	bool eof();
};
#endif
