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

bool ColourHandler::findColour(const std::string &name, RGBA *colour)
{
    if(_colours.count(name) == 0)
        return 0;
    *colour = _colours[name];
    return 1;
}

// Convert from hex format to RGBA
bool ColourHandler::hexColour(const std::string &h, RGBA *colour)
{    
    std::string cs("");

    // 123     becomes  112233FF
    // 1234    becomes  11223344
    // 123456  becomes  123456FF
    // then we convert into an ABGR-formatted uint.
    int len = h.length();
    if (len == 3)
        cs = std::string("FF") + h[2] + h[2] + h[1] + h[1] + h[0] + h[0];
    else if (len == 4)
        cs = std::string() + h[3] + h[3] + h[2] + h[2] + h[1] + h[1] + h[0] + h[0];
    else if (len == 6)
        cs = std::string("FF") + h[4] + h[5] + h[2] + h[3] + h[0] + h[1];
    else if (len == 8)
        cs = std::string() + h[6] + h[7] + h[4] + h[5] + h[2] + h[3] + h[0] + h[1];
    else
        return 0;

    uint col;
    if(hexToInt(cs, &col))
    {
        *colour = RGBA(col);
        return 1;
    }
    return 0;
}

// Adds a colour to the colour map
void ColourHandler::addColour(const std::string &name, RGBA col)
{
    if(col.i == 0)
        _colours.erase(name);
    else
        _colours[name] = col;
}

// Checks whether a named colour has been defined
bool ColourHandler::hasColour(const std::string &name)
{
    return (_colours.find(name) != _colours.end());
}

// Adds a colour to the colour map
void ColourHandler::removeColour(const std::string &name)
{
    _colours.erase(name);
}

// Returns the colour map
const ColourHandler::colourMap *ColourHandler::getColourMap()
{
    return &_colours;
}
