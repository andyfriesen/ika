#ifndef COLOUR_HANDLER_H
#define COLOUR_HANDLER_H

#include "common/types.h"
#include "common/misc.h"
#include <map>

class ColourHandler
{
public:
    typedef std::map<const std::string, RGBA> colourMap;

private:
    colourMap _colours;

public:
    ColourHandler();

    // Looks up a colour in the colour map
    bool findColour(const std::string &name, RGBA *colour);

    // Convert from hex format to RGBA
    static bool hexColour(const std::string &h, RGBA *colour);

    // Adds a colour to the colour map
    void addColour(const std::string &name, RGBA col);
    
    // Checks whether a named colour has been defined
    bool hasColour(const std::string &name);
    
    // Removes a colour from the map
    void removeColour(const std::string &name);
    
    // Returns the colour map
    const colourMap *getColourMap();

};

#endif
