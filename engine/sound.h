
#ifndef SOUND_H
#define SOUND_H

//#include "audiere.h"
namespace audiere { class OutputStream; }

namespace Sound
{
    struct Exception{};

    void Init(bool nullAudio = false);
    void Shutdown();

    audiere::OutputStream* OpenSound(const char* fname);
}

#endif
