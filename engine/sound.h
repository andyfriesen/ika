
#ifndef SOUND_H
#define SOUND_H

#include <string>

namespace audiere {
    class OutputStream; 
    class SoundEffect;
}

namespace Sound {
    struct Exception{};

    void Init(bool nullAudio = false);
    void Shutdown();

    audiere::OutputStream* OpenSound(const std::string& fname);
    audiere::SoundEffect* OpenSoundEffect(const std::string& fname);
}

#endif
