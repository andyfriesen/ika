
#ifndef EVENTS_H
#define EVENTS_H

// Lots of little event arg structs.  Nothing special.

struct Map;
struct Tileset;


struct MapTileSetEvent {
    Map* map;
    Tileset* tileSet;
    int arg;
    MapTileSetEvent(Map* m, Tileset* ts, int a = 0)
        : map(m)
        , tileSet(ts)
        , arg(a)
    {}
};

struct MapEvent {
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

struct TileSetEvent {
    Tileset* tileSet;
    int arg; // context dependant
    TileSetEvent(Tileset* ts, int a = 0)
        : tileSet(ts), arg(a)
    {}

    TileSetEvent(const MapTileSetEvent& e)
        : tileSet(e.tileSet)
        , arg(e.arg)
    {}
};

#endif