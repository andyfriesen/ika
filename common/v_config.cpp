#include "v_config.h"

SUserConfig::SUserConfig()
{
	nInitxres=320;
	nInityres=240;
	nInitbpp=16;
	bFullscreen=true;
	bSound=true;
	nMaxframeskip=10;
	sGraphplugin[0]=0; sSoundplugin[0]=0;	// just to be sure that any garbage that they contained will be ignored by the C string ops.
	strcpy(sGraphplugin,"SYS\\GFX_DD7.DLL");
	strcpy(sSoundplugin,"SYS\\SFX_MIKMOD.DLL");
}

bool SUserConfig::Read(const char* fname)
{
	File f;

	// First, set some defaults

	if (!f.OpenRead(fname))
		return false;

	// TODO: make this stop when it reaches EOF, instead of an unknown token. :P
	while (1)
	{
		char	c[255];

		if (f.eof())
			break;

		f.ReadToken(c);	// get a string

		if (!stricmp(c,"graphdriver"))
			f.ReadToken(sGraphplugin);
		else if (!stricmp(c,"sounddriver"))
			f.ReadToken(sSoundplugin);

		else if (!stricmp(c,"libdir"))
		{
			f.ReadToken(c);
			File::AddPath(string_k("vh"),string_k(c));
		}

		else if (!stricmp(c,"vidmode"))
		{
			f.ReadToken(c);		nInitxres=atoi(c);
			f.ReadToken(c);		nInityres=atoi(c);
		}

		else if (!stricmp(c,"bitdepth"))
			f.ReadToken(c),	nInitbpp=atoi(c);

		else if (!stricmp(c,"window"))
			bFullscreen=false;

		else if (!stricmp(c,"frameskip"))
			f.ReadToken(c),	nMaxframeskip=atoi(c);
		
		else if (!stricmp(c,"nosound"))
			bSound=false;

		else
			break;
//			log("Unknown user.cfg token \"%s\"",c);
	}

	// TODO: more?

	// make sure options are in range
	if (nMaxframeskip<1) nMaxframeskip=1;
	if (nInitxres<1) nInitxres=1;
	if (nInityres<1) nInityres=1;
	if (nInitbpp!=15 && nInitbpp!=16 && nInitbpp!=24 && nInitbpp!=32)
		nInitbpp=16;

	f.Close();

	return true;
}

void SGameConfig::Read(const char* fname)
{
	File f;

	File::ClearPaths();

	if (strlen(fname))
	{
		if (!f.OpenRead(fname))
			return;
	}
	else
		if (!f.OpenRead("game.cfg"))	// default filename
			return;

	while (1)
	{
		char t[255];

		if (f.eof())
			break;

		f.ReadToken(t);
		if (!stricmp(t,"path"))
		{
			char sExt[255];
			char sPath[255];
			f.ReadToken(sExt);
			f.ReadToken(sPath);
			File::AddPath(sExt,sPath);
		}
		else if (!stricmp(t,"defpath"))
		{
			char sPath[255];
			f.ReadToken(sPath);
			ChangeDirectory(sPath);
		}
		else
			break;
	}
}
