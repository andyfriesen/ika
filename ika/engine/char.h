/*
	NOT a byte!!!!!!!!!!!!

	This is the in-engine representation of a CHR file.  Nothing more, nothing less.
*/

#ifndef CHAR_H
#define CHAR_H

#include "controller.h"
#include "graphlib.h"
#include "fileio.h"
#include "strk.h"

const int maxchars			=255;
const int maxscriptlength	=255;

class CCharacter : public CSprite
{
public:
	std::vector<string_k>	sScript;		// move scripts

	short int	hotx,hoty;		// hotspot position
	short int	hotw,hoth;		// hotspot height

	void Init() {}

	bool LoadCHR(const char* fname);
	string_k	sFilename;

};

class CCharacterController : public CController<CCharacter>
{
public:
	int		Load(const char* filename,int x,int y,int numframes);	// loads a PNG file
	int		Load(const char* fname);					// loads a CHR file
};

#endif