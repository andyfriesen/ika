#include "sound.h"

using namespace audiere;

namespace
{
    RefPtr<AudioDevice> _device;
}

namespace Sound
{
    void Init(bool nullAudio)
    {
        // On windows, we try winmm first, because it doesn't crash my PC.
        // If it doesn't work, we use the default device.  On nonwindows,
        // we don't bother trying winmm, because it's not there. ;)

        if (!nullAudio)
        {
#if 0 //defined(WIN32) || defined(_WIN32)
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

    void Shutdown()
    {
        _device = 0;
    }

    OutputStream* OpenSound(const char* fname)
    {
        OutputStream* stream = ::audiere::OpenSound(_device.get(), fname, false);
        if (!stream)
            return 0;

        stream->ref();
        return stream;
    }
}
