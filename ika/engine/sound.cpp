#include <windows.h>
#include "sound.h"

bool (*sfxInit)();		// inits the whole thing, returns true on success
void (*sfxShutdown)();	// shuts everything down

bool (*sfxLoadMusic)(SMusic* m,const char* fname);	// Loads the music into m
bool (*sfxFreeMusic)(SMusic m);						// stops playing and deallocates the music file
bool (*sfxPlayMusic)(SMusic m);						// plays the music file
void (*sfxStopMusic)(SMusic m);						// stops playback. (play can be called again to resume playback)
int  (*sfxGetMusicPos)(SMusic m);
void (*sfxSetMusicPos)(SMusic m,long newpos);
int  (*sfxGetMusicVolume)(SMusic m);
void (*sfxSetMusicVolume)(SMusic m,int newvol);		// sets the volume.  0-255

bool (*sfxLoadEffect)(Ssfx* s,const char* fname);		// loads the .wav file into s
bool (*sfxFreeEffect)(Ssfx s);

bool (*sfxPlayEffect)(Ssfx* s,int vol,int pan);		// vol is 0-255, pan is 0 (left) - 255 (right)

void (*sfxUpdate)();

static void SetupNullSoundDriver();

template<typename T>
static void Assign(T& dest,void* src)
{
	if (!src) throw 0;					// This saves so much typing.
	dest=(T&)src;
}

HINSTANCE hSoundlib=0;

bool SetupSound(const char* dllname)
{
	if (!strlen(dllname))
	{
		SetupNullSoundDriver();
		return true;
	}

	hSoundlib=LoadLibrary(dllname);
	if (!hSoundlib)
	{
		SetupNullSoundDriver();
		return false;
	}

	try
	{
		Assign(sfxInit,GetProcAddress(hSoundlib,"sfxInit"));
		Assign(sfxShutdown,GetProcAddress(hSoundlib,"sfxShutdown"));

		Assign(sfxLoadMusic,GetProcAddress(hSoundlib,"sfxLoadMusic"));
		Assign(sfxFreeMusic,GetProcAddress(hSoundlib,"sfxFreeMusic"));
		Assign(sfxPlayMusic,GetProcAddress(hSoundlib,"sfxPlayMusic"));
		Assign(sfxStopMusic,GetProcAddress(hSoundlib,"sfxStopMusic"));
		Assign(sfxGetMusicPos,GetProcAddress(hSoundlib,"sfxGetMusicPos"));
		Assign(sfxSetMusicPos,GetProcAddress(hSoundlib,"sfxSetMusicPos"));
		Assign(sfxGetMusicVolume,GetProcAddress(hSoundlib,"sfxGetMusicVolume"));
		Assign(sfxSetMusicVolume,GetProcAddress(hSoundlib,"sfxSetMusicVolume"));

		Assign(sfxLoadEffect,GetProcAddress(hSoundlib,"sfxLoadEffect"));
		Assign(sfxFreeEffect,GetProcAddress(hSoundlib,"sfxFreeEffect"));
		Assign(sfxPlayEffect,GetProcAddress(hSoundlib,"sfxPlayEffect"));

		Assign(sfxUpdate,GetProcAddress(hSoundlib,"sfxUpdate"));

		if (!sfxInit())
			throw 0;
	}
	catch (int)
	{
		FreeLibrary(hSoundlib);
		hSoundlib=0;

		SetupNullSoundDriver();

		return false;
	}

	return true;
}

void ShutdownSound()
{
	if (hSoundlib)
	{
		sfxShutdown();
		FreeLibrary(hSoundlib);
	}
	hSoundlib=0;
}

// This is the not-a-sound-driver driver.  It does nothing, but pretends that it works
// Used if the sound driver cannot be loaded.

static bool nullsfxInit(){return true;}
static void nullsfxShutdown(){}

static bool nullsfxLoadMusic(SMusic* m,const char* fname){return true;}
static bool nullsfxFreeMusic(SMusic m){return true;}
static bool nullsfxPlayMusic(SMusic m){return true;}
static void nullsfxStopMusic(SMusic m){}
static int  nullsfxGetMusicPos(SMusic m){return 0;}
static void nullsfxSetMusicPos(SMusic m,long newpos){}
static int  nullsfxGetMusicVolume(SMusic m){return 0;}
static void nullsfxSetMusicVolume(SMusic m,int newvol){}

static bool nullsfxLoadEffect(Ssfx* s,const char* fname){return true;}
static bool nullsfxFreeEffect(Ssfx s){return true;}

static bool nullsfxPlayEffect(Ssfx* s,int vol,int pan){return true;}

static void nullsfxUpdate(){}

static void SetupNullSoundDriver()
{
	Assign(sfxInit,&nullsfxInit);
	Assign(sfxShutdown,&nullsfxShutdown);

	Assign(sfxLoadMusic,&nullsfxLoadMusic);
	Assign(sfxFreeMusic,&nullsfxFreeMusic);
	Assign(sfxPlayMusic,&nullsfxPlayMusic);
	Assign(sfxStopMusic,&nullsfxStopMusic);
	Assign(sfxGetMusicPos,&nullsfxGetMusicPos);
	Assign(sfxSetMusicPos,&nullsfxSetMusicPos);
	Assign(sfxGetMusicVolume,&nullsfxGetMusicVolume);
	Assign(sfxSetMusicVolume,&nullsfxSetMusicVolume);

	Assign(sfxLoadEffect,&nullsfxLoadEffect);
	Assign(sfxFreeEffect,&nullsfxFreeEffect);
	Assign(sfxPlayEffect,&nullsfxPlayEffect);

	Assign(sfxUpdate,&nullsfxUpdate);
}