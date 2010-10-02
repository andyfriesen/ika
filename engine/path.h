#ifndef IKAPATH_H
#define IKAPATH_H

#include <string>

/**
 * By lack of better name and Path was already taken. 
 */
struct IkaPath {
public:
    // Path variable
    static std::string                     _game;                                          ///< path/to/ika-game/file

	 // Path variables for resource loading.
    static std::string                     _map;                                           ///< relative/path/to/maps       
};

#endif
