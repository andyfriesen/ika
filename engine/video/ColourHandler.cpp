#include "ColourHandler.h"

struct colourEntry
{
    std::string name;
    RGBA colour;
}
    stdColours[] = {
        {"aqua",    RGBA(0,255,255)},
        {"black",   RGBA(0,0,0)},
        {"blue",    RGBA(0,0,255)},
        {"fuchsia", RGBA(255,0,255)},
        {"gray",    RGBA(128,128,128)},
        {"grey",    RGBA(128,128,128)},
        {"green",   RGBA(0,128,0)},
        {"lime",    RGBA(0,255,0)},
        {"maroon",  RGBA(128,0,0)},
        {"navy",    RGBA(0,0,128)},
        {"olive",   RGBA(128,128,0)},
        {"orange",  RGBA(255,165,0)},
        {"purple",  RGBA(128,0,128)},
        {"red",     RGBA(255,0,0)},
        {"silver",  RGBA(192,192,192)},
        {"teal",    RGBA(0,128,128)},
        {"white",   RGBA(255,255,255)},
        {"yellow",  RGBA(255,255,0)},
        {"",        RGBA(0)}
    };
    
ColourHandler::ColourHandler()
{
    for(int i = 0; stdColours[i].name.length(); i++)
        _colours[stdColours[i].name] = stdColours[i].colour;
}

RGBA ColourHandler::getColour(const std::string& name)
{
    if (_colours.count(name))
        return _colours[name];
    else
        return RGBA(255, 255, 255);
}

// Adds a colour to the colour map
void ColourHandler::addColour(const std::string& name, RGBA col)
{
    if(col.i == 0)
        _colours.erase(name);
    else
        _colours[name] = col;
}

// Checks whether a named colour has been defined
bool ColourHandler::hasColour(const std::string& name)
{
    return (_colours.find(name) != _colours.end());
}

// Adds a colour to the colour map
void ColourHandler::removeColour(const std::string& name)
{
    _colours.erase(name);
}

// Returns the colour map
const ColourHandler::colourMap& ColourHandler::getColourMap()
{
    return _colours;
}
