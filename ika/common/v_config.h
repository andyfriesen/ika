/*
	Since most of the apps use the same configuration files, we'll stuff them here.

	user.cfg - per USER config options

		- engine runtime options. (resolution etc..)
		- Location of EXEs, DLLs and standard headers. (with the VH extension)

	game.cfg - per GAME config. (seeing a trend?) This should be in the game directory (if applicable)

		- Location of data files. (could be of any extension, arranged any way)
*/

#ifndef CONFIG_H
#define CONFIG_H

#include "fileio.h"						// Used to actually read files, and set default paths.
#include "misc.h"

struct SUserConfig						// Configuration options for a specific user. (these should have no real effect on the game itself)
{
	int		nInitxres;					// initial x resolution
	int		nInityres;					// not telling
	int		nInitbpp;					// bits per pixel
	bool	bFullscreen;				// false if windowed mode
	bool	bSound;						// sound is inited if true
	char	sGraphplugin[255];			// path to video DLL
	char	sSoundplugin[255];			// path to audio DLL

	int		nMaxframeskip;				// maximum number of frames to skip to keep timing correct.

	SUserConfig();
	bool	Read(const char* fname);
};

struct SGameConfig						// configuration info specific to the game being run
{
	// er... :)
	void	Read(const char* fname);
};

#endif