/*
The sound api.


bool sfxInit()		// inits the whole thing, returns true on success
void sfxShutdown();	// shuts everything down

SMusic* sfxLoadMusic(const char* fname);	// Loads the music into m
bool sfxFreeMusic(SMusic* m);						// stops playing and deallocates the music file
bool sfxPlayMusic(SMusic* m);						// plays the music file
void sfxStopMusic(SMusic* m);						// stops playback. (play can be called again to resume playback
int  sfxGetMusicPos(SMusic* m);
void sfxSetMusicPos(SMusic* m);
int  sfxGetMusicVolume(SMusic* m);
void sfxSetMusicVolume(SMusic* m,int newvol);		// sets the volume.  0-255


Ssfx* sfxLoadEffect(const char* fname);		// loads the .wav file and returns it
bool sfxFreeEffect(Ssfx* s);

bool sfxPlayEffect(Ssfx* s,int vol,int pan);		// vol is 0-255, pan is 0 (left) - 255 (right)

void sfxUpdate();									// this'll get called lots

*/

#include "mikmod\mikmod.h"
#include "mikmod\mplayer.h"
#include "mikmod\mdsfx.h"
#include "mikmod\virtch.h"

#include "log.h"

// Structs.  Blech

struct SMusic
{
    UNIMOD*			pMod;
    MPLAYER*		pPlayer;
    
    int				nVolume;
};

struct Ssfx
{
    MD_SAMPLE*		pSample;
};

// Global!! fun!!

MDRIVER*		pDriver;
MD_VOICESET*	pGlobalvoice;
MD_VOICESET*	pMusicvoice;
MD_VOICESET*	pSfxvoice;

#define EXPORT __cdecl

static int	nSounds=0;
static int	nMusic=0;

bool EXPORT sfxInit()
{
#ifdef _DEBUG
    initlog("sfx_mikmod.log");
#endif
    log("Initing sound...");
    Mikmod_RegisterLoader(load_mod);
    Mikmod_RegisterLoader(load_s3m);
    Mikmod_RegisterLoader(load_xm);
    Mikmod_RegisterLoader(load_it);
    Mikmod_RegisterAllDrivers();
    
    pDriver=Mikmod_Init(44100,100,NULL,MD_STEREO, CPU_AUTODETECT, DMODE_16BITS | DMODE_INTERP | DMODE_SAMPLE_DYNAMIC | DMODE_NOCLICK);
    if (!pDriver)
    {
        log("Mikmod_init failed");
        return false;
    }
    
    VC_RegisterStereoMixers();
    
    pGlobalvoice=Voiceset_Create(pDriver,NULL,0,0);
    pMusicvoice =Voiceset_Create(pDriver,pGlobalvoice,0,0);
    pSfxvoice   =Voiceset_Create(pDriver,pGlobalvoice,8,0);
    
    if (!pGlobalvoice || !pMusicvoice || !pSfxvoice)
    {
        log("voice creation failed");
        //		sfxShutdown();
        return false;
    }
    
    Voiceset_SetVolume(pGlobalvoice,128);
    Voiceset_SetVolume(pMusicvoice,128);
    Voiceset_SetVolume(pSfxvoice,128);
    return true;
}

// Assumes that all sound effects and mods have been freed, so watch it. ;)
void EXPORT sfxShutdown()
{
    log("Shutting down sound system...");
    
    if (nSounds!=0)
        log("Shutdown: Sound count at %i!!  BAD!!",nSounds);
    if (nMusic=0)
        log("Shutdown: Music count at %i!!  Bad!!",nMusic);
    
    Mikmod_Exit(pDriver);
}

SMusic* EXPORT sfxLoadMusic(const char* fname)
{
    SMusic* m=new SMusic;
    
    m->pMod=Unimod_Load(pDriver,fname);
    if (!m->pMod)
    {
        delete m;
        return 0;
    }
    
    m->pPlayer=Player_InitSong(m->pMod,pMusicvoice,PF_LOOP,24);
    if (!m->pMod)
    {
        Unimod_Free(m->pMod);
        delete m;
        return 0;
    }
    
    nMusic++;
    return m;
}

bool EXPORT sfxFreeMusic(SMusic* m,const char* fname)
{
    Player_FreeSong(m->pPlayer);
    Unimod_Free(m->pMod);
    delete m;
    
    nMusic--;
    
    return true;
}

bool EXPORT sfxPlayMusic(SMusic* m)
{
    if (Player_Paused(m->pPlayer))
    {
        Player_TogglePause(m->pPlayer,true);
        return true;
    }
    
    Player_Start(m->pPlayer);
    
    return true;
}

void EXPORT sfxStopMusic(SMusic* m)
{
    if (Player_Paused(m->pPlayer))
        return;
    
    Player_Pause(m->pPlayer,false);
}

int  EXPORT sfxGetMusicPos(SMusic* m)
{
    return m->pPlayer->state.curtime;
}

void EXPORT sfxSetMusicPos(SMusic* m,long time)
{
    if (!m)
        log("SetMusicVolume: null passed");
    Player_SetPosTime(m->pPlayer,time);
}

int  EXPORT sfxGetMusicVolume(SMusic* m)
{
    if (!m)
        log("SetMusicVolume: null passed");
    return m->pPlayer->state.volume;
}

void EXPORT sfxSetMusicVolume(SMusic* m,int newvol)
{
    if (!m)
        log("SetMusicVolume: null passed");
    Player_SetVolume(m->pPlayer,newvol);
}

Ssfx* EXPORT sfxLoadEffect(const char* fname)
{
    Ssfx* s=new Ssfx;
    
    s->pSample=mdsfx_loadwav(pDriver,fname);
    
    if (s->pSample)
    {
        nSounds++;
        return s;
    }
    else
        return 0;
}

bool EXPORT sfxFreeEffect(Ssfx* s)
{
    mdsfx_free(s->pSample);
    nSounds--;
    
    delete s;
    return true;
}

bool EXPORT sfxPlayEffect(Ssfx* s,int vol,int pan)
{
    int voice=mdsfx_playeffect(s->pSample,pSfxvoice,0,0);
    Voice_SetVolume(pSfxvoice,voice,vol);
    Voice_SetPanning(pSfxvoice,voice,pan,0);
    return false;
}

void EXPORT sfxUpdate()
{
    Mikmod_Update(pDriver);
}