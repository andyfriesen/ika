#include <stdexcept>

#include "sound.h"
#include "audiere.h"
#include "common/log.h"

using audiere::OpenDevice;
using audiere::OpenSound;

using audiere::AudioDevice;
using audiere::FileFormat;
using audiere::OutputStream;
using audiere::SoundEffect;
using audiere::RefPtr;
using audiere::SampleSource;

namespace Sound {
    RefPtr<AudioDevice> _device;

    enum {
        streamAudio = true,
        dontStreamAudio = false,
    };

    void Init(bool nullAudio) {
        // On windows, we try winmm first, because it doesn't crash my PC.
        // If it doesn't work, we use the default device.  On nonwindows,
        // we don't bother trying winmm, because it's not there. ;)

        if (!nullAudio) {
#if 0 && (defined(WIN32) || defined(_WIN32))
            _device = OpenDevice("winmm", "");
            if (!_device)
#endif
                _device = OpenDevice();
        }

        if (!_device)
            _device = OpenDevice("null", "");

        if (!_device)
            throw std::runtime_error("Unable to open an audio device. (not even the null device!");
    }

    void Shutdown() {
        _device = 0;
    }

    OutputStream* OpenSound(const std::string& fileName) {
        OutputStream* stream = ::audiere::OpenSound(_device.get(), fileName.c_str(), streamAudio);
        if (!stream)
            return 0;

        stream->ref();
        return stream;
    }

    SoundEffect* OpenSoundEffect(const std::string& fileName) {
        SoundEffect* effect = ::audiere::OpenSoundEffect(_device.get(), fileName.c_str(), audiere::MULTIPLE);
        if (!effect)
            return 0;

        effect->ref();
        return effect;
    }
}
