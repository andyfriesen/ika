
#ifndef EVENTS_H
#define EVENTS_H

// Lots of little event arg structs.  Nothing special.

struct Map;
struct Tileset;


struct MapTilesetEvent {
    Map* map;
    Tileset* tileset;
    int arg;
    MapTilesetEvent(Map* m, Tileset* ts, int a = 0)
        : map(m)
        , tileset(ts)
        , arg(a)
    {}
};

struct MapEvent {
    Map* map;
    int  arg; // context dependant.
    MapEvent(Map* m, int a = 0)
        : map(m), arg(a)
    {}

    MapEvent(const MapTilesetEvent& e)
        : map(e.map)
        , arg(e.arg)
    {}
};

struct TilesetEvent {
    Tileset* tileset;
    int arg; // context dependant
    TilesetEvent(Tileset* ts, int a = 0)
        : tileset(ts), arg(a)
    {}

    TilesetEvent(const MapTilesetEvent& e)
        : tileset(e.tileset)
        , arg(e.arg)
    {}
};

#endif