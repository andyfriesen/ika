
#ifndef SOUND_H
#define SOUND_H

typedef void* SMusic;
typedef void* Ssfx;

extern bool (*sfxInit)();		// inits the whole thing, returns true on success
extern void (*sfxShutdown)();	// shuts everything down

extern SMusic (*sfxLoadMusic)(const char* fname);	// Loads the music into m
extern bool (*sfxFreeMusic)(SMusic m);						// stops playing and deallocates the music file
extern bool (*sfxPlayMusic)(SMusic m);						// plays the music file
extern void (*sfxStopMusic)(SMusic m);						// stops playback. (play can be called again to resume playback)
extern int  (*sfxGetMusicPos)(SMusic m);
extern void (*sfxSetMusicPos)(SMusic m,long newtime);
extern int  (*sfxGetMusicVolume)(SMusic m);
extern void (*sfxSetMusicVolume)(SMusic m,int newvol);		// sets the volume.  0-255


extern Ssfx (*sfxLoadEffect)(const char* fname);		// loads the .wav file into s
extern bool (*sfxFreeEffect)(Ssfx s);

extern bool (*sfxPlayEffect)(Ssfx s,int vol,int pan);		// vol is 0-255, pan is 0 (left) - 255 (right)

extern void (*sfxUpdate)();

extern bool SetupSound(const char* dllname);
extern void ShutdownSound();

// Hackage!! Get rid of these ASAP.
class CMusic
{
public:
    SMusic	m;
    void Init() { m=NULL; }
    void Free() { if (m) sfxFreeMusic(m); m=NULL; }
    
    inline operator SMusic() const { return m; }
};

class Csfx
{
public:
    Ssfx	s;
    void Init()	{ s=NULL; }
    void Free() { if (s) sfxFreeEffect(s); s=NULL; }
    
    operator Ssfx() const { return s; }
};

#endif