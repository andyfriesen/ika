#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "canvas.h"
#include "map.h"

namespace
{
    Canvas _curImage;
    //MapClip _curMapClip;
}

/**
 * Functions to simulate a clipboard, because wx's clipboard interface
 * is annoying, and I don't feel like working it out just yet.
 */
namespace Clipboard
{
    void Set(const Canvas& c)
    {
        _curImage = c;
    }

    const Canvas& GetCanvas()
    {
        return _curImage;
    }

    /*void Set(const MapClip& m)
    {
        _curMapClip = m;
    }

    const MapClip& GetMap()
    {
        return _curMapClip;
    }*/
};

#endif