#include "sound.h"

using namespace audiere;

namespace
{
    RefPtr<AudioDevice> _device;
    bool _enabled;
}

namespace Sound
{
    void Init()
    {
        // On windows, we try winmm first, because it doesn't crash my PC.
        // If it doesn't work, we use the default device.  On nonwindows,
        // we don't bother trying winmm, because it's not there. ;)
#if defined(WIN32) || defined(_WIN32)
        _device = OpenDevice("winmm", "");
        if (!_device)
#endif
            _device = OpenDevice();


        if (!_device)
            throw Exception();
    }

    void Shutdown()
    {
        _device = 0;
    }

    OutputStream* OpenSound(const char* fname)
    {
        if (!_device)
            return 0;

        OutputStream* stream = ::audiere::OpenSound(_device.get(), fname, false);
        if (!stream)
            return 0;

        stream->ref();
        return stream;
    }
}
