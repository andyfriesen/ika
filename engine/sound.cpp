#include "sound.h"

using namespace audiere;

namespace
{
    RefPtr<AudioDevice> _device;
    bool _enabled;
}

namespace Sound
{
    bool Init()
    {
        _device = OpenDevice();

        return _device != 0;
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