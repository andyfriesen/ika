#ifndef FONT_H
#define FONT_H

#include <vector>
#include "tileset.h"

class File;

class CFont
{
private:
	struct SubSet
	{
		handle glyph[96];
	};

	std::vector<SubSet>	set;

	int nWidth,nHeight;											// width/height of the widest/highest character in the whole font
	int nTabsize;												// tab granularity, in pixels

	void PrintChar(int& x,int y,int cursubset,char c);

public:
	bool LoadFNT(const char* filename);
	void PrintString(int x,int y,const char* s);

	void Init() { Free(); }
	void Free();

	int StringWidth(const char* s) const;

	int Width() const	{	return nWidth;	}
	int Height() const	{	return nHeight;	}
	int TabSize() const	{	return nTabsize;	}
	void SetTabSize(int tabsize)	{	nTabsize=tabsize;	}
};

#endif