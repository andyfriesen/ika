#pragma once

#include "canvas.h"
#include "map.h"

/**
 * Functions to simulate a clipboard, because wx's clipboard interface
 * is annoying, and I don't feel like working it out just yet.
 */
namespace Clipboard
{
    void Set(const Canvas& c);
    const Canvas& GetCanvas();
    
//    void Set(const MapClip& m);
//    const MapClip& GetMap();

};
