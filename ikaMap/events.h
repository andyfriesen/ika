
#ifndef EVENTS_H
#define EVENTS_H

// Lots of little event arg structs.  Nothing special.

struct Map;
struct TileSet;


struct MapTileSetEvent
{
    Map* map;
    TileSet* tileSet;
    int arg;
    MapTileSetEvent(Map* m, TileSet* ts, int a = 0)
        : map(m)
        , tileSet(ts)
        , arg(a)
    {}
};

struct MapEvent
{
    Map* map;
    int  arg; // context dependant.
    MapEvent(Map* m, int a = 0)
        : map(m), arg(a)
    {}

    MapEvent(const MapTileSetEvent& e)
        : map(e.map)
        , arg(e.arg)
    {}
};

struct TileSetEvent
{
    TileSet* tileSet;
    int arg; // context dependant
    TileSetEvent(TileSet* ts, int a = 0)
        : tileSet(ts), arg(a)
    {}

    TileSetEvent(const MapTileSetEvent& e)
        : tileSet(e.tileSet)
        , arg(e.arg)
    {}
};

#endif