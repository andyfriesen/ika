
#ifndef SOUND_H
#define SOUND_H

/*!
    Sound interface.

    TODO: Get rid of all this gay global business.
*/

typedef void* SMusic;
typedef void* Ssfx;

extern bool (*sfxInit)();                                   //!< inits the audio system, returns true on success
extern void (*sfxShutdown)();                               //!< shuts everything down

extern SMusic (*sfxLoadMusic)(const char* fname);           //!< Loads the music file and returns it
extern bool (*sfxFreeMusic)(SMusic m);                      //!< stops playing and deallocates the music file
extern bool (*sfxPlayMusic)(SMusic m);                      //!< plays the music file
extern void (*sfxStopMusic)(SMusic m);                      //!< stops playback. (play can be called again to resume playback)
extern int  (*sfxGetMusicPos)(SMusic m);                    //!< Returns the current playback time
extern void (*sfxSetMusicPos)(SMusic m,long newtime);       //!< Seeks to the specified position.
extern int  (*sfxGetMusicVolume)(SMusic m);                 //!< Returns the volume the music is playing at.
extern void (*sfxSetMusicVolume)(SMusic m,int newvol);      //!< sets the volume.  0-255


extern Ssfx (*sfxLoadEffect)(const char* fname);            //!< loads a .wav file and returns it
extern bool (*sfxFreeEffect)(Ssfx s);                       //!< Deallocates a sound effect.

extern bool (*sfxPlayEffect)(Ssfx s,int vol,int pan);       //!< vol is 0-255, pan is 0 (left) - 255 (right)

extern void (*sfxUpdate)();                                 //!< Call this a lot or things will skip

extern bool SetupSound(const char* dllname);                //!< Imports the DLL and assigns function pointers.
extern void ShutdownSound();                                //!< Releases the DLL

#endif